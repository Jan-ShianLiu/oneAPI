//
// Copyright (C) 2015-2016 Intel Corporation. All Rights Reserved.
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

#ifndef SDLT_FIXED_STRIDE_H
#define SDLT_FIXED_STRIDE_H

#include <climits>
#include <cstddef>
#include <iostream>
#include <type_traits>

#include "../common.h"
#include "../fixed.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {


// Although logically we want an integral_constant<std::ptrdiff_t>, we will define our own so our overloaded operators will
// apply to our type only and namespace dependent lookups will work properly
// otherside the (std::ptrdiff_t) conversion operator may take precidence
//template <std::ptrdiff_t StrideT> using fixed_stride = std::integral_constant<std::ptrdiff_t, StrideT>;

template<std::ptrdiff_t ValueT>
struct fixed_stride {
    typedef std::ptrdiff_t value_type;

    static constexpr value_type value = ValueT;
    constexpr operator value_type() const noexcept
    {
        return value;
    }

    constexpr value_type operator()() const noexcept
    {
        return value;
    }
};


template<typename T>
struct is_fixed_stride
: public std::false_type
{};

template<std::ptrdiff_t FixedT>
struct is_fixed_stride<fixed_stride<FixedT>>
: public std::true_type
{};

// Need overloaded operators to exist in the same namespace for namespace dependent lookups
// problem is that std::integral_constant<std::ptrdiff_t , NumberT>::operator std::ptrdiff_t() may be considered
// before trying the global namespace
template<std::ptrdiff_t  StrideT>
SDLT_INLINE
std::ostream& operator << (std::ostream& output_stream, fixed_stride<StrideT> &a_stride)
{
    output_stream << "fixed_stride<" << a_stride.value << ">";
    return output_stream;
}

template<std::ptrdiff_t  StrideT>
SDLT_INLINE constexpr
fixed_stride<StrideT> operator +(fixed_stride<StrideT>)
{
    return fixed_stride<+StrideT>();
}

template<std::ptrdiff_t  LeftStrideT, std::ptrdiff_t  RightStrideT>
SDLT_INLINE constexpr
fixed_stride<LeftStrideT + RightStrideT> operator +(const fixed_stride<LeftStrideT> &, const fixed_stride<RightStrideT> &)
{
    return fixed_stride<LeftStrideT + RightStrideT>();
}

template<std::ptrdiff_t  LeftStrideT, std::ptrdiff_t  RightStrideT>
SDLT_INLINE constexpr
fixed_stride<LeftStrideT * RightStrideT> operator *(fixed_stride<LeftStrideT>, fixed_stride<RightStrideT>)
{
    // std::numeric_limits on VS2013 isn't constexpr so we can't use it in a static_assert
    // so using old school <climits> instead
    static_assert(((long long)(LeftStrideT)* (long long)(RightStrideT)) <= (long long)(LONG_MAX), "std::ptrdiff_t overflow");
    static_assert(((long long)(LeftStrideT)* (long long)(RightStrideT)) >= (long long)(LONG_MIN), "std::ptrdiff_t underflow");
    return fixed_stride<LeftStrideT * RightStrideT>();
}

template<int LeftNumberT, std::ptrdiff_t  RightStrideT, typename = internal::enable_if_type<LeftNumberT != 0>>
SDLT_INLINE constexpr
fixed_stride<LeftNumberT * RightStrideT> operator *(fixed<LeftNumberT>, fixed_stride<RightStrideT>)
{
    // std::numeric_limits on VS2013 isn't constexpr so we can't use it in a static_assert
    // so using old school <climits> instead
    static_assert(((long long)(LeftNumberT)* (long long)(RightStrideT)) <= (long long)(LONG_MAX), "std::ptrdiff_t overflow");
    static_assert(((long long)(LeftNumberT)* (long long)(RightStrideT)) >= (long long)(LONG_MIN), "std::ptrdiff_t underflow");
    return fixed_stride<LeftNumberT * RightStrideT>();
}

template<std::ptrdiff_t  LeftStrideT, int RightNumberT, typename = internal::enable_if_type<RightNumberT != 0>>
SDLT_INLINE constexpr
fixed_stride<LeftStrideT * RightNumberT> operator *(fixed_stride<LeftStrideT>, fixed<RightNumberT>)
{
    // std::numeric_limits on VS2013 isn't constexpr so we can't use it in a static_assert
    // so using old school <climits> instead
    static_assert(((long long)(LeftStrideT)* (long long)(RightNumberT)) <= (long long)(LONG_MAX), "std::ptrdiff_t overflow");
    static_assert(((long long)(LeftStrideT)* (long long)(RightNumberT)) >= (long long)(LONG_MIN), "std::ptrdiff_t underflow");
    return fixed_stride<LeftStrideT * RightNumberT>();
}

template<std::ptrdiff_t  LeftStrideT, std::ptrdiff_t  RightStrideT>
SDLT_INLINE constexpr
fixed_stride<LeftStrideT / RightStrideT> operator /(fixed_stride<LeftStrideT>, fixed_stride<RightStrideT>)
{
    return fixed_stride<LeftStrideT / RightStrideT>();
}


} // namepace internal
} // namepace __SDLT_IMPL
} // namespace sdlt


#endif // SDLT_FIXED_STRIDE_H
