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

#ifndef SDLT_ASA_CONST_MEMBER_SIMD_LANE_H
#define SDLT_ASA_CONST_MEMBER_SIMD_LANE_H


#include "../../common.h"
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

template<
    typename RootSimdDataT,
    typename MemberAccessT,
    typename MemberDataT>
class const_member_simdlane
: public sdlt::primitive<MemberDataT>::template const_uniform_interface<const_member_simdlane<RootSimdDataT, MemberAccessT, MemberDataT>,
                                                               RootSimdDataT,
                                                               MemberAccessT,
                                                               const_member_simdlane>
// By inheriting from the primitive's Const Uniform member_interface,
// methods to access to each individual data member of the MemberDataT
// are generated
{
public:
    typedef MemberDataT value_type;

    template <typename DataProviderT>
    explicit SDLT_INLINE
    const_member_simdlane(DataProviderT & data_provider)
    : m_simd_index(data_provider.simd_index())
    , m_root_simd_data(data_provider.root_simd_data())
    {}

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop    explicit SDLT_INLINE
    const_member_simdlane(const const_member_simdlane &other)
    : m_simd_index(other.m_simd_index)
    , m_root_simd_data(other.m_root_simd_data)
    {}

    SDLT_INLINE
    operator MemberDataT const () const {

        MemberDataT value;
        primitive<MemberDataT>::template simd_extract<RootSimdDataT, MemberAccessT>(
            m_root_simd_data,
            m_simd_index,
            value);
        return value;
    }

    // By including PassThroughRvalueOperatorsXMacro,
    // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
    #define __SDLT_XMACRO_SELF const_member_simdlane
    #include "../../internal/pass_through_rvalue_operators_xmacro.h"

protected:
    template<typename>
    friend class sdlt::primitive;

    template<typename, typename, typename>
    friend class const_member_simdlane;

    // Required by sdlt::primitive::const_uniform_interface
    const RootSimdDataT & root_simd_data() const { return m_root_simd_data; }
    int simd_index() const { return m_simd_index; }


    const int m_simd_index;
    const RootSimdDataT & m_root_simd_data;
};

__SDLT_INHERIT_IMPL_END

} // namespace asa1d
} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_ASA_CONST_MEMBER_SIMD_LANE_H
