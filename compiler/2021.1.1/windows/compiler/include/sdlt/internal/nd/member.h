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

#ifndef SDLT_ND_MEMBER_H
#define SDLT_ND_MEMBER_H

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
class member;

// Due to C++11 trailing return type usage, 
// we have to have use a separate class to house a factory function
// versus the a static method on the "member" class itself
// This is because we need a fully defined type to pass to as FactoryT to
// sdlt::primitive<MemberDataT>::template member_interface_alt<DerivedT,FactoryT>
// and at that point "member" is not defined so we can use its methods as part of 
// trailing return type deduction. 
// Luckily with a forward declaration of the "member" template, we can deduce
// its template parameters without the type being fully defined
template<typename BrokerT>
struct nested_member_factory
{
    typedef typename BrokerT::type nested_member_type;

    template<
        typename MemberDataT,
        typename OrganizationT
    >
    static
    SDLT_INLINE auto
    build_member_proxy(const member<MemberDataT, OrganizationT> & a_member)
    ->member<nested_member_type, decltype(OrganizationT::template member_builder<BrokerT>::build(std::declval<OrganizationT>()))>
    {
        static_assert(BrokerT::offset_of >= 0, "logic bug");
        return member<nested_member_type, decltype(OrganizationT::template member_builder<BrokerT>::build(std::declval<OrganizationT>()))>(
            OrganizationT::template member_builder<BrokerT>::build(a_member.m_organization));
    }

};

__SDLT_INHERIT_IMPL_BEGIN
template<
    typename MemberDataT,
    typename OrganizationT
>
class member
: public sdlt::primitive<MemberDataT>::template member_interface_alt<member<MemberDataT, OrganizationT>, nested_member_factory>
// By inheriting from the primitive's member_interface,
// methods to access to each individual data member of the MemberDataT
// are generated
{
template<typename> friend struct nested_member_factory;

public:
    typedef MemberDataT value_type;

    explicit SDLT_INLINE
    member(const OrganizationT & a_organization)
    : m_organization(a_organization)
    {
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    member(const member &a_other)
    : m_organization(a_other.m_organization)
    {
    }

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
        m_organization.import_member(a_value);
        return a_value;
    }

    // NOTE: MemberDataT const prevents rvalue assignment for structs,
    // better protection that nothing
    SDLT_INLINE
    operator value_type const () const {
        MemberDataT value;
        m_organization.export_member(value);
        return value;
    }

    // can't inherit assignment operator so have to define it here
    SDLT_INLINE
    member &
    operator = (const member &other) {
        value_type const value = other;
        this->operator = (value);
        return *this;
    }

    // Support Argument Dependent Lookup to specialize for std::swap
    // NOTE:  Iterators return an instance, not the usual reference
    // to the data.  And some algorithms like std::swap_iter
    // will call swap with value, not reference, arguments.
    // For this reason we override the value version of swap,
    // not the reference.
    friend SDLT_INLINE
    void swap(member left, member right)
    {
        // NOTE:  member swap implemented by pass_through_land_rvalue_operators_xmacro
        left.swap(right);
    }

    // By including pass_through_land_rvalue_operators_xmacro,
    // operators for lvalue based compound assignment, increment, decrement operators are generated
    // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
    #define __SDLT_XMACRO_SELF member
    #include "../../internal/pass_through_land_rvalue_operators_xmacro.h"

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

#endif // SDLT_ND_MEMBER_H