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

#ifndef SDLT_ND_ELEMENT_H
#define SDLT_ND_ELEMENT_H

#include "../../common.h"
#include "../../internal/nd/member.h"
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
    typename PrimitiveT,
    typename OrganizationT
>
class element;


// Due to C++11 trailing return type usage, 
// we have to have use a separate class to house a factory function
// versus the a static method on the "element" class itself
// This is because we need a fully defined type to pass to as FactoryT to
// sdlt::primitive<MemberDataT>::template member_interface_alt<DerivedT,FactoryT>
// and at that point "element" is not defined so we can use its methods as part of 
// trailing return type deduction. 
// Luckily with a forward declaration of the "element" template, we can deduce
// its template parameters without the type being fully defined
template<typename BrokerT>
struct element_member_factory
{
    typedef typename BrokerT::type member_type;

    template<
        typename PrimitiveT,
        typename OrganizationT
    >
    static 
    SDLT_INLINE auto
    build_member_proxy(const element<PrimitiveT, OrganizationT> & a_element)
    ->member<
        member_type, 
        decltype(OrganizationT::template member_builder<BrokerT>::build(std::declval<OrganizationT>()))
    >
    {
        static_assert(BrokerT::offset_of >= 0, "logic bug");
        return member<
            member_type,
            decltype(OrganizationT::template member_builder<BrokerT>::build(std::declval<OrganizationT>()))
        >(OrganizationT::template member_builder<BrokerT>::build(a_element.m_organization));
    }

};


__SDLT_INHERIT_IMPL_BEGIN

template<
    typename PrimitiveT,
    typename OrganizationT
>
class element
: public sdlt::primitive<PrimitiveT>::template member_interface_alt<
    element<PrimitiveT, OrganizationT>,
    element_member_factory
>
{
    template<typename> friend struct element_member_factory;
    typedef OrganizationT organization_type;

public:

    typedef PrimitiveT value_type;

    explicit SDLT_INLINE
    element(const OrganizationT & a_organization)
    : m_organization(a_organization)
    {
    }


    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    element(const element &a_other)
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
        m_organization.import_element(a_value);
        return a_value;
    }


    // NOTE: const value_type prevents rvalue assignment for structs,
    // better protection that nothing
    SDLT_INLINE
    operator value_type const () const {
        value_type value;
        m_organization.export_element(value);
        return value;
    }


    // can't inherit assignment operator so have to define it here
    SDLT_INLINE
    element &
    operator = (const element &other) {
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
    void swap(element left, element right)
    {
        // NOTE:  member swap implemented by pass_through_land_rvalue_operators_xmacro
        left.swap(right);
    }

    // By including pass_through_land_rvalue_operators_xmacro,
    // operators for lvalue based compound assignment, increment, decrement operators are generated
    // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
    #define __SDLT_XMACRO_SELF element
    #include "../pass_through_land_rvalue_operators_xmacro.h"


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

#endif // SDLT_ND_ELEMENT_H
