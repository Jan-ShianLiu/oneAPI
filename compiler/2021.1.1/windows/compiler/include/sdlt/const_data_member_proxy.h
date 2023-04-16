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

#ifndef SDLT_CONST_DATA_MEMBER_PROXY_H
#define SDLT_CONST_DATA_MEMBER_PROXY_H


#include "common.h"
#include "data_member_proxy.h"
#include "internal/check_for_primitive_declaration.h"
#include "internal/enable_if_type.h"
#include "internal/scalar/root_access.h"
#include "internal/uniform/const_scalar_member.h"
#include "internal/unqualified.h"
#include "primitive.h"

namespace sdlt {
namespace __SDLT_IMPL {

__SDLT_INHERIT_IMPL_BEGIN

template<typename PrimitiveT>
struct const_data_member_proxy
: public sdlt::primitive<PrimitiveT>::template const_scalar_uniform_interface<
    const_data_member_proxy<PrimitiveT>,
    PrimitiveT,
    typename internal::scalar::root_access<PrimitiveT>::type,
    internal::uniform::template const_scalar_member>
, private internal::check_for_primitive_declaration<PrimitiveT>
{
public:
    typedef PrimitiveT value_type;

protected:
    typedef typename internal::scalar::template root_access<PrimitiveT>::type root_access_type;
public:

    const_data_member_proxy(const value_type &a_data)
    : m_data(a_data)
    {}

    const_data_member_proxy(const data_member_proxy<PrimitiveT> &a_proxy)
    : m_data(const_cast<const PrimitiveT &>(a_proxy.m_data))
    {}


    __SDLT_INHERIT_IMPL_DESTRUCTOR
    ~const_data_member_proxy() = default;

    SDLT_INLINE
    operator value_type const () const {
        return m_data;
    }

    // By including pass_through_rvalue_operators_xmacro,
    // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
    #define __SDLT_XMACRO_SELF const_data_member_proxy
    #include "internal/pass_through_rvalue_operators_xmacro.h"

private:

    template<
        typename RootScalarDataT,
        typename MemberAccessT,
        typename MemberDataT
    > friend class internal::uniform::const_scalar_member;

    const value_type & root_scalar_data() const { return m_data; }
    const value_type & m_data;
};
__SDLT_INHERIT_IMPL_END

} // namepace __SDLT_IMPL
using __SDLT_IMPL::const_data_member_proxy;
} // namespace sdlt

#endif // SDLT_CONST_DATA_MEMBER_PROXY_H
