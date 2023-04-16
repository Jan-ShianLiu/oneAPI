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

#ifndef SDLT_UNIFORM_SCALAR_MEMBER_H
#define SDLT_UNIFORM_SCALAR_MEMBER_H


#include "../../common.h"
#include "../../primitive.h"
#include "../enable_if_type.h"
#include "../unqualified.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace uniform {


__SDLT_INHERIT_IMPL_BEGIN

template<
    typename RootScalarDataT,
    typename MemberAccessT,
    typename MemberDataT
>
class scalar_member
: public sdlt::primitive<MemberDataT>::template scalar_uniform_interface<scalar_member<RootScalarDataT, MemberAccessT, MemberDataT>,
                                                               RootScalarDataT,
                                                               MemberAccessT,
                                                               const_scalar_member,
                                                               scalar_member>
{
public:
    typedef MemberDataT value_type;

    template <typename DataProviderT>
    explicit scalar_member(DataProviderT & data_provider)
    : m_root_scalar_data(data_provider.root_scalar_data())
    {}

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    scalar_member(const scalar_member & other)
    : m_root_scalar_data(other.m_root_scalar_data)
    {}


    SDLT_INLINE
    operator value_type const () const {
        return MemberAccessT::const_access(m_root_scalar_data);
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
        MemberAccessT::access(m_root_scalar_data) = a_value;
        return a_value;
    }

    // By including pass_through_rvalue_operators_xmacro,
    // operators for lvalue based compound assignment, increment, decrement operators are generated
    // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
    #define __SDLT_XMACRO_SELF scalar_member
    #include "../../internal/pass_through_land_rvalue_operators_xmacro.h"

protected:
    template<typename>
    friend class sdlt::primitive;

    template<typename, typename, typename>
    friend class uniform::scalar_member;

    template<typename, typename, typename>
    friend class uniform::const_scalar_member;

    const RootScalarDataT & root_scalar_data() const { return m_root_scalar_data; }
    RootScalarDataT & root_scalar_data() { return m_root_scalar_data; }

    RootScalarDataT & m_root_scalar_data;
};

__SDLT_INHERIT_IMPL_END
} // namespace uniform
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_UNIFORM_SCALAR_MEMBER_H
