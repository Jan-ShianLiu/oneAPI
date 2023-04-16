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

#ifndef SDLT_UNIFORM_CONST_SCALAR_ELEMENT_H
#define SDLT_UNIFORM_CONST_SCALAR_ELEMENT_H

#include "../../common.h"
#include "../../internal/uniform/const_scalar_member.h"
#include "../../primitive.h"
#include "../deferred.h"
#include "../enable_if_type.h"
#include "../unqualified.h"
#include "../rvalue_operator_helper.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace uniform {

template<typename ScalarDataT>
class const_scalar_element
: public sdlt::primitive<ScalarDataT>::template const_scalar_uniform_interface<
    const_scalar_element<ScalarDataT>,
    ScalarDataT,
    typename scalar::root_access<ScalarDataT>::type,
    const_scalar_member
>
// By inheriting from the primitive's Const Scalar Uniform member_interface,
// methods to access to each individual data member of the MemberDataT
// are generated
{
public:
    typedef ScalarDataT primitive_type;
    typedef primitive_type value_type;

    SDLT_INLINE
    explicit const_scalar_element(const ScalarDataT & iRootScalarData)
    : m_root_scalar_data(iRootScalarData)
    {
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    const_scalar_element(const const_scalar_element & other)
    : m_root_scalar_data(other.m_root_scalar_data)
    {
    }

    // NOTE: "const primitive_type" prevents rvalue assignment for structs,
    // better protection that nothing
    SDLT_INLINE
    operator ScalarDataT const () const {
        return m_root_scalar_data;
    }

    // By including PassThroughRvalueOperatorsXMacro,
    // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
    #define __SDLT_XMACRO_SELF const_scalar_element
    #include "../../internal/pass_through_rvalue_operators_xmacro.h"
protected:
    template<typename>
    friend class sdlt::primitive;

    template<typename, typename, typename>
    friend class uniform::const_scalar_member;

    // Required by sdlt::primitive::const_uniform_interface
    const ScalarDataT & root_scalar_data() const { return m_root_scalar_data; }

    const ScalarDataT & m_root_scalar_data;
};


} // namespace uniform
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_UNIFORM_CONST_SCALAR_ELEMENT_H
