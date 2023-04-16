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

#ifndef SDLT_LINEAR_BASE_SIMD_INDICES_H
#define SDLT_LINEAR_BASE_SIMD_INDICES_H

#include <iostream>

#include "../../common.h"
#include "../../aligned.h"
#include "../../fixed.h"
#include "../compound_index.h"
#include "../is_linear_compatible_index.h"
#include "../is_simd_compatible_index.h"
#include "../operation.h"
#include "../pre_offset_calculator.h"
#include "../post_offset_calculator.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace simd {

template<int SimdLaneCountT, int AlignD1OnIndexT>
class indices
{
public:
    static const int simdlane_count = SimdLaneCountT;
    static const int align_to_simdlane = AlignD1OnIndexT%simdlane_count;
    static const int simdlane_offset = (simdlane_count - align_to_simdlane)%simdlane_count;


    explicit SDLT_INLINE
    indices(const int a_linear_index)
    : m_simd_struct_index((a_linear_index + simdlane_offset)/simdlane_count)
    , m_simdlane_index((a_linear_index + simdlane_offset)%simdlane_count)
    {
        // NOTE: allow negative indexes for loop comparison purposes
        //__SDLT_ASSERT(a_linear_index >= 0);
    }

    explicit SDLT_INLINE
    indices(const int a_simd_struct_index, const int a_simdlane_index)
    : m_simd_struct_index(a_simd_struct_index)
    , m_simdlane_index(a_simdlane_index)
    {
        // Assume caller has taken the simdlane_count offset into account
    }

    // Simd compatible
    template<int OtherSimdLaneCountT>
    SDLT_INLINE int simd_struct_index() const {
        static_assert(OtherSimdLaneCountT == SimdLaneCountT, "Can only call simd_struct_index<OtherSimdLaneCountT> with SimdLaneCountT");
        return m_simd_struct_index;
    }

    SDLT_INLINE int simdlane_index() const { return m_simdlane_index; }

    // Linear compatible
    SDLT_INLINE int pre_offset() const { return m_simd_struct_index*SimdLaneCountT; }
    SDLT_INLINE int varying()    const { return m_simdlane_index; }
    static SDLT_INLINE constexpr int post_offset() { return -simdlane_offset; }

    SDLT_INLINE int value() const
    {
        return (m_simd_struct_index*SimdLaneCountT + m_simdlane_index) - simdlane_offset;
    }
    static_assert((is_simd_compatible_index<indices, SimdLaneCountT>::value), "Must meet simd index requirements");


#if 0
    SDLT_INLINE friend
    indices
    operator + (const no_offset , const indices an_index)
    {
        // NOTE:  Notice return type is by value versus a const &.
        // This is a conscience choice as the return type is used as a template parameter
        // and we want the parameter to just be the value type not a const &.
        // TODO:  There are other solutions to this
        return an_index;
    }
#endif

    template<int NumberT, typename = sdlt::__SDLT_IMPL::internal::enable_if_type<(NumberT != 0)> >
    SDLT_INLINE friend
    compound_index<indices, operation<arithmetic_operator::add, sequence::pre_index, fixed<NumberT> > >
    operator + (const fixed<NumberT> offset, const indices an_index)
    {
        typedef operation<arithmetic_operator::add, sequence::pre_index, fixed<NumberT> > add_op;
        return compound_index<indices, add_op > (an_index, add_op(offset));
    }

    template<int AlignmentT>
    SDLT_INLINE friend
    compound_index<indices, operation<arithmetic_operator::add, sequence::post_index, aligned<AlignmentT> > >
    operator + (const aligned<AlignmentT> offset, const indices an_index)
    {
        typedef operation<arithmetic_operator::add, sequence::post_index, aligned<AlignmentT> > add_op;
        return compound_index<indices, add_op > (an_index, add_op(offset));
    }

    SDLT_INLINE friend
    compound_index<indices, operation<arithmetic_operator::add, sequence::pre_index, int > >
    operator + (const int offset, const indices an_index)
    {
        typedef operation<arithmetic_operator::add, sequence::pre_index, int > add_op;
        return compound_index<indices, add_op > (an_index, add_op(offset));
    }

    SDLT_INLINE friend
    std::ostream& operator << (std::ostream& output_stream, const indices &a_simd_index)
    {
        output_stream << "simd::indices<SimdLaneCountT="
                      << SimdLaneCountT
                      << ",AlignD1OnIndexT="
                      << AlignD1OnIndexT
                      << ">{m_simd_struct_index="
                      << a_simd_index.m_simd_struct_index
                      << ", m_simdlane_index="
                      << a_simd_index.m_simdlane_index <<"}";
        return output_stream;
    }
protected:
    // For comparison purposes, no need for both sides to remove their simdlane_offset
    // TODO:  could probably return a long with high order bits being the struct index
    // and the lower bits being the lane index.
    // For that matter could reinterpret cast *this to get the same affect, although
    // we dislike taking the address
    SDLT_INLINE int comparison_value() const
    {
        // For now assume its cheaper to do an integer multiply and add
        // vs. a second conditional
        return m_simd_struct_index*SimdLaneCountT + m_simdlane_index;
        //return (m_simd_struct_index*SimdLaneCountT + m_simdlane_index) - simdlane_offset;
    }

    int m_simd_struct_index;
    int m_simdlane_index;
};

} // namespace simd

// Specialize calculators

template<int SimdLaneCountT, int AlignD1OnIndexT>
struct pre_offset_calculator<simd::indices<SimdLaneCountT, AlignD1OnIndexT>>
{
    static SDLT_INLINE int compute(const simd::indices<SimdLaneCountT, AlignD1OnIndexT> &a_index )
    {
        return a_index.template simd_struct_index<SimdLaneCountT>()*SimdLaneCountT;
    }
};


template<int SimdLaneCountT, int AlignD1OnIndexT>
struct varying_calculator<simd::indices<SimdLaneCountT, AlignD1OnIndexT>>
{
    static SDLT_INLINE int compute(const simd::indices<SimdLaneCountT, AlignD1OnIndexT> &a_index ) { return a_index.simdlane_index(); }
};


template<int SimdLaneCountT, int AlignD1OnIndexT>
struct post_offset_calculator<simd::indices<SimdLaneCountT, AlignD1OnIndexT>>
{
    // workaround, really T should be simd::indices<SimdLaneCountT, AlignD1OnIndexT>, since it is not a literal, the constexpr
    // would fail to build.  We don't use T at all, but the generic form here expects a
    // parameter
    template<typename T>
    static SDLT_INLINE constexpr int compute(const T ) { return -simd::indices<SimdLaneCountT, AlignD1OnIndexT>::simdlane_offset; }
};

template<int SimdLaneCountT, int AlignD1OnIndexT>
struct value_calculator<simd::indices<SimdLaneCountT,  AlignD1OnIndexT>>
{
    static SDLT_INLINE constexpr auto
    compute(const simd::indices<SimdLaneCountT,  AlignD1OnIndexT> &a_index)
    ->decltype(a_index.value())
    {
        return a_index.value();
    }
};


} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_SIMD_INDEX_VALUE_H
