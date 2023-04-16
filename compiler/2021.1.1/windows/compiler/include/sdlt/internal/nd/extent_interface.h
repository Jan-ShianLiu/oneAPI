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

#ifndef SDLT_ND_EXTENT_INTERFACE_H
#define SDLT_ND_EXTENT_INTERFACE_H

#include <type_traits>

#include "../../common.h"
#include "../../linear_index.h"
#include "../../n_extent.h"
#include "../const_bool.h"
#include "../default_offset.h"
#include "../enable_if_type.h"
#include "../is_linear_compatible_index.h"
#include "element.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace nd {


template<typename DerivedT, int DimenstionIndexT, typename LocalBoundsT>
struct extent_interface_component
{
    // Dimension specific interface
};

#define __SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(DIMENSION_INDEX) \
template<typename DerivedT, typename LocalBoundsT> \
struct extent_interface_component<DerivedT, DIMENSION_INDEX, LocalBoundsT> \
{ \
    SDLT_INLINE auto \
    extent_d##DIMENSION_INDEX() const \
    ->  decltype(bounds_d<DIMENSION_INDEX>(std::declval<LocalBoundsT>()).width()) \
    { \
        return bounds_d<DIMENSION_INDEX>(static_cast<const DerivedT &>(*this).m_local_bounds).width(); \
    } \
};

// Dimension specific interface for first 20 dimensions
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(0)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(1)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(2)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(3)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(4)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(5)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(6)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(7)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(8)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(9)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(10)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(11)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(12)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(13)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(14)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(15)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(16)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(17)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(18)
__SDLT_DECLARE_EXTENT_INTERFACE_FOR_D(19)

#undef __SDLT_DECLARE_EXTENT_INTERFACE_FOR_D

template <typename DerivedT, typename LocalBoundsT, typename IntSequenceT>
struct extent_interface_components; // undefined

template <typename DerivedT, typename... TypeListT, int... IntegerListT>
struct extent_interface_components<DerivedT, n_bounds_t<TypeListT...>, int_sequence<IntegerListT...>>
: public extent_interface_component<DerivedT, IntegerListT, n_bounds_t<TypeListT...>>...
{
protected:
    template<typename T> friend class sdlt::__SDLT_IMPL::internal::is_n_extent_interface;
    template<bool IsExtentInterfaceT, bool IsExtentProviderT, bool IsSizeCompatible, bool IsCArray, typename ObjT> friend struct sdlt::__SDLT_IMPL::internal::extent_for_d;

//    template<bool SupportsSizeT, typename T> friend struct extent_for_d<true, SupportsSizeT, T>


    template<int DimensionT>
    SDLT_INLINE auto
    extent_d() const
    ->  decltype(bounds_d<DimensionT>(std::declval<n_bounds_t<TypeListT...>>()).width())
    {
        // We choose to not disable this function because the compiler error message would say
        //     "error: no instance of function template "..."  matches the argument list
        // which could be hard to figure out.
        // Instead we provide a static assert with a meaningful message
        static_assert(n_bounds_t<TypeListT...>::rank > DimensionT, "To access the extent of dimension, DimensionT must be less than the rank of LocalBoundsT");
        return bounds_d<DimensionT>(static_cast<const DerivedT &>(*this).m_local_bounds).width();
    }

};

template <typename DerivedT, typename LocalBoundsT>
using extent_interface = extent_interface_components<DerivedT, LocalBoundsT, make_int_sequence<LocalBoundsT::rank>>; // undefined


} // namespace nd
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_ND_EXTENT_INTERFACE_H
