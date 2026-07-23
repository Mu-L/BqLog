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
        bounded_hash_cache() = default;

        ~bounded_hash_cache()
        {
            free(keys_);
            free(values_);
            free(hot_keys_);
            free(hot_values_);
        }

        bq_forceinline bool find(uint64_t key, uint32_t& value, uint32_t& insert_token)
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

            const uint32_t slot_index = find_slot(key);
            if (slot_index == invalid_value) {
                insert_token = 0;
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
            const uint32_t slot_index = find_slot(key);
            if (slot_index != invalid_value) {
                values_[slot_index] = value;
            } else {
                insert_new(key, value);
            }
        }

        bq_forceinline void insert(uint64_t key, uint32_t value, uint32_t)
        {
            insert_new(key, value);
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
        }

    private:
        static constexpr uint32_t invalid_value = static_cast<uint32_t>(-1);

        static bq_forceinline uint32_t get_hot_hash(uint64_t key)
        {
            return static_cast<uint32_t>((key * UINT64_C(11400714819323198485)) >> 32);
        }

        static bq_forceinline uint32_t get_table_hash(uint64_t key)
        {
            return static_cast<uint32_t>(key ^ (key >> 32));
        }

        bq_forceinline uint32_t find_slot(uint64_t key) const
        {
            if (!values_) {
                return invalid_value;
            }
            uint32_t slot_index = get_table_hash(key) & (capacity_ - 1);
            while (values_[slot_index] != invalid_value) {
                if (keys_[slot_index] == key) {
                    return slot_index;
                }
                slot_index = (slot_index + 1) & (capacity_ - 1);
            }
            return invalid_value;
        }

        bool resize(uint32_t new_capacity)
        {
            uint8_t* old_entries = nullptr;
            uint64_t* old_keys = nullptr;
            uint32_t* old_values = nullptr;
            const uint32_t old_size = size_;
            if (old_size) {
                old_entries = static_cast<uint8_t*>(malloc((sizeof(uint64_t) + sizeof(uint32_t)) * old_size));
                if (!old_entries) {
                    return false;
                }
                old_keys = reinterpret_cast<uint64_t*>(old_entries);
                old_values = reinterpret_cast<uint32_t*>(old_entries + sizeof(uint64_t) * old_size);
                uint32_t entry_index = 0;
                for (uint32_t i = 0; i < capacity_; ++i) {
                    if (values_[i] != invalid_value) {
                        old_keys[entry_index] = keys_[i];
                        old_values[entry_index] = values_[i];
                        ++entry_index;
                    }
                }
                free(keys_);
                free(values_);
                keys_ = nullptr;
                values_ = nullptr;
                capacity_ = 0;
            }

            uint64_t* new_keys = static_cast<uint64_t*>(malloc(sizeof(uint64_t) * new_capacity));
            uint32_t* new_values = static_cast<uint32_t*>(malloc(sizeof(uint32_t) * new_capacity));
            if (!new_keys || !new_values) {
                free(new_keys);
                free(new_values);
                free(old_entries);
                size_ = 0;
                return false;
            }
            memset(new_values, 0xFF, sizeof(uint32_t) * new_capacity);
            for (uint32_t i = 0; i < old_size; ++i) {
                uint32_t slot_index = get_table_hash(old_keys[i]) & (new_capacity - 1);
                while (new_values[slot_index] != invalid_value) {
                    slot_index = (slot_index + 1) & (new_capacity - 1);
                }
                new_keys[slot_index] = old_keys[i];
                new_values[slot_index] = old_values[i];
            }
            free(old_entries);
            keys_ = new_keys;
            values_ = new_values;
            capacity_ = new_capacity;
            victim_ = 0;
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
        }

        bq_forceinline void insert_new(uint64_t key, uint32_t value)
        {
            if (!values_) {
                if (!resize(min_capacity)) {
                    return;
                }
            } else if (size_ < MAX_SIZE && size_ >= capacity_ / 2) {
                if (!resize(capacity_ * 2)) {
                    return;
                }
            } else if (size_ >= MAX_SIZE) {
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

            uint32_t slot_index = get_table_hash(key) & (capacity_ - 1);
            while (values_[slot_index] != invalid_value) {
                slot_index = (slot_index + 1) & (capacity_ - 1);
            }
            keys_[slot_index] = key;
            values_[slot_index] = value;
            ++size_;
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
            uint64_t* new_keys = static_cast<uint64_t*>(malloc(sizeof(uint64_t) * new_capacity));
            uint32_t* new_values = static_cast<uint32_t*>(malloc(sizeof(uint32_t) * new_capacity));
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
    };
}
