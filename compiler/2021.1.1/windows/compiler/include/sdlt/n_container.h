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

#ifndef SDLT_N_CONTAINER_H
#define SDLT_N_CONTAINER_H

#include <algorithm>
#include <iterator>
#include <limits>
#include <cstddef>
#include <vector>
#include <utility>
#include <tuple>

#include "common.h"

#include "aligned.h"
#include "allocator/default.h"
#include "buffer_offset_in_cachelines.h"
#include "fixed.h"
#include "internal/capacity.h"
#include "internal/enable_if_type.h"
#include "internal/is_linear_compatible_index.h"
#include "internal/static_loop.h"
#include "internal/nd/accessor.h"
#include "internal/nd/origin_type_builder.h"
#include "internal/nd/organization_builder.h"
#include "internal/aos_by_stride/organization.h"
#include "internal/aos_by_struct/organization.h"
#include "internal/soa/organization.h"
#include "internal/soa_per_row/organization.h"
#include "internal/stride_list.h"
#include "layout.h"
#include "n_extent.h"
#include "n_index.h"

namespace sdlt {
namespace __SDLT_IMPL {

namespace internal {
    class unit_test_introspector;
}

// Column is unit stride
template<
    typename PrimitiveT,
    typename LayoutT,
    typename ExtentsT,
    class AllocatorT = allocator::default_alloc
>
class n_container
{
    static_assert(primitive<PrimitiveT>::declared, "You must declare the struct as a SDLT_PRIMITIVE(STRUCT_NAME, MEMBER1, MEMBER2, ..., MemberN) before trying to instantiate a Container with it");
public:
    typedef PrimitiveT primitive_type;
    typedef AllocatorT allocator_type;

protected:
    typedef typename internal::nd::organization_builder<LayoutT, ExtentsT, PrimitiveT>::type organization_type;
    typedef typename internal::nd::origin_type_builder<ExtentsT::rank>::type origin_type;
    ExtentsT m_extents;
    organization_type m_organization;

    buffer_offset_in_cachelines m_buffer_offset;

    // Make sure no one changes the size in bytes after we set it
    size_t m_size_in_bytes;
    unsigned char * m_buffer;
    const AllocatorT * m_allocator;

    friend class internal::unit_test_introspector;
    template<typename DependentT = internal::dependent_arg, typename = sdlt::__SDLT_IMPL::internal::enable_if_type< internal::deferred<organization_type, DependentT>::type::rows_are_byte_aligned, organization_type>>
    static constexpr int row_byte_alignment() { return organization_type::row_byte_alignment; }
public:


    SDLT_INLINE n_container(
        const ExtentsT & a_extents,
        buffer_offset_in_cachelines buffer_offset = buffer_offset_in_cachelines(0),
        const AllocatorT & an_allocator = AllocatorT());

    template<typename = std::enable_if<std::is_default_constructible<ExtentsT>::value>>
    SDLT_INLINE n_container(
        buffer_offset_in_cachelines buffer_offset = buffer_offset_in_cachelines(0),
        const AllocatorT & an_allocator = AllocatorT());

    // Support move constructor and move assignment
    // Allows swap to function efficiently
    SDLT_INLINE n_container(n_container&& donor);
    SDLT_INLINE n_container & operator=(n_container&& donor);

    // Disallow copy construction, we don't want anyone copying these around
    // by value, its a bit too dangerous with lambda functions to be able
    // to capture one of these containers by value, instead users should call
    // the clone method explicitly
    n_container(const n_container &) = delete;
    n_container & operator = (const n_container &) = delete;
    SDLT_NOINLINE n_container clone() const;

    SDLT_INLINE void swap(n_container &other);

    SDLT_NOINLINE ~n_container();

    typedef internal::nd::accessor<
        PrimitiveT,
        decltype(n_bounds(origin_type(), std::declval<ExtentsT>())),
        organization_type,
        origin_type,
        internal::index_list<>,
        true /* IsConstT */> const_accessor;
    typedef internal::nd::accessor<
        PrimitiveT,
        decltype(n_bounds(origin_type(), std::declval<ExtentsT>())),
        organization_type,
        origin_type,
        internal::index_list<>,
        false /* IsConstT */> accessor;

    const ExtentsT & n_extent() const { return m_extents; }


    const_accessor
    const_access() const
    {
        return const_accessor(n_bounds(origin_type(), m_extents), m_organization, origin_type(), internal::index_list<>());
    }


    accessor
    access() const
    {
        return accessor(n_bounds(origin_type(), m_extents), m_organization, origin_type(), internal::index_list<>());
    }

    SDLT_INLINE  friend
    std::ostream& operator << (std::ostream& output_stream, const n_container & a_container)
    {
        output_stream << "n_container{ " << a_container.m_extents << "}";
        return output_stream;
    }


protected:  // member functions

    void do_allocation(const ExtentsT & a_new_extents);
};










////////////////////////////////////////////////////////////////////////////////
// Implementation below
////////////////////////////////////////////////////////////////////////////////


template<typename PrimitiveT, typename LayoutT, typename ExtentsT, class AllocatorT>
n_container<PrimitiveT, LayoutT, ExtentsT, AllocatorT>::
n_container(
    const ExtentsT & a_extents,
    buffer_offset_in_cachelines buffer_offset,
    const AllocatorT & an_allocator)
: m_extents(a_extents)
, m_buffer_offset(buffer_offset)
, m_size_in_bytes(0)
, m_buffer(nullptr)
, m_allocator(&an_allocator)
{
    //std::cout << "m_extents = " << m_extents << std::endl;

    this->do_allocation(m_extents);
    //std::cout << "m_size_in_bytes = " << m_size_in_bytes << std::endl;
}

template<typename PrimitiveT, typename LayoutT, typename ExtentsT, class AllocatorT>
template<typename>
n_container<PrimitiveT, LayoutT, ExtentsT, AllocatorT>::
n_container(
    buffer_offset_in_cachelines buffer_offset,
    const AllocatorT & an_allocator)
    : n_container(ExtentsT(), buffer_offset, an_allocator)
{
}

template<typename PrimitiveT, typename LayoutT, typename ExtentsT, class AllocatorT>
n_container<PrimitiveT, LayoutT, ExtentsT, AllocatorT>::
n_container(n_container&& donor)
: m_extents(donor.m_extents)
, m_organization(donor.m_organization)
, m_buffer_offset(donor.m_buffer_offset)
, m_size_in_bytes(donor.m_size_in_bytes)
, m_buffer(donor.m_buffer)
, m_allocator(donor.m_allocator)
{
    // Cleanup the donor just enough to avoid deallocation
    donor.m_size_in_bytes = 0;
    donor.m_buffer = nullptr;
}

template<typename PrimitiveT, typename LayoutT, typename ExtentsT, class AllocatorT>
n_container<PrimitiveT, LayoutT, ExtentsT, AllocatorT> &
n_container<PrimitiveT, LayoutT, ExtentsT, AllocatorT>::
operator=(n_container&& donor)
{
    if (m_buffer != nullptr)
    {
        m_allocator->free(m_buffer, m_size_in_bytes);
    }

    m_extents = donor.m_extents;
    m_organization = donor.m_organization;
    m_buffer_offset = donor.m_buffer_offset;
    m_size_in_bytes = donor.m_size_in_bytes;
    m_buffer = donor.m_buffer;
    m_allocator = donor.m_allocator;

    // Cleanup the donor just enough to avoid deallocation
    donor.m_size_in_bytes = 0;
    donor.m_buffer = nullptr;
    return *this;
}

template<typename PrimitiveT, typename LayoutT, typename ExtentsT, class AllocatorT>
n_container<PrimitiveT, LayoutT, ExtentsT, AllocatorT>
n_container<PrimitiveT, LayoutT, ExtentsT, AllocatorT>::
clone() const
{
    n_container dest_container(m_extents, m_buffer_offset, *m_allocator);

    dest_container.m_organization.copy_data_from(this->m_organization);

    return dest_container;
}

template<typename PrimitiveT, typename LayoutT, typename ExtentsT, class AllocatorT>
void
n_container<PrimitiveT, LayoutT, ExtentsT, AllocatorT>::
swap(n_container &other)
{
    std::swap(*this, other);
}


template<typename PrimitiveT, typename LayoutT, typename ExtentsT, class AllocatorT>
n_container<PrimitiveT, LayoutT, ExtentsT, AllocatorT>::
~n_container()
{
    if (m_buffer != nullptr)
    {
        m_allocator->free(m_buffer, m_size_in_bytes);
    }
}

template<typename PrimitiveT, typename LayoutT, typename ExtentsT, class AllocatorT>
void
n_container<PrimitiveT, LayoutT, ExtentsT, AllocatorT>::
do_allocation(const ExtentsT & a_new_extents)
{
    typedef internal::nd::organization_builder<LayoutT, ExtentsT, PrimitiveT> builder;
    const int byte_alignment = 64;

//        printf("size of smallest member=%d, size of largest member=%d\n",
//                sizeof(sdlt::primitive<PrimitiveT>::smallest_builtin_type),
//                sizeof(sdlt::primitive<PrimitiveT>::largest_builtin_type));


    unsigned char * data = nullptr;

    if (a_new_extents.size() != 0)
    {

        size_t required_size_in_bytes = builder::template required_bytes_for<byte_alignment>(a_new_extents);
        //printf("\n\nrequiredSizeInBytes[%lu] = sizeof(PrimitiveT)[%lu]*row_stride[%d];\n\n",required_size_in_bytes,sizeof(PrimitiveT),row_stride);
      
        // Add additional cachelines based on the requested padding units
        // to avoid cache set associativity conflicts
        size_t padding_offset = m_buffer_offset.units()*64;
        required_size_in_bytes += padding_offset;
        #ifdef __MIC__
            // due to the faulting nature of unaligned masked load/stores
            // the compiler will generate gather/scatters for safety.
            // As access outside paged memory will fault even if masked.
            // However if we can guarantee that the memory will not be
            // outside paged memory, then compiler can safely used the
            // masked load/stores.
            // We can guarantee access to this container are always in
            // paged memory by adding a cache line's worth of padding to
            // the end.
            // To instruct the compiler to generate code to take advantage
            // of this, add the "-opt-assume-safe-padding" command line
            // option.
            const int safe_padding = 64;
            required_size_in_bytes += safe_padding;
        #endif
        m_size_in_bytes = required_size_in_bytes;

        m_buffer = reinterpret_cast<unsigned char *>(m_allocator->allocate(m_size_in_bytes));
        if (m_buffer == nullptr)
        {
            std::cout << std::endl << ">>>>>>>>>>>>>>>>>>>>>>Failed to allocate " << m_size_in_bytes << " bytes...." << std::endl;
            std::cout <<              ">>>>>>>>>>>>>>>>>>>>>>Terminating Application" << std::endl;
            std::terminate();
        }

        data = reinterpret_cast<unsigned char *>(m_buffer) + padding_offset;
    }
    __SDLT_ASSERT(internal::is_aligned<64>(data));

    m_extents = a_new_extents;

    m_organization = builder::build(m_extents, data);

}

namespace internal {
    // Allow an n_extent_generator to be used by specializing to use the generator's value_type
    // just saving user from having to deal with it.
    template<
        typename PrimitiveT,
        typename LayoutT,
        typename ExtentsT,
        class AllocatorT
    >
    struct specialize_n_container
    {
        typedef n_container<PrimitiveT, LayoutT, ExtentsT, AllocatorT> type;
    };

    template<
        typename PrimitiveT,
        typename LayoutT,
        class... TypesT,
        class AllocatorT
    >
    struct specialize_n_container<PrimitiveT, LayoutT, n_extent_generator<TypesT...>, AllocatorT>
    {
        typedef n_container<PrimitiveT, LayoutT, typename n_extent_generator<TypesT...>::value_type, AllocatorT> type;
    };

    template<
        typename PrimitiveT,
        typename LayoutT,
        class... TypesT,
        class AllocatorT
    >
    struct specialize_n_container<PrimitiveT, LayoutT, const n_extent_generator<TypesT...>, AllocatorT>
    {
        typedef n_container<PrimitiveT, LayoutT, typename n_extent_generator<TypesT...>::value_type, AllocatorT> type;
    };
} // namespace internal
} // namespace __SDLT_IMPL

template<
    typename PrimitiveT,
    typename LayoutT,
    typename ExtentsT,
    class AllocatorT = allocator::default_alloc
>
using n_container = typename __SDLT_IMPL::internal::specialize_n_container<PrimitiveT, LayoutT, ExtentsT, AllocatorT>::type;

namespace __SDLT_IMPL {
template<
    typename PrimitiveT,
    typename LayoutT,
    class AllocatorT = sdlt::allocator::default_alloc,
    typename ExtentsT
>
SDLT_INLINE auto
make_n_container(const ExtentsT &i_extents)
->sdlt::n_container<PrimitiveT, LayoutT, ExtentsT, AllocatorT>
{
    return sdlt::n_container<PrimitiveT, LayoutT, ExtentsT, AllocatorT>(i_extents);
}
} // namespace __SDLT_IMPL
using __SDLT_IMPL::make_n_container;

} // namespace sdlt

#endif // SDLT_N_CONTAINER_H
