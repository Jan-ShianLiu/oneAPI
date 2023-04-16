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

#ifndef SDLT_COMPOUND_INDEX_H
#define SDLT_COMPOUND_INDEX_H

#include <iostream>

#include "../common.h"
#include "../aligned.h"
#include "../fixed.h"
#include "../no_offset.h"
#include "../internal/is_linear_compatible_index.h"
#include "../internal/is_simd_compatible_index.h"
#include "../internal/operation.h"
#include "../internal/post_offset_calculator.h"
#include "../internal/pre_offset_calculator.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

template<typename LoopIndexT, typename OperationT>
class compound_index
: public OperationT
{
    const LoopIndexT m_loop_index;

public:
    typedef LoopIndexT loop_index_type;
    typedef OperationT modifier_type;

    SDLT_INLINE compound_index(
        const LoopIndexT &a_loop_index,
        const OperationT &a_modifier)
    : OperationT(a_modifier)
    , m_loop_index(a_loop_index)
    {
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE compound_index(const compound_index &other)
    : OperationT(other)
    , m_loop_index(other.m_loop_index)
    {
    }

    const LoopIndexT loop_index() const { return m_loop_index; }
    const OperationT operation()  const { return *this; }

    // Simd compatible
    template<int SimdLaneCountT>
    SDLT_INLINE int simd_struct_index() const
    {
        return pre_offset_calculator<OperationT>::template compute_simd_struct<SimdLaneCountT>(operation())
            + m_loop_index.template simd_struct_index<SimdLaneCountT>()
            + post_offset_calculator<OperationT>::template compute_simd_struct<SimdLaneCountT>(operation()); }
    SDLT_INLINE int simdlane_index() const { return m_loop_index.simdlane_index(); }

    // Linear compatible
//    SDLT_INLINE auto pre_offset() const
//    ->decltype(m_loop_index.pre_offset() + pre_offset_calculator<OperationT>::compute(this->operation()))
//    {
//        return m_loop_index.pre_offset() + pre_offset_calculator<OperationT>::compute(operation());
//    }


//    SDLT_INLINE auto varying() const
//    ->decltype(std::declval<loop_index_type>().varying())
//    {
//        return m_loop_index.varying();
//    }

//    SDLT_INLINE auto post_offset() const
//    ->decltype (post_offset_calculator<OperationT>::compute(this->operation()))
//    {
//        return post_offset_calculator<OperationT>::compute(operation());
//    }

    SDLT_INLINE int value() const { return value_of(*this); }


    template<int OffsetT, typename = internal::enable_if_type<OffsetT != 0>>
    SDLT_INLINE compound_index<LoopIndexT, compound_operation<OperationT, internal::operation<arithmetic_operator::add, sequence::post_index, fixed<OffsetT> > > >
    operator + (const fixed<OffsetT> offset) const
    {
        typedef internal::operation<arithmetic_operator::add, sequence::post_index, fixed<OffsetT> > add_op;
        return compound_index<LoopIndexT, compound_operation<OperationT, add_op> > (loop_index(), compound_operation<OperationT, add_op>(operation(), add_op(offset)));
    }

    template<int OffsetT, typename = internal::enable_if_type<OffsetT != 0>>
    SDLT_INLINE compound_index<LoopIndexT, compound_operation<OperationT, internal::operation<arithmetic_operator::subtract, sequence::post_index, fixed<OffsetT> > > >
    operator - (const fixed<OffsetT> offset) const
    {
        typedef internal::operation<arithmetic_operator::subtract, sequence::post_index, fixed<OffsetT> > subtract_op;
        return compound_index<LoopIndexT, compound_operation<OperationT, subtract_op> >(loop_index(), compound_operation<OperationT, subtract_op>(operation(), subtract_op(offset)));
    }

    template<int AlignmentT>
    SDLT_INLINE compound_index<LoopIndexT, compound_operation<OperationT, internal::operation<arithmetic_operator::add, sequence::post_index, aligned<AlignmentT> > > >
    operator + (const aligned<AlignmentT> &offset) const
    {
        typedef internal::operation<arithmetic_operator::add, sequence::post_index, aligned<AlignmentT> > add_op;
        return compound_index<LoopIndexT, compound_operation<OperationT, add_op > > (loop_index(), compound_operation<OperationT, add_op>(operation(), add_op(offset)));
    }

    template<int AlignmentT>
    SDLT_INLINE compound_index<LoopIndexT, compound_operation<OperationT, internal::operation<arithmetic_operator::subtract, sequence::post_index, aligned<AlignmentT> > > >
    operator - (const aligned<AlignmentT> &offset) const
    {
        typedef internal::operation<arithmetic_operator::subtract, sequence::post_index, aligned<AlignmentT> > subtract_op;
        return compound_index<LoopIndexT, compound_operation<OperationT, subtract_op > > (loop_index(), compound_operation<OperationT, subtract_op>(operation(), subtract_op(offset)));
    }

    SDLT_INLINE compound_index<LoopIndexT, compound_operation<OperationT, internal::operation<arithmetic_operator::add, sequence::post_index, int > > >
    operator + (const int offset) const
    {
        typedef internal::operation<arithmetic_operator::add, sequence::post_index, int > add_op;
        return compound_index<LoopIndexT, compound_operation<OperationT, add_op > > (loop_index(), compound_operation<OperationT, add_op>(operation(), add_op(offset)));
    }

    SDLT_INLINE compound_index<LoopIndexT, compound_operation<OperationT, internal::operation<arithmetic_operator::subtract, sequence::post_index, int > > >
    operator - (const int offset) const
    {
        typedef internal::operation<arithmetic_operator::subtract, sequence::post_index, int > subtract_op;
        return compound_index<LoopIndexT, compound_operation<OperationT, subtract_op > > (loop_index(), compound_operation<OperationT, subtract_op>(operation(), subtract_op(offset)));
    }

    // We purposefully don't define an "operator -" because that would mean we would be iterating backwards!   IE:
    // array[99 - index]
    // So as index increases, the resulting index decreases!
    // As this is supposed to be the unit stride axis, we won't allow backward iteration, or at least do our best to avoid it.
    // We do however allow for negative offsets, but the varying index itself still moves forward just starting from an offset


#if 0
    SDLT_INLINE friend
    compound_index
    operator + (const no_offset, const compound_index &an_index)
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
    compound_index<LoopIndexT, compound_operation<internal::operation<arithmetic_operator::add, sequence::pre_index, fixed<OffsetT> >, OperationT > >
    operator + (const fixed<OffsetT> offset, const compound_index &an_index)
    {
        typedef internal::operation<arithmetic_operator::add, sequence::pre_index, fixed<OffsetT> > add_op;
        return compound_index<LoopIndexT, compound_operation<add_op, OperationT> > (an_index.loop_index(), compound_operation<add_op, OperationT>(add_op(offset), an_index.operation()));
    }

    template<int AlignmentT>
    SDLT_INLINE friend
    compound_index<LoopIndexT, compound_operation<internal::operation<arithmetic_operator::add, sequence::pre_index, aligned<AlignmentT> >, OperationT > >
    operator + (const aligned<AlignmentT> &offset, const compound_index &an_index)
    {
        typedef internal::operation<arithmetic_operator::add, sequence::pre_index, aligned<AlignmentT> > add_op;
        return compound_index<LoopIndexT, compound_operation<add_op, OperationT> > (an_index.loop_index(), compound_operation<add_op, OperationT>(add_op(offset), an_index.operation()));
    }

    SDLT_INLINE friend
    compound_index<LoopIndexT, compound_operation<internal::operation<arithmetic_operator::add, sequence::pre_index, int >, OperationT > >
    operator + (const int offset, const compound_index &an_index)
    {
        typedef internal::operation<arithmetic_operator::add, sequence::pre_index, int > add_op;
        return compound_index<LoopIndexT, compound_operation<add_op, OperationT> > (an_index.loop_index(), compound_operation<add_op, OperationT>(add_op(offset), an_index.operation()));
    }

    template<arithmetic_operator OperatorT, sequence SequenceT, typename ArgumentT>
    SDLT_INLINE friend
    compound_index<LoopIndexT, compound_operation<internal::operation<OperatorT, SequenceT, ArgumentT>, OperationT> >
    operator + (const internal::operation<OperatorT, SequenceT, ArgumentT> &an_another_operation, const compound_index &an_index)
    {
        typedef internal::operation<OperatorT, SequenceT, ArgumentT> OpType;
        return compound_index<LoopIndexT, compound_operation<OpType, OperationT> >(an_index.loop_index(), compound_operation<OpType, OperationT>(an_another_operation, an_index.operation()));
    }

    // TODO: Requires all operations define << operator
    friend std::ostream&
    operator << (std::ostream& output_stream, const compound_index &an_index)
    {
        output_stream << "compound_index{ " << an_index.loop_index()
            << ", " << an_index.operation()
            << " }";
        return output_stream;
    }
};



// Specialize calculators

template<typename LoopIndexT, typename OperationT>
struct pre_offset_calculator<compound_index<LoopIndexT, OperationT>>
{
    static SDLT_INLINE auto
    compute(const compound_index<LoopIndexT, OperationT> a_index)
    -> decltype(pre_offset_calculator<LoopIndexT>::compute(a_index.loop_index()) +
                pre_offset_calculator<OperationT>::compute(a_index.operation()))
    {
        return pre_offset_calculator<LoopIndexT>::compute(a_index.loop_index()) +
               pre_offset_calculator<OperationT>::compute(a_index.operation());
    }
};


template<typename LoopIndexT, typename OperationT>
struct varying_calculator<compound_index<LoopIndexT, OperationT>>
{
    static SDLT_INLINE auto
    compute(const compound_index<LoopIndexT, OperationT> a_index)
    -> decltype(varying_calculator<LoopIndexT>::compute(a_index.loop_index()))
    {
        return varying_calculator<LoopIndexT>::compute(a_index.loop_index());
    }
};


template<typename LoopIndexT, typename OperationT>
struct post_offset_calculator<compound_index<LoopIndexT, OperationT>>
{
    static SDLT_INLINE auto
    compute(const compound_index<LoopIndexT, OperationT> a_index)
    -> decltype(post_offset_calculator<OperationT>::compute(a_index.operation()))
    {
        return post_offset_calculator<OperationT>::compute(a_index.operation());
    }
};




} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_COMPOUND_INDEX_H


