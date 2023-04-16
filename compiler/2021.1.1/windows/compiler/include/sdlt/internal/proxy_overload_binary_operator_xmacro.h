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

#ifndef SDLT_UNQUALIFIED_H
#error must include internal/unqualified.h before including this header
#endif

#ifndef SDLT_RVALUE_OPERATOR_HELPER_H
#error must include internal/rvalue_operator_helper.h before including this header
#endif

#ifndef __SDLT_XMACRO_OPERATOR
#error must define __SDLT_XMACRO_OPERATOR to number before including this header
#endif

#ifndef __SDLT_XMACRO_OPERATOR_NAME
#error must define __SDLT_XMACRO_OPERATOR_NAME to number before including this header
#endif

#ifndef __SDLT_XMACRO_SELF
#error must define __SDLT_XMACRO_SELF to number before including this header
#endif

#define __SDLT_OPERATOR_HELPER_NAME(SUFFIX) __SDLT_CONCATENATE_TOKENS_INDIRECT(__SDLT_XMACRO_OPERATOR_NAME,SUFFIX)

// proxy operators are templated to allow deferred instantiation such that
// we don't get build errors if the underlying primitive does not support the operator.

template<typename OtherT>
SDLT_INLINE auto
operator __SDLT_XMACRO_OPERATOR (const OtherT &other) const
-> decltype(internal::rvalue_binary_operator_helper<__SDLT_XMACRO_SELF, internal::unqualified<OtherT> >::__SDLT_OPERATOR_HELPER_NAME(_Proxy_Other)(*this, other, 0))
{
    return internal::rvalue_binary_operator_helper<__SDLT_XMACRO_SELF, internal::unqualified<OtherT> >::__SDLT_OPERATOR_HELPER_NAME(_Proxy_Other)(*this, other, 0);
}

// Some objects have modifying operators like ostream::<< so we need to be able to pass through
// a non const or const OtherT, so we check for NonConst and pass a std::true_type or std::false_type which
// are different types so the underlying helper can use function polymorphism to specialize
// const vs. non-const versions
template<typename MaybeConstOtherT>
SDLT_INLINE friend auto
operator __SDLT_XMACRO_OPERATOR (MaybeConstOtherT &other, const __SDLT_XMACRO_SELF &a_proxy)
->decltype(internal::rvalue_binary_operator_helper<__SDLT_XMACRO_SELF, internal::unqualified<MaybeConstOtherT>>::__SDLT_OPERATOR_HELPER_NAME(_Other_Proxy)(other, a_proxy, std::is_same<typename std::remove_const<MaybeConstOtherT>::type, MaybeConstOtherT>(), 0))
{
   return internal::rvalue_binary_operator_helper<__SDLT_XMACRO_SELF, internal::unqualified<MaybeConstOtherT>>::__SDLT_OPERATOR_HELPER_NAME(_Other_Proxy)(other, a_proxy, std::is_same<typename std::remove_const<MaybeConstOtherT>::type, MaybeConstOtherT>(), 0);
}

#undef __SDLT_XMACRO_OPERATOR
#undef __SDLT_OPERATOR_HELPER_NAME
#undef __SDLT_XMACRO_OPERATOR_NAME
