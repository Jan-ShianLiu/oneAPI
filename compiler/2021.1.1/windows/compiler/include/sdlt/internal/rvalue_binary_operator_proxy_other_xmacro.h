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

#ifndef __SDLT_XMACRO_OPERATOR
#error must define __SDLT_XMACRO_OPERATOR to number before including this header
#endif

#ifndef __SDLT_XMACRO_OPERATOR_NAME
#error must define __SDLT_XMACRO_OPERATOR_NAME to number before including this header
#endif

#define __SDLT_OPERATOR_HELPER_NAME(SUFFIX) __SDLT_CONCATENATE_TOKENS_INDIRECT(__SDLT_XMACRO_OPERATOR_NAME,SUFFIX)


// If the 2nd argument will prefer to match the int version of the function
// above unless that version is not available due to SFINAE.
// Now we will emit the exact same code here, however
// the intent is to generate a compiler error with the exact
// types and operator so that the user knows which type and operator
// in their types that they need to go and override.
// Much more useful than some super nested proxy object doesn't support
// an overload.
template <typename DependentT=dependent_arg>
static SDLT_INLINE auto
__SDLT_OPERATOR_HELPER_NAME(_Proxy_Other) (const deferred_proxy<DependentT> &a_proxy, const OtherT & other, int)
-> decltype(unproxy(a_proxy) __SDLT_XMACRO_OPERATOR other)
{
    static_assert((std::is_same<ProxyT, deferred_proxy<DependentT>>::value), "Illegal call of operator template type different than ProxyT");

    return unproxy(a_proxy) __SDLT_XMACRO_OPERATOR other;
}

template <typename DependentT=dependent_arg>
static SDLT_INLINE auto
__SDLT_OPERATOR_HELPER_NAME(_Proxy_Other) (const deferred_proxy<DependentT> &a_proxy, const OtherT & other, long)
-> decltype(unproxy(a_proxy))
{
    static_assert((std::is_same<ProxyT, deferred_proxy<DependentT>>::value), "Illegal call of operator template type different than ProxyT");
    return unproxy(a_proxy) __SDLT_XMACRO_OPERATOR other;
}

template <typename DependentT=dependent_arg>
static SDLT_INLINE auto
__SDLT_OPERATOR_HELPER_NAME(_Other_Proxy) (const OtherT & other, const deferred_proxy<DependentT> & a_proxy, std::false_type /* IsNonConst */, int)
->decltype(other __SDLT_XMACRO_OPERATOR unproxy(a_proxy))
{
    static_assert((std::is_same<ProxyT, deferred_proxy<DependentT>>::value), "Illegal call of operator template type different than ProxyT");
    return other __SDLT_XMACRO_OPERATOR unproxy(a_proxy);
}

template <typename DependentT=dependent_arg>
static SDLT_INLINE auto
__SDLT_OPERATOR_HELPER_NAME(_Other_Proxy) (const OtherT & other, const deferred_proxy<DependentT> & a_proxy, std::false_type /* IsNonConst */, long)
-> decltype(unproxy(a_proxy))
{
    static_assert((std::is_same<ProxyT, deferred_proxy<DependentT>>::value), "Illegal call of operator template type different than ProxyT");
    return other __SDLT_XMACRO_OPERATOR unproxy(a_proxy);
}


template <typename DependentT=dependent_arg>
static SDLT_INLINE auto
__SDLT_OPERATOR_HELPER_NAME(_Other_Proxy) (OtherT & other, const deferred_proxy<DependentT> & a_proxy, std::true_type /* IsNonConst */, int)
->decltype(other __SDLT_XMACRO_OPERATOR unproxy(a_proxy))
{
    __SDLT_UNITIALIZED_POD_BEGIN
    static_assert((std::is_same<ProxyT, deferred_proxy<DependentT>>::value), "Illegal call of operator template type different than ProxyT");
    return other __SDLT_XMACRO_OPERATOR unproxy(a_proxy);
    __SDLT_UNITIALIZED_POD_END
}

template <typename DependentT=dependent_arg>
static SDLT_INLINE auto
__SDLT_OPERATOR_HELPER_NAME(_Other_Proxy) (OtherT & other, const deferred_proxy<DependentT> & a_proxy, std::true_type /* IsNonConst */, long)
-> decltype(unproxy(a_proxy))
{
    static_assert((std::is_same<ProxyT, deferred_proxy<DependentT>>::value), "Illegal call of operator template type different than ProxyT");
    return other __SDLT_XMACRO_OPERATOR unproxy(a_proxy);
}


#undef __SDLT_OPERATOR_HELPER_NAME
#undef __SDLT_XMACRO_OPERATOR_NAME
#undef __SDLT_XMACRO_OPERATOR
