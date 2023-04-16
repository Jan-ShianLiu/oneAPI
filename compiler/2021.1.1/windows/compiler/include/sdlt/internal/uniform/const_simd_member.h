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

#ifndef SDLT_UNIFORM_CONST_SIMD_MEMBER_H
#define SDLT_UNIFORM_CONST_SIMD_MEMBER_H


#include "../../common.h"
#include "../../primitive.h"
#include "../deferred.h"
#include "../enable_if_type.h"
#include "../unqualified.h"
#include "../rvalue_operator_helper.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace uniform {

__SDLT_INHERIT_IMPL_BEGIN

template<
    typename RootSimdDataT,
    typename MemberAccessT,
    typename MemberDataT>
class const_simd_member
: public sdlt::primitive<MemberDataT>::template const_uniform_interface<const_simd_member<RootSimdDataT, MemberAccessT, MemberDataT>,
                                                               RootSimdDataT,
                                                               MemberAccessT,
                                                               const_simd_member>
{
public:
    typedef MemberDataT value_type;

    template <typename DataProviderT>
    explicit SDLT_INLINE
    const_simd_member(DataProviderT & data_provider)
    : m_simd_index(data_provider.simd_index())
    , m_root_simd_data(data_provider.root_simd_data())
    {}

    template <typename DataProviderT>
    explicit SDLT_INLINE
    const_simd_member(DataProviderT & data_provider, const int a_simd_index)
    : m_simd_index(a_simd_index)
    , m_root_simd_data(data_provider.root_simd_data())
    {}

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    const_simd_member(const const_simd_member & other)
    : m_simd_index(other.m_simd_index)
    , m_root_simd_data(other.m_root_simd_data)
    {}

    // NOTE: "const MemberDataT" prevents rvalue assignment for structs,
    // better protection that nothing
    SDLT_INLINE
    operator MemberDataT const () const {

        MemberDataT data;
        primitive<MemberDataT>::template simd_extract<RootSimdDataT, MemberAccessT>(m_root_simd_data, m_simd_index, data);
        return data;
    }

    // By including pass_through_rvalue_operators_xmacro,
    // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
    #define __SDLT_XMACRO_SELF const_simd_member
    #include "../../internal/pass_through_rvalue_operators_xmacro.h"

protected:
    template<typename>
    friend class sdlt::primitive;

    template<typename, typename, typename>
    friend class const_simd_member;


    const RootSimdDataT & root_simd_data() const { return m_root_simd_data; }
    int simd_index() const { return m_simd_index; }


    const int m_simd_index;
    const RootSimdDataT & m_root_simd_data;
};

__SDLT_INHERIT_IMPL_END

} // namespace uniform
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_UNIFORM_CONST_SIMD_MEMBER_H
