/* Copyright (C) 2025 Tencent.
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

#include "bq_common/bq_common.h"

namespace bq {
    template <uint32_t SET_COUNT, uint32_t WAY_COUNT>
    class fixed_set_cache {
    public:
        static constexpr uint32_t invalid_value = static_cast<uint32_t>(-1);

        fixed_set_cache()
        {
            clear();
        }

        bq_forceinline bool find(uint64_t key, uint32_t& value, uint32_t& insert_token) const
        {
            const uint32_t set_index = get_set_index(key);
            const slot* set = slots_[set_index];
            insert_token = invalid_value;
            for (uint32_t i = 0; i < WAY_COUNT; ++i) {
                if (set[i].value == invalid_value) {
                    insert_token = set_index * WAY_COUNT + i;
                    return false;
                }
                if (set[i].key == key) {
                    value = set[i].value;
                    insert_token = set_index * WAY_COUNT + i;
                    return true;
                }
            }
            insert_token = set_index * WAY_COUNT + next_victim_[set_index];
            return false;
        }

        bq_forceinline void insert(uint64_t key, uint32_t value)
        {
            uint32_t unused_value;
            uint32_t insert_token;
            find(key, unused_value, insert_token);
            insert(key, value, insert_token);
        }

        bq_forceinline void insert(uint64_t key, uint32_t value, uint32_t insert_token)
        {
            const uint32_t set_index = insert_token / WAY_COUNT;
            const uint32_t way = insert_token % WAY_COUNT;
            slots_[set_index][way].key = key;
            slots_[set_index][way].value = value;
            next_victim_[set_index] = static_cast<uint8_t>((way + 1) % WAY_COUNT);
        }

        void clear()
        {
            for (uint32_t set_index = 0; set_index < SET_COUNT; ++set_index) {
                next_victim_[set_index] = 0;
                for (uint32_t way = 0; way < WAY_COUNT; ++way) {
                    slots_[set_index][way].value = invalid_value;
                }
            }
        }

    private:
        static constexpr uint32_t get_set_bits()
        {
            uint32_t bits = 0;
            uint32_t count = SET_COUNT;
            while (count > 1) {
                ++bits;
                count >>= 1;
            }
            return bits;
        }

        static bq_forceinline uint32_t get_set_index(uint64_t key)
        {
            return static_cast<uint32_t>((key * UINT64_C(11400714819323198485)) >> (64 - get_set_bits()));
        }

        struct slot {
            uint64_t key;
            uint32_t value;
        };

        static_assert(SET_COUNT > 1 && (SET_COUNT & (SET_COUNT - 1)) == 0, "SET_COUNT must be a power of two");
        static_assert(WAY_COUNT > 0 && WAY_COUNT <= UINT8_MAX, "invalid WAY_COUNT");

        slot slots_[SET_COUNT][WAY_COUNT];
        uint8_t next_victim_[SET_COUNT];
    };
}
