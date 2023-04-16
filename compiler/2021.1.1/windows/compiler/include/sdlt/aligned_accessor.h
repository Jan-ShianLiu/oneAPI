//
// Copyright (c) 2014-2016 Intel Corporation. All Rights Reserved.
//
// The source code contained or described herein and all documents related
// to the source code ("Material") are owned by Intel Corporation or its
// suppliers or licensors.  Title to the Material remains with Intel
// Corporation or its suppliers and licensors.  The Material is protected
// by worldwide copyright laws and treaty provisions.  No part of the
// Material may be used, copied, reproduced, modified, published, uploaded,
// posted, transmitted, distributed, or disclosed in any way without
// Intel's prior express written permission.
//
// No license under any patent, copyright, trade secret or other intellectual
// property right is granted to or conferred upon you by disclosure or
// delivery of the Materials, either expressly, by implication, inducement,
// estoppel or otherwise. Any license under such intellectual property rights
// must be express and approved by Intel in writing.
//
// Unless otherwise agreed by Intel in writing, you may not remove or alter
// this notice or any other notice embedded in Materials by Intel or Intel's
// suppliers or licensors in any way.
//

#ifndef SDLT_ALIGNED_ACCESSOR_H
#define SDLT_ALIGNED_ACCESSOR_H

#include "common.h"
#include "internal/is_aligned.h"
#include "internal/is_builtin.h"

#include "linear_index.h"

namespace sdlt {
namespace __SDLT_IMPL {

template<typename BuiltInT>
struct const_aligned_accessor {
    static_assert(internal::is_builtin<BuiltInT>::value, "const_aligned_accessor is only valid for built-in types");

    const_aligned_accessor(const BuiltInT * a_data)
    : m_data(a_data)
    {
        static_assert(internal::is_builtin<BuiltInT>::value, "Can only instantiate a const_aligned_accessor with a built in type.");
        __SDLT_ASSERT(internal::is_aligned<64>(m_data));
        __SDLT_ASSUME_CACHE_LINE_ALIGNED(m_data);
    }

    const_aligned_accessor(const const_aligned_accessor &other)
    {
        m_data = other.m_data;
        __SDLT_ASSERT(internal::is_aligned<64>(m_data));
        // When a lambda closure makes a copy by value
        // then this alignment hint will get emitted
        // on the copy, allowing the hint to exist before the loop
        // body that is going to access it.
        __SDLT_ASSUME_CACHE_LINE_ALIGNED(m_data);
    }


    SDLT_INLINE
    BuiltInT
    operator [] (const int an_index_d1) const
    {
        __SDLT_ASSERT(an_index_d1 >= 0);

        return m_data[an_index_d1];
    }

    template<typename IndexD1T>
    SDLT_INLINE
    BuiltInT
    operator [] (const IndexD1T an_index_d1) const
    {
        __SDLT_ASSERT(an_index_d1.value() >= 0);

        return m_data[an_index_d1.value()];
    }


    const BuiltInT * m_data;
};

template<typename BuiltInT>
struct aligned_accessor {
    static_assert(internal::is_builtin<BuiltInT>::value, "const_aligned_accessor is only valid for built-in types");

    aligned_accessor(BuiltInT * a_data)
    : m_data(a_data)
    {
        static_assert(internal::is_builtin<BuiltInT>::value, "Can only instantiate a aligned_accessor with a built in type.");
        __SDLT_ASSUME_CACHE_LINE_ALIGNED(m_data);
    }

    aligned_accessor(const aligned_accessor &other)
    {
        m_data = other.m_data;
        // When a lambda closure makes a copy by value
        // then this alignment hint will get emitted
        // on the copy, allowing the hint to exist before the loop
        // body that is going to access it.
        __SDLT_ASSUME_CACHE_LINE_ALIGNED(m_data);
    }

    template <typename IndexD1T>
    struct element
    : public IndexD1T
    {

        element(BuiltInT * a_data,
                const IndexD1T an_index_d1)
        : IndexD1T(an_index_d1)
        , m_data(a_data)
        {
        }

        SDLT_INLINE
        element<IndexD1T> &
        operator = (const BuiltInT &a_value) {
            m_data[IndexD1T::value()] = a_value;
            return *this;
        }

        SDLT_INLINE
        operator BuiltInT () const {
            return m_data[IndexD1T::value()];
        }
    protected:
        BuiltInT * m_data;
    };


    SDLT_INLINE
    element<linear_index>
    operator [] (const int an_index_d1) const
    {
        __SDLT_ASSERT(an_index_d1 >= 0);

        linear_index xindex(an_index_d1);
        return element<linear_index>(m_data, xindex);
    }

    template<typename IndexD1T>
    SDLT_INLINE
    element<IndexD1T>
    operator [] (const IndexD1T an_index_d1) const
    {
        __SDLT_ASSERT(an_index_d1.value() >= 0);

        return element<IndexD1T>(m_data, an_index_d1);
    }

    BuiltInT * m_data;
};

} // namespace __SDLT_IMPL
using __SDLT_IMPL::const_aligned_accessor;
using __SDLT_IMPL::aligned_accessor;
} // namespace sdlt

#endif // SDLT_ALIGNED_ACCESSOR_H
