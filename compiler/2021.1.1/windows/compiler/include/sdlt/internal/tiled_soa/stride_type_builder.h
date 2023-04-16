//
// Copyright (C) 2012-2016 Intel Corporation. All Rights Reserved.
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
#ifndef SDLT_TILED_SOA_STRIDE_TYPE_BUILDER_H
#define SDLT_TILED_SOA_STRIDE_TYPE_BUILDER_H

#include <cstddef>


#include "../../common.h"
#include "../../primitive.h"
#include "../../fixed.h"
#include "../aligned_stride.h"
#include "../fixed_stride.h"
#include "../int_sequence.h"
#include "../stride_list.h"
#include "../tuple.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace tiled_soa {

template <typename ExtentT>
struct stride_type_for; /* undefined */

template <>
struct stride_type_for<int>
{
    typedef std::size_t type;

    static std::size_t convert(int a_extent)
    {
        __SDLT_ASSERT(a_extent >= 0);
        return a_extent;
    }
};

template <int NumberT>
struct stride_type_for<fixed<NumberT>>
{
    typedef fixed_stride<std::size_t(NumberT)> type;

    static type convert(fixed<NumberT> /* a_extent */)
    {
        return type();
    }
};

template <int AlignmentT>
struct stride_type_for<aligned<AlignmentT>>
{
    typedef aligned_stride<AlignmentT> type;

    static type convert(aligned<AlignmentT> a_extent)
    {
        return type(a_extent);
    }
};

// When the row extent is fixed, we need to handle aligning the row stride
// at compile time.  We use template specialization to
// identify the scenario, then use a constant expression to calculate
// the necessary offset
template <int MemberSizeT, int AlignmentT, typename T>
struct aligned_stride_type_for; /* undefined */

template <int MemberSizeT, int AlignmentT>
struct aligned_stride_type_for<MemberSizeT, AlignmentT, int>
{
    static_assert((AlignmentT%MemberSizeT) == 0, "sdlt logic bug");
    static constexpr int MinimumElementAlignment = AlignmentT/MemberSizeT;

    typedef aligned_stride<MinimumElementAlignment> type;
    static type convert(int a_row_size)
    {
        int bytes_past_boundary = (a_row_size*MemberSizeT) % AlignmentT;
        if (bytes_past_boundary == 0) {
            return type(a_row_size);
        }
        else {
            int bytes_to_next_boundary = AlignmentT - bytes_past_boundary;
            __SDLT_ASSERT(bytes_to_next_boundary%MemberSizeT == 0);
            int elements_to_next_boundary = bytes_to_next_boundary / MemberSizeT;

            return type(a_row_size + elements_to_next_boundary);
        }
    }
};

template <int MemberSizeT, int AlignmentT, int NumberT>
struct aligned_stride_type_for<MemberSizeT, AlignmentT, fixed<NumberT>>
{

    static_assert(AlignmentT%MemberSizeT == 0, "logic bug");

	static constexpr int elementsToAlignmentBoundary = 		// use ternary operator in place of an if else statement
		((NumberT*MemberSizeT) % AlignmentT == 0) ?
		0
		:
		(AlignmentT - ((NumberT*MemberSizeT) % AlignmentT)) / MemberSizeT;

	typedef fixed_stride<std::size_t(NumberT + elementsToAlignmentBoundary)> type;

    static type convert(fixed<NumberT> /* a_row_size */) { return type(); }

};


template <int MemberSizeT, int AlignmentT, int IndexAlignmentT>
struct aligned_stride_type_for<MemberSizeT, AlignmentT, aligned<IndexAlignmentT>>
{

    static_assert(AlignmentT%MemberSizeT == 0, "logic bug");

    typedef aligned_stride<IndexAlignmentT> type;
    static type convert(aligned<IndexAlignmentT> a_row_size) { return type::from_block_count(a_row_size.aligned_block_count()); }
};

template<int MemberSizeT, int ComponentT, int RowComponentT, typename ExtentsT>
struct tile_stride_component
{
    typedef typename tile_stride_component<MemberSizeT, ComponentT+1, RowComponentT,  ExtentsT>::type prev_stride_component;
    typedef type_d<ComponentT, ExtentsT> type_of_extent_component;
    typedef typename stride_type_for<type_of_extent_component>::type cur_stride_component;

    typedef decltype(std::declval<prev_stride_component>()*std::declval<cur_stride_component>()) type;
    
    static type compute(ExtentsT a_extents)
    {
        return tile_stride_component<MemberSizeT, ComponentT + 1, RowComponentT, ExtentsT>::compute(a_extents)*
               stride_type_for<type_of_extent_component>::convert(a_extents.template get<ComponentT>());
    }
};

template<int MemberSizeT, int ComponentT, typename ExtentsT>
struct tile_stride_component<MemberSizeT, ComponentT, ComponentT, ExtentsT>
{
    typedef type_d<ComponentT, ExtentsT> type_of_extent_component;
typedef typename aligned_stride_type_for<MemberSizeT, 64, type_of_extent_component>::type type;

    static type compute(ExtentsT a_extents)
    {
        return aligned_stride_type_for<MemberSizeT, 64, type_of_extent_component>::convert(a_extents.template get<ComponentT>());
    }
};

template<int MemberSizeT, typename IntSequenceT, typename ExtentsT>
struct tile_stride_type_sequencer; /* undefined */

template<int MemberSizeT, int... IntListT, typename ExtentsT>
struct tile_stride_type_sequencer<MemberSizeT, int_sequence<IntListT...>, ExtentsT>
{
    typedef stride_list<typename tile_stride_component<MemberSizeT, IntListT, ExtentsT::rank-1, ExtentsT >::type...> type;

    static type build(ExtentsT a_extents)
    {
        return type(tile_stride_component<MemberSizeT, IntListT, ExtentsT::rank - 1, ExtentsT >::compute(a_extents)...);
    }
};


template<int MemberSizeT, typename ExtentsT>
struct tile_stride_type_builder
{
    typedef make_int_sequence<ExtentsT::rank> dimension_sequence;

    typedef typename tile_stride_type_sequencer<MemberSizeT, dimension_sequence, ExtentsT>::type type;

    static type build(ExtentsT a_extents)
    {
        return tile_stride_type_sequencer<MemberSizeT, dimension_sequence, ExtentsT>::build(a_extents);
    }
};

template<typename PrimitiveT, typename RowSizeT>
auto
aligned_row_stride_for(RowSizeT a_row_size)
->decltype(aligned_stride_type_for<sizeof(typename primitive<PrimitiveT>::smallest_builtin_type), 64, RowSizeT>::convert(a_row_size))
{
    return aligned_stride_type_for<sizeof(typename primitive<PrimitiveT>::smallest_builtin_type), 64, RowSizeT>::convert(a_row_size);
}


} // namepace tiled_soa
} // namepace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_TILED_SOA_STRIDE_TYPE_BUILDER_H
#endif
