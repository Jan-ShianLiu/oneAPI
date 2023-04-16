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

#ifndef SDLT_ND_ACCESSOR_H
#define SDLT_ND_ACCESSOR_H

#include <type_traits>

#include "../../common.h"
#include "../../linear_index.h"
#include "../enable_if_type.h"
#include "../is_linear_compatible_index.h"
#include "../logical_and_of.h"
#include "bounds_interface.h"
#include "extent_interface.h"
#include "element.h"
#include "const_element.h"
#include "origin_type_builder.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {
namespace nd {

template<
    typename PrimitiveT,
    typename OrganizationT,
    bool IsConstT
>
struct element_for; /* undefined */

template<
    typename PrimitiveT,
    typename OrganizationT
>
struct element_for<PrimitiveT, OrganizationT, true /*IsConstT*/>
{
    typedef const_element<PrimitiveT, OrganizationT> type;
};

template<
    typename PrimitiveT,
    typename OrganizationT
>
struct element_for<PrimitiveT, OrganizationT, false /*IsConstT*/>
{
    typedef element<PrimitiveT, OrganizationT> type;
};


template<
    typename PrimitiveT,
    typename LocalBoundsT,
    typename OrganizationT,
    typename OriginT,
    typename IndicesT,
    bool IsConstT
>
class accessor
: public bounds_interface< accessor<PrimitiveT, LocalBoundsT, OrganizationT, OriginT, IndicesT, IsConstT>, LocalBoundsT>
, public extent_interface<accessor<PrimitiveT, LocalBoundsT, OrganizationT, OriginT, IndicesT, IsConstT>, LocalBoundsT>
{
protected:
    template<typename, int, typename >
    friend struct bounds_interface_component;
    template <typename, typename, typename>
    friend struct bounds_interface_components;

    template<typename, int, typename >
    friend struct extent_interface_component;
    template <typename, typename, typename>
    friend struct extent_interface_components;

    static_assert(OriginT::rank > IndicesT::rank, "To access an element the IndicesT must be of the same rank as the LocalBoundsT");


    const LocalBoundsT m_local_bounds;
    const OrganizationT m_organization;
    const OriginT m_origin;
    const IndicesT m_indices;

public:
    typedef PrimitiveT primitive_type;

    static constexpr int rank = OriginT::rank - IndicesT::rank;
    static_assert(LocalBoundsT::rank == rank, "SDLT code logic bug");

    explicit SDLT_INLINE
    accessor(
        const LocalBoundsT & a_global_bounds,
        const OrganizationT & a_organization,
        const OriginT & a_origin,
        const IndicesT & a_indices
    )
    : m_local_bounds(a_global_bounds)
    , m_organization(a_organization)
    , m_origin(a_origin)
    , m_indices(a_indices)
    {
    }

    // MUST PROVIDE USER DEFINED COPY CONSTRUCTOR
    // with individual member assignment to allow compiler
    // to track them through a SIMD loop
    SDLT_INLINE
    accessor(const accessor &a_other)
    : m_local_bounds(a_other.m_local_bounds)
    , m_organization(a_other.m_organization)
    , m_origin(a_other.m_origin)
    , m_indices(a_other.m_indices)
    {
    }

    SDLT_INLINE const IndicesT &
    indices() const { return m_indices; }

    //const ExtentsT & n_extent() const { return m_extents; }

    template<typename ...IndexListT, typename = internal::enable_if_type<sizeof...(IndexListT) == rank>>
    auto
    translated_to(n_index_t<IndexListT...> a_n_index) const
    ->accessor<PrimitiveT,
               typename translated_bounds<LocalBoundsT, n_index_t<IndexListT...>>::type,
               OrganizationT,
               typename translated_origin<LocalBoundsT, n_index_t<IndexListT...>, OriginT>::type,
               IndicesT,
               IsConstT>
    {
        auto offset = (a_n_index - m_local_bounds.lower());

        return accessor<PrimitiveT,
                        typename translated_bounds<LocalBoundsT, n_index_t<IndexListT...>>::type,
                        OrganizationT,
                        typename translated_origin<LocalBoundsT, n_index_t<IndexListT...>, OriginT>::type,
                        IndicesT,
                        IsConstT>
            ((m_local_bounds + offset)
                , m_organization
                , m_origin.overlay_rightmost(m_origin.template rightmost_dimensions<rank>() - offset)
                , m_indices);
    }

protected:
    typedef typename origin_type_builder<rank>::type origin_type;
public:

    auto
    translated_to_zero() const
    ->decltype(this->template translated_to(origin_type()))
    {
        return this->template translated_to(origin_type());
    }


    template<typename ...BoundsTypeListT, typename = internal::enable_if_type<(sizeof...(BoundsTypeListT) == rank)> >
    auto
    section(const n_bounds_t<BoundsTypeListT...> &a_bounds_gen) const
    ->accessor<PrimitiveT,
               n_bounds_t<BoundsTypeListT...>,
               OrganizationT,
               OriginT,
               IndicesT,
               IsConstT>
    {
        __SDLT_ASSERT(m_local_bounds.contains(a_bounds_gen));
        typedef n_bounds_t<BoundsTypeListT...> section_bounds_type;
        static_assert(section_bounds_type::rank == rank, "The n_bounds_t to section to must have same rank as accessor");

        return accessor<PrimitiveT,
                section_bounds_type,
                OrganizationT,
                OriginT,
                IndicesT,
                IsConstT>
            ( a_bounds_gen
            , m_organization
            , m_origin
            , m_indices);
    }

protected:
    template<typename ElementOrganizationT>
    using element_type = typename element_for<PrimitiveT, ElementOrganizationT, IsConstT>::type;
public:

    // returns proxy to cell inside the container
    template<typename IndexT, typename = internal::enable_if_type<is_linear_compatible_index<IndexT>::value && ((OriginT::rank-1) == IndicesT::rank)>>
    SDLT_INLINE
    auto
    operator [] (const IndexT &a_index) const
    ->element_type<decltype(OrganizationT::template element_builder<OriginT, decltype(this->m_indices + IndexT(a_index))>::build(this->m_organization, this->m_origin, this->m_indices + IndexT(a_index)))>
    {
        //__SDLT_ASSERT((m_offset + a_index).value() >= 0);
        //__SDLT_ASSERT((m_offset + a_index).value() < this->get_size_d1());
        // Only need to assume cache line aligned at the row level
        m_organization.emitAlignmentHint();

		// NOTE: Very important detail, notice that a copy of a_index is being added to the list
		// without this, on VS14/ICC16u2 a flow dependency was being detected in the constructor of linear_index.
		// So perhaps the learning is to make sure no & is passed through from one layer of inlined code to the next
		auto new_indices = this->m_indices + IndexT(a_index);

        typedef decltype(OrganizationT::template element_builder<OriginT, decltype(new_indices)>::build(this->m_organization, this->m_origin, new_indices)) element_organization_type;
        return element_type<element_organization_type>(
            OrganizationT::template element_builder<OriginT, decltype(new_indices)>::build(this->m_organization, this->m_origin, new_indices));
    }

    // returns an accessor with one dimension sliced off
    template<typename IndexT, typename = internal::enable_if_type<is_linear_compatible_index<IndexT>::value && ((OriginT::rank-1) > IndicesT::rank)>>
    SDLT_INLINE
    auto
    operator [] (const IndexT &a_index) const
    ->accessor<
        PrimitiveT,
        decltype(std::declval<LocalBoundsT>().template rightmost_dimensions<rank-1>()),
        OrganizationT,
        OriginT,
        decltype(this->m_indices + a_index),
        IsConstT>
    {

		// NOTE: Very important detail, notice that a copy of a_index is being added to the list
		// without this, on VS14/ICC16u2 a flow dependency was being detected in the constructor of linear_index.
		// So perhaps the learning is to make sure no & is passed through from one layer of inlined code to the next
		auto new_indices = this->m_indices + IndexT(a_index);
		return accessor<
            PrimitiveT,
            decltype(std::declval<LocalBoundsT>().template rightmost_dimensions<rank-1>()),
            OrganizationT,
            OriginT,
            decltype(new_indices),
            IsConstT>(m_local_bounds.template rightmost_dimensions<rank-1>(), m_organization, m_origin, new_indices);
    }


    // returns proxy to cell inside the container
    template<typename... IndexListT, typename = internal::enable_if_type<((OriginT::rank) == (IndicesT::rank+sizeof...(IndexListT))) && logical_and_of<is_linear_compatible_index<IndexListT>...>::value>>
    SDLT_INLINE
    auto
    operator ()(const IndexListT &...a_index) const
    ->element_type<
        decltype(OrganizationT::template element_builder<OriginT,
                                                         // ICC 16u3 & 17beta internal error on next line,
                                                         //decltype(this->m_indices.append(IndexListT(a_index)...))
                                                         // the workaround
                                                         decltype(this->m_indices.append(std::declval<IndexListT>()...))
                                                         >::build(this->m_organization,
                                                                  this->m_origin,
                                                                  this->m_indices.append(IndexListT(a_index)...)))
    >
    {
        //__SDLT_ASSERT((m_offset + a_index).value() >= 0);
        //__SDLT_ASSERT((m_offset + a_index).value() < this->get_size_d1());
        // Only need to assume cache line aligned at the row level
        m_organization.emitAlignmentHint();

        // NOTE: Very important detail, notice that a copy of a_index is being added to the list
        // without this, on VS14/ICC16u2 a flow dependency was being detected in the constructor of linear_index.
        // So perhaps the learning is to make sure no & is passed through from one layer of inlined code to the next
        auto new_indices = this->m_indices.append(IndexListT(a_index)...);

        typedef decltype(OrganizationT::template element_builder<OriginT, decltype(new_indices)>::build(this->m_organization, this->m_origin, new_indices)) element_organization_type;
        return element_type<element_organization_type>(
            OrganizationT::template element_builder<OriginT, decltype(new_indices)>::build(this->m_organization, this->m_origin, new_indices));
    }

    // returns an accessor with one or more dimensions sliced off
    template<typename... IndexListT, typename = internal::enable_if_type<((OriginT::rank) > (IndicesT::rank+sizeof...(IndexListT))) && logical_and_of<is_linear_compatible_index<IndexListT>...>::value>>
    SDLT_INLINE
    auto
    operator ()(const IndexListT &...a_index) const
    ->accessor<
        PrimitiveT,
        decltype(std::declval<LocalBoundsT>().template rightmost_dimensions<rank-sizeof...(IndexListT)>()),
        OrganizationT,
        OriginT,
        // ICC 16u3 & 17beta internal error on next line,
        //decltype(this->m_indices.append(IndexListT(a_index)...))
        // the workaround
        decltype(this->m_indices.append(std::declval<IndexListT>()...)),
        IsConstT>
    {

        // NOTE: Very important detail, notice that a copy of a_index is being added to the list
        // without this, on VS14/ICC16u2 a flow dependency was being detected in the constructor of linear_index.
        // So perhaps the learning is to make sure no & is passed through from one layer of inlined code to the next
        auto new_indices = this->m_indices.append(IndexListT(a_index)...);

        return accessor<
            PrimitiveT,
            decltype(std::declval<LocalBoundsT>().template rightmost_dimensions<rank-sizeof...(IndexListT)>()),
            OrganizationT,
            OriginT,
            decltype(new_indices),
            IsConstT>(m_local_bounds.template rightmost_dimensions<rank-sizeof...(IndexListT)>(), m_organization, m_origin, new_indices);
    }


    SDLT_INLINE friend
    std::ostream& operator << (std::ostream& output_stream, const accessor & a_accessor)
    {
        output_stream << "accessor{(" << ((IsConstT) ? "(constant)" : "") << a_accessor.m_local_bounds << ", " << a_accessor.m_origin << ", " << a_accessor.m_organization << ", " << a_accessor.m_indices << " }";
        return output_stream;
    }
};

} // namespace nd
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_ND_ACCESSOR_H
