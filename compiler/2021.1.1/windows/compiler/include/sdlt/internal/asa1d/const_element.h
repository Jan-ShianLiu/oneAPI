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

#ifndef SDLT_ASA_1D_CONST_ELEMENT_H
#define SDLT_ASA_1D_CONST_ELEMENT_H

#include "../../common.h"
#include "../../internal/asa1d/const_member_simdlane.h"
#include "../../primitive.h"
#include "../deferred.h"
#include "../enable_if_type.h"
#include "../unqualified.h"
#include "../rvalue_operator_helper.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace asa1d {

__SDLT_INHERIT_IMPL_BEGIN

template<typename SimdDataT>
class const_element
: public sdlt::primitive<typename SimdDataT::primitive_type>::template const_uniform_interface<
        const_element<SimdDataT>,
        SimdDataT,
        typename simd::root_access<SimdDataT>::type,
        const_member_simdlane
    >
// By inheriting from the primitive's Const Uniform member_interface,
// methods to access to each individual data member of the MemberDataT
// are generated
{
public:
    typedef typename SimdDataT::primitive_type primitive_type;
    typedef primitive_type value_type;
    static const int simdlane_count = SimdDataT::simdlane_count;

    explicit SDLT_INLINE
    const_element(
        SimdDataT const & a_simd_data,
        const int a_simd_index)
    : m_simd_index(a_simd_index)
    , m_simd_data(a_simd_data)
    {
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    const_element(const const_element &other)
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


    // By including PassThroughRvalueOperatorsXMacro,
    // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
    #define __SDLT_XMACRO_SELF const_element
    #include "../../internal/pass_through_rvalue_operators_xmacro.h"

protected:
    template<typename>
    friend class sdlt::primitive;

    template<
        typename RootSimdDataT,
        typename MemberAccessT,
        typename MemberDataT>
    friend class const_member_simdlane;

    // Required by sdlt::primitive::const_uniform_interface
    SDLT_INLINE const SimdDataT & root_simd_data() const { return m_simd_data; }
    SDLT_INLINE int simd_index() const { return m_simd_index; }

private:
    const int m_simd_index;
    SimdDataT const & m_simd_data;
};

__SDLT_INHERIT_IMPL_END

} // namespace asa1d
} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_ASA_1D_CONST_ELEMENT_H
