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

#ifndef SDLT_SOA_1D_IMPORTER_H
#define SDLT_SOA_1D_IMPORTER_H

#include "../../common.h"
#include "../../primitive.h"
#include "../aligned_access.h"
#include "../is_builtin.h"
#include "organization.h"


namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace soa1d {

template<
    typename PrimitiveT,
    int BaseOffsetT,
    int AlignD1OnIndexT,
    typename IndexD1T
>
class importer
{
public:
    explicit SDLT_INLINE
    importer(
        unsigned char * const a_data,
        const organization an_organization,
        const IndexD1T an_index_d1)
    : m_data(a_data)
    , m_organization(an_organization)
    , m_index(an_index_d1)
    {
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    importer(const importer &other)
        : m_data(other.m_data)
        , m_organization(other.m_organization)
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

        __SDLT_ASSERT(m_data != nullptr);
        __SDLT_ASSERT(internal::is_aligned<64>(m_data));

        __SDLT_ASSERT(m_organization.m_stride_d1%(64/sizeof(BuiltInT)) == 0);
        __SDLT_ASSUME(m_organization.m_stride_d1%(64/sizeof(BuiltInT)) == 0);

        // NOTE: There are a large number of intermediate variables and
        // calculations here that would be expensive if issued for each call.
        // However when utilized inside a loop, the compiler will hoist all
        // duplicate calculations outside the loop, leaving a simple indexed
        // array access with compile time known offset inside the loop


        // Using the aligned typed pointer works well for alignment,
        // however for prefetching it treats identical pointers as independent
        // causing repeated prefetches for same or overlapping indices
        BuiltInT * write_to_aligned_d1_array = reinterpret_cast<BuiltInT *>(
            m_data
            + data_offset*static_cast<size_t>(m_organization.m_stride_d1)
        );

        __SDLT_ASSERT(internal::is_aligned<64>(write_to_aligned_d1_array));
        // Rather than tell the compiler this pointer is aligned, we will let the
        // compiler just to a peel loop to hit the aligned pointer.
        // If we did tell it was aligned, the compiler may lose track of the
        // fact overlapping accesses when doing prefetching

        // NOTE: for compiler to realize multiple accesses to the same array may possibly overlap
        // and therefore may not need to generate separate prefetches for each memory accesses
        // the different components of the index must be present inside the same array subscript
        // If any of these index components are added together before the array subsript
        // compiler may lose track of the fact they are from the same base array and not
        // be able to detect they overlap.  Might also be good to enable any
        // dependency flow issues to enable the compiler to detect overlaps.
        __SDLT_ASSERT(internal::is_aligned<64>(write_to_aligned_d1_array + aligned_access<AlignD1OnIndexT, BuiltInT>::offset + AlignD1OnIndexT));

        __SDLT_ASSERT(std::size_t(pre_offset_of(m_index) + varying_of(m_index) + post_offset_of(m_index))  <  std::size_t(m_organization.m_size_d1));

        write_to_aligned_d1_array[
            pre_offset_of(m_index) +
            varying_of(m_index) +
            post_offset_of(m_index) +
            aligned_access<AlignD1OnIndexT, BuiltInT>::offset
        ] = a_member;
    }

    template<int MemberOffsetT, typename StructT>
    SDLT_INLINE void
    move(std::false_type, // is_builtin
         StructT &a_member) const
    {
        // We can recursively define new exporters based off
        // the member's offset until we reach a built in type
        // to transfer
        importer<PrimitiveT, BaseOffsetT + MemberOffsetT, AlignD1OnIndexT, IndexD1T> ex(
            m_data,
            m_organization,
            m_index);

        primitive<StructT>::transfer(ex, a_member);
    }

public:
    template<int MemberOffsetT, typename MemberT>
    SDLT_INLINE void
    move(MemberT &a_member) const
    {
        __SDLT_ASSERT((BaseOffsetT + MemberOffsetT) < sizeof(PrimitiveT));

        // Use specialization to determine if the member type is a built in or not
        // then use polymorphism to generate the correct move function
        move<MemberOffsetT>(internal::is_builtin<MemberT>(), a_member);
    }

private:
    unsigned char * const m_data;
    const organization m_organization;
    IndexD1T m_index;
};


} // namespace soa1d
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_SOA_1D_IMPORTER_H
