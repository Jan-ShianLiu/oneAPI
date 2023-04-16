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

#ifndef SDLT_SOA_1D_CONTAINER_H
#define SDLT_SOA_1D_CONTAINER_H

#include <algorithm>
#include <iterator>
#include <limits>
#include <cstddef>
#include <vector>
#include <utility>

#include "common.h"

#include "aligned.h"
#include "allocator/default.h"
#include "aos1d_const_accessor.h"
#include "buffer_offset_in_cachelines.h"
#include "fixed.h"
#include "internal/capacity.h"
#include "internal/enable_if_type.h"
#include "internal/post_offset_calculator.h"
#include "internal/pre_offset_calculator.h"
#include "internal/is_linear_compatible_index.h"
#include "internal/linear/const_stl_iterator.h"
#include "internal/linear/element_mover.h"
#include "internal/linear/stl_iterator.h"
#include "internal/soa1d/accessor.h"
#include "internal/soa1d/const_accessor.h"
#include "internal/soa1d/const_element.h"
#include "internal/soa1d/element.h"
#include "internal/soa1d/organization.h"
#include "no_offset.h"

namespace sdlt {
namespace __SDLT_IMPL {

__SDLT_INHERIT_IMPL_BEGIN

template<
    typename PrimitiveT,
    int AlignD1OnIndexT = 0,
    class AllocatorT = allocator::default_alloc
>
class soa1d_container
: private internal::soa1d::organization
{
public:
    typedef PrimitiveT primitive_type;
    static const int align_d1_on_index = AlignD1OnIndexT;
    typedef AllocatorT allocator_type;
    typedef size_t size_type;

    template <typename OffsetT = no_offset> using accessor = internal::soa1d::accessor<PrimitiveT, AlignD1OnIndexT, OffsetT>;
    template <typename OffsetT = no_offset> using const_accessor = internal::soa1d::const_accessor<PrimitiveT, AlignD1OnIndexT, OffsetT>;

    // Support move constructor and move assignment
    // Allows swap to function efficiently
    SDLT_INLINE soa1d_container(soa1d_container&& donor);
    SDLT_INLINE soa1d_container & operator=(soa1d_container&& donor);

    // Disallow copy construction, we don't want anyone copying these around
    // by value, its a bit too dangerous with lambda functions to be able
    // to capture one of these containers by value, instead users should call
    // the clone method explicitly
    soa1d_container(const soa1d_container &) = delete;
    soa1d_container & operator = (const soa1d_container &) = delete;
    SDLT_NOINLINE soa1d_container clone() const;

    SDLT_INLINE soa1d_container(
        size_type size_d1 = static_cast<size_type>(0),
        buffer_offset_in_cachelines buffer_offset = buffer_offset_in_cachelines(0),
        const allocator_type & an_allocator = allocator_type());

    SDLT_INLINE soa1d_container(
        size_type size_d1,
        const PrimitiveT &a_value,
        buffer_offset_in_cachelines buffer_offset = buffer_offset_in_cachelines(0),
        const allocator_type & an_allocator = allocator_type());

    template<typename Stlallocator_type>
    SDLT_INLINE soa1d_container(
        const std::vector<PrimitiveT, Stlallocator_type> &other,
        buffer_offset_in_cachelines buffer_offset = buffer_offset_in_cachelines(0),
        const allocator_type & an_allocator = allocator_type());

    SDLT_INLINE soa1d_container(
        const PrimitiveT *other_array, size_type number_of_elements,
        buffer_offset_in_cachelines buffer_offset = buffer_offset_in_cachelines(0),
        const allocator_type & an_allocator = allocator_type());

    template< typename IteratorT >
    SDLT_INLINE soa1d_container(
        IteratorT a_begin,
        IteratorT an_end,
        buffer_offset_in_cachelines buffer_offset = buffer_offset_in_cachelines(0),
        const allocator_type & an_allocator = allocator_type());

    SDLT_NOINLINE ~soa1d_container();

    SDLT_INLINE const allocator_type & get_allocator() const;

    SDLT_INLINE int get_size_d1() const;

    SDLT_NOINLINE accessor<>
    access();

    SDLT_NOINLINE accessor<int>
    access(int offset);

    template<int IndexAlignmentT>
    SDLT_NOINLINE  accessor<aligned<IndexAlignmentT> >
    access(aligned<IndexAlignmentT> offset);

    template<int OffsetT>
    SDLT_NOINLINE accessor<fixed<OffsetT> >
    access(fixed<OffsetT>);


    SDLT_NOINLINE const_accessor<>
    const_access() const;

    SDLT_NOINLINE const_accessor<int>
    const_access(int offset) const;

    template<int IndexAlignmentT>
    SDLT_NOINLINE  const_accessor<aligned<IndexAlignmentT> >
    const_access(aligned<IndexAlignmentT> offset) const;

    template<int OffsetT>
    SDLT_NOINLINE const_accessor<fixed<OffsetT> >
    const_access(fixed<OffsetT>) const;

    // Conversion operator for passing Container as function that takes accessor parameters
    SDLT_INLINE operator accessor<>();
    SDLT_INLINE operator const_accessor<>();

#if 0
    SDLT_INLINE auto operator [](const int an_index_d1) const -> decltype(const_accessor<>(nullptr,0,0)[linear_index(an_index_d1)]);
    SDLT_INLINE auto operator [](const int an_index_d1) -> decltype(accessor<>(nullptr,0,0)[linear_index(an_index_d1)]);
#endif

    template<typename IndexD1T, typename = internal::enable_if_type<internal::is_linear_compatible_index<IndexD1T>::value>>
    SDLT_INLINE auto operator [](const IndexD1T an_index_d1) const -> decltype(const_accessor<>(nullptr,0,0)[an_index_d1]);
    template<typename IndexD1T, typename = internal::enable_if_type<internal::is_linear_compatible_index<IndexD1T>::value>>
    SDLT_INLINE auto operator [](const IndexD1T an_index_d1)-> decltype(accessor<>(nullptr,0,0)[an_index_d1]);


    // stl iterator compatibility
    // NOTE operator-> is not supported from iterators
    typedef PrimitiveT value_type;
    typedef internal::linear::const_stl_iterator< const_accessor<> > const_iterator;
    typedef internal::linear::stl_iterator< accessor<> > iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;

    // NOTE: a copy of the primitive, not a reference, is returned
    // as the storage layout of the primitive doesn't match we cant
    // allow direct access

    // NOTE: const object prevents rvalue assignment for structs,
    // better protection that nothing
    SDLT_INLINE PrimitiveT const front() const;
    SDLT_INLINE PrimitiveT const back() const;
    SDLT_INLINE PrimitiveT const at(size_type an_index) const;

    // choose not to support data() as it would only be valid for AOS
    // and deem it not useful to create a local array and export all values
    // to it for compatibility with this function
    // value_type * data();

    SDLT_INLINE const_iterator cbegin() const;
    SDLT_INLINE const_iterator cend() const;
    SDLT_INLINE const_iterator begin() const;
    SDLT_INLINE const_iterator end() const;
    SDLT_INLINE iterator begin();
    SDLT_INLINE iterator end();
    SDLT_INLINE const_reverse_iterator crbegin() const;
    SDLT_INLINE const_reverse_iterator crend() const;
    SDLT_INLINE const_reverse_iterator rbegin() const;
    SDLT_INLINE const_reverse_iterator rend() const;
    SDLT_INLINE reverse_iterator rbegin();
    SDLT_INLINE reverse_iterator rend();

    // std::vector capacity member function compatibility
    SDLT_INLINE size_type size() const;
    SDLT_INLINE size_type max_size() const;
    SDLT_INLINE size_type capacity() const;
    SDLT_INLINE bool empty() const;

    SDLT_INLINE void reserve(size_type minimum_capacity);
    SDLT_INLINE void resize(size_type new_size_d1);
    SDLT_INLINE void shrink_to_fit();


    // std::vector Modifier member function compatibility
    SDLT_INLINE void assign(size_type new_size, const PrimitiveT & a_value);
    template< typename IteratorT >
    SDLT_INLINE void assign(IteratorT a_begin, const IteratorT an_end);
    SDLT_INLINE void push_back(const PrimitiveT & a_value);
    SDLT_INLINE void pop_back();
    SDLT_INLINE void clear();

    SDLT_NOINLINE void insert(int an_index, const PrimitiveT & a_value);
    SDLT_NOINLINE iterator insert(const iterator & iterator_pos, const PrimitiveT & a_value);
    SDLT_NOINLINE void insert(const iterator & iterator_pos, size_type count, const PrimitiveT & a_value);
    template< typename IteratorT >
    SDLT_NOINLINE void insert(const iterator & iterator_pos, IteratorT a_begin, const IteratorT an_end);
    template <typename... ArgsT>
    SDLT_INLINE iterator emplace (const_iterator position, ArgsT&&... args);
    template <typename... ArgsT>
    SDLT_INLINE void emplace_back(ArgsT&&... args);

    SDLT_NOINLINE iterator erase(const iterator & iterator_pos);
    SDLT_NOINLINE iterator erase(const iterator & start_at, const iterator & end_before);
    SDLT_INLINE void swap(soa1d_container &other);

    // std::vector relational operator compatibility
    SDLT_NOINLINE bool operator == (const soa1d_container &other) const;
    SDLT_NOINLINE bool operator != (const soa1d_container &other) const;
    // didn't bother with <, >, <=, >= operators, if someone has a valid
    // scenario requiring them, please request their support

    struct offload_digest
    {
        size_t array_size;
        size_t data_offset_in_array;
        int strideD1;
        int size_d1;
    };

    offload_digest get_offload_digest();
    SDLT_INLINE unsigned char * & get_offload_array();
    // Only for use when offloading, the user may offload the underlying linear
    // array + digest to a device, then build an accessor on the device side
    static accessor<> access(
        unsigned char *an_offload_array,
        const offload_digest &a_digest
    );

protected:  // member functions
    SDLT_INLINE soa1d_container(
        const int size_d1,
        buffer_offset_in_cachelines buffer_offset,
        const allocator_type * an_allocator);

    SDLT_INLINE soa1d_container(
        const internal::capacity a_capacity,
        const int size_d1,
        buffer_offset_in_cachelines buffer_offset,
        const allocator_type * an_allocator);

    SDLT_NOINLINE void do_reserve(internal::capacity new_capacity);
    SDLT_NOINLINE void do_allocation(internal::capacity new_capacity);

    typedef internal::linear::element_mover<soa1d_container> element_mover;

protected: // member data
    buffer_offset_in_cachelines m_buffer_offset;
    internal::capacity m_capacity;
    // Make sure no one changes the size in bytes after we set it
    size_t m_size_in_bytes;
    unsigned char * m_buffer;
    unsigned char * m_data; // can be offset into the buffer for alignment purposes
    const allocator_type * m_allocator;
};

__SDLT_INHERIT_IMPL_END









////////////////////////////////////////////////////////////////////////////////
// Implementation below
////////////////////////////////////////////////////////////////////////////////







template<typename PrimitiveT, int AlignD1OnIndexT, class allocator_type>
soa1d_container<PrimitiveT, AlignD1OnIndexT, allocator_type>::
soa1d_container(
    const int size_d1,
    buffer_offset_in_cachelines buffer_offset,
    const allocator_type * an_allocator)
: organization(size_d1, 0)
, m_buffer_offset(buffer_offset)
, m_capacity(0)
, m_size_in_bytes(0)
, m_buffer(nullptr)
, m_data(nullptr)
, m_allocator(an_allocator)
{
    this->do_allocation(internal::capacity(this->m_size_d1));
    __SDLT_ASSERT(internal::is_aligned<64>(m_data));
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
soa1d_container(
    const internal::capacity a_capacity,
    const int size_d1,
    buffer_offset_in_cachelines buffer_offset,
    const AllocatorT * an_allocator)
: organization(size_d1, 0)
, m_buffer_offset(buffer_offset)
, m_capacity(0)
, m_size_in_bytes(0)
, m_buffer(nullptr)
, m_data(nullptr)
, m_allocator(an_allocator)
{
    this->do_allocation(a_capacity);
    __SDLT_ASSERT(internal::is_aligned<64>(m_data));
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
soa1d_container(soa1d_container&& donor)
: organization(std::move(donor))
, m_buffer_offset(donor.m_buffer_offset)
, m_capacity(donor.m_capacity)
, m_size_in_bytes(donor.m_size_in_bytes)
, m_buffer(donor.m_buffer)
, m_data(donor.m_data)
, m_allocator(donor.m_allocator)
{
    __SDLT_ASSERT(internal::is_aligned<64>(m_data));

    // Cleanup the donor just enough to avoid deallocation
    donor.m_size_in_bytes = 0;
    donor.m_buffer = nullptr;
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT> &
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
operator=(soa1d_container&& donor)
{
    if (m_buffer != nullptr)
    {
        m_allocator->free(m_buffer, m_size_in_bytes);
    }

    static_cast<organization &>(*this) = std::move(donor);
    m_buffer_offset = donor.m_buffer_offset;
    m_capacity = donor.m_capacity;
    m_size_in_bytes = donor.m_size_in_bytes;
    m_buffer = donor.m_buffer;
    m_data = donor.m_data;
    m_allocator = donor.m_allocator;
    // Cleanup the donor just enough to avoid deallocation
    donor.m_size_in_bytes = 0;
    donor.m_buffer = nullptr;
    return *this;
}


template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
soa1d_container(
    size_type size_d1,
    buffer_offset_in_cachelines buffer_offset,
    const AllocatorT & an_allocator)
: soa1d_container(size_d1, buffer_offset, &an_allocator)
{
    __SDLT_ASSERT(size_d1 < max_size());
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
soa1d_container(
    size_type size_d1,
    const PrimitiveT &a_value,
    buffer_offset_in_cachelines buffer_offset,
    const AllocatorT & an_allocator)
: soa1d_container(size_d1, buffer_offset, &an_allocator)
{
    __SDLT_ASSERT(size_d1 < max_size());
    element_mover::fill_with(access(), a_value, 0, this->get_size_d1());
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
template<typename StlAllocatorT>
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
soa1d_container(
    const std::vector<PrimitiveT, StlAllocatorT> &other,
    buffer_offset_in_cachelines buffer_offset,
    const AllocatorT & an_allocator)
: soa1d_container(static_cast<int>(other.size()), buffer_offset, &an_allocator)
{
    __SDLT_ASSERT(other.size() < max_size());
    aos1d_const_accessor<PrimitiveT, access_by_stride> source(other);
    element_mover::copy_all_values_from(source, access());
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
soa1d_container(
    const PrimitiveT *other_array, size_type number_of_elements,
    buffer_offset_in_cachelines buffer_offset,
    const AllocatorT & an_allocator)
: soa1d_container(number_of_elements, buffer_offset, &an_allocator)
{
    __SDLT_ASSERT(number_of_elements < max_size());
    aos1d_const_accessor<PrimitiveT, access_by_stride> source(other_array, number_of_elements);
    element_mover::copy_all_values_from(source, access());
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
template< typename IteratorT >
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
soa1d_container(IteratorT a_begin, const IteratorT an_end,
    buffer_offset_in_cachelines buffer_offset,
    const AllocatorT & an_allocator)
: soa1d_container(std::distance(a_begin, an_end), buffer_offset, &an_allocator)
{
    __SDLT_ASSERT(static_cast<size_type>(std::distance(a_begin, an_end)) < max_size());
    element_mover::copy_all_values_from(a_begin,an_end, 0, this->get_size_d1(), access());
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
clone() const
{
    soa1d_container dest_container(this->get_size_d1(), m_buffer_offset, m_allocator);
    auto source = const_access();
    element_mover::copy_all_values_from(source, dest_container.access());
    return dest_container;
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
~soa1d_container()
{
    if (m_buffer != nullptr)
    {
        m_allocator->free(m_buffer, m_size_in_bytes);
    }
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
const AllocatorT &
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
get_allocator() const
{
    return *m_allocator;
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
int
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
get_size_d1() const
{
    return this->m_size_d1;
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
unsigned char * &
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
get_offload_array()
{
    return this->m_buffer;
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
access() -> accessor<>
{
    return accessor<>(this->m_data,
                    this->m_size_d1,
                    this->m_stride_d1);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
access(const int offset)
->accessor<int>
{
    return accessor<int>(
        this->m_data,
        this->m_size_d1,
        this->m_stride_d1,
        offset);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
template<int IndexAlignmentT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
access(aligned<IndexAlignmentT> offset)
->accessor< aligned<IndexAlignmentT> >
{
    return accessor< aligned<IndexAlignmentT> >(
        this->m_data,
        this->m_size_d1,
        this->m_stride_d1,
        offset);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
template<int OffsetT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
access(fixed<OffsetT>)
->accessor< fixed<OffsetT> >
{
    return accessor< fixed<OffsetT> >(
        this->m_data,
        this->m_size_d1,
        this->m_stride_d1);
}


template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
const_access() const -> const_accessor<>
{
    return const_accessor<>(this->m_data,
                    this->m_size_d1,
                    this->m_stride_d1);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
const_access(const int offset) const
->const_accessor<int>
{
    return const_accessor<int>(
        this->m_data,
        this->m_size_d1,
        this->m_stride_d1,
        offset);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
template<int IndexAlignmentT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
const_access(aligned<IndexAlignmentT> offset) const
->const_accessor< aligned<IndexAlignmentT> >
{
    return const_accessor< aligned<IndexAlignmentT> >(
        this->m_data,
        this->m_size_d1,
        this->m_stride_d1,
        offset);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
template<int OffsetT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
const_access(fixed<OffsetT>) const
->const_accessor< fixed<OffsetT> >
{
    return const_accessor< fixed<OffsetT> >(
        this->m_data,
        this->m_size_d1,
        this->m_stride_d1);
}


#if 0
template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
operator [](const int an_index_d1) const
-> decltype(const_accessor<>(nullptr,0,0)[linear_index(an_index_d1)])
{
    __SDLT_ASSERT(an_index_d1 >= 0);
    __SDLT_ASSERT(an_index_d1 < this->get_size_d1());

    linear_index xindex(an_index_d1);

    return internal::soa1d::const_element<PrimitiveT, AlignD1OnIndexT, decltype(xindex) >(m_data, *this, xindex);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
operator [](const int an_index_d1)
-> decltype(accessor<>(nullptr,0,0)[linear_index(an_index_d1)])
{
    __SDLT_ASSERT(an_index_d1 >= 0);
    __SDLT_ASSERT(an_index_d1 < this->get_size_d1());

    linear_index xindex(an_index_d1);

    return internal::soa1d::element<PrimitiveT, AlignD1OnIndexT, linear_index >(m_data, *this, xindex);
}
#endif

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
template<typename IndexD1T, typename /* is_linear_compatible_index */>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
operator [](const IndexD1T an_index_d1) const
-> decltype(const_accessor<>(nullptr,0,0)[an_index_d1])
{
    __SDLT_ASSERT(internal::value_of(an_index_d1) >= 0);
    __SDLT_ASSERT(internal::value_of(an_index_d1) < this->get_size_d1());
    return internal::soa1d::const_element<PrimitiveT, AlignD1OnIndexT, IndexD1T>(m_data, *this, an_index_d1);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
template<typename IndexD1T, typename /* is_linear_compatible_index */>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
operator [](const IndexD1T an_index_d1)
-> decltype(accessor<>(nullptr,0,0)[an_index_d1])
{
    __SDLT_ASSERT(std::ptrdiff_t(internal::value_of(an_index_d1)) >= 0);
    __SDLT_ASSERT(std::size_t(internal::value_of(an_index_d1)) < std::size_t(this->get_size_d1()));
    return internal::soa1d::element<PrimitiveT, AlignD1OnIndexT, IndexD1T>(m_data, *this, an_index_d1);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
operator accessor<> ()
{
    return access();
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
operator const_accessor<> ()
{
    return const_access();
}


template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
get_offload_digest() -> offload_digest
{
    offload_digest digest;
    digest.array_size = this->m_size_in_bytes;
    digest.data_offset_in_array = this->m_data - this->m_buffer;
    digest.strideD1 = this->m_stride_d1;
    digest.size_d1 = this->m_size_d1;
    return digest;
}

// Only for use when offloading, the user may offload the underlying linear
// array + digest to a device, then build an accessor on the device side
template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
access(
    unsigned char *an_offload_array,
    const offload_digest &a_digest
) -> accessor<>
{
    #if SDLT_DEBUG
        size_t required_size = sizeof(PrimitiveT)*a_digest.strideD1;
        __SDLT_ASSERT(a_digest.array_size >= required_size);
    #endif
    unsigned char *data = (an_offload_array + a_digest.data_offset_in_array);

    return accessor<>(
        data,
        a_digest.size_d1,
        a_digest.strideD1);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
void
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
do_reserve(internal::capacity new_capacity)
{
    // At this point we have determined we need a new allocation
    // keep it simple by just creating a new instance and swapping
    // with it.
    soa1d_container dest_container(new_capacity, this->get_size_d1(), m_buffer_offset, m_allocator);
    auto source = const_access();
    element_mover::copy_all_values_from(source, dest_container.access());
    std::swap(*this, dest_container);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
size() const
->size_type
{
    return get_size_d1();
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
max_size() const
->size_type
{
    return std::numeric_limits<int>::max();
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
capacity() const
->size_type
{
    return m_capacity.count();
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
bool
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
empty() const
{
    return get_size_d1() == 0;
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
void
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
reserve(size_type minimum_capacity)
{
    __SDLT_ASSERT(minimum_capacity < max_size());
    if (static_cast<int>(minimum_capacity) > m_capacity.count()) {
        do_reserve(internal::capacity(minimum_capacity));
    }
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
void
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
resize(size_type new_size_d1)
{
    __SDLT_ASSERT(new_size_d1 < max_size());
    if (capacity() < new_size_d1) {
        do_reserve(internal::capacity(new_size_d1));
    }
    this->m_size_d1 = new_size_d1;
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
void
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
shrink_to_fit()
{
    if (m_capacity.count() > this->m_size_d1)
    {
        do_reserve(internal::capacity(this->m_size_d1));
    }
}


template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
void
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
assign(size_type new_size,
            const PrimitiveT & a_value)
{
    __SDLT_ASSERT(new_size < max_size());
    resize(new_size);
    element_mover::fill_with(access(), a_value, 0, this->get_size_d1());
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
template< typename IteratorT >
void
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
assign(IteratorT a_begin, const IteratorT an_end)
{
    size_type distance = std::distance(a_begin, an_end);
    __SDLT_ASSERT(distance < max_size());
    size_type new_size = static_cast<size_type>(distance);
    resize(new_size);

    element_mover::copy_all_values_from(a_begin,an_end, 0, this->get_size_d1(), access());
}


template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
void
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
push_back(const PrimitiveT & a_value)
{
    int index_to_assign = this->m_size_d1;
    int post_size_d1 = index_to_assign + 1;
    if (post_size_d1 >= m_capacity.count()) {
        do_reserve(internal::capacity(2*std::max(1,m_capacity.count())));
    }
    __SDLT_ASSERT(m_capacity.count() > post_size_d1);

    // need to set the size before trying to use an accessor
    // because of boundary checks against the index
    this->m_size_d1 = post_size_d1;
    access()[index_to_assign] = a_value;
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
void
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
pop_back()
{
    __SDLT_ASSERT(this->m_size_d1 > 0);
    --this->m_size_d1;
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
void
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
clear()
{
    // Primitives are supposed to be Plain Old data_type and not have any complicated
    // destructors
    // We will assume this an not bother calling any destructors
    this->m_size_d1 = 0;
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
void
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
insert(int an_index, const PrimitiveT & a_value)
{
    __SDLT_ASSERT(an_index >= 0);
    // Could be inserting at the very end
    __SDLT_ASSERT(an_index <= get_size_d1());

    const int new_element_count = 1;
    int post_size_d1 = get_size_d1() + new_element_count;
    if (post_size_d1 >= m_capacity.count()) {
        do_reserve(internal::capacity(2*std::max(1,m_capacity.count())));
    }
    __SDLT_ASSERT(m_capacity.count() > post_size_d1);

    // need to set the size before trying to use an accessor
    // because of boundary checks against the index
    this->m_size_d1 = post_size_d1;

    // First we need to shift all the elements up
    element_mover::shift_elements_forwards(access(), an_index, new_element_count);

    access()[an_index] = a_value;
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
insert(const iterator & iterator_pos, const PrimitiveT & a_value)
-> iterator
{
    __SDLT_ASSERT(iterator_pos.m_accessor == access());
    insert(iterator_pos.m_index, a_value);
    return iterator(access(), iterator_pos.m_index);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
template <typename... ArgsT>
typename soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::iterator
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
emplace (const_iterator iterator_pos, ArgsT&&... iArgs)
{
    __SDLT_ASSERT(iterator_pos.m_accessor == const_access());
    PrimitiveT value(std::forward<ArgsT>(iArgs)...);
    insert(iterator_pos.m_index, value);
    return iterator(access(), iterator_pos.m_index);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
template <typename... ArgsT>
void
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
emplace_back(ArgsT&&... iArgs)
{
    int index_to_assign = this->m_size_d1;
    int post_size_d1 = index_to_assign + 1;
    if (post_size_d1 >= m_capacity.count()) {
        do_reserve(internal::capacity(2*std::max(1,m_capacity.count())));
    }
    __SDLT_ASSERT(m_capacity.count() > post_size_d1);

    // need to set the size before trying to use an accessor
    // because of boundary checks against the index
    this->m_size_d1 = post_size_d1;
    PrimitiveT value(std::forward<ArgsT>(iArgs)...);
    access()[index_to_assign] = value;
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
void
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
insert(const iterator & iterator_pos, size_type count, const PrimitiveT & a_value)
{
    __SDLT_ASSERT(count < max_size());
    __SDLT_ASSERT(iterator_pos.m_accessor == access());
    __SDLT_ASSERT(iterator_pos.m_index >= 0);
    // Could be inserting at the very end
    __SDLT_ASSERT(iterator_pos.m_index <= get_size_d1());

    const int index_to_start_at = iterator_pos.m_index;
    const int new_element_count = count;

    __SDLT_ASSERT((size() + count) < max_size());
    int post_size_d1 = get_size_d1() + new_element_count;
    if (post_size_d1 >= m_capacity.count()) {
        do_reserve(internal::capacity(std::max(new_element_count, 2*std::max(1,m_capacity.count()))));
    }
    __SDLT_ASSERT(m_capacity.count() > post_size_d1);

    // need to set the size before trying to use an accessor
    // because of boundary checks against the index
    this->m_size_d1 = post_size_d1;

    // First we need to shift all the elements up
    element_mover::shift_elements_forwards(access(), index_to_start_at, new_element_count);

    element_mover::fill_with(access(), a_value, index_to_start_at, index_to_start_at + new_element_count);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
template< typename IteratorT >
void
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
insert(const iterator & iterator_pos, IteratorT a_begin, const IteratorT an_end)
{
    __SDLT_ASSERT(iterator_pos.m_accessor == access());
    __SDLT_ASSERT(iterator_pos.m_index >= 0);
    // Could be inserting at the very end
    __SDLT_ASSERT(iterator_pos.m_index <= get_size_d1());

    const int index_to_start_at = iterator_pos.m_index;
    const int new_element_count = std::distance(a_begin, an_end);

    int post_size_d1 = get_size_d1() + new_element_count;
    if (post_size_d1 >= m_capacity.count()) {
        do_reserve(internal::capacity(std::max(new_element_count, 2*std::max(1,m_capacity.count()))));
    }
    __SDLT_ASSERT(m_capacity.count() > post_size_d1);

    // need to set the size before trying to use an accessor
    // because of boundary checks against the index
    this->m_size_d1 = post_size_d1;

    // First we need to shift all the elements up
    element_mover::shift_elements_forwards(access(), index_to_start_at, new_element_count);

    element_mover::copy_all_values_from(a_begin,an_end, index_to_start_at, index_to_start_at + new_element_count, access());
}



template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
erase(const iterator & iterator_pos)
-> iterator
{
    __SDLT_ASSERT(iterator_pos.m_accessor == access());
    __SDLT_ASSERT(iterator_pos.m_index >= 0);
    __SDLT_ASSERT(iterator_pos.m_index < get_size_d1());

    const int remove_element_count = 1;
    int post_size_d1 = get_size_d1() - remove_element_count;
    __SDLT_ASSERT(m_capacity.count() > post_size_d1);

    // First we need to shift all the elements down
    element_mover::shift_elements_backwards(access(), iterator_pos.m_index, remove_element_count);

    this->m_size_d1 = post_size_d1;

    // return iterator to new place of element that followed the
    // last element erased, which for this case is the original iterator
    return iterator(access(), iterator_pos.m_index);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
erase(const iterator & start_at, const iterator & end_before)
-> iterator
{
    __SDLT_ASSERT(start_at.m_accessor == access());
    __SDLT_ASSERT(start_at.m_accessor == end_before.m_accessor);
    __SDLT_ASSERT(start_at.m_index >= 0);
    __SDLT_ASSERT(start_at.m_index <= get_size_d1());
    __SDLT_ASSERT(end_before.m_index >= 0);
    __SDLT_ASSERT(end_before.m_index <= get_size_d1());
    __SDLT_ASSERT(end_before.m_index >= start_at.m_index);

    const int remove_element_count = end_before.m_index - start_at.m_index;
    int post_size_d1 = get_size_d1() - remove_element_count;
    __SDLT_ASSERT(m_capacity.count() > post_size_d1);

    // First we need to shift all the elements down
    element_mover::shift_elements_backwards(access(), start_at.m_index, remove_element_count);

    this->m_size_d1 = post_size_d1;

    // return iterator to new place of element that followed the
    // last element erased, which for this case is the original iterator
    return iterator(access(), start_at.m_index);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
void
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
swap(soa1d_container &other)
{
    std::swap(*this, other);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
PrimitiveT const
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
front() const
{
    return const_access()[0];
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
PrimitiveT const
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
back() const
{
    int lastValidIndex = this->m_size_d1-1;
    return const_access()[lastValidIndex];
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
PrimitiveT const
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
at(size_type an_index) const
{
    __SDLT_ASSERT(an_index < size());
    return const_access()[an_index];
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
cbegin() const
-> const_iterator
{
    return const_iterator(const_access(), 0);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
cend() const
-> const_iterator
{
    return const_iterator(const_access(), get_size_d1());
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
begin() const
-> const_iterator
{
    return cbegin();
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
end() const
-> const_iterator
{
    return cend();
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
begin()
-> iterator
{
    return iterator(access(), 0);
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
end()
-> iterator
{
    return iterator(access(), get_size_d1());
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
crbegin() const
-> const_reverse_iterator
{
    return const_reverse_iterator(cend());
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
crend() const
-> const_reverse_iterator
{
    return const_reverse_iterator(cbegin());
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
rbegin() const
-> const_reverse_iterator
{
    return crbegin();
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
rend() const
-> const_reverse_iterator
{
    return crend();
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
rbegin()
-> reverse_iterator
{
    return reverse_iterator(end());
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
auto
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
rend()
-> reverse_iterator
{
    return reverse_iterator(begin());
}



// std::vector relational operator compatibility

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
bool
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
operator == (const soa1d_container &other) const
{
    SDLT_INLINE_BLOCK
    {
        if (size() != other.size())
            return false;
        return std::equal(cbegin(), cend(), other.begin());
    }
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
bool
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
operator != (const soa1d_container &other) const
{
    SDLT_INLINE_BLOCK
    {
        return false == (*this == other);
    }
}

template<typename PrimitiveT, int AlignD1OnIndexT, class AllocatorT>
void
soa1d_container<PrimitiveT, AlignD1OnIndexT, AllocatorT>::
do_allocation(internal::capacity new_capacity)
{
    if (new_capacity.count() != 0)
    {
        const int byte_alignment = 64;

//        printf("size of smallest member=%d, size of largest member=%d\n",
//                sizeof(sdlt::primitive<PrimitiveT>::smallest_builtin_type),
//                sizeof(sdlt::primitive<PrimitiveT>::largest_builtin_type));

        // Use the smallest data member to figure out how many pad elements
        // will be needed for an entire row to become aligned.
        // Larger built in types should be divisible in size (1,2,4,8),
        // thus larger data members will be aligned
        const int sizeof_smallest_member = sizeof(typename sdlt::primitive<PrimitiveT>::smallest_builtin_type);
        int bytes_past_boundary = (new_capacity.count()*sizeof_smallest_member)%byte_alignment;
        if (bytes_past_boundary == 0) {
            this->m_stride_d1 = new_capacity.count();
        } else {
            int bytes_to_next_boundary = byte_alignment - bytes_past_boundary;
            __SDLT_ASSERT(bytes_to_next_boundary%sizeof_smallest_member == 0);
            int elements_to_next_boundary = bytes_to_next_boundary/sizeof_smallest_member;

            this->m_stride_d1 = new_capacity.count() + elements_to_next_boundary;
        }
        __SDLT_ASSERT(this->m_stride_d1*sizeof_smallest_member%byte_alignment == 0);


        size_t required_size_in_bytes = sizeof(PrimitiveT)*this->m_stride_d1;
        //printf("\n\nrequiredSizeInBytes[%lu] = sizeof(PrimitiveT)[%lu]*this->m_stride_d1[%d];\n\n",required_size_in_bytes,sizeof(PrimitiveT),this->m_stride_d1);

        __SDLT_ASSERT(this->m_stride_d1*sizeof_smallest_member%byte_alignment == 0);

        if (AlignD1OnIndexT > 0)
        {
            // Add some padding on so we can shift start of the
            // data inside the buffer such that accessing
            // AlignD1OnIndexT will be on a alignment boundary
            static_assert((AlignD1OnIndexT == 0) || (sizeof_smallest_member == sizeof(typename sdlt::primitive<PrimitiveT>::largest_builtin_type)), "Can only utilize AlignD1OnIndexT when all members of your primitive are the same size");
            m_size_in_bytes = required_size_in_bytes + byte_alignment;
        }
        else
        {
            m_size_in_bytes = required_size_in_bytes;
        }
        // Add additional cachelines based on the requested padding units
        // to avoid cache set associativity conflicts
        size_t padding_offset = m_buffer_offset.units()*64;
        m_size_in_bytes += padding_offset;
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
            m_size_in_bytes += safe_padding;
        #endif

        m_buffer = reinterpret_cast<unsigned char *>(m_allocator->allocate(m_size_in_bytes));

        m_data = reinterpret_cast<unsigned char *>(m_buffer) + padding_offset;
        __SDLT_ASSERT(internal::is_aligned<64>(m_data));
    }

    m_capacity = new_capacity;
}

} // namespace __SDLT_IMPL
using __SDLT_IMPL::soa1d_container;
} // namespace sdlt

#endif // SDLT_SOA_1D_CONTAINER_H
