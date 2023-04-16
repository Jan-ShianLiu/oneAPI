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

#ifndef SDLT_DATA_MEMBER_PROXY_H
#define SDLT_DATA_MEMBER_PROXY_H


#include "common.h"
#include "internal/check_for_primitive_declaration.h"
#include "internal/scalar/root_access.h"
#include "internal/uniform/const_scalar_member.h"
#include "internal/uniform/scalar_member.h"
#include "internal/enable_if_type.h"
#include "internal/unqualified.h"
#include "primitive.h"

namespace sdlt {
namespace __SDLT_IMPL {

// Forward
template<typename PrimitiveT>
struct const_data_member_proxy;

__SDLT_INHERIT_IMPL_BEGIN

template<typename PrimitiveT>
struct data_member_proxy
: public sdlt::primitive<PrimitiveT>::template scalar_uniform_interface<
    data_member_proxy<PrimitiveT>,
    PrimitiveT,
    typename internal::scalar::root_access<PrimitiveT>::type,
    internal::uniform::template const_scalar_member,
    internal::uniform::template scalar_member>
, private internal::check_for_primitive_declaration<PrimitiveT>
{
public:
    typedef PrimitiveT value_type;

protected:
    typedef typename internal::scalar::template root_access<PrimitiveT>::type root_access_type;
public:

    data_member_proxy(value_type &a_data)
    : m_data(a_data)
    {}

    __SDLT_INHERIT_IMPL_DESTRUCTOR
    ~data_member_proxy() = default;

    // NOTE:  the return value is constant, intent is to allow chained
    // assignments, not modification of return value.  As we can't ever
    // provide direct access to the underlying object for all containers,
    // just import export of its values.
    // We could have returned *this, but in the case of chaining wanted
    // to avoid re-exporting the primitive when we already have it
    // represented as a_value
    SDLT_INLINE
    const value_type &
    operator = (const value_type &a_value) {
        m_data = a_value;
        return a_value;
    }

    SDLT_INLINE
    operator value_type const () const {
        return m_data;
    }

    // By including pass_through_rvalue_operators_xmacro,
    // operators for lvalue based compound assignment, increment, decrement operators are generated
    // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
    #define __SDLT_XMACRO_SELF data_member_proxy
    #include "internal/pass_through_land_rvalue_operators_xmacro.h"

private:

    template<typename>
    friend struct const_data_member_proxy;

    template<
        typename RootScalarDataT,
        typename MemberAccessT,
        typename MemberDataT
    > friend class internal::uniform::scalar_member;

    template<
        typename RootScalarDataT,
        typename MemberAccessT,
        typename MemberDataT
    > friend class internal::uniform::const_scalar_member;

    const value_type & root_scalar_data() const { return m_data; }
    value_type & root_scalar_data() { return m_data; }
    value_type & m_data;
};
__SDLT_INHERIT_IMPL_END

} // namepace __SDLT_IMPL
using __SDLT_IMPL::data_member_proxy;
} // namespace sdlt

#endif // SDLT_DATA_MEMBER_PROXY_H
