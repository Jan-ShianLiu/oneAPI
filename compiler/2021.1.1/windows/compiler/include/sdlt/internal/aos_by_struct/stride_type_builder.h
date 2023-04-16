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

#ifndef SDLT_AOS_BY_STRUCT_STRIDE_TYPE_BUILDER_H
#define SDLT_AOS_BY_STRUCT_STRIDE_TYPE_BUILDER_H

#include <cstddef>

#include "../../common.h"
#include "../../fixed.h"
#include "../aligned_stride.h"
#include "../fixed_stride.h"
#include "../int_sequence.h"
#include "../stride_list.h"
#include "../tuple.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace aos_by_struct {

template <typename ExtentT>
struct stride_type_for; /* undefined */

template <>
struct stride_type_for<int>
{
    typedef std::ptrdiff_t type;

    static type convert(int a_extent)
    {
        __SDLT_ASSERT(a_extent >= 0);
        return a_extent;
    }
};

template <int NumberT>
struct stride_type_for<fixed<NumberT>>
{
    typedef fixed_stride<std::ptrdiff_t(NumberT)> type;

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
        return type::from_block_count(a_extent.aligned_block_count());
    }
};


template<int ComponentT, int RowComponentT, typename ExtentsT>
struct stride_component
{
    typedef typename stride_component<ComponentT+1, RowComponentT,  ExtentsT>::type prev_stride_component;
    typedef type_d<ComponentT, ExtentsT> type_of_extent_component;
    typedef typename stride_type_for<type_of_extent_component>::type cur_stride_component;

    typedef decltype(std::declval<prev_stride_component>()*std::declval<cur_stride_component>()) type;
    
    static type compute(ExtentsT a_extents)
    {
        return stride_component<ComponentT + 1, RowComponentT, ExtentsT>::compute(a_extents)*
               stride_type_for<type_of_extent_component>::convert(a_extents.template get<ComponentT>());
    }
};

template<int ComponentT, typename ExtentsT>
struct stride_component<ComponentT, ComponentT, ExtentsT>
{
    typedef type_d<ComponentT, ExtentsT> type_of_extent_component;
    typedef typename stride_type_for<type_of_extent_component>::type type;

    static type compute(ExtentsT a_extents)
    {
        return stride_type_for<type_of_extent_component>::convert(a_extents.template get<ComponentT>());
    }
};

template<typename IntSequenceT, typename ExtentsT>
struct stride_type_sequencer; /* undefined */

template<int... IntListT, typename ExtentsT>
struct stride_type_sequencer<int_sequence<IntListT...>, ExtentsT>
{
    typedef stride_list<typename stride_component<IntListT, ExtentsT::rank-1, ExtentsT >::type...> type;

    static type build(ExtentsT a_extents)
    {
        return type(stride_component<IntListT, ExtentsT::rank - 1, ExtentsT >::compute(a_extents)...);
    }
};

} // namepace aos_by_struct
} // namepace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_AOS_BY_STRUCT_STRIDE_TYPE_BUILDER_H
