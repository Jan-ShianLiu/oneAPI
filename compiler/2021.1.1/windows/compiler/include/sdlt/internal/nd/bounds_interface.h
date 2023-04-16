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

#ifndef SDLT_ND_BOUNDS_INTERFACE_H
#define SDLT_ND_BOUNDS_INTERFACE_H

#include <type_traits>

#include "../../n_bounds.h"
#include "../../common.h"
#include "../../linear_index.h"
#include "../../n_extent.h"
#include "../../n_index.h"
#include "../const_bool.h"
#include "../default_offset.h"
#include "../enable_if_type.h"
#include "../is_linear_compatible_index.h"
#include "element.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace nd {


// Due to use of trailing return types, any type involved must be fully defined.
// This can create issues when wishing to use a decltype from method on the same
// class being defined.
// By moving the work and types those methods would do and produce to helper
// classes that are fully defined, those helpers can participate in trailing return
// types successfully

template<typename DerivedT, int DimenstionIndexT, typename LocalBoundsT>
struct bounds_interface_component
{
    // Dimension specific interface
};

#define __SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(DIMENSION_INDEX) \
template<typename DerivedT, typename LocalBoundsT> \
struct bounds_interface_component<DerivedT, DIMENSION_INDEX, LocalBoundsT> \
{ \
    auto bounds_d##DIMENSION_INDEX() const \
    ->  decltype(std::declval<LocalBoundsT>().template get<DIMENSION_INDEX>()) \
    { \
        return static_cast<const DerivedT &>(*this).m_local_bounds.template get<DIMENSION_INDEX>(); \
    } \
};


// Dimension specific interface for first 20 dimensions
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(0)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(1)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(2)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(3)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(4)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(5)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(6)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(7)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(8)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(9)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(10)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(11)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(12)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(13)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(14)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(15)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(16)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(17)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(18)
__SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D(19)

#undef __SDLT_DECLARE_BOUNDS_INTERFACE_FOR_D

template <typename DerivedT, typename LocalBoundsT, typename IntSequenceT>
struct bounds_interface_components; // undefined

template <typename DerivedT, typename... BoundsTypeListT, int... IntegerListT>
struct bounds_interface_components<DerivedT, n_bounds_t<BoundsTypeListT...>, int_sequence<IntegerListT...>>
: public bounds_interface_component<DerivedT, IntegerListT, n_bounds_t<BoundsTypeListT...>>...
{
protected:
    template<typename T> friend class sdlt::__SDLT_IMPL::internal::is_n_bounds_interface;
    template<bool IsBoundsInterfaceT, bool IsBoundsProviderT, bool IsSizeProviderT, bool IsArrayT, typename ObjT> friend struct sdlt::__SDLT_IMPL::internal::bounds_for_d;

    template<int DimensionT>
    auto bounds_d() const
        //->  decltype(sdlt::__SDLT_IMPL::bounds_d<DimensionT>(static_cast<const DerivedT &>(*this).m_local_bounds))
        ->decltype(std::declval<n_bounds_t<BoundsTypeListT...>>().template get<DimensionT>())
    {
        return static_cast<const DerivedT &>(*this).m_local_bounds.template get<DimensionT>();
    }
};

template <typename DerivedT, typename LocalBoundsT>
using bounds_interface = bounds_interface_components<DerivedT, LocalBoundsT, make_int_sequence<LocalBoundsT::rank>>; // undefined

// Used by both accessor and const_accessor to "help" icc 16u2 on Windows correctly deduce trailing return types
// Needed to keep someplace both classes include
template<typename LocalBoundsT, typename IndicesT>
struct translated_bounds
{
    typedef decltype(std::declval<IndicesT>() - std::declval<LocalBoundsT>().lower()) indices_minus_bounds_lower;
    typedef decltype(std::declval<LocalBoundsT>().overlay_rightmost(
                        std::declval<LocalBoundsT>() +
                        std::declval<indices_minus_bounds_lower>())) type;
};

//decltype(std::declval<OriginT>() - (std::declval<n_index_t<IndexTypeListT...>>() - std::declval<LocalBoundsT>().lower())),
template<typename LocalBoundsT, typename IndicesT, typename OriginT>
struct translated_origin
{
    typedef decltype(std::declval<IndicesT>() - std::declval<LocalBoundsT>().lower()) indices_minus_bounds_lower;
    typedef decltype(std::declval<OriginT>().overlay_rightmost(
                        std::declval<OriginT>().template rightmost_dimensions<IndicesT::rank>() -
                        std::declval<indices_minus_bounds_lower>())) type;
};


} // namespace nd
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_ND_BOUNDS_INTERFACE_H
