﻿#pragma once
/*
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
/*!
 * \file basic_types.h
 *
 * \author pippocao
 * \date 2022/07/14
 *
 */
#include <stdint.h>
#include "bq_common/misc/assert.h"
#include "bq_common/types/type_traits.h"
#include "bq_common/types/type_tools.h"
namespace bq {
    enum class log_level {
        verbose,
        debug,
        info,
        warning,
        error,
        fatal,

        log_level_max = 32,
    };

    // This is a wrapper struct which can define your data alignment.
    //"alignas" keyword can only ensure alignment on heap allocation when c++ standard is up to c++17
    // and it does not work correctly when object is constructed with "placement new" when starting address is not aligned.
    //***important: This implementation might introduce some performance overhead.
    template <typename T, uint32_t align>
    struct aligned_type;

    template <typename T>
    struct is_aligned_type : public bq::false_type {
    };

    template <typename T, uint32_t align>
    struct is_aligned_type<bq::aligned_type<T, align>> : public bq::true_type {
    };

    template <typename T, uint32_t align>
    struct aligned_type {
    private:
        typedef uint32_t offset_type;
        static_assert(sizeof(offset_type) <= 4, "size of offset_type should <= 4");
        static constexpr size_t offset_mask = ~(sizeof(offset_type) - 1);

        uint8_t offset_[sizeof(offset_type) + sizeof(offset_type)];
        uint8_t padding_[align + sizeof(T)];

    private:
        uint32_t& offset() const
        {
            return *(uint32_t*)(((size_t)offset_ + (sizeof(offset_type) - 1)) & offset_mask);
        }

        void init()
        {
            size_t addr = (size_t)padding_;
            uint32_t mod = (uint32_t)(addr % align);
            if (mod == 0) {
                offset() = 0;
            } else {
                offset() = (uint32_t)align - mod;
            }
        }

        void* get_addr() const
        {
            return (void*)(padding_ + offset());
        }

    public:
        aligned_type()
        {
            init();
            new (get_addr(), bq::enum_new_dummy::dummy) T();
        }

        ~aligned_type()
        {
            bq::object_destructor<T>::destruct((T*)get_addr());
        }

        inline aligned_type(const aligned_type<T, align>& rhs)
        {
            init();
            new (get_addr(), bq::enum_new_dummy::dummy) T(rhs.get());
        }

        inline aligned_type(aligned_type<T, align>&& rhs) noexcept
        {
            init();
            new (get_addr(), bq::enum_new_dummy::dummy) T(bq::move(rhs.get()));
        }

        template <typename U, uint32_t U_align>
        inline aligned_type(const aligned_type<U, U_align>& rhs)
        {
            init();
            new (get_addr(), bq::enum_new_dummy::dummy) T(rhs.get());
        }

        template <typename U, uint32_t U_align>
        inline aligned_type(aligned_type<U, U_align>&& rhs) noexcept
        {
            init();
            new (get_addr(), bq::enum_new_dummy::dummy) T(bq::move(rhs.get()));
        }

        template <typename U, typename enable_if<!is_aligned_type<typename decay<U>::type>::value>::type* = nullptr>
        inline aligned_type(U&& init_value)
        {
            init();
            new (get_addr(), bq::enum_new_dummy::dummy) T(bq::forward<U>(init_value));
        }

        inline aligned_type<T, align>& operator=(const aligned_type<T, align>& rhs)
        {
            get() = rhs.get();
            return *this;
        }

        inline aligned_type<T, align>& operator=(aligned_type<T, align>&& rhs) noexcept
        {
            get() = bq::move(rhs.get());
            return *this;
        }

        template <typename U, uint32_t U_align>
        inline aligned_type<T, align>& operator=(const aligned_type<U, U_align>& rhs)
        {
            get() = rhs.get();
            return *this;
        }

        template <typename U, uint32_t U_align>
        inline aligned_type<T, align>& operator=(aligned_type<U, U_align>&& rhs) noexcept
        {
            get() = bq::move(rhs.get());
            return *this;
        }

        template <typename U, typename enable_if<!is_aligned_type<typename decay<U>::type>::value>::type* = nullptr>
        inline aligned_type<T, align>& operator=(U&& value)
        {
            get() = bq::forward<U>(value);
            return *this;
        }

        inline T& get()
        {
            return *(T*)get_addr();
        }
        inline const T& get() const
        {
            return *(const T*)get_addr();
        }
    };

    // This is a wrapper struct which can ensure your data monopolize L1 cache line.
    // It works in C++ 11 and later version, regardless whether object is allocated in heap or stack.
    // Warning : There maybe some memory waste.
    template <typename T>
    struct cache_friendly_type {
    private:
        static constexpr size_t CACHE_LINE_SIZE = 64;
        uint8_t padding_left_[CACHE_LINE_SIZE];
        T value_;
        uint8_t padding_right_[CACHE_LINE_SIZE];

    public:
        template <typename U>
        cache_friendly_type(U&& rhs)
            : value_(bq::forward<U>(rhs))
        {
        }

        inline T& get()
        {
            return value_;
        }

        inline const T& get() const
        {
            return value_;
        }
    };

    template <typename T>
    struct scoped_obj {
    private:
        T* ptr;

    public:
        scoped_obj()
            : ptr(nullptr)
        {
        }
        scoped_obj(T* new_obj)
        {
            ptr = new_obj;
        }
        scoped_obj& operator=(const scoped_obj& rhs) = delete;
        ~scoped_obj()
        {
            if (ptr) {
                delete ptr;
            }
        }
        T* operator->()
        {
            return ptr;
        }
        T* transfer()
        {
            T* tmp = ptr;
            ptr = nullptr;
            return tmp;
        }
    };

    // simple substitude of std::unique_ptr
    // which can handle the vast majority of cases.
    template <typename T>
    class unique_ptr {
    public:
        unique_ptr()
            : ptr(nullptr)
        {
        }
        ~unique_ptr() { reset(); }
        unique_ptr(T* new_ptr)
            : ptr(new_ptr)
        {
        }
        template <typename D>
        unique_ptr(unique_ptr<D>&& rhs) noexcept
            : ptr(rhs.ptr)
        {
            rhs.ptr = nullptr;
        }

        unique_ptr(const unique_ptr& rhs) = delete;
        unique_ptr& operator=(T* new_ptr) = delete;
        unique_ptr& operator=(const unique_ptr& rhs) = delete;

        template <typename D>
        unique_ptr& operator=(unique_ptr<D>&& rhs)
        {
            if (ptr != rhs.ptr) {
                reset();
                ptr = rhs.ptr;
                rhs.ptr = nullptr;
            }
            return *this;
        }
        T* operator->()
        {
            return ptr;
        }
        const T* operator->() const
        {
            return ptr;
        }
        T& operator*()
        {
            return *ptr;
        }
        const T& operator*() const
        {
            return *ptr;
        }

        operator void*() const
        {
            return static_cast<void*>(ptr);
        }

        template <typename D>
        bool operator==(const unique_ptr<D>& rhs) const
        {
            return ptr == rhs.operator->();
        }
        bool operator==(decltype(nullptr)) const
        {
            return ptr == nullptr;
        }
        template <typename D>
        bool operator!=(const unique_ptr<D>& rhs) const
        {
            return ptr != rhs.operator->();
        }
        template <typename D>
        bool operator<(const unique_ptr<D>& rhs) const
        {
            return ptr < rhs.operator->();
        }
        template <typename D>
        bool operator<=(const unique_ptr<D>& rhs) const
        {
            return ptr <= rhs.operator->();
        }
        template <typename D>
        bool operator>(const unique_ptr<D>& rhs) const
        {
            return ptr > rhs.operator->();
        }
        template <typename D>
        bool operator>=(const unique_ptr<D>& rhs) const
        {
            return ptr >= rhs.operator->();
        }
        T* get()
        {
            return ptr;
        }
        template <typename D>
        void swap(unique_ptr<D>& rhs)
        {
            auto old = ptr;
            ptr = rhs.ptr;
            rhs.ptr = old;
        }
        void reset()
        {
            if (ptr) {
                delete ptr;
                ptr = nullptr;
            }
        }

    private:
        template <typename>
        friend class unique_ptr;

        T* ptr;
    };

    template <typename T, typename... Ts>
    unique_ptr<T> make_unique(Ts&&... params)
    {
        return unique_ptr<T>(new T(bq::forward<Ts>(params)...));
    }

    // make a unique_ptr with default initialization
    template <class T>
    unique_ptr<T> make_unique_for_overwrite()
    {
        return unique_ptr<T>(new T);
    }
}
