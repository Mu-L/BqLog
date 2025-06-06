﻿/*
 * Copyright (C) 2024 THL A29 Limited, a Tencent company.
 * BQLOG is licensed under the Apache License, Version 2.0.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "bq_log/types/ring_buffer.h"
#include "bq_log/misc/bq_log_api.h"

namespace bq {

    ring_buffer::ring_buffer(uint32_t capacity, uint64_t serialize_id /* =0*/)
        : write_cursor_(cursor_type())
        , read_cursor_(cursor_type())
        , current_reading_cursor_(cursor_type().atomic_value.load())
        , current_reading_cursor_tmp_((uint32_t)-1)
        , real_buffer_(nullptr)
    {
        (void)cache_line_padding0_;
        (void)cache_line_padding1_;
        (void)cache_line_padding2_;
        (void)cache_line_padding3_;
        (void)padding_;
        if (capacity < 16 * cache_line_size) {
            bq::util::log_device_console(bq::log_level::warning, "invalid ring_buffer capacity {}, it should not be less than 16 * 64 bytes. it will be set to 16 * 64 automatically", capacity);
            capacity = 16 * cache_line_size;
        }
        auto mmap_result = create_memory_map(capacity, serialize_id);
        switch (mmap_result) {
        case bq::ring_buffer::failed:
            init_with_memory(capacity);
            break;
        case bq::ring_buffer::use_existed:
            if (!try_recover_from_exist_memory_map()) {
                init_with_memory_map();
            }
            break;
        case bq::ring_buffer::new_created:
            init_with_memory_map();
            break;
        default:
            break;
        }

#if BQ_RING_BUFFER_DEBUG
        total_write_bytes_ = 0;
        total_read_bytes_ = 0;
        for (int32_t i = 0; i < (int32_t)enum_buffer_result_code::result_code_count; ++i) {
            result_code_statistics_[i].store(0);
        }
#endif
    }

    ring_buffer::~ring_buffer()
    {
        if (!uninit_memory_map()) {
            free(real_buffer_);
        }
    }

    ring_buffer_write_handle ring_buffer::alloc_write_chunk(uint32_t size)
    {
        ring_buffer_write_handle handle;
        int32_t max_try_count = 100;

        uint32_t size_required = size + (uint32_t)data_block_offset;
        uint32_t need_block_count = (size_required + (cache_line_size - 1)) >> cache_line_size_log2;
        if (need_block_count > aligned_blocks_count_ || need_block_count == 0) {
#if BQ_RING_BUFFER_DEBUG
            ++result_code_statistics_[(int32_t)enum_buffer_result_code::err_alloc_size_invalid];
#endif
            handle.result = enum_buffer_result_code::err_alloc_size_invalid;
            return handle;
        }

        uint32_t current_write_cursor = write_cursor_.volatile_value;
        uint32_t current_read_cursor = read_cursor_.atomic_value.load(bq::platform::memory_order::acquire);
        if (memory_map_handle_.get_mapped_data()) {
            //Due to the bidirectional synchronization feature of mmap memory, 
            //it disrupts our delicate memory model. Sometimes, the read_cursor may advance past the write_cursor. 
            //In such cases, we need to make an adjustment to it. 
            //This is a tricky issue, and perhaps in the future, we can find a better and more intuitive way to correct it.
            if (current_read_cursor - current_write_cursor <= aligned_blocks_count_) {
                current_read_cursor = current_write_cursor;
            }
        }
        uint32_t used_blocks_count = current_write_cursor - current_read_cursor;
        handle.approximate_used_blocks_count = used_blocks_count;
        do {
            uint32_t new_cursor = current_write_cursor + need_block_count;
            if (new_cursor - current_read_cursor > aligned_blocks_count_) {
                // not enough space
#if BQ_RING_BUFFER_DEBUG
                ++result_code_statistics_[(int32_t)enum_buffer_result_code::err_not_enough_space];
#endif
                handle.result = enum_buffer_result_code::err_not_enough_space;
                return handle;
            }

            if (use_cas_alloc) {
                // #1 version  CAS
                if (!write_cursor_.atomic_value.compare_exchange_strong(current_write_cursor, new_cursor, bq::platform::memory_order::relaxed, bq::platform::memory_order::relaxed)) {
#if BQ_RING_BUFFER_DEBUG
                    ++result_code_statistics_[(int32_t)enum_buffer_result_code::err_alloc_failed_by_race_condition];
#endif
                    handle.result = enum_buffer_result_code::err_alloc_failed_by_race_condition;
                    continue;
                }
            } else {
                // #2 version   fetch_add
                //  This version has better high-concurrency performance compared to the #1 CAS version, but it is not rigorous enough.
                //  In a hypothetical scenario with concurrent thread competition, if the total memory allocated by all threads exceeds 256GB, it could lead to an overflow issue.
                current_write_cursor = write_cursor_.atomic_value.fetch_add(need_block_count, bq::platform::memory_order::relaxed);
                uint32_t next_write_cursor = current_write_cursor + need_block_count;
                while (next_write_cursor - current_read_cursor >= aligned_blocks_count_) {
                    // fall back
                    uint32_t expected = next_write_cursor;
                    if (write_cursor_.atomic_value.compare_exchange_strong(expected, current_write_cursor, platform::memory_order::relaxed, platform::memory_order::relaxed)) {
                        // not enough space
#if BQ_RING_BUFFER_DEBUG
                        ++result_code_statistics_[(int32_t)enum_buffer_result_code::err_not_enough_space];
#endif
                        handle.result = enum_buffer_result_code::err_not_enough_space;
                        return handle;
                    }
                    current_read_cursor = read_cursor_.atomic_value.load(bq::platform::memory_order::acquire);
                }
            }

            block& new_block = cursor_to_block(current_write_cursor);
            new_block.data_section_head.block_num = need_block_count;
            if ((current_write_cursor & (aligned_blocks_count_ - 1)) + need_block_count > aligned_blocks_count_) {
                // data must be guaranteed to be contiguous in memory
                handle.result = enum_buffer_result_code::err_data_not_contiguous;
#if BQ_RING_BUFFER_DEBUG
                ++result_code_statistics_[(int32_t)enum_buffer_result_code::err_data_not_contiguous];
#endif
                ++max_try_count;
                new_block.data_section_head.status.store(bq::ring_buffer::block_status::invalid, platform::memory_order::release);
                continue;
            }
            new_block.data_section_head.data_size = size;
            handle.result = enum_buffer_result_code::success;
            handle.data_addr = new_block.data_section_head.data;
            handle.approximate_used_blocks_count += need_block_count;
#if BQ_RING_BUFFER_DEBUG
            ++result_code_statistics_[(int32_t)enum_buffer_result_code::success];
#endif
            return handle;
        } while (--max_try_count > 0);
        return handle;
    }

    void ring_buffer::commit_write_chunk(const ring_buffer_write_handle& handle)
    {
        if (handle.result != enum_buffer_result_code::success) {
            return;
        }
        block* block_ptr = (block*)(handle.data_addr - data_block_offset);
        block_ptr->data_section_head.status.store(bq::ring_buffer::block_status::used, bq::platform::memory_order::release);
#if BQ_RING_BUFFER_DEBUG
        total_write_bytes_ += block_ptr->data_section_head.block_num * sizeof(block);
#endif
    }

    void ring_buffer::begin_read()
    {
#if BQ_RING_BUFFER_DEBUG
        if (check_thread_) {
            bq::platform::thread::thread_id current_thread_id = bq::platform::thread::get_current_thread_id();
            if (read_thread_id_ == empty_thread_id_) {
                read_thread_id_ = current_thread_id;
            } else {
                assert(current_thread_id == read_thread_id_ && "only single thread reading is supported for ring buffer!");
            }
            assert(current_reading_cursor_tmp_ == (uint32_t)-1 && "Please ensure that you call the functions in the following order: first begin_read() -> read() -> end_read().");
        }
#endif
        current_reading_cursor_tmp_ = current_reading_cursor_;
    }

    ring_buffer_read_handle ring_buffer::read()
    {
#if BQ_RING_BUFFER_DEBUG
        if (check_thread_) {
            bq::platform::thread::thread_id current_thread_id = bq::platform::thread::get_current_thread_id();
            assert(current_thread_id == read_thread_id_ && "only single thread reading is supported for ring buffer!");
            assert(current_reading_cursor_tmp_ != (uint32_t)-1 && "Please ensure that you call the functions in the following order: first begin_read() -> read() -> end_read().");
        }
#endif
        ring_buffer_read_handle handle;
        while (true) {
            block& block_ref = cursor_to_block(current_reading_cursor_tmp_);
            auto status = block_ref.data_section_head.status.load(bq::platform::memory_order::acquire);
            auto block_count = block_ref.data_section_head.block_num;
            switch (status) {
            case block_status::invalid:
#if BQ_RING_BUFFER_DEBUG
                assert((current_reading_cursor_tmp_ & (aligned_blocks_count_ - 1)) + block_count > aligned_blocks_count_);
                assert(block_count <= aligned_blocks_count_);
#endif
                current_reading_cursor_tmp_ += block_count;
                continue;
                break;
            case block_status::unused:
#if BQ_RING_BUFFER_DEBUG
                ++result_code_statistics_[(int32_t)enum_buffer_result_code::err_empty_ring_buffer];
#endif
                handle.result = enum_buffer_result_code::err_empty_ring_buffer;
                break;
            case block_status::used:
#if BQ_RING_BUFFER_DEBUG
                assert((current_reading_cursor_tmp_ & (aligned_blocks_count_ - 1)) + block_count <= aligned_blocks_count_);
#endif
                handle.result = enum_buffer_result_code::success;
                handle.data_addr = block_ref.data_section_head.data;
                handle.data_size = block_ref.data_section_head.data_size;
                current_reading_cursor_tmp_ += block_count;
                break;
            default:
                //this error only occurs when memory map is applied
                assert((memory_map_handle_.has_been_mapped()) && "invalid read ring buffer block status");
#if BQ_RING_BUFFER_DEBUG
                    ++result_code_statistics_[(int32_t)enum_buffer_result_code::err_mmap_sync];
#endif
                handle.result = enum_buffer_result_code::err_mmap_sync;
                break;
            }
            break;
        }

#if BQ_RING_BUFFER_DEBUG
        ++result_code_statistics_[(int32_t)handle.result];
#endif
        return handle;
    }

    void ring_buffer::end_read()
    {
#if BQ_RING_BUFFER_DEBUG
        if (check_thread_) {
            bq::platform::thread::thread_id current_thread_id = bq::platform::thread::get_current_thread_id();
            assert(current_thread_id == read_thread_id_ && "only single thread reading is supported for ring buffer!");
            assert(current_reading_cursor_tmp_ != (uint32_t)-1 && "Please ensure that you call the functions in the following order: first begin_read() -> read() -> end_read().");
        }
#endif
        uint32_t block_count = current_reading_cursor_tmp_ - current_reading_cursor_;
        if (block_count > 0) {
            for (uint32_t i = 0; i < block_count; ++i) {
                cursor_to_block(current_reading_cursor_ + i).data_section_head.odinary_status = block_status::unused;
            }
#if BQ_RING_BUFFER_DEBUG
            total_read_bytes_ += block_count * sizeof(block);
#endif
            buffer_head_->read_cursor_consumer_cache_ = current_reading_cursor_tmp_;
            current_reading_cursor_ = current_reading_cursor_tmp_;
            read_cursor_.atomic_value.fetch_add(block_count, bq::platform::memory_order::release);
        }
#if BQ_RING_BUFFER_DEBUG
        current_reading_cursor_tmp_ = (uint32_t)-1;
#endif
    }

    ring_buffer::create_memory_map_result ring_buffer::create_memory_map(uint32_t capacity, uint64_t serialize_id)
    {
        if (!bq::memory_map::is_platform_support() || serialize_id == 0) {
            return create_memory_map_result::failed;
        }
        char name_tmp[64];
        snprintf(name_tmp, sizeof(name_tmp), "mmap/%" PRIu64 ".mmp", serialize_id);
        string path = TO_ABSOLUTE_PATH(name_tmp, true);
        memory_map_file_ = bq::file_manager::instance().open_file(path, file_open_mode_enum::auto_create | file_open_mode_enum::read_write | file_open_mode_enum::exclusive);
        if (!memory_map_file_.is_valid()) {
            bq::util::log_device_console(bq::log_level::warning, "failed to open mmap file %s, use memory instead of mmap file, error code:%d", path.c_str(), bq::file_manager::get_and_clear_last_file_error());
            return create_memory_map_result::failed;
        }
        capacity = bq::roundup_pow_of_two(capacity);
        aligned_blocks_count_ = capacity >> cache_line_size_log2;
        size_t head_size = (sizeof(buffer_head) + cache_line_size - 1) & ~(cache_line_size - 1);
        size_t map_size = (uint32_t)(capacity + head_size);
        size_t file_size = bq::memory_map::get_min_size_of_memory_map_file(0, map_size);
        create_memory_map_result result = create_memory_map_result::failed;
        size_t current_file_size = bq::file_manager::instance().get_file_size(memory_map_file_);
        if (current_file_size != file_size) {
            if (!bq::file_manager::instance().truncate_file(memory_map_file_, file_size)) {
                bq::util::log_device_console(log_level::warning, "ring buffer truncate memory map file \"%s\" failed, use memory instead.", memory_map_file_.abs_file_path().c_str());
                return create_memory_map_result::failed;
            }
            result = create_memory_map_result::new_created;
        } else {
            result = create_memory_map_result::use_existed;
        }

        memory_map_handle_ = bq::memory_map::create_memory_map(memory_map_file_, 0, map_size);
        if (!memory_map_handle_.has_been_mapped()) {
            bq::util::log_device_console(log_level::warning, "ring buffer create memory map failed from file \"%s\" failed, use memory instead.", memory_map_file_.abs_file_path().c_str());
            return create_memory_map_result::failed;
        }

        if (((uintptr_t)memory_map_handle_.get_mapped_data() & (cache_line_size - 1)) != 0) {
            bq::util::log_device_console(log_level::warning, "ring buffer memory map file \"%s\" memory address is not aligned, use memory instead.", memory_map_file_.abs_file_path().c_str());
            bq::memory_map::release_memory_map(memory_map_handle_);
            return create_memory_map_result::failed;
        }
        real_buffer_ = (uint8_t*)memory_map_handle_.get_mapped_data();
        buffer_head_ = (buffer_head*)real_buffer_;
        aligned_blocks_ = (ring_buffer::block*)(real_buffer_ + head_size);
        return result;
    }

    void ring_buffer::init_with_memory_map()
    {
        for (uint32_t i = 0; i < aligned_blocks_count_; ++i) {
            aligned_blocks_[i].data_section_head.status.store(bq::ring_buffer::block_status::unused, platform::memory_order::release);
        }
        current_reading_cursor_ = 0;
        current_reading_cursor_tmp_ = (uint32_t)-1;
        buffer_head_->read_cursor_consumer_cache_ = 0;
        write_cursor_.atomic_value.store(0, platform::memory_order::release);
        read_cursor_.atomic_value.store(0, platform::memory_order::release);
    }

    bool ring_buffer::try_recover_from_exist_memory_map()
    {
        // verify
        buffer_head_->read_cursor_consumer_cache_ = buffer_head_->read_cursor_consumer_cache_ & (aligned_blocks_count_ - 1);
        uint32_t current_cursor = buffer_head_->read_cursor_consumer_cache_;
        bool data_parse_finished = false;
        while ((current_cursor - buffer_head_->read_cursor_consumer_cache_ < aligned_blocks_count_) && !data_parse_finished) {
            block& current_block = cursor_to_block(current_cursor);
            auto block_num = current_block.data_section_head.block_num;
            if(current_cursor + block_num > buffer_head_->read_cursor_consumer_cache_ + aligned_blocks_count_)
            {
                return false;
            }
            switch (current_block.data_section_head.status.load(bq::platform::memory_order::relaxed)) {
            case block_status::used:
                if((current_cursor & (aligned_blocks_count_ - 1)) + block_num > aligned_blocks_count_)
                {
                    return false;
                }
                if((size_t)current_block.data_section_head.data_size > (block_num * sizeof(block) - data_block_offset)
                    || (size_t)current_block.data_section_head.data_size <= (block_num == 1 ? 0 : (block_num * sizeof(block) - sizeof(block) - data_block_offset))
                )
                {
                    return false;
                }
                current_cursor += block_num;
                break;
            case block_status::invalid:
                current_cursor += block_num;
                if((current_cursor & (aligned_blocks_count_ - 1)) + block_num <= aligned_blocks_count_)
                {
                    return false;
                }
                break;
            case block_status::unused:
                data_parse_finished = true;
                break;
            default:
                return false;
                break;
            }
        }
        if (current_cursor - buffer_head_->read_cursor_consumer_cache_ > aligned_blocks_count_) {
            return false;
        }
        write_cursor_.atomic_value.store(current_cursor, platform::memory_order::release);
        read_cursor_.atomic_value.store(buffer_head_->read_cursor_consumer_cache_, platform::memory_order::release);
        current_reading_cursor_ = buffer_head_->read_cursor_consumer_cache_;
        current_reading_cursor_tmp_ = (uint32_t)-1;

        for (uint32_t i = current_cursor; i < current_reading_cursor_ + aligned_blocks_count_; ++i) {
            cursor_to_block(i).data_section_head.status.store(block_status::unused, bq::platform::memory_order::release);
        }
        return true;
    }

    void ring_buffer::init_with_memory(uint32_t capacity)
    {
        capacity = bq::roundup_pow_of_two(capacity);
        aligned_blocks_count_ = capacity >> cache_line_size_log2;

        size_t head_size = (sizeof(buffer_head) + cache_line_size - 1) & ~(cache_line_size - 1);

        size_t alloc_size = (uint32_t)(capacity + head_size + cache_line_size);
        real_buffer_ = (uint8_t*)malloc(sizeof(uint8_t) * alloc_size);

        size_t align_unit = cache_line_size;
        uintptr_t aligned_addr = (uintptr_t)real_buffer_;
        aligned_addr = (aligned_addr + align_unit - 1) & (~(align_unit - 1));
        buffer_head_ = (buffer_head*)aligned_addr;
        aligned_blocks_ = (ring_buffer::block*)(aligned_addr + head_size);
        for (uint32_t i = 0; i < aligned_blocks_count_; ++i) {
            aligned_blocks_[i].data_section_head.status.store(bq::ring_buffer::block_status::unused, platform::memory_order::release);
        }
        current_reading_cursor_ = 0;
        current_reading_cursor_tmp_ = (uint32_t)-1;
        buffer_head_->read_cursor_consumer_cache_ = 0;
        write_cursor_.atomic_value.store(0, platform::memory_order::release);
        read_cursor_.atomic_value.store(0, platform::memory_order::release);
    }

    bool ring_buffer::uninit_memory_map()
    {
        if (memory_map_handle_.has_been_mapped()) {
            bq::memory_map::release_memory_map(memory_map_handle_);
            return true;
        }
        return false;
    }

}
