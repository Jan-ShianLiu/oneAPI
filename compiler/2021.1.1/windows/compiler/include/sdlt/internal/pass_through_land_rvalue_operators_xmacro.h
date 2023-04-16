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

#ifndef __SDLT_XMACRO_SELF
#error must define __SDLT_XMACRO_SELF to number before including this header
#endif

#define __SDLT_XMACRO_NO_UNDEF_SELF 1

#include "pass_through_rvalue_operators_xmacro.h"
#if !defined(__SDLT_XMACRO_SELF) || defined(__SDLT_XMACRO_NO_UNDEF_SELF)
#error SDLT library logic bug exists
#endif

    // Try and pretend to be an lvalue instance of the value_type
    // by passing through common operators,
    // As this is an lvalue only
    // compound assignment operators:
    //    +=, -=, *=, /=, %=, >>=, <<=, &=, ^=, |=
    // or increment and decrement
    //    ++, --
    // __SDLT_XMACRO_SELF is responsible for implementing the assignment operator
    // rvalue operators are handled separately

    // Pass through compound assignment operators



#define __SDLT_XMACRO_OPERATOR +=
#include "../internal/proxy_overload_assignment_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR -=
#include "../internal/proxy_overload_assignment_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR *=
#include "../internal/proxy_overload_assignment_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR /=
#include "../internal/proxy_overload_assignment_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR %=
#include "../internal/proxy_overload_assignment_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR >>=
#include "../internal/proxy_overload_assignment_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR <<=
#include "../internal/proxy_overload_assignment_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR &=
#include "../internal/proxy_overload_assignment_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR |=
#include "../internal/proxy_overload_assignment_operator_xmacro.h"

#define __SDLT_XMACRO_OPERATOR ^=
#include "../internal/proxy_overload_assignment_operator_xmacro.h"


    // proxy operators are templated to allow deferred instantiation such that
    // we don't get build errors if the underlying primitive does not support the operator.
    // Howevever
    template<int UnusedDeferredT=0>
    SDLT_INLINE __SDLT_XMACRO_SELF &
    operator ++()
    {
        value_type value = unproxy(*this);
        ++value;
        this->operator=(value);
        return *this;
    }

    template<int UnusedDeferredT=0>
    SDLT_INLINE __SDLT_XMACRO_SELF &
    operator --()
    {
        value_type value = unproxy(*this);
        --value;
        this->operator=(value);
        return *this;
    }

    template<int UnusedDeferredT=0>
    SDLT_INLINE value_type
    operator ++(int /* Post */)
    {
       value_type value = unproxy(*this);
       value_type post_value(value);
       post_value++;
       this->operator=(post_value);
       return value;
    }

    template<int UnusedDeferredT=0>
    SDLT_INLINE value_type
    operator --(int /* Post */)
    {
        value_type value = unproxy(*this);
        value_type post_value(value);
        post_value--;
        this->operator=(post_value);
        return value;
    }

    SDLT_INLINE
    void swap(__SDLT_XMACRO_SELF &other)
    {
        value_type my_value = unproxy(*this);
        value_type other_value = unproxy(other);

        this->operator=(other_value);
        other.operator=(my_value);
    }

#undef __SDLT_XMACRO_SELF
