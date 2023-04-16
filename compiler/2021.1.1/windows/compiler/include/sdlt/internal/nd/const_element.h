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

#ifndef SDLT_ND_CONST_ELEMENT_H
#define SDLT_ND_CONST_ELEMENT_H

#include "../../common.h"
#include "../../primitive.h"
#include "../deferred.h"
#include "../enable_if_type.h"
#include "../rvalue_operator_helper.h"
#include "../unqualified.h"
#include "const_member.h"


namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace nd {

// Forward declare
template<
    typename PrimitiveT,
    typename OrganizationT
>
class const_element;


// Due to C++11 trailing return type usage,
// we have to have use a separate class to house a factory function
// versus the a static method on the "const_element" class itself
// This is because we need a fully defined type to pass to as FactoryT to
// sdlt::primitive<MemberDataT>::template member_interface_alt<DerivedT,FactoryT>
// and at that point "const_element" is not defined so we can use its methods as part of
// trailing return type deduction.
// Luckily with a forward declaration of the "const_element" template, we can deduce
// its template parameters without the type being fully defined
template<typename BrokerT>
struct const_member_factory
{
    typedef typename BrokerT::type member_type;

    template<
        typename PrimitiveT,
        typename OrganizationT
    >
    static
    SDLT_INLINE auto
    build_member_proxy(const const_element<PrimitiveT, OrganizationT> & a_element)
    ->const_member<
        member_type,
        decltype(OrganizationT::template member_builder<BrokerT>::build(std::declval<OrganizationT>()))
    >
    {
        static_assert(BrokerT::offset_of >= 0, "logic bug");
        return const_member<
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
class const_element
: public sdlt::primitive<PrimitiveT>::template member_interface_alt<
    const_element<PrimitiveT, OrganizationT>,
    const_member_factory
>
{
    template<typename> friend struct const_member_factory;
    typedef OrganizationT organization_type;

public:

    typedef PrimitiveT value_type;

    explicit SDLT_INLINE
    const_element(const OrganizationT & a_organization)
    : m_organization(a_organization)
    {
    }


    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    const_element(const const_element &a_other)
    : m_organization(a_other.m_organization)
    {
    }


    // NOTE: const value_type prevents rvalue assignment for structs,
    // better protection that nothing
    SDLT_INLINE
    operator value_type const () const {
        value_type value;
        m_organization.export_element(value);
        return value;
    }


    // By including PassThroughRvalueOperatorsXMacro,
    // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
    #define __SDLT_XMACRO_SELF const_element
    #include "../pass_through_rvalue_operators_xmacro.h"


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


#endif // SDLT_ND_CONST_ELEMENT_H
