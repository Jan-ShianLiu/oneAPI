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

#ifndef SDLT_LINEAR_INDEX_H
#define SDLT_LINEAR_INDEX_H

#include <iostream>

#include "common.h"

#include "aligned.h"
#include "fixed.h"
// TODO: Need to have different include order here until all dependencies have been fixed up
#include "internal/post_offset_calculator.h"
#include "internal/compound_index.h"
#include "internal/operation.h"
#include "no_offset.h"


namespace sdlt {
namespace __SDLT_IMPL {

class linear_index
{
public:
    SDLT_INLINE explicit linear_index(int an_index)
    : m_index(an_index)
    {
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE linear_index(const linear_index &other)
        : m_index(other.m_index)
    {
    }

    SDLT_INLINE linear_index & operator = (const linear_index & other) = delete;

    SDLT_INLINE bool operator == (const linear_index & other) const
    {
        return m_index == other.m_index;
    }
    SDLT_INLINE bool operator != (const linear_index & other) const
    {
        return m_index != other.m_index;
    }

    SDLT_INLINE linear_index & operator ++()
    {
        ++m_index;
        return *this;
    }

    // used for trailing return type definitions
    static linear_index invalid_instance()
    {
        static int dummy_index = 0;
        return linear_index(dummy_index);
    }

    //static SDLT_INLINE constexpr int pre_offset() { return 0; }
    //static SDLT_INLINE constexpr int simd_struct_index() { return 0; }

    //SDLT_INLINE int varying() const { return m_index; }
    //static SDLT_INLINE constexpr int post_offset() { return 0; }

    SDLT_INLINE int value() const { return m_index; }

    template<int OffsetT, typename = internal::enable_if_type<OffsetT != 0>>
    SDLT_INLINE internal::compound_index<linear_index, internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, fixed<OffsetT> > >
    operator + (const fixed<OffsetT> offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, fixed<OffsetT> > add_op;
        return internal::compound_index<linear_index, add_op > (*this, add_op(offset));
    }

    template<int OffsetT, typename = internal::enable_if_type<OffsetT != 0>>
    SDLT_INLINE internal::compound_index<linear_index, internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, fixed<OffsetT> > >
    operator - (const fixed<OffsetT> offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, fixed<OffsetT> > subtract_op;
        return internal::compound_index<linear_index, subtract_op > (*this, subtract_op(offset));
    }

    template<int AlignmentT>
    SDLT_INLINE internal::compound_index<linear_index, internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, aligned<AlignmentT> > >
    operator + (const aligned<AlignmentT> offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, aligned<AlignmentT> > add_op;
        return internal::compound_index<linear_index, add_op > (*this, add_op(offset));
    }

    template<int AlignmentT>
    SDLT_INLINE internal::compound_index<linear_index, internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, aligned<AlignmentT> > >
    operator - (const aligned<AlignmentT> offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, aligned<AlignmentT> > subtract_op;
        return internal::compound_index<linear_index, subtract_op > (*this, subtract_op(offset));
    }


    SDLT_INLINE internal::compound_index<linear_index, internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, int > >
    operator + (const int offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::post_index, int > add_op;
        return internal::compound_index<linear_index, add_op > (*this, add_op(offset));
    }

    SDLT_INLINE internal::compound_index<linear_index, internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, int > >
    operator - (const int offset) const
    {
        typedef internal::operation<internal::arithmetic_operator::subtract, internal::sequence::post_index, int > subtract_op;
        return internal::compound_index<linear_index, subtract_op > (*this, subtract_op(offset));
    }


    // We purposefully don't define an "operator -" because that would mean we would be iterating backwards!   IE:
    // array[99 - index]
    // So as index increases, the resulting index decreases!
    // As this is supposed to be the unit stride axis, we won't allow backward iteration, or at least do our best to avoid it.
    // We do however allow for negative offsets, but the varying index itself still moves forward just starting from an offset

#if 0
    SDLT_INLINE friend
    linear_index
    operator + (const no_offset , const linear_index an_index)
    {
        // NOTE:  Notice return type is by value versus a const &.
        // This is a conscience choice as the return type is used as a template parameter
        // and we want the parameter to just be the value type not a const &.
        // TODO:  There are other solutions to this
        return an_index;
    }
#endif


    template<int OffsetT, typename = internal::enable_if_type<OffsetT != 0>>
    SDLT_INLINE friend
    internal::compound_index<linear_index, internal::operation<internal::arithmetic_operator::add, internal::sequence::pre_index, fixed<OffsetT> > >
    operator + (const fixed<OffsetT> offset, const linear_index an_index)
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::pre_index, fixed<OffsetT> > add_op;
        return internal::compound_index<linear_index, add_op > (an_index, add_op(offset));
    }

    template<int AlignmentT>
    SDLT_INLINE  friend
    internal::compound_index<linear_index, internal::operation<internal::arithmetic_operator::add, internal::sequence::pre_index, aligned<AlignmentT> > >
    operator + (const aligned<AlignmentT> offset, const linear_index an_index)
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::pre_index, aligned<AlignmentT> > add_op;
        return internal::compound_index<linear_index, add_op > (an_index, add_op(offset));
    }

    SDLT_INLINE  friend
    internal::compound_index<linear_index, internal::operation<internal::arithmetic_operator::add, internal::sequence::pre_index, int > >
    operator + (const int offset, const linear_index an_index)
    {
        typedef internal::operation<internal::arithmetic_operator::add, internal::sequence::pre_index, int > add_op;
        return internal::compound_index<linear_index, add_op > (an_index, add_op(offset));
    }

    SDLT_INLINE  friend
    std::ostream& operator << (std::ostream& output_stream, const linear_index &a_linear_index)
    {
        output_stream << "linear_index{" << a_linear_index.m_index <<"}";
        return output_stream;
    }

protected:
    int m_index;
};


namespace internal {

    // Specialize calculators

    template<>
    struct pre_offset_calculator<linear_index>
    {
        // workaround, really T should be linear_index, since it is not a literal, the constexpr
        // would fail to build.  We don't use T at all, but the generic form here expects a
        // parameter
        template<typename T>
        static SDLT_INLINE constexpr fixed<0> compute(const T ) { return fixed<0>(); }
    };


    template<>
    struct varying_calculator<linear_index>
    {
        static SDLT_INLINE int compute(const linear_index a_index ) { return a_index.value(); }
    };


    template<>
    struct post_offset_calculator<linear_index>
    {
        // workaround, really T should be linear_index, since it is not a literal, the constexpr
        // would fail to build.  We don't use T at all, but the generic form here expects a
        // parameter
        template<typename T>
        static SDLT_INLINE constexpr fixed<0> compute(const T ) { return fixed<0>(); }
    };


} // internal


} // namepace __SDLT_IMPL
using __SDLT_IMPL::linear_index;
} // namespace sdlt

#endif // SDLT_LINEAR_INDEX_H
