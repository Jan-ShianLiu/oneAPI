//
// Copyright (C) 2014-2016 Intel Corporation. All Rights Reserved.
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

#ifndef SDLT_SOA_PER_ROW_STRIDE_OFFSET_CALCULATOR_H
#define SDLT_SOA_PER_ROW_STRIDE_OFFSET_CALCULATOR_H

#include <type_traits>

#include "../../common.h"
#include "../aligned_stride.h"
#include "../fixed_stride.h"
#include "../index_list.h"
#include "../int_sequence.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace soa_per_row {

template<typename PrimitiveT, typename BuiltInT, typename IntSequenceT, typename IndicesT, typename OriginT, typename StridesT>
struct stride_offset_calculator; /* undefined */

template<typename PrimitiveT, typename BuiltInT, typename IndicesT, typename OriginT, typename StridesT>
struct stride_offset_calculator<PrimitiveT, BuiltInT, int_sequence<>, IndicesT, OriginT, StridesT>
{
    static SDLT_INLINE constexpr fixed_stride<0> compute(const IndicesT &, const OriginT &, const StridesT &)
    {
        return fixed_stride<0>();
    }
};

template<typename PrimitiveT, typename BuiltInT, typename IndexT, typename OriginOffsetT, typename StrideT,
         typename = internal::enable_if_type<(sizeof(PrimitiveT)%sizeof(BuiltInT) == 0)>>
SDLT_INLINE
auto compute_stride(IndexT a_index, OriginOffsetT a_origin_offset, StrideT a_stride)
->decltype(
                   a_origin_offset*a_stride*fixed<sizeof(PrimitiveT)/sizeof(BuiltInT)>() +
                   pre_offset_of(a_index)*a_stride*fixed<sizeof(PrimitiveT)/sizeof(BuiltInT)>() +
                   varying_of(a_index)*static_cast<std::ptrdiff_t>(a_stride*fixed<sizeof(PrimitiveT)/sizeof(BuiltInT)>()) +
                   post_offset_of(a_index)*a_stride*fixed<sizeof(PrimitiveT)/sizeof(BuiltInT)>()
)
{
    // At this point we should be translated back into the original index space starting at 0
    __SDLT_ASSERT(std::ptrdiff_t(a_origin_offset + pre_offset_of(a_index) + varying_of(a_index)) >= 0);

    // In our final address calculation, we will be multiplying by sizeof(BuiltInT),
    // having the same multiplier as accessing an array of BuiltInT (which happens for the unit stride
    // access) allows the compiler to extract uniform offset calculations and if we know those at
    // compile time, can use offset based memory addressing, allowing a single register with offset
    // memory addressing to work
    return
            a_origin_offset*a_stride*fixed<sizeof(PrimitiveT)/sizeof(BuiltInT)>() +
                       pre_offset_of(a_index)*a_stride*fixed<sizeof(PrimitiveT)/sizeof(BuiltInT)>() +
                       // NOTE:  For the loop index variable cast stride to built in type to avoid any
                       // possible type conversions to aligned which could loose the fact a loop
                       // index was involved
                       varying_of(a_index)*static_cast<std::ptrdiff_t>(a_stride*fixed<sizeof(PrimitiveT)/sizeof(BuiltInT)>()) +
                       post_offset_of(a_index)*a_stride*fixed<sizeof(PrimitiveT)/sizeof(BuiltInT)>();
}

template<typename PrimitiveT, typename BuiltInT, typename IndexT, typename OriginOffsetT, typename StrideT,
         typename = internal::enable_if_type<(sizeof(PrimitiveT)%sizeof(BuiltInT) != 0)>>
SDLT_INLINE
auto compute_stride(IndexT a_index, OriginOffsetT a_origin_offset, StrideT a_stride)
->decltype(
        a_origin_offset*a_stride*fixed<sizeof(PrimitiveT)>()/fixed<sizeof(BuiltInT)>() +
                   pre_offset_of(a_index)*a_stride*fixed<sizeof(PrimitiveT)>()/fixed<sizeof(BuiltInT)>() +
                   varying_of(a_index)*static_cast<std::ptrdiff_t>(a_stride*fixed<sizeof(PrimitiveT)>()/fixed<sizeof(BuiltInT)>()) +
                   post_offset_of(a_index)*a_stride*fixed<sizeof(PrimitiveT)>()/fixed<sizeof(BuiltInT)>()
)
{
    // At this point we should be translated back into the original index space starting at 0
    __SDLT_ASSERT((a_origin_offset + pre_offset_of(a_index) + varying_of(a_index)) >= 0);

    // In our final address calculation, we will be multiplying by sizeof(BuiltInT),
    // having the same multiplier as accessing an array of BuiltInT (which happens for the unit stride
    // access) allows the compiler to extract uniform offset calculations and if we know those at
    // compile time, can use offset based memory addressing, allowing a single register with offset
    // memory addressing to work
    return
           a_origin_offset*a_stride*fixed<sizeof(PrimitiveT)>()/fixed<sizeof(BuiltInT)>() +
           pre_offset_of(a_index)*a_stride*fixed<sizeof(PrimitiveT)>()/fixed<sizeof(BuiltInT)>() +
           // NOTE:  For the loop index variable cast stride to built in type to avoid any
           // possible type conversions to aligned which could loose the fact a loop
           // index was involved
           varying_of(a_index)*static_cast<std::ptrdiff_t>(a_stride*fixed<sizeof(PrimitiveT)>()/fixed<sizeof(BuiltInT)>()) +
           post_offset(a_index)*a_stride*fixed<sizeof(PrimitiveT)>()/fixed<sizeof(BuiltInT)>();
}

template<typename PrimitiveT, typename BuiltInT, int HeadIndexT, typename IndicesT, typename OriginT, typename StridesT>
struct stride_offset_calculator<PrimitiveT, BuiltInT, int_sequence<HeadIndexT>, IndicesT, OriginT, StridesT>
{
    static SDLT_INLINE
    auto compute(const IndicesT &a_indices, const OriginT &a_origin, const StridesT &a_strides)
        ->decltype(compute_stride<PrimitiveT, BuiltInT>(a_indices.template get<HeadIndexT>(), a_origin.template get<HeadIndexT>(), a_strides.template get<HeadIndexT + 1>()))
    {
        auto index = a_indices.template get<HeadIndexT>();
        auto origin_offset = a_origin.template get<HeadIndexT>();
        auto stride = a_strides.template get<HeadIndexT+1>();

        return compute_stride<PrimitiveT, BuiltInT>(index, origin_offset, stride);
    }
};

template<typename PrimitiveT, typename BuiltInT, int HeadIndexT, int... TailOfIndicesT, typename IndicesT, typename OriginT, typename StridesT>
struct stride_offset_calculator<PrimitiveT, BuiltInT, int_sequence<HeadIndexT, TailOfIndicesT...>, IndicesT, OriginT, StridesT>
{
    static SDLT_INLINE auto compute(const IndicesT &a_indices, const OriginT &a_origin, const StridesT &a_strides)
        ->decltype(compute_stride<PrimitiveT, BuiltInT>(a_indices.template get<HeadIndexT>(), a_origin.template get<HeadIndexT>(), a_strides.template get<HeadIndexT + 1>()) +
            stride_offset_calculator<PrimitiveT, BuiltInT, int_sequence<TailOfIndicesT...>, IndicesT, OriginT, StridesT>::compute(a_indices, a_origin, a_strides))
    {
        auto index = a_indices.template get<HeadIndexT>();
        auto origin_offset = a_origin.template get<HeadIndexT>();
        auto stride = a_strides.template get<HeadIndexT+1>();
        return compute_stride<PrimitiveT, BuiltInT>(index, origin_offset, stride) +
               stride_offset_calculator<PrimitiveT, BuiltInT, int_sequence<TailOfIndicesT...>, IndicesT, OriginT, StridesT>::compute(a_indices, a_origin, a_strides);
    }
};

template<typename PrimitiveT, typename BuiltInT, typename IndicesT, typename OriginT, typename StridesT>
struct pre_stride_offset_calculator; /* undefined */

template<typename PrimitiveT, typename BuiltInT, class... IndexTypeListT, typename OriginT, typename StridesT>
struct pre_stride_offset_calculator<PrimitiveT, BuiltInT, index_list<IndexTypeListT...>, OriginT, StridesT>
{
    typedef make_int_sequence<sizeof...(IndexTypeListT)-1> int_seq_type;
    typedef index_list<IndexTypeListT...> indices_type;

    static SDLT_INLINE auto
        compute(const indices_type &a_indices, const OriginT &a_origin, const StridesT &a_strides)
        ->decltype(stride_offset_calculator<PrimitiveT, BuiltInT, int_seq_type, indices_type, OriginT, StridesT>::compute(a_indices, a_origin, a_strides))
    {
        return stride_offset_calculator<PrimitiveT, BuiltInT, int_seq_type, indices_type, OriginT, StridesT>::compute(a_indices, a_origin, a_strides);
    }
};


template<typename PrimitiveT, typename BuiltInT, class... IndexTypeListT, typename OriginT, typename StridesT>
static SDLT_INLINE auto
calculate_stride_offset(const index_list<IndexTypeListT...> &a_indices, const OriginT &a_origin, const StridesT &a_strides)
->decltype(pre_stride_offset_calculator<PrimitiveT, BuiltInT, index_list<IndexTypeListT...>, OriginT, StridesT>::compute(a_indices, a_origin, a_strides))
{
    return pre_stride_offset_calculator<PrimitiveT, BuiltInT, index_list<IndexTypeListT...>, OriginT, StridesT>::compute(a_indices, a_origin, a_strides);
}

} // namespace soa_per_row
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_SOA_STRIDE_OFFSET_CALCULATOR_H
