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

#ifndef SDLT_AOS_BY_STRIDE_EXPORTER_H
#define SDLT_AOS_BY_STRIDE_EXPORTER_H

#include <type_traits>

#include "../../common.h"
#include "../../primitive.h"
#include "../aligned_access.h"
#include "../is_builtin.h"
#include "stride_offset_calculator.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace aos_by_stride {

template<
    typename PrimitiveT,
    int BaseOffsetT,
    typename OrganizationT,
    typename OriginT,
    typename IndicesT
>
class exporter
{
    typedef typename OrganizationT::strides_type strides_type;
public:
    static_assert(IndicesT::rank == strides_type::rank, "Indices vs Rank mismatch");

    // Have to handle references with const correctness.
    // Allowing an implicit cast can cause bugs when
    // optimizations are applied.  Always explicitly
    // const_cast any references to pointers
    explicit SDLT_INLINE
    exporter(
        unsigned char const * const SDLT_RESTRICT a_data,
        const OrganizationT & a_organization,
        const OriginT & a_origin,
        const IndicesT & a_indices)
    : m_data(a_data)
    , m_organization(a_organization)
    , m_origin(a_origin)
    , m_indices(a_indices)
    {
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    exporter(const exporter &a_other)
    : m_data(a_other.m_data)
    , m_organization(a_other.m_organization)
    , m_origin(a_other.m_origin)
    , m_indices(a_other.m_indices)
    {}


private:

    template<int MemberOffsetT, typename BuiltInT>
    SDLT_INLINE void
        move(std::true_type, // is_builtin
            BuiltInT &a_member) const
    {

        fixed<BaseOffsetT + MemberOffsetT> data_offset;

        __SDLT_ASSERT(m_data != nullptr);
        __SDLT_ASSERT(internal::is_aligned<64>(m_data));
        //        __SDLT_ASSUME_CACHE_LINE_ALIGNED(m_data);

        // NOTE: There are a large number of intermediate variables and
        // calculations here that would be expensive if issued for each call.
        // However when utilized inside a loop, the compiler will hoist all
        // duplicate calculations outside the loop, leaving a simple indexed
        // array access with compile time known offset inside the loop

        const BuiltInT * readable_row_array = reinterpret_cast<const BuiltInT *>(
            m_data +
            (
                data_offset
                + static_cast<std::ptrdiff_t>(calculate_stride_offset<PrimitiveT>(m_indices, m_origin, m_organization.strides()))*sizeof(PrimitiveT)
            )
        );

        // NOTE: for compiler to realize multiple accesses to the same array may possibly overlap
        // and therefore may not need to generate separate prefetches for each memory accesses
        // the different components of the index must be present inside the same array subscript
        // If any of these index components are added together before the array subscript
        // compiler may lose track of the fact they are from the same base array and not
        // be able to detect they overlap.  Might also be good to enable any
        // dependency flow issues to enable the compiler to detect overlaps.

        static_assert(sizeof(PrimitiveT) % sizeof(BuiltInT) == 0, "primitive needs padding to be divisible by largest built in type used");
        fixed_stride<sizeof(PrimitiveT) / sizeof(BuiltInT)> member_stride;

        auto row_index = m_indices.template get<IndicesT::row_element>();
        auto row_origin_offset = get_row(m_origin);
        __SDLT_ASSERT(std::ptrdiff_t(pre_offset_of(row_index) + varying_of(row_index) + post_offset_of(row_index) + row_origin_offset)  < m_organization.strides().template get<strides_type::row>());

        a_member = readable_row_array[
                static_cast<std::ptrdiff_t>(pre_offset_of(row_index) +
                varying_of(row_index) +
                post_offset_of(row_index) +
               row_origin_offset)*member_stride
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
        exporter<PrimitiveT, BaseOffsetT + MemberOffsetT, OrganizationT, OriginT, IndicesT> ex(
            m_data,
            m_organization,
            m_origin,
            m_indices);

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
    unsigned char const * const SDLT_RESTRICT m_data;
    OrganizationT m_organization;
    OriginT m_origin;
    IndicesT m_indices;
};


} // namespace aos_by_stride
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_AOS_BY_STRIDE_EXPORTER_H
