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

#ifndef SDLT_DEFERRED_H
#error must include deferred.h before including this header
#endif

#ifndef SDLT_RVALUE_OPERATOR_HELPER_H
#error must include rvalue_operator_helper.h before including this header
#endif

#ifndef __SDLT_XMACRO_SELF
#error must define __SDLT_XMACRO_SELF to number before including this header
#endif

    // Try and pretend to be an rvalue instance of the PrimitiveT
    // by passing through common operators,
    // As this is an rvalue, no assignment, compound assignment operators,
    // or increment and decrement

    // NOTE: const value_type type prevents rvalue assignment for structs,
    // better protection that nothing
    SDLT_INLINE friend
    value_type const unproxy(const __SDLT_XMACRO_SELF &self) {
        return self.operator value_type const ();
    }

    // Pass through rvalue relational operators
#define __SDLT_XMACRO_OPERATOR ==
#define __SDLT_XMACRO_OPERATOR_NAME equalTo
#include "../internal/proxy_overload_binary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR !=
#define __SDLT_XMACRO_OPERATOR_NAME notEqualTo
#include "../internal/proxy_overload_binary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR <
#define __SDLT_XMACRO_OPERATOR_NAME lessThan
#include "../internal/proxy_overload_binary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR >
#define __SDLT_XMACRO_OPERATOR_NAME greaterThan
#include "../internal/proxy_overload_binary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR <=
#define __SDLT_XMACRO_OPERATOR_NAME lessThanOrEqualTo
#include "../internal/proxy_overload_binary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR >=
#define __SDLT_XMACRO_OPERATOR_NAME greaterThanOrEqualTo
#include "../internal/proxy_overload_binary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR *
#define __SDLT_XMACRO_OPERATOR_NAME unaryIndirection
#include "../internal/proxy_overload_unary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR +
#define __SDLT_XMACRO_OPERATOR_NAME unaryPlus
#include "../internal/proxy_overload_unary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR -
#define __SDLT_XMACRO_OPERATOR_NAME unaryMinus
#include "../internal/proxy_overload_unary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR +
#define __SDLT_XMACRO_OPERATOR_NAME addition
#include "../internal/proxy_overload_binary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR -
#define __SDLT_XMACRO_OPERATOR_NAME subtraction
#include "../internal/proxy_overload_binary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR *
#define __SDLT_XMACRO_OPERATOR_NAME multiplication
#include "../internal/proxy_overload_binary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR /
#define __SDLT_XMACRO_OPERATOR_NAME division
#include "../internal/proxy_overload_binary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR %
#define __SDLT_XMACRO_OPERATOR_NAME modulo
#include "../internal/proxy_overload_binary_operator_xmacro.h"

// Pass through rvalue logical operators
#define __SDLT_XMACRO_OPERATOR !
#define __SDLT_XMACRO_OPERATOR_NAME logicalNegation
#include "../internal/proxy_overload_unary_operator_xmacro.h"

// Providing overrides for && and || will cause a behavior difference
// from standard C++ where evaluation of the right operand can be skipped
// But as the performance critical portion SDLT will be vectorized loop
// where typically both sides of a conditional are executed anyway
// we feel its a worth tradeoff
__SDLT_ALWAYS_EVALUATE_BOTH_ARGUMENTS_BEGIN

#define __SDLT_XMACRO_OPERATOR &&
#define __SDLT_XMACRO_OPERATOR_NAME logicalAnd
#include "../internal/proxy_overload_binary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR ||
#define __SDLT_XMACRO_OPERATOR_NAME logicialOr
#include "../internal/proxy_overload_binary_operator_xmacro.h"

__SDLT_ALWAYS_EVALUATE_BOTH_ARGUMENTS_END

#define __SDLT_XMACRO_OPERATOR &
#define __SDLT_XMACRO_OPERATOR_NAME bitwiseAnd
#include "../internal/proxy_overload_binary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR |
#define __SDLT_XMACRO_OPERATOR_NAME bitwiseOr
#include "../internal/proxy_overload_binary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR ^
#define __SDLT_XMACRO_OPERATOR_NAME bitwiseXor
#include "../internal/proxy_overload_binary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR ~
#define __SDLT_XMACRO_OPERATOR_NAME bitwiseNot
#include "../internal/proxy_overload_unary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR <<
#define __SDLT_XMACRO_OPERATOR_NAME bitwiseLeftShift
#include "../internal/proxy_overload_binary_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR >>
#define __SDLT_XMACRO_OPERATOR_NAME bitwiseRightShift
#include "../internal/proxy_overload_binary_operator_xmacro.h"

#ifndef __SDLT_XMACRO_NO_UNDEF_SELF
    #undef __SDLT_XMACRO_SELF
#else
    #undef __SDLT_XMACRO_NO_UNDEF_SELF
#endif
