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

#ifndef SDLT_AOS_MEMBER_H
#define SDLT_AOS_MEMBER_H

#include "../common.h"
#include "../primitive.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {

template<typename MemberDataT>
class aos_member
: public sdlt::primitive<MemberDataT>::template aos_interface<aos_member<MemberDataT>, aos_member>
// By inheriting from the primitive's AOS member_interface,
// methods to access to each individual data member of the MemberDataT
// are generated
{
public:
    SDLT_INLINE
    aos_member(MemberDataT &iMemberData)
    : m_member_data(iMemberData)
    {
    }            //

    SDLT_INLINE
    void
    operator = (const MemberDataT &a_value) {
        m_member_data = a_value;
    }

    SDLT_INLINE
    operator MemberDataT () const {
        return m_member_data;
    }

    SDLT_INLINE
    MemberDataT operator *() const {
        return this->operator MemberDataT ();
    }

protected:
    template<typename>
    friend class sdlt::primitive;

    SDLT_INLINE
    MemberDataT & ref_data() const
    {
        return m_member_data;
    }

private:
    MemberDataT & m_member_data;
};

} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_AOS_MEMBER_H
