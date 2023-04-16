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

#ifndef SDLT_SIMD_INDEX_REF_H
#define SDLT_SIMD_INDEX_REF_H

#include <iostream>

#include "../common.h"

#include "../aligned.h"
#include "../fixed.h"
#include "../no_offset.h"
#include "compound_index.h"
#include "is_linear_compatible_index.h"
#include "is_simd_compatible_index.h"
#include "operation.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

// NOTE: this name isn't valid as they aren't references anymore.
// This class differs from sdlt::simd_index in that it doesn't know the 
// SimdLaneCount at compile time.
// The lack of knowledge is because the kernel's don't know the 
// SimdLaneCount and can be handed to engines with different SimdLaneCounts
// We may wish to change this in the future, and just make the user call 
// an inlined function to avoid repeating kernel code,
// but life could be much easier if we knew the SimdLaneCount.
// It also has an m_linear_offset_to_simd which supposedly 
// would == a_simd_struct_index*SimdLaneCount
class simd_index_ref
{
public:
    SDLT_INLINE simd_index_ref(
        const int a_simd_struct_index,
        const int a_linear_offset_to_simd,
        const int a_simdlane_index)
    : m_simd_struct_index(a_simd_struct_index)
    , m_linear_offset_to_simd(a_linear_offset_to_simd)
    , m_simdlane_index(a_simdlane_index)
    {}

    // DO NOT PROVIDE A NON-DEFAULT COPY CONSTRUCTOR
    // On Windows, avoids assumed flow dependency for index values
    // when the default copy constructor is used
    SDLT_INLINE simd_index_ref(const simd_index_ref &other) = default;

    SDLT_INLINE simd_index_ref & operator = (const simd_index_ref & other) = delete;


    // used for trailing return type definitions
    static simd_index_ref invalid_instance()
    {
        static int dummySimdLaneIndex = 0;
        return simd_index_ref(0, 0, dummySimdLaneIndex);
    }

    // Simd compatible
    template<int OtherSimdLaneCountT>
    SDLT_INLINE int simd_struct_index() const { return m_simd_struct_index; }
    SDLT_INLINE int simd_struct_index() const { return m_simd_struct_index; }
    SDLT_INLINE int simdlane_index() const { return m_simdlane_index; }

    // Linear compatible
    //SDLT_INLINE int pre_offset() const { return m_linear_offset_to_simd; }
    //SDLT_INLINE int varying() const { return m_simdlane_index; }
    //static SDLT_INLINE constexpr int post_offset() { return 0; }

    SDLT_INLINE int value() const { return m_linear_offset_to_simd + m_simdlane_index; }

    static_assert((is_simd_compatible_index<simd_index_ref, 4>::value), "Must meet simd index requirements");
    static_assert((is_simd_compatible_index<simd_index_ref, 8>::value), "Must meet simd index requirements");

    template<int OffsetT>
    SDLT_INLINE internal::compound_index<simd_index_ref, internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, fixed<OffsetT> > >
    operator + (const fixed<OffsetT> offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, fixed<OffsetT> > add_op;
        return internal::compound_index<simd_index_ref, add_op > (*this, add_op(offset));
    }

    template<int OffsetT>
    SDLT_INLINE internal::compound_index<simd_index_ref, internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, fixed<OffsetT> > >
    operator - (const fixed<OffsetT> offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, fixed<OffsetT> > subtract_op;
        return internal::compound_index<simd_index_ref, subtract_op > (*this, subtract_op(offset));
    }

    template<int AlignmentT>
    SDLT_INLINE internal::compound_index<simd_index_ref, internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, aligned<AlignmentT> > >
    operator + (const aligned<AlignmentT> &offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, aligned<AlignmentT> > add_op;
        return internal::compound_index<simd_index_ref, add_op > (*this, add_op(offset));
    }

    template<int AlignmentT>
    SDLT_INLINE internal::compound_index<simd_index_ref, internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, aligned<AlignmentT> > >
    operator - (const aligned<AlignmentT> &offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, aligned<AlignmentT> > subtract_op;
        return internal::compound_index<simd_index_ref, subtract_op > (*this, subtract_op(offset));
    }


    SDLT_INLINE internal::compound_index<simd_index_ref, internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, int > >
    operator + (const int offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, int > add_op;
        return internal::compound_index<simd_index_ref, add_op > (*this, add_op(offset));
    }

    SDLT_INLINE internal::compound_index<simd_index_ref, internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, int > >
    operator - (const int offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, int > subtract_op;
        return internal::compound_index<simd_index_ref, subtract_op > (*this, subtract_op(offset));
    }


    // We purposefully don't define an "operator -" because that would mean we would be iterating backwards!   IE:
    // array[99 - index]
    // So as index increases, the resulting index decreases!
    // As this is supposed to be the unit stride axis, we won't allow backward iteration, or at least do our best to avoid it.
    // We do however allow for negative offsets, but the varying index itself still moves forward just starting from an offset

#if 0
    SDLT_INLINE friend
    simd_index_ref
    operator + (const no_offset, const simd_index_ref &an_index)
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
    internal::compound_index<simd_index_ref, internal::operation<internal::arithmetic_operator::add, internal::sequence::pre_index, fixed<OffsetT> > >
    operator + (const fixed<OffsetT> offset, const simd_index_ref &an_index)
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::pre_index, fixed<OffsetT> > add_op;
        return internal::compound_index<simd_index_ref, add_op > (an_index, add_op(offset));
    }

    template<int AlignmentT>
    SDLT_INLINE friend
    internal::compound_index<simd_index_ref, internal::operation<internal::arithmetic_operator::add, internal::sequence::pre_index, aligned<AlignmentT> > >
    operator + (const aligned<AlignmentT> &offset, const simd_index_ref &an_index)
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::pre_index, aligned<AlignmentT> > add_op;
        return internal::compound_index<simd_index_ref, add_op > (an_index, add_op(offset));
    }

    SDLT_INLINE friend
    internal::compound_index<simd_index_ref, internal::operation<internal::arithmetic_operator::add, internal::sequence::pre_index, int > >
    operator + (const int offset, const simd_index_ref &an_index)
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::pre_index, int > add_op;
        return internal::compound_index<simd_index_ref, add_op > (an_index, add_op(offset));
    }

    SDLT_INLINE friend
    std::ostream& operator << (std::ostream& output_stream, const simd_index_ref &a_simd_index)
    {
        output_stream << "simd_index_ref{m_simd_struct_index=" << a_simd_index.m_simd_struct_index
                      << ", m_simdlane_index=" << a_simd_index.m_simdlane_index
                      << ", m_linear_offset_to_simd=" << a_simd_index.m_linear_offset_to_simd <<"}";
        return output_stream;
    }

protected:
    const int m_simd_struct_index;
    const int m_linear_offset_to_simd;
    const int m_simdlane_index;

    template<typename> friend struct pre_offset_calculator;
    template<typename> friend struct varying_calculator;
};


// Specialize calculators

template<>
struct pre_offset_calculator<simd_index_ref>
{
    static SDLT_INLINE int compute(const simd_index_ref a_index ) { return a_index.m_linear_offset_to_simd; }
};


template<>
struct varying_calculator<simd_index_ref>
{
    static SDLT_INLINE int compute(const simd_index_ref a_index ) { return a_index.m_simdlane_index; }
};


template<>
struct post_offset_calculator<simd_index_ref>
{
    // workaround, really T should be simd_index_ref, since it is not a literal, the constexpr
    // would fail to build.  We don't use T at all, but the generic form here expects a
    // parameter
    template<typename T>
    static SDLT_INLINE constexpr fixed<0> compute(const T ) { return fixed<0>(); }
};



} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_SIMD_INDEX_REF_H
