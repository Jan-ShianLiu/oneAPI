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

#ifndef SDLT_UNIFORM_CONST_SIMD_ELEMENT_H
#define SDLT_UNIFORM_CONST_SIMD_ELEMENT_H

#include "../../common.h"
#include "../../internal/uniform/const_simd_member.h"
#include "../../primitive.h"
#include "../deferred.h"
#include "../enable_if_type.h"
#include "../unqualified.h"
#include "../rvalue_operator_helper.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace uniform {


template<typename SimdDataT>
class const_simd_element
: public sdlt::primitive<typename SimdDataT::primitive_type>::template const_uniform_interface<
        const_simd_element<SimdDataT>,
        SimdDataT,
        typename simd::root_access<SimdDataT>::type,
        const_simd_member
    >
// By inheriting from the primitive's Const Uniform member_interface,
// methods to access to each individual data member of the MemberDataT
// are generated
{
public:
    typedef typename SimdDataT::primitive_type primitive_type;
    typedef primitive_type value_type;

    SDLT_INLINE
    explicit const_simd_element(
        const SimdDataT & a_simd_data,
        const int a_simd_index)
    : m_simd_index(a_simd_index)
    , m_simd_data(a_simd_data)
    {
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    const_simd_element(const const_simd_element &other)
    : m_simd_index(other.m_simd_index)
    , m_simd_data(other.m_simd_data)
    {            
    }

    // NOTE: "const primitive_type" prevents rvalue assignment for structs,
    // better protection that nothing
    SDLT_INLINE
    operator primitive_type const () const {
        primitive_type value;
        typedef typename simd::root_access<SimdDataT>::type RootAccessType;
        primitive<primitive_type>::template simd_extract<SimdDataT, RootAccessType>(
            m_simd_data,
            m_simd_index,
            value);
        return value;
    }

    // By including pass_through_rvalue_operators_xmacro,
    // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
    #define __SDLT_XMACRO_SELF const_simd_element
    #include "../../internal/pass_through_rvalue_operators_xmacro.h"

protected:
    template<typename>
    friend class sdlt::primitive;

    template<
        typename RootSimdDataT,
        typename MemberAccessT,
        typename MemberDataT>
    friend class const_simd_member;

    // Required by sdlt::primitive::const_uniform_interface
    const SimdDataT & root_simd_data() const { return m_simd_data; }
    int simd_index() const { return m_simd_index; }

private:
    const int m_simd_index;
    const SimdDataT &m_simd_data;
};


} // namespace asa1d
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_UNIFORM_CONST_SIMD_ELEMENT_H
