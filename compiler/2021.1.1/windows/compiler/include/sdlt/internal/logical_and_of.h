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

#ifndef SDLT_LOGICAL_AND_OF_H
#define SDLT_LOGICAL_AND_OF_H

#include <type_traits>

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

template<class... ValueListT>
struct logical_and_of;

template<>
struct logical_and_of<>
: public std::true_type
{};

template<class ValueT>
struct logical_and_of<ValueT>
: public std::integral_constant<bool, ValueT::value>
{};

template<class HeadValueT, class... TailValueListT>
struct logical_and_of<HeadValueT, TailValueListT...>
: public std::integral_constant<
      bool,
      HeadValueT::value && logical_and_of<TailValueListT...>::value
>
{};


// TODO: when possible move min_of to its own file
template<typename T, T... ValueListT>
struct min_of;

template<typename T, T ValueT>
struct min_of<T, ValueT>
{
    static constexpr T value = ValueT;
};

template<typename T, T LeftValueT, T RightValueT>
struct min_of<T, LeftValueT, RightValueT>
{
    static constexpr T value = (LeftValueT < RightValueT) ? LeftValueT : RightValueT;
};

template<typename T, T HeadValueT, T... TailValueListT>
struct min_of<T, HeadValueT, TailValueListT...>
{
    static constexpr int value = (HeadValueT < min_of<T, TailValueListT...>::value) ? HeadValueT : min_of<T, TailValueListT...>::value;
};

template<int ... ValueListT>
using min_int = min_of<int, ValueListT...>;

template<typename T, T... ValueListT>
struct max_of;

template<typename T, T ValueT>
struct max_of<T, ValueT>
{
    static constexpr T value = ValueT;
};

template<typename T, T LeftValueT, T RightValueT>
struct max_of<T, LeftValueT, RightValueT>
{
    static constexpr T value = (LeftValueT < RightValueT) ? RightValueT : LeftValueT;
};

template<typename T, T HeadValueT, T... TailValueListT>
struct max_of<T, HeadValueT, TailValueListT...>
{
    static constexpr int value = (HeadValueT < max_of<T, TailValueListT...>::value) ? max_of<T, TailValueListT...>::value : HeadValueT;
};

template<int ... ValueListT>
using max_int = max_of<int, ValueListT...>;



template<bool ValueWhenEmptyT>
SDLT_INLINE constexpr bool
calc_logical_and_of()
{
    return ValueWhenEmptyT;
}

template<bool ValueWhenEmptyT>
SDLT_INLINE bool
calc_logical_and_of(bool a_one_value)
{
    return a_one_value;
}

template<bool ValueWhenEmptyT>
SDLT_INLINE bool
calc_logical_and_of(bool a_left, bool a_right)
{
    return a_left && a_right;
}

template<bool ValueWhenEmptyT, typename ... BinaryListT>
SDLT_INLINE bool
calc_logical_and_of(bool a_left, bool a_right, BinaryListT ... a_binary_list)
{
    return calc_logical_and_of<ValueWhenEmptyT>((a_left && a_right), a_binary_list...);
}



template<bool ValueWhenEmptyT>
SDLT_INLINE constexpr bool
calc_logical_or_of()
{
    return ValueWhenEmptyT;
}

template<bool ValueWhenEmptyT>
SDLT_INLINE bool
calc_logical_or_of(bool a_one_value)
{
    return a_one_value;
}

template<bool ValueWhenEmptyT>
SDLT_INLINE bool
calc_logical_or_of(bool a_left, bool a_right)
{
    return a_left || a_right;
}

template<bool ValueWhenEmptyT, typename ... BinaryListT>
SDLT_INLINE bool
calc_logical_or_of(bool a_left, bool a_right, BinaryListT ... a_binary_list)
{
    return calc_logical_or_of<ValueWhenEmptyT>((a_left || a_right), a_binary_list...);
}

} // namepace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_LOGICAL_AND_OF_H
