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

#ifndef SDLT_SOA_1D_CONST_ELEMENT_H
#define SDLT_SOA_1D_CONST_ELEMENT_H

#include "../../common.h"
#include "../../internal/soa1d/const_member.h"
#include "../../internal/soa1d/exporter.h"
#include "../../internal/soa1d/organization.h"
#include "../../primitive.h"
#include "../deferred.h"
#include "../enable_if_type.h"
#include "../unqualified.h"
#include "../rvalue_operator_helper.h"


namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace soa1d {

__SDLT_INHERIT_IMPL_BEGIN

template<
    typename PrimitiveT,
    int AlignD1OnIndexT,
    typename IndexD1T
>
class const_element
: public sdlt::primitive<PrimitiveT>::template member_interface<const_element<PrimitiveT, AlignD1OnIndexT, IndexD1T>, 0, const_member<PrimitiveT, AlignD1OnIndexT, IndexD1T>::template proxy >
// By inheriting from the primitive's member_interface,
// methods to access to each individual data member of PrimitiveT
// are generated
{
public:
    typedef PrimitiveT value_type;

    explicit SDLT_INLINE
    const_element(
        unsigned char const * const a_data,
        const organization an_organization,
        const IndexD1T an_index_d1)
    : m_data(a_data)
    , m_organization(an_organization)
    , m_index(an_index_d1)
    {
        __SDLT_ASSERT(m_data != nullptr);
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    const_element(const const_element &other)
        : m_data(other.m_data)
        , m_organization(other.m_organization)
        , m_index(other.m_index)
    {
        __SDLT_ASSERT(m_data != nullptr);
    }


    // NOTE: const value_type prevents rvalue assignment for structs,
    // better protection that nothing
    SDLT_INLINE
    operator value_type const () const {
        value_type value;

        exporter<PrimitiveT, 0, AlignD1OnIndexT, IndexD1T> ex(
            m_data,
            m_organization,
            m_index);

        primitive<PrimitiveT>::transfer(ex, value);
        return value;
    }

    // By including PassThroughRvalueOperatorsXMacro,
    // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
    #define __SDLT_XMACRO_SELF const_element
    #include "../../internal/pass_through_rvalue_operators_xmacro.h"

protected:
    template<typename>
    friend class sdlt::primitive;

    template<typename MemberDataT, int OffsetT>
    SDLT_INLINE
    typename const_member<PrimitiveT, AlignD1OnIndexT, IndexD1T>::template proxy<MemberDataT, OffsetT> member() const
    {
        return typename const_member<PrimitiveT, AlignD1OnIndexT, IndexD1T>::template proxy<MemberDataT, OffsetT>(m_data, m_organization, m_index);
    }

private:
    unsigned char const * const m_data;
    const organization m_organization;
    IndexD1T m_index;
};


__SDLT_INHERIT_IMPL_END

} // namespace soa1d
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_SOA_1D_CONST_ELEMENT_H
