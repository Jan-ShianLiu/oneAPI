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

#ifndef SDLT_OPERATION_H
#define SDLT_OPERATION_H

#include "../common.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

struct operation_none
{
};

enum class sequence : int
{
    pre_index,
    post_index
};

SDLT_INLINE std::ostream&
operator << (std::ostream& output_stream, const sequence &a_sequence)
{
    const char * stringByEnum[] = {
        "pre_index",
        "post_index"
    };
    output_stream << stringByEnum[int(a_sequence)];
    return output_stream;
}


enum class arithmetic_operator : int
{
    add,
    subtract


};

SDLT_INLINE std::ostream&
operator << (std::ostream& output_stream, const arithmetic_operator &a_arithmetic_operator)
{
    const char * stringByEnum[] = {
        "add",
        "subtract"
    };
    output_stream << stringByEnum[int(a_arithmetic_operator)];
    return output_stream;
}


template<arithmetic_operator OperatorT, sequence SequenceT, typename ArgumentT>
struct operation
{
public:
    static const sequence sequence_type = SequenceT;
    static const arithmetic_operator arithmetic_operator_type = OperatorT;

    const ArgumentT m_operand;

    SDLT_INLINE operation(const ArgumentT &an_operand)
    : m_operand(an_operand)
    {}

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE operation(const operation &other)
    : m_operand(other.m_operand)
    {}

    friend std::ostream&
    operator << (std::ostream& output_stream, const operation &a_op)
    {
        output_stream << "operation{ "
            << OperatorT
            << ", " << SequenceT
            << ", "<< a_op.m_operand
            << " }";
        return output_stream;
    }

};


// When LeftOptT and RightOptT are the same type of operation
// it causes a compilation error if a compound_operation has 2 base
// classes of the same type.
// We use a wrapper class for the Right operand of compound_operation
// So that compound_operation will never have identical base classes
template<typename LeftOpT>
struct left_operation
: protected LeftOpT
{
    SDLT_INLINE left_operation(const LeftOpT &a_left_op)
    : LeftOpT(a_left_op)
    {}

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE left_operation(const left_operation &other)
    : LeftOpT(other)
    {
    }

    const LeftOpT & op() const
    {
        return *this;
    }
};

template<typename RightOpT>
struct right_operation
: protected RightOpT
{
    //typedef RightOpT right_op_type;

    SDLT_INLINE right_operation(const RightOpT &a_right_op)
    : RightOpT(a_right_op)
    {}

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE right_operation(const right_operation &other)
    : RightOpT(other)
    {}

    const RightOpT & op() const
    {
        return *this;
    }
};

template<typename LeftOpT, typename RightOpT>
struct compound_operation
: protected left_operation<LeftOpT>
, protected right_operation<RightOpT>
{
    typedef LeftOpT left_op_type;
    typedef RightOpT right_op_type;

    SDLT_INLINE compound_operation(const left_op_type &a_left_op,
                     const right_op_type &a_right_op)
    : left_operation<left_op_type>(a_left_op)
    , right_operation<right_op_type>(a_right_op)
    {}


    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE compound_operation(const compound_operation &other)
    : left_operation<left_op_type>(other.left_op())
    , right_operation<right_op_type>(other.right_op())
    {}

    SDLT_INLINE const left_op_type  & left_op()  const { return left_operation<left_op_type>::op(); }
    SDLT_INLINE const right_op_type & right_op() const { return right_operation<right_op_type>::op(); }

    friend std::ostream&
    operator << (std::ostream& output_stream, const compound_operation &a_op)
    {
        output_stream << "compound_operation{ "
            << a_op.left_op()
            << ", " << a_op.right_op()
            << " }";
        return output_stream;
    }

};

} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_OPERATION_H
