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

#ifndef SDLT_ASA_1D_CONST_STL_ITERATOR_H
#define SDLT_ASA_1D_CONST_STL_ITERATOR_H

#include <iterator>
#include <iostream>

#include "../../common.h"
#include "../../internal/simd/iterator_index.h"

namespace sdlt {
namespace __SDLT_IMPL {

// Forward declarations
template<
    typename PrimitiveT,
    int SimdLaneCountT,
    int ByteAlignmentT,
    int AlignD1OnIndexT,
    class AllocatorT
>
class asa1d_container;

namespace internal {
namespace asa1d {

// Forward declaration
template <typename AccessorT>
class stl_iterator;

template <typename ConstAccessorT>
class const_stl_iterator
: public std::iterator<
    std::random_access_iterator_tag,
    typename ConstAccessorT::primitive_type, // value_type
    int, // difference_type
    typename ConstAccessorT::primitive_type *, // pointer
    // NOTE:  we are NOT going to really give a reference out,
    // but want to be able to use the std::reverse_iterator that makes use
    // of the typedef
    typename ConstAccessorT::primitive_type // reference
>
{
public:
    typedef typename ConstAccessorT::primitive_type primitive_type;
    static const int simdlane_count = ConstAccessorT::simdlane_count;
    static const int align_d1_on_index = ConstAccessorT::align_d1_on_index;

protected:

    ConstAccessorT & accessor() { return m_accessor; }

    ConstAccessorT m_accessor;
    typedef simd::iterator_index<simdlane_count, align_d1_on_index> indices;
    indices m_indices;

    template<typename, int , int, int, class> friend class sdlt::__SDLT_IMPL::asa1d_container;

public:
    SDLT_INLINE
    const_stl_iterator()
    : m_accessor(nullptr, 0, 0)
    , m_indices(-1, -1)
    {
    }

    explicit SDLT_INLINE
    const_stl_iterator(
        const ConstAccessorT &iAccessor,
        int an_index
    )
    : m_accessor(iAccessor)
    , m_indices(an_index)
    {
        __SDLT_ASSERT(an_index >= 0);
        __SDLT_ASSERT(an_index <= m_accessor.get_size_d1());
    }

    explicit SDLT_INLINE
    const_stl_iterator(
        const ConstAccessorT &iAccessor,
        const indices & iIndices
    )
    : m_accessor(iAccessor)
    , m_indices(iIndices)
    {
        __SDLT_ASSERT(m_indices.value() >= 0);
        __SDLT_ASSERT(m_indices.value() <= m_accessor.get_size_d1());
    }


    // Implicit conversion constructor from non-const stl_iterator
    template<typename AccessorT, typename = std::enable_if<std::is_same<typename AccessorT::primitive_type, typename ConstAccessorT::primitive_type>::value>>
    SDLT_INLINE
    const_stl_iterator(const stl_iterator<AccessorT> &an_iterator)
    : m_accessor(an_iterator.m_accessor)
    , m_indices(an_iterator.m_indices)
    {
        __SDLT_ASSERT(m_indices.value() >= 0);
        __SDLT_ASSERT(m_indices.value() <= m_accessor.get_size_d1());
    }

    SDLT_INLINE
    auto operator *() const -> decltype(m_accessor[m_indices])
    {
        return m_accessor[m_indices];
    }

    // operator -> TOO DANGEROUS
    // We choose not to support the -> as it would require some sort of
    // local memory storage for the object.
    // We could return a proxy object like the accessor's do
    // but then the caller would need to use it differently, and at that
    // point should just use Accessors instead of iterators.
    // WORKAROUND:  Extract object to stack then can pass pointer to it
    // to existing code instead of an iterator
    // IE:
    // primitive_type local = *iter;
    // primitive_type * localPtr = &local;
    // localPtr->member

    SDLT_INLINE
    auto operator [](int offset) const
        -> decltype(m_accessor[m_indices + offset])
    {
        return m_accessor[m_indices + offset];
    }

    SDLT_INLINE
    bool operator == (const const_stl_iterator &other) const
    {
        __SDLT_ASSERT(m_accessor == other.m_accessor);
        return m_indices == other.m_indices;
    }

    SDLT_INLINE
    bool operator != (const const_stl_iterator &other) const
    {
        __SDLT_ASSERT(m_accessor == other.m_accessor);
        return m_indices != other.m_indices;
    }

    // Pre increment
    SDLT_INLINE
    const_stl_iterator &
    operator ++()
    {
        ++m_indices;
        __SDLT_ASSERT(m_indices.value() <= m_accessor.get_size_d1());
        return *this;
    }

    // Post increment
    SDLT_INLINE
    const_stl_iterator
    operator ++(int)
    {
        const_stl_iterator temp = *this;
        ++m_indices;
        __SDLT_ASSERT(m_indices.value() <= m_accessor.get_size_d1());
        return temp;
    }

    // Pre decrement
    SDLT_INLINE
    const_stl_iterator &
    operator --()
    {
        __SDLT_ASSERT(m_indices.value() > 0);
        --m_indices;
        return *this;
    }

    // Post decrement
    SDLT_INLINE
    const_stl_iterator
    operator --(int)
    {
        __SDLT_ASSERT(m_indices.value() > 0);
        const_stl_iterator temp = *this;
        --m_indices;
        return temp;
    }

    SDLT_INLINE
    const_stl_iterator &
    operator +=(int offset)
    {
        m_indices += offset;
        __SDLT_ASSERT(m_indices.value() <= m_accessor.get_size_d1());
        return *this;
    }

    SDLT_INLINE
    const_stl_iterator &
    operator -=(int offset)
    {
        m_indices -= offset;
        __SDLT_ASSERT(m_indices.value() >= 0);
        __SDLT_ASSERT(m_indices.value() <= m_accessor.get_size_d1());
        return *this;
    }

    SDLT_INLINE
    int operator -(const const_stl_iterator & other) const
    {
        __SDLT_ASSERT(m_accessor == other.m_accessor);
        return m_indices - other.m_indices;
    }

    SDLT_INLINE
    const_stl_iterator operator + (int offset) const
    {
        const_stl_iterator offsetIter(m_accessor, m_indices + offset);
        return offsetIter;
    }

    SDLT_INLINE
    const_stl_iterator operator - (int offset) const
    {
        const_stl_iterator offsetIter(m_accessor, m_indices - offset);
        return offsetIter;
    }

    SDLT_INLINE
    bool operator < (const const_stl_iterator & other) const
    {
        __SDLT_ASSERT(m_accessor == other.m_accessor);
        return m_indices < other.m_indices;
    }

    SDLT_INLINE
    bool operator > (const const_stl_iterator & other) const
    {
        __SDLT_ASSERT(m_accessor == other.m_accessor);
        return m_indices > other.m_indices;
    }

    SDLT_INLINE
    bool operator <= (const const_stl_iterator & other) const
    {
        __SDLT_ASSERT(m_accessor == other.m_accessor);
        return m_indices <= other.m_indices;
    }

    SDLT_INLINE
    bool operator >= (const const_stl_iterator & other) const
    {
        __SDLT_ASSERT(m_accessor == other.m_accessor);
        return m_indices >= other.m_indices;
    }

    SDLT_INLINE friend
    const_stl_iterator operator + (int offset, const const_stl_iterator &an_iterator)
    {
        const_stl_iterator offsetIter(an_iterator.m_accessor, an_iterator.m_indices + offset);
        return offsetIter;
    }

    SDLT_INLINE friend
    std::ostream& operator << (std::ostream& output_stream, const const_stl_iterator &an_iterator)
    {
        output_stream << "const_stl_iterator{m_indices=" << an_iterator.m_indices << "}";
        return output_stream;
    }

    SDLT_INLINE friend
    std::ostream& operator << (std::ostream& output_stream, const std::reverse_iterator<const_stl_iterator> &an_iterator)
    {
        output_stream << "std::reverse_iterator<const_stl_iterator>{m_indices=" << an_iterator.base().m_indices << "}";
        return output_stream;
    }
};


} // namespace asa1d
} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_ASA_1D_CONST_STL_ITERATOR_H
