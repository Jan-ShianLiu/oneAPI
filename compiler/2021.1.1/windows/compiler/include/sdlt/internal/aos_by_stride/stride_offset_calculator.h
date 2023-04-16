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

#ifndef SDLT_AOS_BY_STRIDE_STRIDE_OFFSET_CALCULATOR_H
#define SDLT_AOS_BY_STRIDE_STRIDE_OFFSET_CALCULATOR_H

#include <type_traits>

#include "../../common.h"
#include "../aligned_stride.h"
#include "../fixed_stride.h"
#include "../index_list.h"
#include "../int_sequence.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace aos_by_stride {


template<typename PrimitiveT, typename IntSequenceT, typename IndicesT, typename OriginT, typename StridesT>
struct stride_offset_calculator; /* undefined */

template<typename PrimitiveT, typename IndicesT, typename OriginT, typename StridesT>
struct stride_offset_calculator<PrimitiveT, int_sequence<>, IndicesT, OriginT, StridesT>
{
    static SDLT_INLINE constexpr fixed_stride<0> compute(const IndicesT &, const OriginT &, const StridesT &)
    {
        return fixed_stride<0>();
    }
};

template<typename PrimitiveT, typename IndexT, typename OriginOffsetT, typename StrideT>
SDLT_INLINE
auto compute_stride(IndexT a_index, OriginOffsetT a_origin_offset, StrideT a_stride)
->decltype(
    a_origin_offset*a_stride +
    pre_offset_of(a_index)*a_stride +
    varying_of(a_index)*static_cast<std::ptrdiff_t>(a_stride) +
    post_offset_of(a_index)*a_stride
)
{
    // At this point we should be translated back into the original index space starting at 0
    __SDLT_ASSERT(std::ptrdiff_t(a_origin_offset + pre_offset_of(a_index) + varying_of(a_index)) >= 0);

    return a_origin_offset*a_stride +
           pre_offset_of(a_index)*a_stride +
           varying_of(a_index)*static_cast<std::ptrdiff_t>(a_stride) +
           post_offset_of(a_index)*a_stride;
}


template<typename PrimitiveT, int HeadIndexT, typename IndicesT, typename OriginT, typename StridesT>
struct stride_offset_calculator<PrimitiveT, int_sequence<HeadIndexT>, IndicesT, OriginT, StridesT>
{
    static SDLT_INLINE
        auto compute(const IndicesT &a_indices, const OriginT &a_origin, const StridesT &a_strides)
        ->decltype(compute_stride<PrimitiveT>(a_indices.template get<HeadIndexT>(), a_origin.template get<HeadIndexT>(), a_strides.template get<HeadIndexT + 1>()))
    {
        auto index = a_indices.template get<HeadIndexT>();
        auto origin_offset = a_origin.template get<HeadIndexT>();
        auto stride = a_strides.template get<HeadIndexT + 1>();

        return compute_stride<PrimitiveT>(index, origin_offset, stride);
    }
};

template<typename PrimitiveT, int HeadIndexT, int... TailOfIndicesT, typename IndicesT, typename OriginT, typename StridesT>
struct stride_offset_calculator<PrimitiveT, int_sequence<HeadIndexT, TailOfIndicesT...>, IndicesT, OriginT, StridesT>
{
    static SDLT_INLINE auto compute(const IndicesT &a_indices, const OriginT &a_origin, const StridesT &a_strides)
        ->decltype(compute_stride<PrimitiveT>(a_indices.template get<HeadIndexT>(), a_origin.template get<HeadIndexT>(), a_strides.template get<HeadIndexT + 1>()) +
            stride_offset_calculator<PrimitiveT, int_sequence<TailOfIndicesT...>, IndicesT, OriginT, StridesT>::compute(a_indices, a_origin, a_strides))
    {
        auto index = a_indices.template get<HeadIndexT>();
        auto origin_offset = a_origin.template get<HeadIndexT>();
        auto stride = a_strides.template get<HeadIndexT + 1>();
        return compute_stride<PrimitiveT>(index, origin_offset, stride) +
            stride_offset_calculator<PrimitiveT, int_sequence<TailOfIndicesT...>, IndicesT, OriginT, StridesT>::compute(a_indices, a_origin, a_strides);
    }
};

template<typename PrimitiveT, typename IndicesT, typename OriginT, typename StridesT>
struct pre_stride_offset_calculator; /* undefined */

template<typename PrimitiveT, class... IndexTypeListT, typename OriginT, typename StridesT>
struct pre_stride_offset_calculator<PrimitiveT, index_list<IndexTypeListT...>, OriginT, StridesT>
{
    typedef make_int_sequence<sizeof...(IndexTypeListT)-1> int_seq_type;
    typedef index_list<IndexTypeListT...> indices_type;

    static SDLT_INLINE auto
        compute(const indices_type &a_indices, const OriginT &a_origin, const StridesT &a_strides)
        ->decltype(stride_offset_calculator<PrimitiveT, int_seq_type, indices_type, OriginT, StridesT>::compute(a_indices, a_origin, a_strides))
    {
        return stride_offset_calculator<PrimitiveT, int_seq_type, indices_type, OriginT, StridesT>::compute(a_indices, a_origin, a_strides);
    }
};


template<typename PrimitiveT, class... IndexTypeListT, typename OriginT, typename StridesT>
static SDLT_INLINE auto
    calculate_stride_offset(const index_list<IndexTypeListT...> &a_indices, const OriginT &a_origin, const StridesT &a_strides)
    ->decltype(pre_stride_offset_calculator<PrimitiveT, index_list<IndexTypeListT...>, OriginT, StridesT>::compute(a_indices, a_origin, a_strides))
{
    return pre_stride_offset_calculator<PrimitiveT, index_list<IndexTypeListT...>, OriginT, StridesT>::compute(a_indices, a_origin, a_strides);
}

} // namespace aos_by_stride
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_AOS_BY_STRIDE_STRIDE_OFFSET_CALCULATOR_H
