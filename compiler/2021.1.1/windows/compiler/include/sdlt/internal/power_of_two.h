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

#ifndef SDLT_POWER_OF_TWO_H
#define SDLT_POWER_OF_TWO_H

#include "../common.h"
#include "const_bool.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

template <int iNumber>
struct power_of_two
{
    static const bool exists = false; \
};

#define __SDLT_DEFINE_POWER_OF_TWO(POWER_OF_TWO) \
template <> \
struct power_of_two< (1<<POWER_OF_TWO) > \
{ \
    static const bool exists = true; \
    static const int exponent = POWER_OF_TWO; \
    static const int mask_to_pass_less_than = (1u<<POWER_OF_TWO)-1;\
};

__SDLT_DEFINE_POWER_OF_TWO(0)
__SDLT_DEFINE_POWER_OF_TWO(1)
__SDLT_DEFINE_POWER_OF_TWO(2)
__SDLT_DEFINE_POWER_OF_TWO(3)
__SDLT_DEFINE_POWER_OF_TWO(4)
__SDLT_DEFINE_POWER_OF_TWO(5)
__SDLT_DEFINE_POWER_OF_TWO(6)
__SDLT_DEFINE_POWER_OF_TWO(7)
__SDLT_DEFINE_POWER_OF_TWO(8)
__SDLT_DEFINE_POWER_OF_TWO(9)
__SDLT_DEFINE_POWER_OF_TWO(10)
__SDLT_DEFINE_POWER_OF_TWO(11)
__SDLT_DEFINE_POWER_OF_TWO(12)
__SDLT_DEFINE_POWER_OF_TWO(13)
__SDLT_DEFINE_POWER_OF_TWO(14)
__SDLT_DEFINE_POWER_OF_TWO(15)
__SDLT_DEFINE_POWER_OF_TWO(16)
__SDLT_DEFINE_POWER_OF_TWO(17)
__SDLT_DEFINE_POWER_OF_TWO(18)
__SDLT_DEFINE_POWER_OF_TWO(19)
__SDLT_DEFINE_POWER_OF_TWO(20)
__SDLT_DEFINE_POWER_OF_TWO(21)
__SDLT_DEFINE_POWER_OF_TWO(22)
__SDLT_DEFINE_POWER_OF_TWO(23)
__SDLT_DEFINE_POWER_OF_TWO(24)
__SDLT_DEFINE_POWER_OF_TWO(25)
__SDLT_DEFINE_POWER_OF_TWO(26)
__SDLT_DEFINE_POWER_OF_TWO(27)
__SDLT_DEFINE_POWER_OF_TWO(28)
__SDLT_DEFINE_POWER_OF_TWO(29)
__SDLT_DEFINE_POWER_OF_TWO(30)
#undef __SDLT_DEFINE_POWER_OF_TWO


template <int NumberT>
using is_power_of_two = const_bool<power_of_two<NumberT>::exists>;


template<int NumberT, int PowerOfTwoValueT = 1, bool TerminateWhenT = (PowerOfTwoValueT >= NumberT)>
struct power_of_two_ceil_impl
{
    static_assert(NumberT > 0, "power_of_two_ceil_impl is only defined for NumberT > 0");
    static_assert(is_power_of_two<PowerOfTwoValueT>::value, "PowerOfTwoValueT must be a power of two");

    static constexpr int value = power_of_two_ceil_impl<NumberT, (2 * PowerOfTwoValueT)>::value;
};
template<int NumberT, int PowerOfTwoValueT>
struct power_of_two_ceil_impl<NumberT, PowerOfTwoValueT, true /* TerminateWhenT */>
{
    static_assert(is_power_of_two<PowerOfTwoValueT>::value, "PowerOfTwoValueT must be a power of two");

    static constexpr int value = PowerOfTwoValueT;
};


template<int NumberT>
struct power_of_two_ceil
{
    static constexpr int value = power_of_two_ceil_impl<NumberT>::value;
};

__SDLT_STATIC_ASSERT((power_of_two_ceil<1>::value == 1), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_ceil<2>::value == 2), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_ceil<3>::value == 4), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_ceil<4>::value == 4), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_ceil<5>::value == 8), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_ceil<7>::value == 8), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_ceil<8>::value == 8), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_ceil<9>::value == 16), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_ceil<15>::value == 16), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_ceil<16>::value == 16), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_ceil<17>::value == 32), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_ceil<31>::value == 32), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_ceil<32>::value == 32), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_ceil<33>::value == 64), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_ceil<63>::value == 64), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_ceil<64>::value == 64), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_ceil<65>::value == 128), "logic bug");

template<int NumberT, int PowerOfTwoValueT = 1, bool TerminateWhenT = (PowerOfTwoValueT > NumberT)>
struct power_of_two_floor_impl
{
    static_assert(NumberT > 0, "power_of_two_floor is only defined for NumberT > 0");
    static_assert(internal::is_power_of_two<PowerOfTwoValueT>::value, "PowerOfTwoValueT must be a power of two");

    static constexpr int value = power_of_two_floor_impl<NumberT, (2 * PowerOfTwoValueT)>::value;
};
template<int NumberT, int PowerOfTwoValueT>
struct power_of_two_floor_impl<NumberT, PowerOfTwoValueT, true /* TerminateWhenT */>
{
    static_assert(internal::is_power_of_two<PowerOfTwoValueT>::value, "PowerOfTwoValueT must be a power of two");

    static constexpr int value = PowerOfTwoValueT / 2;
};

template<int NumberT>
struct power_of_two_floor
{
    static constexpr int value = power_of_two_floor_impl<NumberT>::value;
};
__SDLT_STATIC_ASSERT((power_of_two_floor<1>::value == 1), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_floor<2>::value == 2), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_floor<3>::value == 2), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_floor<4>::value == 4), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_floor<5>::value == 4), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_floor<7>::value == 4), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_floor<8>::value == 8), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_floor<9>::value == 8), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_floor<15>::value == 8), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_floor<16>::value == 16), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_floor<17>::value == 16), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_floor<31>::value == 16), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_floor<32>::value == 32), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_floor<33>::value == 32), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_floor<63>::value == 32), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_floor<64>::value == 64), "logic bug");
__SDLT_STATIC_ASSERT((power_of_two_floor<65>::value == 64), "logic bug");


} // namepace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_POWER_OF_TWO_H
