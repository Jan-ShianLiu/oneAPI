//
// Copyright (C) 2013-2016 Intel Corporation. All Rights Reserved.
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

#ifndef SDLT_SIMD_ITERATION_INDEX_H
#define SDLT_SIMD_ITERATION_INDEX_H

#include <iostream>

#include "../../common.h"
#include "../../internal/simd/indices.h"
#include "../../no_offset.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace simd {

// For internal use by iterators with offsets over simd_data_type,
// we need an index that has a copy of the offset indexes as they are locals
// we can't just have references to the indices
template<int SimdLaneCountT, int AlignD1OnIndexT>
class iterator_index : public indices<SimdLaneCountT, AlignD1OnIndexT>
{
    static_assert((is_simd_compatible_index<iterator_index, SimdLaneCountT>::value), "Must meet simd index requirements");
    typedef indices<SimdLaneCountT, AlignD1OnIndexT> BaseIndices;
public:
    static const int simdlane_count = SimdLaneCountT;

    explicit SDLT_INLINE
    iterator_index(const int a_linear_index)
    : BaseIndices(a_linear_index)
    {}

    explicit SDLT_INLINE
    iterator_index(const int a_simd_struct_index, const int a_simdlane_index)
    : BaseIndices(a_simd_struct_index, a_simdlane_index)
    {}

    // Pre increment
    iterator_index &
    operator ++()
    {
        ++this->m_simdlane_index;
        if (this->m_simdlane_index >= simdlane_count)
        {
            this->m_simdlane_index = 0;
            ++this->m_simd_struct_index;
        }
        return *this;
    }

    // Post increment
    iterator_index
    operator ++(int)
    {
        iterator_index temp = *this;
        ++this->m_simdlane_index;
        if (this->m_simdlane_index >= simdlane_count)
        {
            this->m_simdlane_index = 0;
            ++this->m_simd_struct_index;
        }
        return temp;

    }

    // Pre decrement
    iterator_index &
    operator --()
    {
        --this->m_simdlane_index;
        if (this->m_simdlane_index < 0)
        {
            this->m_simdlane_index = simdlane_count-1;
            --this->m_simd_struct_index;
        }
        return *this;
    }

    // Post decrement
    iterator_index
    operator --(int)
    {
        iterator_index temp = *this;
        --this->m_simdlane_index;
        if (this->m_simdlane_index < 0)
        {
            this->m_simdlane_index = simdlane_count-1;
            --this->m_simd_struct_index;
        }
        return temp;
    }

    int operator -(const iterator_index & other) const
    {
        return this->value() - other.value();
    }

    iterator_index operator + (int offset) const
    {
        iterator_index offset_iter(this->value() + offset);
        return offset_iter;
    }

    iterator_index operator - (int offset) const
    {
        iterator_index offset_iter(this->value() - offset);
        return offset_iter;
    }

    iterator_index & operator += (int offset)
    {
        static_cast<BaseIndices &>(*this) = BaseIndices(this->value() + offset);
        return *this;
    }

    iterator_index &  operator -= (int offset)
    {
        static_cast<BaseIndices &>(*this) = BaseIndices(this->value() - offset);
        return *this;
    }


    bool operator == (const iterator_index & other) const
    {
        return this->comparison_value() == other.comparison_value();
    }

    bool operator != (const iterator_index & other) const
    {
        return this->comparison_value() != other.comparison_value();
    }

    bool operator < (const iterator_index & other) const
    {
        return this->comparison_value() < other.comparison_value();
    }

    bool operator > (const iterator_index & other) const
    {
        return this->comparison_value() > other.comparison_value();
    }

    bool operator <= (const iterator_index & other) const
    {
        return this->comparison_value() <= other.comparison_value();
    }

    bool operator >= (const iterator_index & other) const
    {
        return this->comparison_value() >= other.comparison_value();
    }

#if 0
    SDLT_INLINE friend
    iterator_index
    operator + (const no_offset , const iterator_index an_index)
    {
        // NOTE:  Notice return type is by value versus a const &.
        // This is a conscience choice as the return type is used as a template parameter
        // and we want the parameter to just be the value type not a const &.
        // TODO:  There are other solutions to this
        return an_index;
    }
#endif

    SDLT_INLINE friend
    std::ostream& operator << (std::ostream& output_stream, const iterator_index &a_simd_index)
    {
        output_stream << "iterator_index<SimdLaneCountT="
                      << SimdLaneCountT
                      << ">{m_simd_struct_index="
                      << a_simd_index.m_simd_struct_index
                      << ", m_simdlane_index="
                      << a_simd_index.m_simdlane_index <<"}";
        return output_stream;
    }
};


} // namespace simd

// Specialize calculators

template<int SimdLaneCountT, int AlignD1OnIndexT>
struct pre_offset_calculator<simd::iterator_index<SimdLaneCountT, AlignD1OnIndexT>>
{
    static SDLT_INLINE int compute(const simd::iterator_index<SimdLaneCountT, AlignD1OnIndexT> &a_index )
    {
        return a_index.template simd_struct_index<SimdLaneCountT>()*SimdLaneCountT;
    }
};


template<int SimdLaneCountT, int AlignD1OnIndexT>
struct varying_calculator<simd::iterator_index<SimdLaneCountT, AlignD1OnIndexT>>
{
    static SDLT_INLINE int compute(const simd::iterator_index<SimdLaneCountT, AlignD1OnIndexT> &a_index ) { return a_index.simdlane_index(); }
};


template<int SimdLaneCountT, int AlignD1OnIndexT>
struct post_offset_calculator<simd::iterator_index<SimdLaneCountT, AlignD1OnIndexT>>
{
    // workaround, really T should be simd::indices<SimdLaneCountT, AlignD1OnIndexT>, since it is not a literal, the constexpr
    // would fail to build.  We don't use T at all, but the generic form here expects a
    // parameter
    template<typename T>
    static SDLT_INLINE constexpr int compute(const T ) { return -simd::iterator_index<SimdLaneCountT, AlignD1OnIndexT>::simdlane_offset; }
};

template<int SimdLaneCountT, int AlignD1OnIndexT>
struct value_calculator<simd::iterator_index<SimdLaneCountT,  AlignD1OnIndexT>>
{
    static SDLT_INLINE constexpr auto
    compute(const simd::iterator_index<SimdLaneCountT,  AlignD1OnIndexT> &a_index)
    ->decltype(a_index.value())
    {
        return a_index.value();
    }
};

} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_SIMD_ITERATION_INDEX_H
