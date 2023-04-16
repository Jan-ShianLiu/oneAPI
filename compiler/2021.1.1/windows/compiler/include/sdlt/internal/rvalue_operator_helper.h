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

#ifndef SDLT_RVALUE_OPERATOR_HELPER_H
#define SDLT_RVALUE_OPERATOR_HELPER_H

#include "../common.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

struct rvalue_unary_operator_helper
{
    // If the "long" version of an operator helper function is instantiated,
    // then the operator doesn't exist between the types.
    // Otherwise the "int" version would of been called instead of the
    // "long" version.  The "long" version requires implicit conversion of
    // an int argument and therefore is not as good of a match.

    // In the long version, we will go ahead and emit code to utilize the
    // operator with the required types which in turn should create a nice
    // compiler message
    // error: no operator ?? matches these operands
public:

#define __SDLT_XMACRO_OPERATOR *
#define __SDLT_XMACRO_OPERATOR_NAME unaryIndirection
#include "../internal/rvalue_unary_operator_helper_xmacro.h"

#define __SDLT_XMACRO_OPERATOR +
#define __SDLT_XMACRO_OPERATOR_NAME unaryPlus
#include "../internal/rvalue_unary_operator_helper_xmacro.h"

#define __SDLT_XMACRO_OPERATOR -
#define __SDLT_XMACRO_OPERATOR_NAME unaryMinus
#include "../internal/rvalue_unary_operator_helper_xmacro.h"

// Pass through rvalue logical operators
//
#define __SDLT_XMACRO_OPERATOR !
#define __SDLT_XMACRO_OPERATOR_NAME logicalNegation
#define __SDLT_XMACRO_OPTIONAL_DEFAULT_RETURN_TYPE bool
#include "../internal/rvalue_unary_operator_helper_xmacro.h"


#define __SDLT_XMACRO_OPERATOR ~
#define __SDLT_XMACRO_OPERATOR_NAME bitwiseNot
#include "../internal/rvalue_unary_operator_helper_xmacro.h"

};


template<typename ProxyT, typename OtherT>
struct rvalue_binary_operator_proxy_other
{
    // If the "long" version of an operator helper function is instantiated,
    // then the operator doesn't exist between the types.
    // Otherwise the "int" version would of been called instead of the
    // "long" version.  The "long" version requires implicit conversion of
    // an int argument and therefore is not as good of a match.

    // In the long version, we will go ahead and emit code to utilize the
    // operator with the required types which in turn should create a nice
    // compiler message
    // error: no operator ?? matches these operands
protected:
    template <typename DependentT> using deferred_proxy = typename deferred<ProxyT, DependentT>::type;
public:


#define __SDLT_XMACRO_OPERATOR ==
#define __SDLT_XMACRO_OPERATOR_NAME equalTo
#include "rvalue_binary_operator_proxy_other_xmacro.h"

#define __SDLT_XMACRO_OPERATOR !=
#define __SDLT_XMACRO_OPERATOR_NAME notEqualTo
#include "rvalue_binary_operator_proxy_other_xmacro.h"

#define __SDLT_XMACRO_OPERATOR <
#define __SDLT_XMACRO_OPERATOR_NAME lessThan
#include "rvalue_binary_operator_proxy_other_xmacro.h"

#define __SDLT_XMACRO_OPERATOR >
#define __SDLT_XMACRO_OPERATOR_NAME greaterThan
#include "rvalue_binary_operator_proxy_other_xmacro.h"

#define __SDLT_XMACRO_OPERATOR <=
#define __SDLT_XMACRO_OPERATOR_NAME lessThanOrEqualTo
#include "rvalue_binary_operator_proxy_other_xmacro.h"

#define __SDLT_XMACRO_OPERATOR >=
#define __SDLT_XMACRO_OPERATOR_NAME greaterThanOrEqualTo
#include "rvalue_binary_operator_proxy_other_xmacro.h"

#define __SDLT_XMACRO_OPERATOR +
#define __SDLT_XMACRO_OPERATOR_NAME addition
#include "rvalue_binary_operator_proxy_other_xmacro.h"

#define __SDLT_XMACRO_OPERATOR -
#define __SDLT_XMACRO_OPERATOR_NAME subtraction
#include "rvalue_binary_operator_proxy_other_xmacro.h"

#define __SDLT_XMACRO_OPERATOR *
#define __SDLT_XMACRO_OPERATOR_NAME multiplication
#include "rvalue_binary_operator_proxy_other_xmacro.h"

#define __SDLT_XMACRO_OPERATOR /
#define __SDLT_XMACRO_OPERATOR_NAME division
#include "rvalue_binary_operator_proxy_other_xmacro.h"

#define __SDLT_XMACRO_OPERATOR %
#define __SDLT_XMACRO_OPERATOR_NAME modulo
#include "rvalue_binary_operator_proxy_other_xmacro.h"

// Pass through rvalue logical operators
//

#define __SDLT_XMACRO_OPERATOR &&
#define __SDLT_XMACRO_OPERATOR_NAME logicalAnd
#include "rvalue_binary_operator_proxy_other_xmacro.h"

#define __SDLT_XMACRO_OPERATOR ||
#define __SDLT_XMACRO_OPERATOR_NAME logicialOr
#include "rvalue_binary_operator_proxy_other_xmacro.h"

#define __SDLT_XMACRO_OPERATOR &
#define __SDLT_XMACRO_OPERATOR_NAME bitwiseAnd
#include "rvalue_binary_operator_proxy_other_xmacro.h"

#define __SDLT_XMACRO_OPERATOR |
#define __SDLT_XMACRO_OPERATOR_NAME bitwiseOr
#include "rvalue_binary_operator_proxy_other_xmacro.h"

#define __SDLT_XMACRO_OPERATOR ^
#define __SDLT_XMACRO_OPERATOR_NAME bitwiseXor
#include "rvalue_binary_operator_proxy_other_xmacro.h"

#define __SDLT_XMACRO_OPERATOR <<
#define __SDLT_XMACRO_OPERATOR_NAME bitwiseLeftShift
#include "rvalue_binary_operator_proxy_other_xmacro.h"

#define __SDLT_XMACRO_OPERATOR >>
#define __SDLT_XMACRO_OPERATOR_NAME bitwiseRightShift
#include "rvalue_binary_operator_proxy_other_xmacro.h"

};


template<typename ProxyT>
struct rvalue_binary_operator_proxy_proxy
{
    // If the "long" version of an operator helper function is instantiated,
    // then the operator doesn't exist between the types.
    // Otherwise the "int" version would of been called instead of the
    // "long" version.  The "long" version requires implicit conversion of
    // an int argument and therefore is not as good of a match.

    // In the long version, we will go ahead and emit code to utilize the
    // operator with the required types which in turn should create a nice
    // compiler message
    // error: no operator ?? matches these operands
protected:
    template <typename DependentT> using deferred_proxy = typename deferred<ProxyT, DependentT>::type;
public:


#define __SDLT_XMACRO_OPERATOR ==
#define __SDLT_XMACRO_OPERATOR_NAME equalTo
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

#define __SDLT_XMACRO_OPERATOR !=
#define __SDLT_XMACRO_OPERATOR_NAME notEqualTo
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

#define __SDLT_XMACRO_OPERATOR <
#define __SDLT_XMACRO_OPERATOR_NAME lessThan
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

#define __SDLT_XMACRO_OPERATOR >
#define __SDLT_XMACRO_OPERATOR_NAME greaterThan
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

#define __SDLT_XMACRO_OPERATOR <=
#define __SDLT_XMACRO_OPERATOR_NAME lessThanOrEqualTo
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

#define __SDLT_XMACRO_OPERATOR >=
#define __SDLT_XMACRO_OPERATOR_NAME greaterThanOrEqualTo
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

#define __SDLT_XMACRO_OPERATOR +
#define __SDLT_XMACRO_OPERATOR_NAME addition
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

#define __SDLT_XMACRO_OPERATOR -
#define __SDLT_XMACRO_OPERATOR_NAME subtraction
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

#define __SDLT_XMACRO_OPERATOR *
#define __SDLT_XMACRO_OPERATOR_NAME multiplication
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

#define __SDLT_XMACRO_OPERATOR /
#define __SDLT_XMACRO_OPERATOR_NAME division
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

#define __SDLT_XMACRO_OPERATOR %
#define __SDLT_XMACRO_OPERATOR_NAME modulo
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

// Pass through rvalue logical operators
#define __SDLT_XMACRO_OPERATOR &&
#define __SDLT_XMACRO_OPERATOR_NAME logicalAnd
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

#define __SDLT_XMACRO_OPERATOR ||
#define __SDLT_XMACRO_OPERATOR_NAME logicialOr
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

#define __SDLT_XMACRO_OPERATOR &
#define __SDLT_XMACRO_OPERATOR_NAME bitwiseAnd
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

#define __SDLT_XMACRO_OPERATOR |
#define __SDLT_XMACRO_OPERATOR_NAME bitwiseOr
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

#define __SDLT_XMACRO_OPERATOR ^
#define __SDLT_XMACRO_OPERATOR_NAME bitwiseXor
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

#define __SDLT_XMACRO_OPERATOR <<
#define __SDLT_XMACRO_OPERATOR_NAME bitwiseLeftShift
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

#define __SDLT_XMACRO_OPERATOR >>
#define __SDLT_XMACRO_OPERATOR_NAME bitwiseRightShift
#include "rvalue_binary_operator_proxy_proxy_xmacro.h"

};

template<typename ProxyT, typename OtherT>
struct rvalue_binary_operator_helper
: public rvalue_binary_operator_proxy_other<ProxyT, OtherT>
{
};

template<typename ProxyT>
struct rvalue_binary_operator_helper<ProxyT, ProxyT>
: public rvalue_binary_operator_proxy_proxy<ProxyT>
{
};


} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_RVALUE_OPERATOR_HELPER_H
