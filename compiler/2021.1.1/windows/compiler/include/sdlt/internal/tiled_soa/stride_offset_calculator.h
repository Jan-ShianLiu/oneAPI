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

// Reserved for future use
#ifdef __SDLT_RESERVED_FOR_FUTURE_USE
#ifndef SDLT_TILED_SOA_STRIDE_OFFSET_CALCULATOR_H
#define SDLT_TILED_SOA_STRIDE_OFFSET_CALCULATOR_H

#include <type_traits>

#include "../../common.h"
#include "../aligned_stride.h"
#include "../fixed_stride.h"
#include "../index_list.h"
#include "../int_sequence.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace tiled_soa {


template<typename TileExtentsT, typename BuiltInT, typename IntSequenceT, typename IndicesT, typename OriginT, typename TileStridesT, typename InsideTileStridesT>
struct stride_offset_calculator; /* undefined */

template<typename TileExtentsT, typename BuiltInT, typename IndicesT, typename OriginT, typename TileStridesT, typename InsideTileStridesT>
struct stride_offset_calculator<BuiltInT, int_sequence<>, IndicesT, OriginT, StridesT>
{
    static SDLT_INLINE constexpr fixed_stride<0> compute(const IndicesT &, const OriginT &, const TileStridesT &, const InsideTileStridesT &)
    {
        return fixed_stride<0>();
    }
};


template<typename TileExtentT, typename BuiltInT, typename IndexT, typename OriginOffsetT, typename TileStrideT, typename InsideTileStrideT>
SDLT_INLINE
auto compute_stride(IndexT a_index, OriginOffsetT a_origin_offset, TileStrideT a_stride, InsideTileStrideT a_inside_stride)
->decltype((a_origin_offset + a_index.pre_offset() + a_index.varying() + a_index.post_offset())*(a_stride*fixed<sizeof(BuiltInT)>()))
{
    //__SDLT_ASSUME(a_stride % (64 / sizeof(BuiltInT)) == 0);
    static_assert(internal::power_of_two<int(TileExtentT)>::exists, "TileExtentT must be a power of two");


    // Note, we can't compute the varying and offset terms 
    // separately, this is because the chosen tile could be totally
    // different based on the offset.

    /*
        ((iD3.value() >> power_of_two<TileSizeZ>::exponent)*iTileCountY*rowsPerTile +
        ((iD3.value() & power_of_two<TileSizeZ>::mask_to_pass_less_than)<<power_of_two<TileSizeY>::exponent))*input_stride_d1*sizeof(BuiltInT)
    */
    // Note we need to combine the integer based offsets and index values together before
    // multiplying by an unsigned value. 
    __SDLT_ASSERT((a_origin_offset + a_index.pre_offset() + a_index.varying()) >= 0);
    // At this point we should be translated back into the original index space starting at 0,
    // so converting to unsigned should be OK
    auto tile_index = (index_value >> power_of_two<TileExtentT>::exponent);
    auto inside_tile_index = (index_value & power_of_two<TileExtentT>::mask_to_pass_less_than);
    auto index_value = a_origin_offset + a_index.pre_offset() + a_index.varying() + a_index.post_offset();
    return (tile_index*TileStrideT + inside_tile_index*InsideTileStrideT)*sizeof(BuiltInT);
}



template<typename TileExtentsT, typename BuiltInT, int HeadIndexT, typename IndicesT, typename OriginT, typename TileStridesT, typename InsideTileStridesT>
struct stride_offset_calculator<TileExtentsT, BuiltInT, int_sequence<HeadIndexT>, IndicesT, OriginT, TileStridesT, InsideTileStridesT>
{
    static SDLT_INLINE
    auto compute(const IndicesT &a_indices, const OriginT &a_origin, const TileStridesT & a_tile_strides, const InsideTileStridesT & a_inside_tile_strides)
        ->decltype(compute_stride<BuiltInT>(a_indices.template get<HeadIndexT>(), a_origin.template get<HeadIndexT>(), a_strides.template get<HeadIndexT + 1>()))
    {
        auto index = a_indices.template get<HeadIndexT>();
        auto origin_offset = a_origin.template get<HeadIndexT>();
        auto tile_stride = a_tile_strides.template get<HeadIndexT+1>();
        auto inside_tile_stride = a_inside_tile_strides.template get<HeadIndexT + 1>();
        typedef tuple_element<HeadIndexT, TileExtentsT::strides_tuple>::type tile_extent;
        return compute_stride<tile_extent, BuiltInT>(index, origin_offset, tile_stride, inside_tile_stride);
    }
};

template<typename TileExtentsT, typename BuiltInT, int HeadIndexT, int... TailOfIndicesT, typename IndicesT, typename OriginT, typename TileStridesT, typename InsideTileStridesT>
struct stride_offset_calculator<BuiltInT, int_sequence<HeadIndexT, TailOfIndicesT...>, IndicesT, OriginT, TileStridesT, InsideTileStridesT>
{
    static SDLT_INLINE auto compute(const IndicesT &a_indices, const OriginT &a_origin, const TileStridesT & a_tile_strides, const InsideTileStridesT & a_inside_tile_strides)
        ->decltype(compute_stride<BuiltInT>(a_indices.template get<HeadIndexT>(), a_origin.template get<HeadIndexT>(), a_strides.template get<HeadIndexT + 1>()) +
            stride_offset_calculator<BuiltInT, int_sequence<TailOfIndicesT...>, IndicesT, OriginT, TileStridesT, InsideTileStridesT>::compute(a_indices, a_origin, a_tile_strides, a_inside_tile_strides))
    {
        auto index = a_indices.template get<HeadIndexT>();
        auto origin_offset = a_origin.template get<HeadIndexT>();
        auto stride = a_strides.template get<HeadIndexT+1>();
        typedef tuple_element<HeadIndexT, TileExtentsT::strides_tuple>::type tile_extent;
        return compute_stride<tile_extent, BuiltInT>(index, origin_offset, stride) +
               stride_offset_calculator<TileExtentsT, BuiltInT, int_sequence<TailOfIndicesT...>, IndicesT, OriginT, TileStridesT, InsideTileStridesT>::compute(a_indices, a_origin, a_tile_strides, a_inside_tile_strides);
    }
};

template<typename TileExtentsT, typename BuiltInT, typename IndicesT, typename OriginT, typename TileStridesT, typename InsideTileStridesT>
struct pre_stride_offset_calculator; /* undefined */

template<typename TileExtentsT, typename BuiltInT, class... IndexTypeListT, typename OriginT, typename TileStridesT, typename InsideTileStridesT>
struct pre_stride_offset_calculator<TileExtentsT, BuiltInT, index_list<IndexTypeListT...>, OriginT, TileStridesT, InsideTileStridesT>
{
    typedef make_int_sequence<sizeof...(IndexTypeListT)-1> int_seq_type;
    typedef index_list<IndexTypeListT...> indices_type;

    static SDLT_INLINE auto
        compute(const indices_type &a_indices, const OriginT &a_origin, const TileStridesT & a_tile_strides, const InsideTileStridesT & a_inside_tile_strides)
        ->decltype(stride_offset_calculator<TileExtentsT, BuiltInT, int_seq_type, indices_type, OriginT, TileStridesT, InsideTileStridesT>::compute(a_indices, a_origin, a_tile_strides, a_inside_tile_strides))
    {
        return stride_offset_calculator<TileExtentsT, BuiltInT, int_seq_type, indices_type, OriginT, TileStridesT, InsideTileStridesT>::compute(a_indices, a_origin, a_tile_strides, a_inside_tile_strides);
    }
};


template<typename TileExtentsT, typename BuiltInT, class... IndexTypeListT, typename OriginT, typename TileStridesT, typename InsideTileStridesT>
static SDLT_INLINE auto
calculate_stride_offset(const index_list<IndexTypeListT...> &a_indices, const OriginT &a_origin, const TileStridesT & a_tile_strides, const InsideTileStridesT & a_inside_tile_strides)
->decltype(pre_stride_offset_calculator<TileExtentsT, BuiltInT, index_list<IndexTypeListT...>, OriginT, StridesT>::compute(a_indices, a_origin, a_tile_strides, a_inside_tile_strides))
{
    return pre_stride_offset_calculator<TileExtentsT, BuiltInT, index_list<IndexTypeListT...>, OriginT, StridesT>::compute(a_indices, a_origin, a_tile_strides, a_inside_tile_strides);
}

} // namespace tiled_soa
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_TILED_SOA_STRIDE_OFFSET_CALCULATOR_H
#endif
