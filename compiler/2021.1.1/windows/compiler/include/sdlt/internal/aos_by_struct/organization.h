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

#ifndef SDLT_AOS_BY_STRUCT_ORGANIZATION_H
#define SDLT_AOS_BY_STRUCT_ORGANIZATION_H

#include <cstring>
#include <iostream>

#include "../../common.h"
#include "../../layout.h"
#include "../../primitive.h"
#include "../nd/organization_builder.h"
#include "stride_type_builder.h"
#include "stride_offset_calculator.h"


namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace aos_by_struct {

    template<typename MemberDataT>
    struct member_organization
    {
        // Must be default constructible
        SDLT_INLINE
            member_organization()
        {}

        SDLT_INLINE
            member_organization(
                MemberDataT &a_data)
            : m_data(a_data)
        {
        }

        // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
        // with individual member assignment to allow compiler
        // to track them through a SIMD loop
        SDLT_INLINE
            member_organization(const member_organization &a_other)
            : m_data(a_other.m_data)
        {}
  
    public:


        SDLT_INLINE
        void import_member(const MemberDataT &a_value) const
        {
            m_data = a_value;
        }

        SDLT_INLINE
        void export_member(MemberDataT &a_value) const
        {
            a_value =  m_data;
        }

        template<typename MemberBrokerT>
        struct member_builder
        {
            static 
            SDLT_INLINE auto
            build(const member_organization &a_org)
            ->member_organization<typename MemberBrokerT::type>
            {
                return member_organization<typename MemberBrokerT::type>(MemberBrokerT::ref(a_org.m_data));
            }
        };


        SDLT_INLINE  friend
            std::ostream& operator << (std::ostream& output_stream, const member_organization & a_org)
        {
            output_stream << "aos_by_struct::member_organization{ " << a_org.m_data << " }";
            return output_stream;
        }

    protected:
        MemberDataT & m_data; // can be offset into the buffer for alignment purposes
    };

    template<typename PrimitiveT>
    struct element_organization
    {
        // Must be default constructible
        SDLT_INLINE
            element_organization()
        {}

        SDLT_INLINE
            element_organization(
                PrimitiveT &a_data)
            : m_data(a_data)
        {
        }

        // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
        // with individual member assignment to allow compiler
        // to track them through a SIMD loop
        SDLT_INLINE
            element_organization(const element_organization &a_other)
            : m_data(a_other.m_data)
        {}

    public:


        SDLT_INLINE
        void import_element(const PrimitiveT &a_value) const
        {
            m_data = a_value;
        }

        SDLT_INLINE
        void export_element(PrimitiveT &a_value) const
        {
            a_value = m_data;
        }

        template<typename MemberBrokerT>
        struct member_builder
        {
            static 
            SDLT_INLINE auto
            build(const element_organization &a_org)
            ->member_organization<typename MemberBrokerT::type>
            {
                return member_organization<typename MemberBrokerT::type>(MemberBrokerT::ref(a_org.m_data));
            }
        };


        SDLT_INLINE  friend
            std::ostream& operator << (std::ostream& output_stream, const element_organization & a_org)
        {
            output_stream << "aos_by_struct::element_organization{ " << a_org.m_data << " }";
            return output_stream;
        }

    protected:
        PrimitiveT & m_data; // can be offset into the buffer for alignment purposes
    };


    template<typename StridesT, typename PrimitiveT>
    struct organization
    {
        static constexpr int rank = StridesT::rank;
        static constexpr bool rows_are_byte_aligned = false;

        typedef StridesT strides_type;

        // Must be default constructible
        SDLT_INLINE
            organization()
        {}

        SDLT_INLINE
            organization(
                strides_type a_strides,
                PrimitiveT * a_data)
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


    protected:
        template<typename OriginT, typename IndicesT>
        size_t calculate_linear_index_of(
            const OriginT & a_origin,
            const IndicesT & a_indices) const
        {
            auto row_index = a_indices.template get<IndicesT::row_element>();
            auto row_origin_offset = get_row(a_origin);
            __SDLT_ASSERT(std::ptrdiff_t(pre_offset_of(row_index) + varying_of(row_index) + post_offset_of(row_index) + row_origin_offset)  < m_strides.row_stride());

            return
                calculate_stride_offset(a_indices, a_origin, m_strides) +
                (   // Need to add signed values together before adding to unsigned value
                    pre_offset_of(row_index) +
                    varying_of(row_index) +
                    post_offset_of(row_index) +
                    row_origin_offset
                );
        }
    public:
     
        template<typename OriginT, typename IndicesT>
        struct element_builder
        {
            static 
            SDLT_INLINE auto
            build(const organization &a_org,
                const OriginT & a_origin,
                const IndicesT & a_indices)
            ->element_organization<PrimitiveT>
            {
                return element_organization<PrimitiveT>(a_org.m_data[a_org.calculate_linear_index_of(a_origin, a_indices)]);
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
        PrimitiveT * m_data; // can be offset into the buffer for alignment purposes
    };

} // namespace soa



namespace nd {

    template<typename ExtentsT, typename PrimitiveT>
    struct organization_builder<layout::aos_by_struct, ExtentsT, PrimitiveT>
    {
    protected:
        typedef make_int_sequence<ExtentsT::rank> index_sequence;

        typedef aos_by_struct::stride_type_sequencer<index_sequence, ExtentsT> stride_builder;
        typedef typename stride_builder::type strides_type;
    public:
        typedef aos_by_struct::organization<strides_type, PrimitiveT> type;

        template<int ByteAlignmentT>
        static SDLT_INLINE size_t
        required_bytes_for(ExtentsT a_extents)
        {
            static_assert(ByteAlignmentT >= 0, "invalid input");

            size_t required_size_in_bytes = sizeof(PrimitiveT)*a_extents.size();

            return required_size_in_bytes;
        }

        static 
        SDLT_INLINE type 
        build(ExtentsT a_extents, unsigned char * a_data)
        {
            return type(stride_builder::build(a_extents), reinterpret_cast<PrimitiveT *>(a_data));
        }
    };

} // namepace nd
} // namepace internal
} // namepace __SDLT_IMPL
} // namespace sdlt


#endif // SDLT_AOS_BY_STRUCT_ORGANIZATION_H
