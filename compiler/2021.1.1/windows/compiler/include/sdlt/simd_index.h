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

#ifndef SDLT_SIMD_INDEX_H
#define SDLT_SIMD_INDEX_H

#include <iostream>

#include "common.h"

#include "aligned.h"
#include "fixed.h"
#include "internal/compound_index.h"
#include "internal/operation.h"
#include "internal/simd/indices.h"
#include "internal/is_linear_compatible_index.h"
#include "internal/simd/indices.h"
#include "no_offset.h"

namespace sdlt {
namespace __SDLT_IMPL {

// Intention is for developer to have an outer loop representing
// a structure index in a ASA container, and an inner loop that
// just iterates from 0 to SimdLaneCountT.
// Those indices are passed into a simd_index that just refers
// to them.  Then the simd_index can be used with any accessor
// as well as capture +- offsets of integer, aligned<offset>,
// or fixed<offset>.  By capturing offsets they are deferred
// until the accessor expands them during address calculation.
template<int SimdLaneCountT, int AlignD1OnIndexT = 0>
class simd_index
{
public:
    static const int simdlane_count = SimdLaneCountT;

    explicit SDLT_INLINE simd_index(
        const int a_simd_struct_index,
        const int a_simdlane_index)
    : m_simd_struct_index(a_simd_struct_index)
    , m_simdlane_index(a_simdlane_index)
    {}

    // WARNING: Constructing a value_type is expensive involving
    // a divide and remainder to calculate the simd struct and
    // simd lane indices.  It is only provided for compatibility
    // and testing purposes.
    // It is intentionally obtuse and difficult to use as
    // a linear loop using value_type based indices to accessors will be slow.
    typedef internal::simd::indices<SimdLaneCountT, AlignD1OnIndexT> value_type;
    static_assert(internal::is_linear_compatible_index<value_type>::value, "SDLT internal coding error");


    explicit SDLT_INLINE simd_index(const value_type &a_value)
    : m_simd_struct_index(a_value.template simd_struct_index<SimdLaneCountT>())
    , m_simdlane_index(a_value.simdlane_index())
    {}


    // DO NOT PROVIDE A NON-DEFAULT COPY CONSTRUCTOR
    // On Windows, avoids assumed flow dependency for index values
    // when the default copy constructor is used
    SDLT_INLINE simd_index(const simd_index & other) = default;
    SDLT_INLINE simd_index & operator = (const simd_index & other) = delete;


    // Simd compatible
    template<int OtherSimdLaneCountT>
    SDLT_INLINE int simd_struct_index() const { return m_simd_struct_index; }
    SDLT_INLINE int simdlane_index() const { return m_simdlane_index; }

    // Linear compatible
//    SDLT_INLINE int pre_offset() const { return m_simd_struct_index*SimdLaneCountT; }
//    SDLT_INLINE int varying() const { return m_simdlane_index; }
//    static SDLT_INLINE constexpr int post_offset() { return - value_type::simdlane_offset; }

    SDLT_INLINE int value() const { return m_simd_struct_index*SimdLaneCountT + m_simdlane_index - value_type::simdlane_offset; }
    static_assert((internal::is_simd_compatible_index<simd_index, SimdLaneCountT>::value), "Must meet simd index requirements");

    template<int OffsetT>
    SDLT_INLINE internal::compound_index<simd_index, internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, fixed<OffsetT> > >
    operator + (const fixed<OffsetT> offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, fixed<OffsetT> > add_op;
        return internal::compound_index<simd_index, add_op > (*this, add_op(offset));
    }

    template<int OffsetT>
    SDLT_INLINE internal::compound_index<simd_index, internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, fixed<OffsetT> > >
    operator - (const fixed<OffsetT> offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, fixed<OffsetT> > subtract_op;
        return internal::compound_index<simd_index, subtract_op > (*this, subtract_op(offset));
    }

    template<int AlignmentT>
    SDLT_INLINE internal::compound_index<simd_index, internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, aligned<AlignmentT> > >
    operator + (const aligned<AlignmentT> &offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, aligned<AlignmentT> > add_op;
        return internal::compound_index<simd_index, add_op > (*this, add_op(offset));
    }

    template<int AlignmentT>
    SDLT_INLINE internal::compound_index<simd_index, internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, aligned<AlignmentT> > >
    operator - (const aligned<AlignmentT> &offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, aligned<AlignmentT> > subtract_op;
        return internal::compound_index<simd_index, subtract_op > (*this, subtract_op(offset));
    }


    SDLT_INLINE internal::compound_index<simd_index, internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, int > >
    operator + (const int offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, int > add_op;
        return internal::compound_index<simd_index, add_op > (*this, add_op(offset));
    }

    SDLT_INLINE internal::compound_index<simd_index, internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, int > >
    operator - (const int offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, int > subtract_op;
        return internal::compound_index<simd_index, subtract_op > (*this, subtract_op(offset));
    }



    // We purposefully don't define an "operator -" because that would mean we would be iterating backwards!   IE:
    // array[99 - index]
    // So as index increases, the resulting index decreases!
    // As this is supposed to be the unit stride axis, we won't allow backward iteration, or at least do our best to avoid it.
    // We do however allow for negative offsets, but the varying index itself still moves forward just starting from an offset

#if 0
    SDLT_INLINE friend
    simd_index
    operator + (const no_offset, const simd_index &an_index)
    {
        // NOTE:  Notice return type is by value versus a const &.
        // This is a conscience choice as the return type is used as a template parameter
        // and we want the parameter to just be the value type not a const &.
        // TODO:  There are other solutions to this
        return an_index;
    }
#endif

    template<int OffsetT>
    SDLT_INLINE friend
    internal::compound_index<simd_index, internal::operation<internal::arithmetic_operator::add, internal::sequence::pre_index, fixed<OffsetT> > >
    operator + (const fixed<OffsetT> offset, const simd_index &an_index)
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::pre_index, fixed<OffsetT> > add_op;
        return internal::compound_index<simd_index, add_op > (an_index, add_op(offset));
    }

    template<int AlignmentT>
    SDLT_INLINE friend
    internal::compound_index<simd_index, internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, aligned<AlignmentT> > >
    operator + (const aligned<AlignmentT> &offset, const simd_index &an_index)
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, aligned<AlignmentT> > add_op;
        return internal::compound_index<simd_index, add_op > (an_index, add_op(offset));
    }

    SDLT_INLINE friend
    internal::compound_index<simd_index, internal::operation<internal::arithmetic_operator::add, internal::sequence::pre_index, int > >
    operator + (const int offset, const simd_index &an_index)
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::pre_index, int > add_op;
        return internal::compound_index<simd_index, add_op > (an_index, add_op(offset));
    }

    SDLT_INLINE friend
    std::ostream& operator << (std::ostream& output_stream, const simd_index & a_simd_index)
    {
        output_stream << "simd_index{m_simd_struct_index=" << a_simd_index.m_simd_struct_index
                      << ", m_simdlane_index=" << a_simd_index.m_simdlane_index
                      << "}";
        return output_stream;
    }

protected:
    const int m_simd_struct_index;
    const int m_simdlane_index;
};

namespace internal {

    // Specialize calculators

    template<int SimdLaneCountT, int AlignD1OnIndexT>
    struct pre_offset_calculator<simd_index<SimdLaneCountT, AlignD1OnIndexT>>
    {
        static SDLT_INLINE int compute(const simd_index<SimdLaneCountT, AlignD1OnIndexT> &a_index ) { return a_index.template simd_struct_index<SimdLaneCountT>()*SimdLaneCountT; }
    };


    template<int SimdLaneCountT, int AlignD1OnIndexT>
    struct varying_calculator<simd_index<SimdLaneCountT, AlignD1OnIndexT>>
    {
        static SDLT_INLINE int compute(const simd_index<SimdLaneCountT, AlignD1OnIndexT> &a_index ) { return a_index.simdlane_index(); }
    };


    template<int SimdLaneCountT, int AlignD1OnIndexT>
    struct post_offset_calculator<simd_index<SimdLaneCountT, AlignD1OnIndexT>>
    {
        // workaround, really T should be simd_index<SimdLaneCountT, AlignD1OnIndexT>, since it is not a literal, the constexpr
        // would fail to build.  We don't use T at all, but the generic form here expects a
        // parameter
        template<typename T>
        static SDLT_INLINE constexpr fixed<0> compute(const T ) { return fixed<0>(); }
    };


} // internal



} // namepace __SDLT_IMPL
using __SDLT_IMPL::simd_index;
} // namespace sdlt

#endif // SDLT_SIMD_INDEX_REF_H
