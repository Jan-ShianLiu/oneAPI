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

#ifndef SDLT_LINEAR_CONST_STL_ITERATOR_H
#define SDLT_LINEAR_CONST_STL_ITERATOR_H

#include <iterator>
#include <iostream>

#include "../../access_by.h"
#include "../../common.h"
#include "../../linear_index.h"

namespace sdlt {
namespace __SDLT_IMPL {

// Forward declaration
template<
    typename PrimitiveT,
    access_by AccessByT,
    class AllocatorT
>
class aos1d_container;


// Forward declaration
template<
    typename PrimitiveT,
    int AlignD1OnIndexT,
    class AllocatorT
>
class soa1d_container;


namespace internal {
namespace linear {

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
    // of the typedef, use proxy to element instead
       decltype(std::declval<ConstAccessorT>()[std::declval<int>()])
>
{
protected:

    ConstAccessorT & accessor() { return m_accessor; }

    ConstAccessorT m_accessor;
    int m_index;

    template<typename, access_by , class> friend class sdlt::__SDLT_IMPL::aos1d_container;
    template<typename, int , class> friend class sdlt::__SDLT_IMPL::soa1d_container;

public:
    typedef typename ConstAccessorT::primitive_type primitive_type;

    SDLT_INLINE
    const_stl_iterator()
    : m_accessor()
    , m_index(-1)
    {
    }

    explicit SDLT_INLINE
    const_stl_iterator(
        const ConstAccessorT &an_accessor,
        int an_index
    )
    : m_accessor(an_accessor)
    , m_index(an_index)
    {
        __SDLT_ASSERT(m_index >= 0);
        __SDLT_ASSERT(m_index <= m_accessor.get_size_d1());
    }

    // Implicit conversion constructor from non-const stl_iterator
    template<typename AccessorT>
    SDLT_INLINE
    const_stl_iterator(const stl_iterator<AccessorT> &an_iterator)
    : m_accessor(an_iterator.m_accessor)
    , m_index(an_iterator.m_index)
    {
//        static_assert(AccessorT::primitive_type == const_accessor::primitive_type);
        __SDLT_ASSERT(m_index >= 0);
        __SDLT_ASSERT(m_index <= m_accessor.get_size_d1());
    }

    SDLT_INLINE
    auto operator *() const -> typename const_stl_iterator::reference
    {
        return m_accessor[m_index];
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

    // NOTE: reference to integer gets embedded in returned proxy object
    // it is the caller's responsibility to ensure lifetime is adequate
    // TODO: consider templatizing type of offset to allow fixed<offset> and aligned<offset>
    SDLT_INLINE
    auto operator [](const int offset) const
        -> typename const_stl_iterator::reference
    {
        return m_accessor[m_index + offset];
    }

    SDLT_INLINE
    bool operator == (const const_stl_iterator &other) const
    {
        __SDLT_ASSERT(m_accessor == other.m_accessor);
        return m_index == other.m_index;
    }

    SDLT_INLINE
    bool operator != (const const_stl_iterator &other) const
    {
        __SDLT_ASSERT(m_accessor == other.m_accessor);
        return m_index != other.m_index;
    }

    // Pre increment
    SDLT_INLINE
    const_stl_iterator &
    operator ++()
    {
        ++m_index;
        __SDLT_ASSERT(m_index <= m_accessor.get_size_d1());
        return *this;
    }

    // Post increment
    SDLT_INLINE
    const_stl_iterator
    operator ++(int)
    {
        const_stl_iterator temp = *this;
        ++m_index;
        __SDLT_ASSERT(m_index <= m_accessor.get_size_d1());
        return temp;
    }

    // Pre decrement
    SDLT_INLINE
    const_stl_iterator &
    operator --()
    {
        __SDLT_ASSERT(m_index > 0);
        --m_index;
        return *this;
    }

    // Post decrement
    SDLT_INLINE
    const_stl_iterator
    operator --(int)
    {
        const_stl_iterator temp = *this;
        __SDLT_ASSERT(m_index > 0);
        --m_index;
        return temp;
    }

    SDLT_INLINE
    const_stl_iterator &
    operator +=(int offset)
    {
        m_index += offset;
        __SDLT_ASSERT(m_index <= m_accessor.get_size_d1());
        return *this;
    }

    SDLT_INLINE
    const_stl_iterator &
    operator -=(int offset)
    {
        m_index -= offset;
        __SDLT_ASSERT(m_index >= 0);
        return *this;
    }

    SDLT_INLINE
    int operator -(const const_stl_iterator & other) const
    {
        __SDLT_ASSERT(m_accessor == other.m_accessor);
        return m_index - other.m_index;
    }

    SDLT_INLINE
    const_stl_iterator operator + (int offset) const
    {
        const_stl_iterator offsetIter(m_accessor, m_index + offset);
        return offsetIter;
    }

    SDLT_INLINE
    const_stl_iterator operator - (int offset) const
    {
        const_stl_iterator offsetIter(m_accessor, m_index - offset);
        return offsetIter;
    }

    SDLT_INLINE
    bool operator < (const const_stl_iterator & other) const
    {
        __SDLT_ASSERT(m_accessor == other.m_accessor);
        return m_index < other.m_index;
    }

    SDLT_INLINE
    bool operator > (const const_stl_iterator & other) const
    {
        __SDLT_ASSERT(m_accessor == other.m_accessor);
        return m_index > other.m_index;
    }

    SDLT_INLINE
    bool operator <= (const const_stl_iterator & other) const
    {
        __SDLT_ASSERT(m_accessor == other.m_accessor);
        return m_index <= other.m_index;
    }

    SDLT_INLINE
    bool operator >= (const const_stl_iterator & other) const
    {
        __SDLT_ASSERT(m_accessor == other.m_accessor);
        return m_index >= other.m_index;
    }

    SDLT_INLINE friend
    const_stl_iterator operator + (int offset, const const_stl_iterator &an_iterator)
    {
        const_stl_iterator offsetIter(an_iterator.m_accessor, offset + an_iterator.m_index);
        return offsetIter;
    }

    SDLT_INLINE friend
    std::ostream& operator << (std::ostream& output_stream, const const_stl_iterator &an_iterator)
    {
        output_stream << "const_stl_iterator{index=" << an_iterator.m_index << "}";
        return output_stream;
    }

    SDLT_INLINE friend
    std::ostream& operator << (std::ostream& output_stream, const std::reverse_iterator<const_stl_iterator> &an_iterator)
    {
        output_stream << "std::reverse_iterator<const_stl_iterator>{index=" << an_iterator.base().m_index << "}";
        return output_stream;
    }
};


} // namespace linear
} // namespace internal
} // namespace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_LINEAR_CONST_STL_ITERATOR_H
