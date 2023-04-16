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

#ifndef SDLT_FIXED_H
#define SDLT_FIXED_H

#include <climits>
#include <iostream>
#include <limits>
#include <type_traits>

#include "common.h"
#include "internal/default_offset.h"
#include "internal/zero_offset.h"
#include "internal/enable_if_type.h"

namespace sdlt {
namespace __SDLT_IMPL {

// Although logically we want an integral_constant<int>, we will define our own so our overloaded operators will
// apply to our type only and namespace dependent lookups will work properly
// otherside the (int) conversion operator may take precidence 
//template <int NumberT> using fixed = std::integral_constant<int, NumberT>;

template<int ValueT>
struct fixed {
	static constexpr int value = ValueT;
	typedef int value_type;

    constexpr operator value_type() const noexcept
    { return value; }

    constexpr value_type operator()() const noexcept
    { return value; }
};


// Need overloaded operators to exist in the same namespace for namespace dependent lookups
// problem is that std::integral_constant<int, NumberT>::operator int() may be considered 
// before trying the global namespace
template<int NumberT>
SDLT_INLINE
std::ostream& operator << (std::ostream& output_stream, const sdlt::__SDLT_IMPL::fixed<NumberT> &an_index)
{
	output_stream << "fixed<" << an_index.value << ">";
	return output_stream;
}

// Unary plus
template<int NumberT>
SDLT_INLINE constexpr
fixed<NumberT> operator +(fixed<NumberT>)
{
	return fixed<+NumberT>();
}

// Unary minus
template<int NumberT>
SDLT_INLINE constexpr
fixed<-NumberT> operator -(fixed<NumberT>)
{
	return fixed<-NumberT>();
}

template<int LeftNumberT, int RightNumberT, typename = internal::enable_if_type<LeftNumberT != 0 && RightNumberT != 0>>
SDLT_INLINE constexpr
fixed<LeftNumberT - RightNumberT> operator -(const fixed<LeftNumberT> &, const fixed<RightNumberT> &)
{
	return fixed<LeftNumberT - RightNumberT>();
}

template<int LeftNumberT, int RightNumberT, typename = internal::enable_if_type<LeftNumberT != 0 && RightNumberT != 0>>
SDLT_INLINE constexpr
fixed<LeftNumberT + RightNumberT> operator +(const fixed<LeftNumberT> &, const fixed<RightNumberT> &)
{
	return fixed<LeftNumberT + RightNumberT>();
}

template<int LeftNumberT, int RightNumberT, typename = internal::enable_if_type<LeftNumberT != 0 && RightNumberT != 0>>
SDLT_INLINE constexpr
fixed<LeftNumberT * RightNumberT> operator *(fixed<LeftNumberT>, fixed<RightNumberT>)
{
    // std::numeric_limits on VS2013 isn't constexpr so we can't use it in a static_assert
    // so using old school <climits> instead
	static_assert(((long long)(LeftNumberT)* (long long)(RightNumberT)) <= (long long)(INT_MAX), "Integer overflow");
	static_assert(((long long)(LeftNumberT)* (long long)(RightNumberT)) >= (long long)(INT_MIN), "Integer underflow");
	return fixed<LeftNumberT * RightNumberT>();
}

template<int LeftNumberT, int RightNumberT>
SDLT_INLINE constexpr
fixed<LeftNumberT / RightNumberT> operator /(fixed<LeftNumberT>, fixed<RightNumberT>)
{
	return fixed<LeftNumberT / RightNumberT>();
}

// Specialize for fixed<0> to be cheap or at least not increase chained types unnecessarily,
// although still returning a copy to avoid flow dependencies
SDLT_INLINE constexpr
fixed<0> operator +(const fixed<0>, const fixed<0>)
{
    return fixed<0>();
}

template<typename OtherT>
SDLT_INLINE constexpr
OtherT operator +(const fixed<0>, const OtherT &a_other)
{
    return a_other;
}

template<typename OtherT>
SDLT_INLINE constexpr
OtherT operator +(const OtherT &a_other, const fixed<0>)
{
    return a_other;
}

SDLT_INLINE constexpr
fixed<0> operator -(const fixed<0>, const fixed<0>)
{
    return fixed<0>();
}

template<typename OtherT>
SDLT_INLINE constexpr
auto operator -(const fixed<0>, const OtherT &a_other)
-> decltype(-a_other)
{
    return -a_other;
}

template<typename OtherT>
SDLT_INLINE constexpr
OtherT operator -(const OtherT &a_other, const fixed<0>)
{
    return a_other;
}

SDLT_INLINE constexpr
fixed<0> operator *(fixed<0>, fixed<0>)
{
    return fixed<0>();
}

template<typename RightT>
SDLT_INLINE constexpr
fixed<0> operator *(fixed<0>, RightT)
{
    return fixed<0>();
}

template<typename LeftT>
SDLT_INLINE constexpr
fixed<0> operator *(LeftT, fixed<0>)
{
    return fixed<0>();
}


namespace internal {

template<>
struct zero_offset< fixed<0> >
{
    static constexpr fixed<0> value = fixed<0>();
};

template<int OffsetT>
struct default_offset< fixed<OffsetT> >
{
    static constexpr fixed<OffsetT> value = fixed<OffsetT>();
};

// Support for building a _fixed string literal
template<char... CharListT>
struct char_sequence
{
};

template<char DigitT>
static constexpr int integer_value_of()
{
    static_assert(DigitT >= '0' && DigitT <= '9', "Base 10 only");
    return DigitT - '0';
}

template<char HeadDigitT>
static constexpr int multiply_by_ten_and_add_digits(int sum, char_sequence<HeadDigitT>)
{
    return 10 * sum + integer_value_of<HeadDigitT>();
}

template<char HeadDigitT, char NextDigitT, char... TailDigitsT>
static constexpr int multiply_by_ten_and_add_digits(int sum, char_sequence<HeadDigitT, NextDigitT, TailDigitsT...>)
{
    return multiply_by_ten_and_add_digits(10 * sum + integer_value_of<HeadDigitT>(), char_sequence<NextDigitT, TailDigitsT...>());
}



template<char... DigitsT>
struct integer_from_digits
{
    static constexpr int value = multiply_by_ten_and_add_digits(0, char_sequence<DigitsT...>());
};


} // namepace internal
} // namepace __SDLT_IMPL
using __SDLT_IMPL::fixed;

namespace literals {
namespace __SDLT_IMPL {

    template<char... DigitsT>
    SDLT_INLINE constexpr
    sdlt::__SDLT_IMPL::fixed<sdlt::__SDLT_IMPL::internal::integer_from_digits<DigitsT...>::value>
    operator "" _fixed()
    {
        return sdlt::__SDLT_IMPL::fixed<sdlt::__SDLT_IMPL::internal::integer_from_digits<DigitsT...>::value>();
    }
} // namepace __SDLT_IMPL
using __SDLT_IMPL::operator "" _fixed;
} // namespace literals
using literals::operator "" _fixed;

} // namespace sdlt

#endif // SDLT_FIXED_H
