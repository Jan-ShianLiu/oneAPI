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

#ifndef SDLT_ASA_1D_CONTAINER_H
#define SDLT_ASA_1D_CONTAINER_H

#include <algorithm>
#include <iterator>
#include <limits>
#include <vector>

#include "common.h"

#include "access_by.h"
#include "aligned.h"
#include "allocator/default.h"
#include "aos1d_const_accessor.h"
#include "buffer_offset_in_cachelines.h"
#include "defaults.h"
#include "fixed.h"
#include "internal/asa1d/accessor.h"
#include "internal/asa1d/const_accessor.h"
#include "internal/asa1d/const_element.h"
#include "internal/asa1d/const_stl_iterator.h"
#include "internal/asa1d/element.h"
#include "internal/asa1d/organization.h"
#include "internal/asa1d/stl_iterator.h"
#include "internal/capacity.h"
#include "internal/enable_if_type.h"
#include "internal/is_simd_compatible_index.h"
#include "internal/simd/element_mover.h"
#include "internal/simd/indices.h"
#include "internal/simd/iterator_index.h"
#include "no_offset.h"

namespace sdlt {
namespace __SDLT_IMPL {

__SDLT_INHERIT_IMPL_BEGIN

template<
    typename PrimitiveT,
    int SimdLaneCountT,
    int ByteAlignmentT = defaults<PrimitiveT>::byte_alignment,
    int AlignD1OnIndexT = 0,
    class AllocatorT = allocator::default_alloc
>
class asa1d_container
: private internal::asa1d::organization<AlignD1OnIndexT>
{
public:
    typedef PrimitiveT primitive_type;
    static const int simdlane_count = SimdLaneCountT;
    static const int align_d1_on_index = AlignD1OnIndexT;
    typedef typename primitive<PrimitiveT>::template simd_type<SimdLaneCountT, ByteAlignmentT> simd_data_type;
    typedef AllocatorT allocator_type;
    typedef size_t size_type;

    template <typename OffsetT = no_offset> using accessor = internal::asa1d::accessor<simd_data_type, AlignD1OnIndexT, OffsetT>;
    template <typename OffsetT = no_offset> using const_accessor = internal::asa1d::const_accessor<simd_data_type, AlignD1OnIndexT, OffsetT>;

    // Support move constructor and move assignment
    // Allows swap to function efficiently
    SDLT_INLINE asa1d_container(asa1d_container&& donor);
    SDLT_INLINE asa1d_container & operator=(asa1d_container&& donor);

    // Disallow copy construction, we don't want anyone copying these around
    // by value, its a bit too dangerous with lambda functions to be able
    // to capture one of these containers by value, instead users should call
    // the clone method explicitly
    asa1d_container(const asa1d_container &) = delete;
    asa1d_container & operator = (const asa1d_container &) = delete;
    SDLT_NOINLINE asa1d_container clone() const;

    SDLT_INLINE asa1d_container(
        size_type size_d1 = static_cast<size_type>(0),
        buffer_offset_in_cachelines buffer_offset = buffer_offset_in_cachelines(0),
        const allocator_type & an_allocator = allocator_type());

    SDLT_INLINE asa1d_container(
        size_type size_d1,
        const PrimitiveT &a_value,
        buffer_offset_in_cachelines buffer_offset = buffer_offset_in_cachelines(0),
        const allocator_type & an_allocator = allocator_type());

    template<typename Stlallocator_type>
    SDLT_INLINE asa1d_container(
        const std::vector<PrimitiveT, Stlallocator_type> &other,
        buffer_offset_in_cachelines buffer_offset = buffer_offset_in_cachelines(0),
        const allocator_type & an_allocator = allocator_type());

    SDLT_INLINE asa1d_container(
        const PrimitiveT *other_array, size_type number_of_elements,
        buffer_offset_in_cachelines buffer_offset = buffer_offset_in_cachelines(0),
        const allocator_type & an_allocator = allocator_type());

    template< typename IteratorT >
    SDLT_INLINE asa1d_container(
        IteratorT a_begin,
        IteratorT an_end,
        buffer_offset_in_cachelines buffer_offset = buffer_offset_in_cachelines(0),
        const allocator_type & an_allocator = allocator_type());

    SDLT_NOINLINE ~asa1d_container();

    SDLT_INLINE const allocator_type & get_allocator() const;

    SDLT_INLINE int get_size_d1() const;

    SDLT_INLINE accessor<>
    access();

    // NOTE: no integer variable base offset is allowed on the Asa
    // because we could not guarantee that only a single structure of fixed
    // sized arrays would be required during iteration

    // NOTE: IndexAlignmentT%SimdLaneCountT must be 0
    template<int IndexAlignmentT>
    SDLT_INLINE  accessor<aligned<IndexAlignmentT> >
    access(aligned<IndexAlignmentT> offset);

    // NOTE: OffsetT%SimdLaneCountT must be 0
    template<int OffsetT>
    SDLT_INLINE accessor<fixed<OffsetT> >
    access(fixed<OffsetT>);


    SDLT_INLINE const_accessor<>
    const_access() const;

    // NOTE: no integer variable base offset is allowed on the Asa
    // because we could not guarantee that only a single structure of fixed
    // sized arrays would be required during iteration

    // NOTE: IndexAlignmentT%SimdLaneCountT must be 0
    template<int IndexAlignmentT>
    SDLT_INLINE  const_accessor<aligned<IndexAlignmentT> >
    const_access(aligned<IndexAlignmentT> offset) const;

    // NOTE: OffsetT%SimdLaneCountT must be 0
    template<int OffsetT>
    SDLT_INLINE const_accessor<fixed<OffsetT> >
    const_access(fixed<OffsetT>) const;

    // Conversion operator for passing Container to functions
    SDLT_INLINE operator accessor<> ();
    SDLT_INLINE operator const_accessor<> ();


protected:
    typedef internal::simd::indices<simdlane_count, AlignD1OnIndexT> index_value_type;
    typedef internal::compound_index<index_value_type, internal::operation_none> index_from_integer_type;
    SDLT_INLINE index_from_integer_type make_index_from_integer(const int an_index) const;
public:
    SDLT_INLINE auto operator [](const int an_index_d1) const
        -> decltype(std::declval<asa1d_container>().const_access()[std::declval<asa1d_container>().make_index_from_integer(an_index_d1)]);

    SDLT_INLINE auto operator [](const int an_index_d1)
        -> decltype(std::declval<asa1d_container>().access()[std::declval<asa1d_container>().make_index_from_integer(an_index_d1)]);

    template<typename IndexD1T, typename = internal::enable_if_type<internal::is_simd_compatible_index<IndexD1T, simdlane_count>::value> >
    SDLT_INLINE auto operator [](const IndexD1T an_index_d1) const
        -> decltype(std::declval<asa1d_container>().const_access()[an_index_d1]);

    template<typename IndexD1T, typename = internal::enable_if_type<internal::is_simd_compatible_index<IndexD1T, simdlane_count>::value> >
    SDLT_INLINE auto operator [](const IndexD1T an_index_d1)
        -> decltype(std::declval<asa1d_container>().access()[an_index_d1]);


    // stl iterator compatibility
    // NOTE operator-> is not supported from iterators
    typedef PrimitiveT value_type;
    typedef internal::asa1d::const_stl_iterator< const_accessor<> > const_iterator;
    typedef internal::asa1d::stl_iterator< accessor<> > iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;

    // NOTE: a copy of the primitive, not a reference, is returned.
    // As the storage layout of the primitive doesn't match we can't
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

protected:
    typedef internal::simd::iterator_index<simdlane_count, AlignD1OnIndexT> iterator_index;

    SDLT_NOINLINE void insert(const iterator_index & a_simd_index, const PrimitiveT & a_value);
public:

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
    SDLT_INLINE void swap(asa1d_container &other);

    // std::vector relational operator compatibility
    SDLT_NOINLINE bool operator == (const asa1d_container &other) const;
    SDLT_NOINLINE bool operator != (const asa1d_container &other) const;
    // didn't bother with <, >, <=, >= operators, if someone has a valid
    // scenario requiring them, please request their support

    struct offload_digest
    {
        size_t array_size;
        size_t data_offset_in_array;
        int m_simd_block_stride_d1;
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
    SDLT_INLINE asa1d_container(
        const int size_d1,
        buffer_offset_in_cachelines buffer_offset,
        const allocator_type * an_allocator);

    SDLT_INLINE asa1d_container(
        const internal::capacity a_capacity,
        const int size_d1,
        buffer_offset_in_cachelines buffer_offset,
        const allocator_type * an_allocator);

    SDLT_NOINLINE void do_reserve(internal::capacity new_capacity);
    SDLT_NOINLINE void do_allocation(internal::capacity new_capacity);

    typedef internal::simd::element_mover<asa1d_container> element_mover;

protected: // member data
    buffer_offset_in_cachelines m_buffer_offset;
    internal::capacity m_capacity;
    // Make sure no one changes the size in bytes after we set it
    size_t m_size_in_bytes;
    unsigned char * m_buffer;
    simd_data_type * m_simd_data; // can be offset into the buffer for alignment purposes
    const allocator_type * m_allocator;
};


__SDLT_INHERIT_IMPL_END








////////////////////////////////////////////////////////////////////////////////
// Implementation below
////////////////////////////////////////////////////////////////////////////////







template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
asa1d_container(
    const int size_d1,
    buffer_offset_in_cachelines buffer_offset,
    const AllocatorT * an_allocator)
: internal::asa1d::organization<AlignD1OnIndexT>(size_d1, 0)
, m_buffer_offset(buffer_offset)
, m_capacity(0)
, m_size_in_bytes(0)
, m_buffer(nullptr)
, m_simd_data(nullptr)
, m_allocator(an_allocator)
{
    this->do_allocation(internal::capacity(this->m_size_d1));
    __SDLT_ASSERT(internal::is_aligned<64>(m_simd_data));
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
asa1d_container(
    const internal::capacity a_capacity,
    const int size_d1,
    buffer_offset_in_cachelines buffer_offset,
    const AllocatorT * an_allocator)
: internal::asa1d::organization<AlignD1OnIndexT>(size_d1, 0)
, m_buffer_offset(buffer_offset)
, m_capacity(0)
, m_size_in_bytes(0)
, m_buffer(nullptr)
, m_simd_data(nullptr)
, m_allocator(an_allocator)
{
    this->do_allocation(a_capacity);
    __SDLT_ASSERT(internal::is_aligned<64>(m_simd_data));
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
asa1d_container(asa1d_container&& donor)
: internal::asa1d::organization<AlignD1OnIndexT>(std::move(donor))
, m_buffer_offset(donor.m_buffer_offset)
, m_capacity(donor.m_capacity)
, m_size_in_bytes(donor.m_size_in_bytes)
, m_buffer(donor.m_buffer)
, m_simd_data(donor.m_simd_data)
, m_allocator(donor.m_allocator)
{
    __SDLT_ASSERT(internal::is_aligned<64>(m_simd_data));

    // Cleanup the donor just enough to avoid deallocation
    donor.m_size_in_bytes = 0;
    donor.m_buffer = nullptr;
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT> &
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
operator=(asa1d_container&& donor)
{
    if (m_buffer != nullptr)
    {
        m_allocator->free(m_buffer, m_size_in_bytes);
    }

    static_cast<internal::asa1d::organization<AlignD1OnIndexT> &>(*this) = std::move(donor);
    m_buffer_offset = donor.m_buffer_offset;
    m_capacity = donor.m_capacity;
    m_size_in_bytes = donor.m_size_in_bytes;
    m_buffer = donor.m_buffer;
    m_simd_data = donor.m_simd_data;
    m_allocator = donor.m_allocator;
    // Cleanup the donor just enough to avoid deallocation
    donor.m_size_in_bytes = 0;
    donor.m_buffer = nullptr;
    return *this;
}


template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
asa1d_container(
    size_type size_d1,
    buffer_offset_in_cachelines buffer_offset,
    const AllocatorT & an_allocator)
: asa1d_container(size_d1, buffer_offset, &an_allocator)
{
    __SDLT_ASSERT(size_d1 < max_size());
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
asa1d_container(
    size_type size_d1,
    const PrimitiveT &a_value,
    buffer_offset_in_cachelines buffer_offset,
    const AllocatorT & an_allocator)
: asa1d_container(size_d1, buffer_offset, &an_allocator)
{
    __SDLT_ASSERT(size_d1 < max_size());
    element_mover::fill_with(access(), a_value, 0, this->get_size_d1());
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
template<typename StlAllocatorT>
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
asa1d_container(
    const std::vector<PrimitiveT, StlAllocatorT> &other,
    buffer_offset_in_cachelines buffer_offset,
    const AllocatorT & an_allocator)
: asa1d_container(other.size(), buffer_offset, &an_allocator)
{
    __SDLT_ASSERT(other.size() <= max_size());
    aos1d_const_accessor<PrimitiveT, access_by_stride> source(other);
    element_mover::copy_all_values_from(source, access());
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
asa1d_container(
    const PrimitiveT *other_array, size_type number_of_elements,
    buffer_offset_in_cachelines buffer_offset,
    const AllocatorT & an_allocator)
: asa1d_container(number_of_elements, buffer_offset, &an_allocator)
{
    __SDLT_ASSERT(number_of_elements < max_size());
    aos1d_const_accessor<PrimitiveT, access_by_stride> source(other_array, number_of_elements);
    element_mover::copy_all_values_from(source, access());
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
template< typename IteratorT >
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
asa1d_container(IteratorT a_begin, const IteratorT an_end,
    buffer_offset_in_cachelines buffer_offset,
    const AllocatorT & an_allocator)
: asa1d_container(std::distance(a_begin, an_end), buffer_offset, &an_allocator)
{
    __SDLT_ASSERT(static_cast<size_type>(std::distance(a_begin, an_end)) <= max_size());
    element_mover::copy_all_values_from(a_begin,an_end, 0, this->get_size_d1(), access());
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
clone() const
{
    asa1d_container dest_container(this->get_size_d1(), m_buffer_offset, m_allocator);
    auto source = const_access();
    element_mover::copy_all_values_from(source, dest_container.access());
    return dest_container;
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
~asa1d_container()
{
    if (m_buffer != nullptr)
    {
        m_allocator->free(m_buffer, m_size_in_bytes);
    }
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
const AllocatorT &
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
get_allocator() const
{
    return *m_allocator;
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
int
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
get_size_d1() const
{
    return this->m_size_d1;
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
unsigned char * &
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
get_offload_array()
{
    return this->m_buffer;
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
access() -> accessor<>
{
    return accessor<>(this->m_simd_data,
                    this->m_size_d1,
                    this->m_simd_block_stride_d1);
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
template<int IndexAlignmentT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
access(aligned<IndexAlignmentT> offset)
->accessor< aligned<IndexAlignmentT> >
{
    static_assert(IndexAlignmentT%SimdLaneCountT == 0, "IndexAlignmentT of aligned<IndexAlignmentT> must be a multiple of the SimdLaneCountT of the asa1d_container");
    return accessor< aligned<IndexAlignmentT> >(
        this->m_simd_data,
        this->m_size_d1,
        this->m_simd_block_stride_d1,
        offset);
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
template<int OffsetT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
access(fixed<OffsetT>)
->accessor< fixed<OffsetT> >
{
    static_assert(OffsetT%SimdLaneCountT == 0, "OffsetT of fixed<OffsetT> must be a multiple of the SimdLaneCountT of the asa1d_container");
    return accessor< fixed<OffsetT> >(
        this->m_simd_data,
        this->m_size_d1,
        this->m_simd_block_stride_d1);
}


template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
const_access() const -> const_accessor<>
{
    return const_accessor<>(
            this->m_simd_data,
            this->m_size_d1,
            this->m_simd_block_stride_d1);
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
template<int IndexAlignmentT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
const_access(aligned<IndexAlignmentT> offset) const
->const_accessor< aligned<IndexAlignmentT> >
{
    static_assert(IndexAlignmentT%SimdLaneCountT == 0, "IndexAlignmentT of aligned<IndexAlignmentT> must be a multiple of the SimdLaneCountT of the asa1d_container");
    return const_accessor< aligned<IndexAlignmentT> >(
        this->m_simd_data,
        this->m_size_d1,
        this->m_simd_block_stride_d1,
        offset);
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
template<int OffsetT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
const_access(fixed<OffsetT>) const
->const_accessor< fixed<OffsetT> >
{
    static_assert(OffsetT%SimdLaneCountT == 0, "OffsetT of fixed<OffsetT> must be a multiple of the SimdLaneCountT of the asa1d_container");
    return const_accessor< fixed<OffsetT> >(
        this->m_simd_data,
        this->m_size_d1,
        this->m_simd_block_stride_d1);
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
make_index_from_integer(const int iIntegerIndex) const
->index_from_integer_type
{
    index_value_type indices(iIntegerIndex);
    __SDLT_ASSERT(indices.value() < this->get_size_d1());

    index_from_integer_type index(indices, internal::operation_none());

    __SDLT_ASSERT(index.template simd_struct_index<simdlane_count>() < this->m_simd_block_stride_d1);
    __SDLT_ASSERT(index.simdlane_index() < simdlane_count);

    return index;
}


template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
operator [](const int an_index_d1) const
-> decltype(std::declval<asa1d_container>().const_access()[std::declval<asa1d_container>().make_index_from_integer(an_index_d1)])
{
    auto simdIndex = make_index_from_integer(an_index_d1);

    return internal::asa1d::const_element<simd_data_type>
           (m_simd_data[simdIndex.template simd_struct_index<SimdLaneCountT>()],
                                                   simdIndex.simdlane_index());
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
operator [](const int an_index_d1)
-> decltype(std::declval<asa1d_container>().access()[std::declval<asa1d_container>().make_index_from_integer(an_index_d1)])
{
    auto simdIndex = make_index_from_integer(an_index_d1);

    return internal::asa1d::element<simd_data_type>
           (m_simd_data[simdIndex.template simd_struct_index<SimdLaneCountT>()],
                                                   simdIndex.simdlane_index());
}


template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
template<typename IndexD1T, typename>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
operator [](const IndexD1T an_index_d1) const
-> decltype(std::declval<asa1d_container>().const_access()[an_index_d1])
{
    __SDLT_ASSERT(an_index_d1.value() >= 0);
    __SDLT_ASSERT(an_index_d1.value() < this->get_size_d1());

    return internal::asa1d::const_element<simd_data_type>
           (m_simd_data[an_index_d1.template simd_struct_index<SimdLaneCountT>()],
                                                    an_index_d1.simdlane_index());
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
template<typename IndexD1T, typename>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
operator [](const IndexD1T an_index_d1)
-> decltype(std::declval<asa1d_container>().access()[an_index_d1])
{
    __SDLT_ASSERT(an_index_d1.value() >= 0);
    __SDLT_ASSERT(an_index_d1.value() < this->get_size_d1());

    return internal::asa1d::element<simd_data_type>
           (m_simd_data[an_index_d1.template simd_struct_index<SimdLaneCountT>()],
                                                    an_index_d1.simdlane_index());
}


template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
operator accessor<> ()
{
    return access();
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
operator const_accessor<> ()
{
    return const_access();
}


template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
get_offload_digest() -> offload_digest
{
    offload_digest digest;
    digest.array_size = this->m_size_in_bytes;
    digest.data_offset_in_array = reinterpret_cast<unsigned char *>(this->m_simd_data) - this->m_buffer;
    digest.m_simd_block_stride_d1 = this->m_simd_block_stride_d1;
    digest.size_d1 = this->m_size_d1;
    return digest;
}

// Only for use when offloading, the user may offload the underlying linear
// array + digest to a device, then build an accessor on the device side
template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
access(
    unsigned char *an_offload_array,
    const offload_digest &a_digest
) -> accessor<>
{
    #if SDLT_DEBUG
        size_t required_size = sizeof(simd_data_type)*a_digest.m_simd_block_stride_d1;
        __SDLT_ASSERT(a_digest.array_size >= required_size);
    #endif
    unsigned char *data = (an_offload_array + a_digest.data_offset_in_array);

    return accessor<>(
        reinterpret_cast<simd_data_type * const>(data),
        a_digest.size_d1,
        a_digest.m_simd_block_stride_d1);
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
void
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
do_reserve(internal::capacity new_capacity)
{
    // At this point we have determined we need a new allocation
    // keep it simple by just creating a new instance and swapping
    // with it.
    asa1d_container dest_container(new_capacity, this->get_size_d1(), m_buffer_offset, m_allocator);
    auto source = const_access();
    element_mover::copy_all_values_from(source, dest_container.access());
    std::swap(*this, dest_container);
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
size() const
->size_type
{
    return get_size_d1();
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
max_size() const
->size_type
{
    return std::numeric_limits<int>::max();
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
capacity() const
->size_type
{
    return m_capacity.count();
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
bool
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
empty() const
{
    return get_size_d1() == 0;
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
void
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
reserve(size_type minimum_capacity)
{
    __SDLT_ASSERT(minimum_capacity < max_size());
    if (minimum_capacity > size_type(m_capacity.count())) {
        do_reserve(internal::capacity(minimum_capacity));
    }
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
void
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
resize(size_type new_size_d1)
{
    __SDLT_ASSERT(new_size_d1 < max_size());
    if (capacity() < new_size_d1) {
        do_reserve(internal::capacity(new_size_d1));
    }
    this->m_size_d1 = new_size_d1;
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
void
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
shrink_to_fit()
{
    if (m_capacity.count() > this->m_size_d1)
    {
        do_reserve(internal::capacity(this->m_size_d1));
    }
}


template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
void
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
assign(size_type new_size,
            const PrimitiveT & a_value)
{
    __SDLT_ASSERT(new_size < max_size());
    resize(new_size);
    element_mover::fill_with(access(), a_value, 0, this->get_size_d1());
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
template< typename IteratorT >
void
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
assign(IteratorT a_begin, const IteratorT an_end)
{
    size_type new_size = std::distance(a_begin, an_end);
    __SDLT_ASSERT(new_size <= max_size());
    resize(new_size);

    element_mover::copy_all_values_from(a_begin,an_end, 0, this->get_size_d1(), access());
}


template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
void
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
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
    // TODO: could keep a simd::iterator_index to track the last element and avoid
    // index math, could help if inserting a large number of elements as well as
    // make it cheaper to create an end() iterator or call back()
    access()[index_value_type(index_to_assign)] = a_value;
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
void
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
pop_back()
{
    __SDLT_ASSERT(this->m_size_d1 > 0);
    --this->m_size_d1;
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
void
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
clear()
{
    // Primitives are supposed to be Plain Old data_type and not have any complicated
    // destructors
    // We will assume this an not bother calling any destructors
    this->m_size_d1 = 0;
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
void
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
insert(const iterator_index & a_simd_index, const PrimitiveT & a_value)
{
    __SDLT_ASSERT(a_simd_index.value() >= 0);
    // Could be inserting at the very end
    __SDLT_ASSERT(a_simd_index.value() <= get_size_d1());

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
    element_mover::shift_elements_forwards(access(), a_simd_index, new_element_count);

    access()[a_simd_index] = a_value;
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
void
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
insert(int an_index_d1, const PrimitiveT & a_value)
{
    __SDLT_ASSERT(an_index_d1 >= 0);
    // Could be inserting at the very end
    __SDLT_ASSERT(an_index_d1 <= get_size_d1());

    const iterator_index simd_index(an_index_d1);
    insert(simd_index, a_value);
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
insert(const iterator & iterator_pos, const PrimitiveT & a_value)
-> iterator
{
    __SDLT_ASSERT(iterator_pos.m_accessor == access());
    insert(iterator_pos.m_indices, a_value);
    return iterator(access(), iterator_pos.m_indices);
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
void
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
insert(const iterator & iterator_pos, size_type count, const PrimitiveT & a_value)
{
    __SDLT_ASSERT(count < max_size());
    __SDLT_ASSERT(iterator_pos.m_accessor == access());
    __SDLT_ASSERT(iterator_pos.m_indices.value() >= 0);
    // Could be inserting at the very end
    __SDLT_ASSERT(iterator_pos.m_indices.value() <= get_size_d1());

    __SDLT_ASSERT((size() + count) < max_size());
    const int new_element_count = count;
    int post_size_d1 = get_size_d1() + new_element_count;
    if (post_size_d1 >= m_capacity.count()) {
        do_reserve(internal::capacity(std::max(new_element_count, 2*std::max(1,m_capacity.count()))));
    }
    __SDLT_ASSERT(m_capacity.count() > post_size_d1);

    // need to set the size before trying to use an accessor
    // because of boundary checks against the index
    this->m_size_d1 = post_size_d1;

    // First we need to shift all the elements up
    element_mover::shift_elements_forwards(access(), iterator_pos.m_indices, new_element_count);

    const int linear_index_to_start_at = iterator_pos.m_indices.value();
    element_mover::fill_with(access(), a_value, linear_index_to_start_at, (linear_index_to_start_at + new_element_count));
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
template< typename IteratorT >
void
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
insert(const iterator & iterator_pos, IteratorT a_begin, const IteratorT an_end)
{
    __SDLT_ASSERT(iterator_pos.m_accessor == access());

    __SDLT_ASSERT(iterator_pos.m_indices.value() >= 0);
    // Could be inserting at the very end
    __SDLT_ASSERT(iterator_pos.m_indices.value() <= get_size_d1());
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
    element_mover::shift_elements_forwards(access(), iterator_pos.m_indices, new_element_count);

    const int linear_index_to_start_at = iterator_pos.m_indices.value();
    element_mover::copy_all_values_from(a_begin,an_end, linear_index_to_start_at, (linear_index_to_start_at + new_element_count), access());
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
template <typename... ArgsT>
typename asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::iterator
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
emplace (const_iterator iterator_pos, ArgsT&&... iArgs)
{
    __SDLT_ASSERT(iterator_pos.m_accessor == access());
    PrimitiveT value(std::forward<ArgsT>(iArgs)...);
    insert(iterator_pos.m_indices, value);
    return iterator(access(), iterator_pos.m_indices);

}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
template <typename... ArgsT>
void
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
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
    access()[index_value_type(index_to_assign)] = value;
}



template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
erase(const iterator & iterator_pos)
-> iterator
{
    __SDLT_ASSERT(iterator_pos.m_accessor == access());
    __SDLT_ASSERT(iterator_pos.m_indices.value() >= 0);
    __SDLT_ASSERT(iterator_pos.m_indices.value() < get_size_d1());

    const int remove_element_count = 1;
    int post_size_d1 = get_size_d1() - remove_element_count;
    __SDLT_ASSERT(m_capacity.count() > post_size_d1);

    // First we need to shift all the elements down
    element_mover::shift_elements_backwards(access(), iterator_pos.m_indices.value(), remove_element_count);

    this->m_size_d1 = post_size_d1;

    // return iterator to new place of element that followed the
    // last element erased, which for this case is the original iterator
    return iterator(access(), iterator_pos.m_indices.value());

}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
erase(const iterator & start_at, const iterator & end_before)
-> iterator
{
    __SDLT_ASSERT(start_at.m_accessor == access());
    __SDLT_ASSERT(start_at.m_accessor == end_before.m_accessor);
    __SDLT_ASSERT(start_at.m_indices.value() >= 0);
    __SDLT_ASSERT(start_at.m_indices.value() <= get_size_d1());

    __SDLT_ASSERT(end_before.m_indices.value() >= 0);
    __SDLT_ASSERT(end_before.m_indices.value() <= get_size_d1());

    __SDLT_ASSERT(end_before.m_indices >= start_at.m_indices);

    const int remove_element_count = end_before.m_indices - start_at.m_indices;
    int post_size_d1 = get_size_d1() - remove_element_count;
    __SDLT_ASSERT(m_capacity.count() > post_size_d1);

    // First we need to shift all the elements down
    element_mover::shift_elements_backwards(access(), start_at.m_indices.value(), remove_element_count);

    this->m_size_d1 = post_size_d1;

    // return iterator to new place of element that followed the
    // last element erased, which for this case is the original iterator
    return iterator(access(), start_at.m_indices);
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
void
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
swap(asa1d_container &other)
{
    std::swap(*this, other);
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
PrimitiveT const
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
front() const
{
    return const_access()[index_value_type(0)];
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
PrimitiveT const
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
back() const
{
    int lastValidIndex = this->m_size_d1-1;
    return const_access()[index_value_type(lastValidIndex)];
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
PrimitiveT const
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
at(size_type an_index) const
{
    __SDLT_ASSERT(an_index < size());
    return const_access()[index_value_type(an_index)];
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
cbegin() const
-> const_iterator
{
    return const_iterator(const_access(), 0);
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
cend() const
-> const_iterator
{
    return const_iterator(const_access(), get_size_d1());
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
begin() const
-> const_iterator
{
    return cbegin();
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
end() const
-> const_iterator
{
    return cend();
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
begin()
-> iterator
{
    return iterator(access(), 0);
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
end()
-> iterator
{
    return iterator(access(), get_size_d1());
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
crbegin() const
-> const_reverse_iterator
{
    return const_reverse_iterator(cend());
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
crend() const
-> const_reverse_iterator
{
    return const_reverse_iterator(cbegin());
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
rbegin() const
-> const_reverse_iterator
{
    return crbegin();
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
rend() const
-> const_reverse_iterator
{
    return crend();
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
rbegin()
-> reverse_iterator
{
    return reverse_iterator(end());
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
auto
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
rend()
-> reverse_iterator
{
    return reverse_iterator(begin());
}



// std::vector relational operator compatibility

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
bool
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
operator == (const asa1d_container &other) const
{
    SDLT_INLINE_BLOCK
    {
        if (size() != other.size())
            return false;
        return std::equal(cbegin(), cend(), other.begin());
    }
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
bool
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
operator != (const asa1d_container &other) const
{
    SDLT_INLINE_BLOCK
    {
        return false == (*this == other);
    }
}

template<typename PrimitiveT, int SimdLaneCountT, int ByteAlignmentT, int AlignD1OnIndexT, class AllocatorT>
void
asa1d_container<PrimitiveT, SimdLaneCountT, ByteAlignmentT, AlignD1OnIndexT, AllocatorT>::
do_allocation(internal::capacity new_capacity)
{
    if (new_capacity.count() != 0)
    {
        const int align_to_simdlane = AlignD1OnIndexT%SimdLaneCountT;
        // If we are going to align to a non zero simd lane we will need an extra simd block
        const int extra_block = (align_to_simdlane > 0) ? 1 : 0;

        this->m_simd_block_stride_d1 = extra_block + (new_capacity.count() + SimdLaneCountT - 1)/SimdLaneCountT;

        size_t required_size_in_bytes = this->m_simd_block_stride_d1*sizeof(simd_data_type);

        m_size_in_bytes = required_size_in_bytes;

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

        m_simd_data = reinterpret_cast<simd_data_type *>(m_buffer) + padding_offset;
        __SDLT_ASSERT(internal::is_aligned<64>(m_simd_data));
    }

    m_capacity = new_capacity;
}

} // namepace __SDLT_IMPL
using __SDLT_IMPL::asa1d_container;
} // namespace sdlt

#endif // SDLT_ASA_1D_CONTAINER_H
