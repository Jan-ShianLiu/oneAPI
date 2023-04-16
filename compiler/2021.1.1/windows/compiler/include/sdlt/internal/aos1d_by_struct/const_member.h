//
// Copyright (C) 2012-2016 Intel Corporation. All Rights Reserved.
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

#ifndef SDLT_AOS_1D_BY_STRUCT_CONST_MEMBER_H
#define SDLT_AOS_1D_BY_STRUCT_CONST_MEMBER_H

#include "../../common.h"
#include "../../primitive.h"
#include "../deferred.h"
#include "../enable_if_type.h"
#include "../unqualified.h"
#include "../rvalue_operator_helper.h"

namespace sdlt
{
namespace __SDLT_IMPL
{
namespace internal
{
namespace aos1d_by_struct
{

    template<typename MemberDataT>
    class const_member
    : public primitive<MemberDataT>::template aos_interface<const_member<MemberDataT>, const_member>
    // By inheriting from the primitive's AOS member_interface,
    // methods to access to each individual data member of the MemberDataT
    // are generated
    {
    public:
        typedef MemberDataT value_type;

        explicit SDLT_INLINE
        const_member(const MemberDataT &initial_member_data)
        : m_member_data(initial_member_data)
        {
        }

        // NOTE: MemberDataT const prevents rvalue assignment for structs,
        // better protection that nothing
        SDLT_INLINE
        operator MemberDataT const () const {
            return m_member_data;
        }

        // By including pass_through_rvalue_operators_xmacro,
        // operators for rvalue based relational, arithmetic, logic, and bitwise operators are generated
        #define __SDLT_XMACRO_SELF const_member
        #include "../../internal/pass_through_rvalue_operators_xmacro.h"

    protected:
        template<typename>
        friend class sdlt::primitive;

        SDLT_INLINE
        const MemberDataT & ref_data() const
        {
            return m_member_data;
        }

    private:
        const MemberDataT & m_member_data;
    };


} // namespace aos1d_by_struct
} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_AOS_1D_BY_STRUCT_CONST_MEMBER_H
