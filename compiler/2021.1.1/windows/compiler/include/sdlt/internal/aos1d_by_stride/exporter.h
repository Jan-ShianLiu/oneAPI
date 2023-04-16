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

#ifndef SDLT_AOS_1D_BY_STRIDE_EXPORTER_H
#define SDLT_AOS_1D_BY_STRIDE_EXPORTER_H

#include "../../common.h"
#include "../../primitive.h"
#include "../is_builtin.h"

namespace sdlt
{
namespace __SDLT_IMPL
{
namespace internal
{
namespace aos1d_by_stride
{

template<
    typename PrimitiveT,
    int BaseOffsetT,
    typename IndexD1T
>
class exporter
{
public:
    explicit SDLT_INLINE
    exporter(
        const unsigned char * const a_data,
        const IndexD1T an_index_d1)
    : m_data(a_data)
    , m_index(an_index_d1)
    {
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    exporter(const exporter &other)
    : m_data(other.m_data)
    , m_index(other.m_index)
    {
    }

private:
    template<int MemberOffsetT, typename BuiltInT>
    SDLT_INLINE void
    move(std::true_type, // is_builtin
         BuiltInT &a_member) const
    {

        const int data_offset = BaseOffsetT+MemberOffsetT;

        static_assert(sizeof(PrimitiveT)%sizeof(BuiltInT)==0, "primitive needs padding to be divisible by largest built in type used");
        const int stride = static_cast<int>(sizeof(PrimitiveT)/sizeof(BuiltInT));
        const BuiltInT * __restrict read_from_d1_array = reinterpret_cast<const BuiltInT *>(
            m_data
            + data_offset
            );
        a_member = read_from_d1_array[
            pre_offset_of(m_index)*stride +
            varying_of(m_index)*stride +
            post_offset_of(m_index)*stride
            ];
    }

    template<int MemberOffsetT, typename StructT>
    SDLT_INLINE void
    move(std::false_type, // is_builtin
         StructT &a_member) const
    {
        // We can recursively define new exporters based off
        // the member's offset until we reach a built in type
        // to transfer
        exporter<PrimitiveT, BaseOffsetT + MemberOffsetT, IndexD1T> ex(
            m_data,
            m_index);

        primitive<StructT>::transfer(ex, a_member);
    }

public:
    template<int MemberOffsetT, typename MemberT>
    SDLT_INLINE void
    move(MemberT &a_member) const
    {
        // Don't have PrimitiveT available, but maybe should add it back
        //__SDLT_ASSERT((BaseOffsetT + MemberOffsetT) < sizeof(PrimitiveT));

        // Use specialization to determine if the member type is a built in or not
        // then use polymorphism to generate the correct move function
        move<MemberOffsetT>(internal::is_builtin<MemberT>(), a_member);
    }

private:
    const unsigned char * const m_data;
    IndexD1T m_index;
};


} // namespace aos1d_by_stride
} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_AOS_1D_BY_STRIDE_EXPORTER_H
