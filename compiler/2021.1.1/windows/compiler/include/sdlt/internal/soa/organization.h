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

#ifndef SDLT_SOA_ORGANIZATION_H
#define SDLT_SOA_ORGANIZATION_H

#include <cstring>
#include <iostream>

#include "../../common.h"
#include "../../layout.h"
#include "../../primitive.h"
#include "../nd/organization_builder.h"
#include "exporter.h"
#include "importer.h"
#include "stride_type_builder.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace soa {

template<
    int AlignOnColumnIndexT, 
    typename StridesT, 
    typename PrimitiveT,
    typename OriginT, 
    typename IndicesT,
    typename MemberDataT,
    int MemberOffsetT
>
struct member_organization
{
    static constexpr int rank = StridesT::rank;
    static constexpr int align_on_column_index = AlignOnColumnIndexT;

    typedef StridesT strides_type;

    // Must be default constructible
    SDLT_INLINE
        member_organization()
    {}

    SDLT_INLINE
        member_organization(
            strides_type a_strides,
            unsigned char * a_data,
            const OriginT & a_origin,
            const IndicesT & a_indices)
        :m_strides(a_strides)
        , m_data(a_data)
        , m_origin(a_origin)
        , m_indices(a_indices)
    {
        __SDLT_ASSERT(internal::is_aligned<64>(m_data));
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
        member_organization(const member_organization &a_other)
    : m_strides(a_other.m_strides)
    , m_data(a_other.m_data)
    , m_origin(a_other.m_origin)
    , m_indices(a_other.m_indices)
    {}

    SDLT_INLINE
        const strides_type &
        strides() const { return m_strides; }

    SDLT_INLINE
        unsigned char *
        data() const { return m_data; }

    SDLT_INLINE
    void import_member(const MemberDataT &a_value) const
    {
        soa::importer<PrimitiveT, MemberOffsetT, AlignOnColumnIndexT, member_organization, OriginT, IndicesT> imp(
            m_data,
            *this,
            m_origin,
            m_indices);

        // In order to use the same layout strategy for import and export
        // we have to use non-const parameter,
        // thus we have to remove constness of the value
        primitive<MemberDataT>::transfer(imp, const_cast<MemberDataT &>(a_value));
    }

    SDLT_INLINE
    void export_member(MemberDataT &a_value) const
    {
        soa::exporter<PrimitiveT, MemberOffsetT, AlignOnColumnIndexT, member_organization, OriginT, IndicesT> ex(
            m_data,
            *this,
            m_origin,
            m_indices);

        primitive<MemberDataT>::transfer(ex, a_value);
    }

    template<typename MemberBrokerT>
    struct member_builder
    {
        static 
        SDLT_INLINE auto
            build(const member_organization &a_org)
            ->member_organization<AlignOnColumnIndexT, 
                                  StridesT, 
                                  PrimitiveT, 
                                  OriginT, 
                                  IndicesT, 
                                  typename MemberBrokerT::type, MemberOffsetT + MemberBrokerT::offset_of>
        {
            return member_organization<AlignOnColumnIndexT, 
                                       StridesT, 
                                       PrimitiveT, 
                                       OriginT, 
                                       IndicesT,
                                       typename MemberBrokerT::type,
                                       MemberOffsetT + MemberBrokerT::offset_of>
                (a_org.m_strides, a_org.m_data, a_org.m_origin, a_org.m_indices);
        }
    };


    SDLT_INLINE  friend
        std::ostream& operator << (std::ostream& output_stream, const member_organization & a_org)
    {
        output_stream << "soa::member_organization{ " << a_org.m_strides << " }";
        return output_stream;
    }

protected:
    strides_type m_strides;
    unsigned char * m_data; // can be offset into the buffer for alignment purposes
    const OriginT m_origin;
    const IndicesT m_indices;
};


template<int AlignOnColumnIndexT, typename StridesT, typename PrimitiveT, typename OriginT, typename IndicesT>
struct element_organization
{
    static constexpr int rank = StridesT::rank;
    static constexpr int align_on_column_index = AlignOnColumnIndexT;

    typedef StridesT strides_type;

    // Must be default constructible
    SDLT_INLINE
        element_organization()
    {}

    SDLT_INLINE
        element_organization(
            strides_type a_strides,
            unsigned char * a_data,
            const OriginT & a_origin,
            const IndicesT & a_indices)
        : m_strides(a_strides)
        , m_data(a_data)
        , m_origin(a_origin)
        , m_indices(a_indices)
    {
        __SDLT_ASSERT(internal::is_aligned<64>(m_data));
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
        element_organization(const element_organization &a_other)
        : m_strides(a_other.m_strides)
        , m_data(a_other.m_data)
        , m_origin(a_other.m_origin)
        , m_indices(a_other.m_indices)
    {}

    SDLT_INLINE
        const strides_type &
        strides() const { return m_strides; }

    SDLT_INLINE void emitAlignmentHint() const
    {
        __SDLT_ASSUME_CACHE_LINE_ALIGNED(this->m_data);
    }


    SDLT_INLINE
    void import_element(const PrimitiveT &a_value) const
    {
        soa::importer<PrimitiveT, 0, AlignOnColumnIndexT, element_organization, OriginT, IndicesT> imp(
            m_data,
            *this,
            m_origin,
            m_indices);

        // In order to use the same layout strategy for import and export
        // we have to use non-const parameter,
        // thus we have to remove constness of the value
        primitive<PrimitiveT>::transfer(imp, const_cast<PrimitiveT &>(a_value));
    }

    SDLT_INLINE
    void export_element(PrimitiveT &a_value) const
    {
        soa::exporter<PrimitiveT, 0, AlignOnColumnIndexT, element_organization, OriginT, IndicesT> ex(
            m_data,
            *this,
            m_origin,
            m_indices);

        primitive<PrimitiveT>::transfer(ex, a_value);
    }

    template<typename MemberBrokerT>
    struct member_builder
    {
        static auto
            build(const element_organization &a_org)
            ->member_organization<AlignOnColumnIndexT, StridesT, PrimitiveT, OriginT, IndicesT, typename MemberBrokerT::type, MemberBrokerT::offset_of>
        {
            return member_organization<AlignOnColumnIndexT, StridesT, PrimitiveT, OriginT, IndicesT, typename MemberBrokerT::type, MemberBrokerT::offset_of>(a_org.m_strides, a_org.m_data, a_org.m_origin, a_org.m_indices);
        }
    };

    SDLT_INLINE  friend
        std::ostream& operator << (std::ostream& output_stream, const element_organization & a_org)
    {
        output_stream << "soa::element_organization{ m_data=" << static_cast<void *>(a_org.m_data) << ",  " << a_org.m_strides << " }";
        return output_stream;
    }

protected:
    strides_type m_strides;
    unsigned char * m_data; // can be offset into the buffer for alignment purposes
    const OriginT m_origin;
    const IndicesT m_indices;
};

template<int RowByteAlignmentT, int AlignOnColumnIndexT, typename StridesT, typename PrimitiveT>
struct organization
{
    static constexpr int rank = StridesT::rank;
    static constexpr int align_on_column_index = AlignOnColumnIndexT;
    static constexpr bool rows_are_byte_aligned = true;
    static constexpr int row_byte_alignment = RowByteAlignmentT;

    typedef StridesT strides_type;

    // Must be default constructible
    SDLT_INLINE
    organization()
    {}

    SDLT_INLINE
    organization(
        strides_type a_strides, 
        unsigned char * a_data)
    : m_strides(a_strides)
    , m_data(a_data)
    {
        __SDLT_ASSERT(internal::is_aligned<64>(m_data));
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    organization(const organization &a_other)
    : m_strides(a_other.m_strides)
    , m_data(a_other.m_data)
    {}


    SDLT_NOINLINE void
    copy_data_from(const organization &a_other)
    {
        size_t required_bytes = static_cast<size_t>(m_strides.template get<0>())*sizeof(PrimitiveT);
        std::memcpy(m_data, a_other.m_data, required_bytes);
    }

    SDLT_INLINE
    const strides_type & 
    strides() const { return m_strides; }


    SDLT_INLINE void emitAlignmentHint() const
    {
        __SDLT_ASSUME_CACHE_LINE_ALIGNED(this->m_data);
    }

    template<typename OriginT, typename IndicesT>
    struct element_builder
    {
        static auto
            build(const organization &a_org,
                const OriginT & a_origin,
                const IndicesT & a_indices)
            ->element_organization<align_on_column_index, StridesT, PrimitiveT, OriginT, IndicesT>
        {
            return element_organization<align_on_column_index, StridesT, PrimitiveT, OriginT, IndicesT>(a_org.m_strides, a_org.m_data, a_origin, a_indices);
        }
    };

    SDLT_INLINE  friend
    std::ostream& operator << (std::ostream& output_stream, const organization & a_org)
    {
        output_stream << "soa::organization{ m_data=" << static_cast<void *>(a_org.m_data) << ",  " << a_org.m_strides << " }";
        return output_stream;
    }

protected:
    strides_type m_strides;
    unsigned char * m_data; // can be offset into the buffer for alignment purposes
};

} // namespace soa



namespace nd {

template<int AlignOnColumnIndexT, typename ExtentsT, typename PrimitiveT>
struct organization_builder<layout::soa<AlignOnColumnIndexT>, ExtentsT, PrimitiveT>
{
protected:
    // Use the smallest data member to figure out how many pad elements
    // will be needed for an entire row to become aligned.
    // Larger built in types should be divisible in size (1,2,4,8),
    // thus larger data members will be aligned
    static constexpr int sizeof_smallest_member = sizeof(typename primitive<PrimitiveT>::smallest_builtin_type);

    typedef soa::stride_type_builder<sizeof_smallest_member, ExtentsT> stride_builder;
    typedef typename stride_builder::type strides_type;
public:
    typedef soa::organization<stride_builder::byte_alignment, AlignOnColumnIndexT, strides_type, PrimitiveT> type;


    template<int ByteAlignmentT> static 
    SDLT_INLINE size_t
    required_bytes_for(ExtentsT a_extents)
    {
        static_assert(ByteAlignmentT >= 0, "invalid input");

        auto row_stride = soa::template aligned_row_stride_for<PrimitiveT>(get_row(a_extents));


        __SDLT_ASSERT(row_stride*sizeof_smallest_member%64 == 0);

        size_t required_size_in_bytes =  sizeof(PrimitiveT)*padded_size(a_extents, row_stride);

        if (AlignOnColumnIndexT > 0)
        {
            static_assert((AlignOnColumnIndexT == 0) || (sizeof_smallest_member == sizeof(typename sdlt::primitive<PrimitiveT>::largest_builtin_type)), "With data layout::soa, you can only utilize AlignOnColumnIndexT when all members of your primitive are the same size");

            // Add some padding on so we can shift start of the
            // data inside the buffer such that accessing
            // AlignOnColumnIndexT will be on a alignment boundary
            required_size_in_bytes += required_size_in_bytes + ByteAlignmentT;
        }
        return required_size_in_bytes;
    }

    static 
    SDLT_INLINE type 
    build(ExtentsT a_extents, unsigned char * a_data)
    {
        return type(stride_builder::build(a_extents), a_data);
    }
};

} // namepace nd
} // namepace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_SOA_ORGANIZATION_H
