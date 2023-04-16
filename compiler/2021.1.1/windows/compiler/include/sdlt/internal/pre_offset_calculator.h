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

#ifndef SDLT_PRE_OFFSET_CALCULATOR_H
#define SDLT_PRE_OFFSET_CALCULATOR_H

#include <stdint.h>

#include "../common.h"
#include "../aligned.h"
#include "../fixed.h"
#include "../internal/operation.h"
#include "always_false_type.h"


namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {


template<typename T>
struct pre_offset_calculator;

template<>
struct pre_offset_calculator<operation_none>
{
    static SDLT_INLINE constexpr fixed<0> compute(const operation_none ) { return fixed<0>(); }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<0> compute_simd_struct(const operation_none ) { return fixed<0>(); }
};

template<>
struct pre_offset_calculator<char>
{
    static SDLT_INLINE constexpr fixed<0> compute(const char ) { return fixed<0>(); }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<0> compute_simd_struct(const char ) { return fixed<0>(); }
};

template<>
struct pre_offset_calculator<int8_t>
{
    static SDLT_INLINE constexpr fixed<0> compute(const int8_t) { return fixed<0>(); }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<0> compute_simd_struct(const int8_t) { return fixed<0>(); }
};


template<>
struct pre_offset_calculator<uint8_t>
{
    static SDLT_INLINE constexpr fixed<0> compute(const uint8_t) { return fixed<0>(); }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<0> compute_simd_struct(const uint8_t) { return fixed<0>(); }
};


template<>
struct pre_offset_calculator<int16_t>
{
    static SDLT_INLINE constexpr fixed<0> compute(const int16_t) { return fixed<0>(); }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<0> compute_simd_struct(const int16_t) { return fixed<0>(); }
};

template<>
struct pre_offset_calculator<uint16_t>
{
    static SDLT_INLINE constexpr fixed<0> compute(const uint16_t) { return fixed<0>(); }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<0> compute_simd_struct(const uint16_t) { return fixed<0>(); }
};


template<>
struct pre_offset_calculator<int32_t>
{
    static SDLT_INLINE constexpr fixed<0> compute(const int32_t) { return fixed<0>(); }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<0> compute_simd_struct(const int32_t) { return fixed<0>(); }
};

template<>
struct pre_offset_calculator<uint32_t>
{
    static SDLT_INLINE constexpr fixed<0> compute(const uint32_t) { return fixed<0>(); }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<0> compute_simd_struct(const uint32_t) { return fixed<0>(); }
};

// define these types only for 32bit compiler, because these
// types have different meanings on 32bit and and 64bit.
// if define them for 64bit compiler these specializations will collide
// with specializations above
template<>
struct pre_offset_calculator<int64_t>
{
    static SDLT_INLINE constexpr fixed<0> compute(const int64_t) { return fixed<0>(); }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<0> compute_simd_struct(const int64_t) { return fixed<0>(); }
};

template<>
struct pre_offset_calculator<uint64_t>
{
    static SDLT_INLINE constexpr fixed<0> compute(const uint64_t) { return fixed<0>(); }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<0> compute_simd_struct(const uint64_t) { return fixed<0>(); }
};

template<>
struct pre_offset_calculator<std::conditional<std::is_same<int64_t, signed long long>::value,
                                              always_false_type<signed long long>,
                                              signed long long>::type>
{
    static SDLT_INLINE constexpr fixed<0> compute(const signed long long) { return fixed<0>(); }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<0> compute_simd_struct(const signed long long) { return fixed<0>(); }
};

template<>
struct pre_offset_calculator<std::conditional<std::is_same<uint64_t, unsigned long long>::value,
                                              always_false_type<unsigned long long>,
                                              unsigned long long>::type>
{
    static SDLT_INLINE constexpr fixed<0> compute(const unsigned long long) { return fixed<0>(); }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<0> compute_simd_struct(const unsigned long long) { return fixed<0>(); }
};

template<>
struct pre_offset_calculator<std::conditional<std::is_same<int64_t, signed long>::value ||
                                                  std::is_same<int32_t, signed long>::value,
                                              always_false_type<int32_t>,
                                              signed long>::type>
{
    static SDLT_INLINE constexpr fixed<0> compute(const signed long ) { return fixed<0>(); }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<0> compute_simd_struct(const signed long ) { return fixed<0>(); }
};

template<>
struct pre_offset_calculator<std::conditional<std::is_same<uint64_t, unsigned long>::value ||
                                                  std::is_same<uint32_t, unsigned long>::value,
                                              always_false_type<uint32_t>,
                                              unsigned long>::type>
{
    static SDLT_INLINE constexpr fixed<0> compute(const unsigned long ) { return fixed<0>(); }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<0> compute_simd_struct(const unsigned long ) { return fixed<0>(); }
};

template<int NumberT>
struct pre_offset_calculator<fixed<NumberT>>
{
    static SDLT_INLINE constexpr fixed<0> compute(const int ) { return fixed<0>(); }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<0> compute_simd_struct(const int ) { return fixed<0>(); }
};

template<int AlignmentT>
struct pre_offset_calculator<aligned<AlignmentT>>
{
    static SDLT_INLINE constexpr fixed<0> compute(const aligned<AlignmentT> ) { return fixed<0>(); }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<0> compute_simd_struct(const aligned<AlignmentT> ) { return fixed<0>(); }
};


template<arithmetic_operator OperatorT, typename ArgumentT>
struct pre_offset_calculator<operation<OperatorT, sequence::post_index, ArgumentT > >
{
    // All operations located post contribute nothing the the pre-offset
    static SDLT_INLINE constexpr fixed<0> compute(const operation<OperatorT, sequence::post_index, ArgumentT > ) { return fixed<0>(); }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<0> compute_simd_struct(const operation<OperatorT, sequence::post_index, ArgumentT > ) { return fixed<0>(); }
};

template<int OffsetT>
struct pre_offset_calculator<operation<arithmetic_operator::add, sequence::pre_index, fixed<OffsetT> > >
{
    static SDLT_INLINE constexpr fixed<OffsetT> compute(const operation<arithmetic_operator::add, sequence::pre_index, fixed<OffsetT> > an_operation)
    {
        return an_operation.m_operand;
    }

    template<int SimdLaneCountT>
    static SDLT_INLINE constexpr fixed<OffsetT/SimdLaneCountT> compute_simd_struct(const operation<arithmetic_operator::add, sequence::pre_index, fixed<OffsetT> > )
    {
        static_assert(OffsetT%SimdLaneCountT == 0, "OffsetT of fixed<OffsetT> must be a multiple of the SimdLaneCountT of the accessor");
        return fixed<OffsetT/SimdLaneCountT>();
    }
};

template<int AlignmentT>
struct pre_offset_calculator<operation<arithmetic_operator::add, sequence::pre_index, aligned<AlignmentT> > >
{
    static SDLT_INLINE int compute(const operation<arithmetic_operator::add, sequence::pre_index, aligned<AlignmentT> > an_operation)
    {
        return an_operation.m_operand.value();
    }

    template<int SimdLaneCountT>
    static SDLT_INLINE int compute_simd_struct(const operation<arithmetic_operator::add, sequence::pre_index, aligned<AlignmentT> > an_operation)
    {
        static_assert(AlignmentT%SimdLaneCountT == 0, "AlignmentT of aligned<AlignmentT> must be a multiple of the SimdLaneCountT of the accessor");
        return an_operation.m_operand.aligned_block_count();
    }
};

template<>
struct pre_offset_calculator<operation<arithmetic_operator::add, sequence::pre_index, int > >
{
    static SDLT_INLINE int compute(const operation<arithmetic_operator::add, sequence::pre_index, int > an_operation) { return an_operation.m_operand; }
};

template<typename LeftOpT, typename RightOpT>
struct pre_offset_calculator<compound_operation<LeftOpT, RightOpT> >
{
    static SDLT_INLINE auto compute(const compound_operation<LeftOpT, RightOpT> a_compound)
    ->decltype(pre_offset_calculator<LeftOpT>::compute(a_compound.left_op()) +
               pre_offset_calculator<RightOpT>::compute(a_compound.right_op()))
    {
        return pre_offset_calculator<LeftOpT>::compute(a_compound.left_op()) +
                pre_offset_calculator<RightOpT>::compute(a_compound.right_op());
    }

    template<int SimdLaneCountT>
    static SDLT_INLINE int compute_simd_struct(const compound_operation<LeftOpT, RightOpT> a_compound)
    {
        return pre_offset_calculator<LeftOpT>::template compute_simd_struct<SimdLaneCountT>(a_compound.left_op()) +
                pre_offset_calculator<RightOpT>::template compute_simd_struct<SimdLaneCountT>(a_compound.right_op());
    }
};

template<typename T>
SDLT_INLINE auto pre_offset_of(const T a_index)
-> decltype(pre_offset_calculator<T>::compute(a_index))
{
    return pre_offset_calculator<T>::compute(a_index);
}


} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_PRE_OFFSET_CALCULATOR_H
