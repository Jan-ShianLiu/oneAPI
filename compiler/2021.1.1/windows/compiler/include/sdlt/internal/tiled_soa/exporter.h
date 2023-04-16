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

// Reserved for future use
#ifdef __SDLT_RESERVED_FOR_FUTURE_USE
#ifndef SDLT_TILED_SOA_EXPORTER_H
#define SDLT_TILED_SOA_EXPORTER_H

#include <type_traits>

#include "../../common.h"
#include "../../primitive.h"
#include "../aligned_access.h"
#include "../is_builtin.h"
#include "stride_offset_calculator.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace tiled_soa {

template<
    typename TileExtentsT,
    typename PrimitiveT,
    int BaseOffsetT,
    int AlignOnColumnIndexT,
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
        //const int data_offset = BaseOffsetT+MemberOffsetT;
        fixed<BaseOffsetT + MemberOffsetT> data_offset;

        __SDLT_ASSERT(m_data != nullptr);
        __SDLT_ASSERT(internal::is_aligned<64>(m_data));

        // NOTE: There are a large number of intermediate variables and
        // calculations here that would be expensive if issued for each call.
        // However when utilized inside a loop, the compiler will hoist all
        // duplicate calculations outside the loop, leaving a simple indexed
        // array access with compile time known offset inside the loop

        // Using the aligned typed pointer works well for alignment,
        // however for prefetching it treats identical pointers as independent
        // causing repeated prefetches for same or overlapping indices
        const BuiltInT * readable_aligned_d1_array = reinterpret_cast<const BuiltInT * >(
            m_data +
            (
                +data_offset*m_organization.strides().template get<0>()
                + calculate_stride_offset<TileExtentsT, BuiltInT>(m_indices, m_origin, m_organization.tile_strides(), m_organization.inside_tile_strides())
            )
        );
//        __SDLT_ASSUME_CACHE_LINE_ALIGNED(readable_aligned_d1_array);
        //auto dump = caclulate_stride_offset<BuiltInT>(m_indices, m_origin, m_organization.strides());
        //typedef decltype(dump)::not_there not_there;

        __SDLT_ASSERT(internal::is_aligned<64>(readable_aligned_d1_array));
        // Rather than tell the compiler this pointer is aligned, we will let the
        // compiler just to a peel loop to hit the aligned pointer.
        // If we did tell it was aligned, the compiler may lose track of the
        // fact overlapping accesses when doing prefetching

        // NOTE: for compiler to realize multiple accesses to the same array may possibly overlap
        // and therefore may not need to generate separate prefetches for each memory accesses
        // the different components of the index must be present inside the same array subscript
        // If any of these index components are added together before the array subscript
        // compiler may lose track of the fact they are from the same base array and not
        // be able to detect they overlap.  Might also be good to enable any
        // dependency flow issues to enable the compiler to detect overlaps.
        __SDLT_ASSERT(internal::is_aligned<64>(readable_aligned_d1_array + aligned_access<AlignOnColumnIndexT, BuiltInT>::offset + AlignOnColumnIndexT));

        auto row_index = m_indices.template get<IndicesT::row_element>();
        auto row_origin_offset = m_origin.row_index();
        __SDLT_ASSERT(std::size_t(row_index.pre_offset() + row_index.varying() + row_index.post_offset() + row_origin_offset)  < m_organization.strides().template get<strides_type::row>());

        a_member = readable_aligned_d1_array[
               row_index.pre_offset() +
               row_index.varying() +
               row_index.post_offset() +
               row_origin_offset +
               aligned_access<AlignOnColumnIndexT, BuiltInT>::offset
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
        exporter<PrimitiveT, BaseOffsetT + MemberOffsetT, AlignOnColumnIndexT, OrganizationT, OriginT, IndicesT> ex(
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


} // namespace tiled_soa
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_TILED_SOA_EXPORTER_H
#endif
