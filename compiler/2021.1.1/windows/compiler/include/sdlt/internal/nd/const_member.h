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

#ifndef SDLT_ND_CONST_MEMBER_H
#define SDLT_ND_CONST_MEMBER_H

#include "../../common.h"
#include "../../primitive.h"
#include "../deferred.h"
#include "../enable_if_type.h"
#include "../rvalue_operator_helper.h"
#include "../unqualified.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace nd {


// Forward declare
template<
    typename MemberDataT,
    typename OrganizationT
>
class const_member;

// Due to C++11 trailing return type usage,
// we have to have use a separate class to house a factory function
// versus the a static method on the "const_member" class itself
// This is because we need a fully defined type to pass to as FactoryT to
// sdlt::primitive<MemberDataT>::template member_interface_alt<DerivedT,FactoryT>
// and at that point "const_member" is not defined so we can use its methods as part of
// trailing return type deduction.
// Luckily with a forward declaration of the "const_member" template, we can deduce
// its template parameters without the type being fully defined
template<typename BrokerT>
struct const_nested_member_factory
{
    typedef typename BrokerT::type nested_member_type;

    template<
        typename MemberDataT,
        typename OrganizationT
    >
    static
    SDLT_INLINE auto
    build_member_proxy(const const_member<MemberDataT, OrganizationT> & a_member)
    ->const_member<nested_member_type, decltype(OrganizationT::template member_builder<BrokerT>::build(std::declval<OrganizationT>()))>
    {
        static_assert(BrokerT::offset_of >= 0, "logic bug");
        return const_member<nested_member_type, decltype(OrganizationT::template member_builder<BrokerT>::build(std::declval<OrganizationT>()))>(
            OrganizationT::template member_builder<BrokerT>::build(a_member.m_organization));
    }

};

__SDLT_INHERIT_IMPL_BEGIN
template<
    typename MemberDataT,
    typename OrganizationT
>
class const_member
: public sdlt::primitive<MemberDataT>::template member_interface_alt<const_member<MemberDataT, OrganizationT>, const_nested_member_factory>
// By inheriting from the primitive's member_interface,
// methods to access to each individual data member of the MemberDataT
// are generated
{
template<typename> friend struct const_nested_member_factory;

public:
    typedef MemberDataT value_type;

    explicit SDLT_INLINE
    const_member(const OrganizationT & a_organization)
    : m_organization(a_organization)
    {
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    const_member(const const_member &a_other)
    : m_organization(a_other.m_organization)
    {
    }

    // NOTE: MemberDataT const prevents rvalue assignment for structs,
    // better protection that nothing
    SDLT_INLINE
    operator value_type const () const {
        MemberDataT value;
        m_organization.export_member(value);
        return value;
    }

    // By including PassThroughRvalueOperatorsXMacro,
    // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
    #define __SDLT_XMACRO_SELF const_member
    #include "../../internal/pass_through_rvalue_operators_xmacro.h"

protected:
    template<typename>
    friend class sdlt::primitive;

private:
    const OrganizationT m_organization;
};

__SDLT_INHERIT_IMPL_END

} // namespace nd
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_ND_CONST_MEMBER_H
