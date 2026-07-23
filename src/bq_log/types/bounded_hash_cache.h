/* Copyright (C) 2026 Tencent.
 * BQLOG is licensed under the Apache License, Version 2.0.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "bq_common/platform/macros.h"

namespace bq {
    template <uint32_t MAX_SIZE, uint32_t HOT_MAX_CAPACITY>
    class bounded_hash_cache {
    public:
        struct insert_token {
            uint32_t slot_index = UINT32_MAX;
            uint32_t table_revision = 0;

            bool is_valid() const
            {
                return slot_index != UINT32_MAX;
            }
        };

        explicit bounded_hash_cache(uint32_t max_size = MAX_SIZE)
            : max_size_(normalize_max_size(max_size))
        {
        }

        ~bounded_hash_cache()
        {
            free(keys_);
            free(values_);
            free(hot_keys_);
            free(hot_values_);
        }

        bq_forceinline bool find(uint64_t key, uint32_t& value, insert_token& token)
        {
            if (hot_values_ && !hot_bypassed_) {
                const uint32_t hot_hash = get_hot_hash(key);
                const bool hot_found = find_hot(key, hot_hash, value);
                ++hot_queries_;
                hot_hits_ += hot_found ? 1 : 0;
                if (hot_queries_ == hot_sample_size) {
                    if (hot_hits_ < hot_sample_size / 2) {
                        free_hot();
                        hot_bypassed_ = true;
                    }
                    hot_queries_ = 0;
                    hot_hits_ = 0;
                }
                if (hot_found) {
                    return true;
                }
            }

            bool found = false;
            const uint32_t slot_index = find_slot_or_empty(key, found);
            if (!found) {
                token.slot_index = slot_index;
                token.table_revision = table_revision_;
                return false;
            }
            value = values_[slot_index];
            if (!hot_bypassed_) {
                insert_hot(key, get_hot_hash(key), value);
            }
            return true;
        }

        bq_forceinline void insert(uint64_t key, uint32_t value)
        {
            update_hot(key, value);
            bool found = false;
            const uint32_t slot_index = find_slot_or_empty(key, found);
            if (found) {
                values_[slot_index] = value;
            } else {
                insert_new(key, value, insert_token());
            }
        }

        bq_forceinline void insert(uint64_t key, uint32_t value, const insert_token& token)
        {
            insert_new(key, value, token);
        }

        bool set_max_size(uint32_t max_size)
        {
            const uint32_t normalized = normalize_max_size(max_size);
            if (normalized == max_size_) {
                return true;
            }
            if (size_ != 0 && normalized < max_size_) {
                return false;
            }
            max_size_ = normalized;
            return true;
        }

        uint32_t get_max_size() const
        {
            return max_size_;
        }

        void clear()
        {
            free(keys_);
            free(values_);
            keys_ = nullptr;
            values_ = nullptr;
            size_ = 0;
            capacity_ = 0;
            victim_ = 0;
            admission_ = 0;
            free_hot();
            hot_queries_ = 0;
            hot_hits_ = 0;
            hot_bypassed_ = false;
            ++table_revision_;
        }

#if defined(BQ_UNIT_TEST)
        void fail_allocation_after_for_test(int32_t successful_allocations_before_failure)
        {
            allocation_successes_before_failure_ = successful_allocations_before_failure;
        }

        void clear_allocation_failure_for_test()
        {
            allocation_successes_before_failure_ = -1;
        }
#endif

    private:
        static constexpr uint32_t invalid_value = static_cast<uint32_t>(-1);

        static constexpr uint32_t normalize_max_size(uint32_t max_size)
        {
            return max_size < min_capacity
                ? min_capacity
                : (max_size > MAX_SIZE ? MAX_SIZE : max_size);
        }

        static bq_forceinline uint32_t get_hot_hash(uint64_t key)
        {
            return static_cast<uint32_t>((key * UINT64_C(11400714819323198485)) >> 32);
        }

        static bq_forceinline uint32_t get_table_hash(uint64_t key)
        {
            return static_cast<uint32_t>(key ^ (key >> 32));
        }

        bq_forceinline uint32_t find_slot_or_empty(uint64_t key, bool& found) const
        {
            if (!values_) {
                found = false;
                return invalid_value;
            }
            uint32_t slot_index = get_table_hash(key) & (capacity_ - 1);
            while (values_[slot_index] != invalid_value) {
                if (keys_[slot_index] == key) {
                    found = true;
                    return slot_index;
                }
                slot_index = (slot_index + 1) & (capacity_ - 1);
            }
            found = false;
            return slot_index;
        }

        void* allocate_bytes(size_t size)
        {
#if defined(BQ_UNIT_TEST)
            if (allocation_successes_before_failure_ >= 0) {
                if (allocation_successes_before_failure_ == 0) {
                    allocation_successes_before_failure_ = -1;
                    return nullptr;
                }
                --allocation_successes_before_failure_;
            }
#endif
            return malloc(size);
        }

        bool resize(uint32_t new_capacity)
        {
            uint64_t* new_keys = static_cast<uint64_t*>(allocate_bytes(sizeof(uint64_t) * new_capacity));
            if (!new_keys) {
                return false;
            }
            uint32_t* new_values = static_cast<uint32_t*>(allocate_bytes(sizeof(uint32_t) * new_capacity));
            if (!new_keys || !new_values) {
                free(new_keys);
                free(new_values);
                return false;
            }
            memset(new_values, 0xFF, sizeof(uint32_t) * new_capacity);
            for (uint32_t i = 0; i < capacity_; ++i) {
                if (values_[i] != invalid_value) {
                    uint32_t slot_index = get_table_hash(keys_[i]) & (new_capacity - 1);
                    while (new_values[slot_index] != invalid_value) {
                        slot_index = (slot_index + 1) & (new_capacity - 1);
                    }
                    new_keys[slot_index] = keys_[i];
                    new_values[slot_index] = values_[i];
                }
            }
            free(keys_);
            free(values_);
            keys_ = new_keys;
            values_ = new_values;
            capacity_ = new_capacity;
            victim_ = 0;
            ++table_revision_;
            return true;
        }

        void erase(uint32_t slot_index)
        {
            const uint32_t mask = capacity_ - 1;
            uint32_t empty = slot_index;
            uint32_t current = (slot_index + 1) & mask;
            while (values_[current] != invalid_value) {
                const uint32_t home = get_table_hash(keys_[current]) & mask;
                if (((current - home) & mask) > ((empty - home) & mask)) {
                    keys_[empty] = keys_[current];
                    values_[empty] = values_[current];
                    empty = current;
                }
                current = (current + 1) & mask;
            }
            values_[empty] = invalid_value;
            --size_;
            ++table_revision_;
        }

        bq_forceinline void insert_new(uint64_t key, uint32_t value, const insert_token& token)
        {
            if (!values_) {
                if (!resize(min_capacity)) {
                    return;
                }
            } else if (size_ < max_size_ && size_ >= capacity_ / 2) {
                if (!resize(capacity_ * 2)) {
                    return;
                }
            } else if (size_ >= max_size_) {
                if ((++admission_ & 63) != 0) {
                    return;
                }
                while (values_[victim_] == invalid_value) {
                    victim_ = (victim_ + 1) & (capacity_ - 1);
                }
                const uint32_t old_victim = victim_;
                victim_ = (victim_ + 1) & (capacity_ - 1);
                erase(old_victim);
            }

            uint32_t slot_index = invalid_value;
            if (token.table_revision == table_revision_
                && token.slot_index < capacity_
                && values_[token.slot_index] == invalid_value) {
                slot_index = token.slot_index;
            } else {
                bool found = false;
                slot_index = find_slot_or_empty(key, found);
                if (found) {
                    values_[slot_index] = value;
                    return;
                }
            }
            keys_[slot_index] = key;
            values_[slot_index] = value;
            ++size_;
            ++table_revision_;
        }

        bq_forceinline bool find_hot(uint64_t key, uint32_t hot_hash, uint32_t& value) const
        {
            uint32_t slot_index = hot_hash >> hot_index_shift_;
            while (hot_values_[slot_index] != invalid_value) {
                if (hot_keys_[slot_index] == key) {
                    value = hot_values_[slot_index];
                    return true;
                }
                slot_index = (slot_index + 1) & (hot_capacity_ - 1);
            }
            return false;
        }

        bq_forceinline void insert_hot(uint64_t key, uint32_t hot_hash, uint32_t value)
        {
            if (!hot_values_) {
                if (!resize_hot(hot_min_capacity)) {
                    return;
                }
            } else if (hot_capacity_ < HOT_MAX_CAPACITY && hot_size_ >= hot_capacity_ / 2) {
                if (!resize_hot(hot_capacity_ * 2)) {
                    return;
                }
            } else if (hot_size_ >= hot_capacity_ / 2) {
                return;
            }

            uint32_t slot_index = hot_hash >> hot_index_shift_;
            while (hot_values_[slot_index] != invalid_value) {
                slot_index = (slot_index + 1) & (hot_capacity_ - 1);
            }
            hot_keys_[slot_index] = key;
            hot_values_[slot_index] = value;
            ++hot_size_;
        }

        bq_forceinline void update_hot(uint64_t key, uint32_t value)
        {
            if (!hot_values_ || hot_bypassed_) {
                return;
            }
            uint32_t slot_index = get_hot_hash(key) >> hot_index_shift_;
            while (hot_values_[slot_index] != invalid_value) {
                if (hot_keys_[slot_index] == key) {
                    hot_values_[slot_index] = value;
                    return;
                }
                slot_index = (slot_index + 1) & (hot_capacity_ - 1);
            }
        }

        bool resize_hot(uint32_t new_capacity)
        {
            uint64_t* new_keys = static_cast<uint64_t*>(allocate_bytes(sizeof(uint64_t) * new_capacity));
            if (!new_keys) {
                return false;
            }
            uint32_t* new_values = static_cast<uint32_t*>(allocate_bytes(sizeof(uint32_t) * new_capacity));
            if (!new_keys || !new_values) {
                free(new_keys);
                free(new_values);
                return false;
            }
            memset(new_values, 0xFF, sizeof(uint32_t) * new_capacity);

            uint32_t new_size = 0;
            uint32_t new_index_shift = 32;
            for (uint32_t count = new_capacity; count > 1; count >>= 1) {
                --new_index_shift;
            }
            for (uint32_t i = 0; i < hot_capacity_; ++i) {
                if (hot_values_[i] == invalid_value) {
                    continue;
                }
                uint32_t slot_index = get_hot_hash(hot_keys_[i]) >> new_index_shift;
                while (new_values[slot_index] != invalid_value) {
                    slot_index = (slot_index + 1) & (new_capacity - 1);
                }
                new_keys[slot_index] = hot_keys_[i];
                new_values[slot_index] = hot_values_[i];
                ++new_size;
            }

            free(hot_keys_);
            free(hot_values_);
            hot_keys_ = new_keys;
            hot_values_ = new_values;
            hot_size_ = new_size;
            hot_capacity_ = new_capacity;
            hot_index_shift_ = new_index_shift;
            return true;
        }

        void free_hot()
        {
            free(hot_keys_);
            free(hot_values_);
            hot_keys_ = nullptr;
            hot_values_ = nullptr;
            hot_size_ = 0;
            hot_capacity_ = 0;
            hot_index_shift_ = 0;
        }

        static constexpr uint32_t min_capacity = 8;
        static constexpr uint32_t hot_min_capacity = 8;
        static constexpr uint32_t hot_sample_size = 8192;

        static_assert(MAX_SIZE >= min_capacity, "MAX_SIZE is too small");
        static_assert(HOT_MAX_CAPACITY >= hot_min_capacity && (HOT_MAX_CAPACITY & (HOT_MAX_CAPACITY - 1)) == 0, "HOT_MAX_CAPACITY must be a power of two");

        bounded_hash_cache(const bounded_hash_cache&) = delete;
        bounded_hash_cache& operator=(const bounded_hash_cache&) = delete;

        uint64_t* keys_ = nullptr;
        uint32_t* values_ = nullptr;
        uint32_t size_ = 0;
        uint32_t capacity_ = 0;
        uint32_t victim_ = 0;
        uint32_t admission_ = 0;

        uint64_t* hot_keys_ = nullptr;
        uint32_t* hot_values_ = nullptr;
        uint32_t hot_size_ = 0;
        uint32_t hot_capacity_ = 0;
        uint32_t hot_index_shift_ = 0;
        uint32_t hot_queries_ = 0;
        uint32_t hot_hits_ = 0;
        bool hot_bypassed_ = false;
        uint32_t max_size_ = MAX_SIZE;
        uint32_t table_revision_ = 0;

#if defined(BQ_UNIT_TEST)
        int32_t allocation_successes_before_failure_ = -1;
#endif
    };
}
