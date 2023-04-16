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

#ifndef SDLT_ALLOCATOR_STD_ADAPTER_H
#define SDLT_ALLOCATOR_STD_ADAPTER_H

#include "../common.h"
#include "../internal/is_aligned.h"

#include <limits>
#include <type_traits>
#include <utility>


// gcc 4.4.7 std has issues with a c++11 compliant allocator
// Test for GCC > 4.7.0 or higher
#if __SDLT_ON_WINDOWS > 0 || \
    __GNUC__ > 4 || \
    (__GNUC__ == 4 && (__GNUC_MINOR__ > 7 || \
                       (__GNUC_MINOR__ == 7 && \
                        __GNUC_PATCHLEVEL__ >= 0)))
    #define __SDLT_ENABLE_CPP11_STD_ALLOCATOR 1
#else
    #define __SDLT_ENABLE_CPP11_STD_ALLOCATOR 0
#endif

namespace sdlt {
namespace allocator {
namespace __SDLT_IMPL {

// Adapt an existing Allocator for use with a std Container.  eg:
//  std::vector< MyStruct, allocator::std_adapter<MyStruct, allocator::malloc<64> > > MyStructArray;

template <class ObjT, class AllocatorT>
class std_adapter
{
    template <class, class> friend class std_adapter;
public:
    typedef ObjT value_type;
    typedef ObjT* pointer;
    typedef const ObjT* const_pointer;
    typedef ObjT& reference;
    typedef const ObjT& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef std::true_type propagate_on_container_move_assignment;
    typedef std::true_type is_always_equal;
    template< class OtherObjT >
    struct rebind
    {
        typedef std_adapter<OtherObjT, AllocatorT> other;
    };


    SDLT_INLINE std_adapter(const AllocatorT & an_allocator = AllocatorT());

    template <class T> std_adapter(const std_adapter<T, AllocatorT> & other);

    SDLT_INLINE pointer address( reference a_ref) const;
    SDLT_INLINE const_pointer address( const_reference a_ref) const;

    SDLT_INLINE pointer allocate(size_type a_count, std::allocator<void>::const_pointer a_hint = 0);
    SDLT_INLINE void deallocate(pointer a_pointer, size_type a_count);

    SDLT_INLINE size_type max_size() const;

    SDLT_INLINE void construct(pointer a_pointer, const_reference a_value);

    SDLT_INLINE void destroy(pointer a_pointer);

#if __SDLT_ENABLE_CPP11_STD_ALLOCATOR
        template< class OtherObjT, class... ArgsT >
        SDLT_INLINE void construct(OtherObjT* a_pointer, ArgsT&&... args);
        template< class OtherObjT >
        SDLT_INLINE void destroy(OtherObjT* a_pointer);
#endif
private:
    const AllocatorT * m_allocator;

};

// Implementation

template <class ObjT, class AllocatorT>
std_adapter<ObjT, AllocatorT>::std_adapter(const AllocatorT & an_allocator)
: m_allocator(&an_allocator)
{
}

template <class ObjT, class AllocatorT>
template <class T> 
std_adapter<ObjT, AllocatorT>::std_adapter(const std_adapter<T, AllocatorT>& other)
: m_allocator(other.m_allocator)
{
}

template <class ObjT, class AllocatorT>
typename std_adapter<ObjT, AllocatorT>::pointer
std_adapter<ObjT, AllocatorT>::address( reference a_ref) const
{
    return &a_ref;
}

template <class ObjT, class AllocatorT>
typename std_adapter<ObjT, AllocatorT>::const_pointer
std_adapter<ObjT, AllocatorT>::address( const_reference a_ref) const
{
    return &a_ref;
}

template <class ObjT, class AllocatorT>
typename std_adapter<ObjT, AllocatorT>::pointer
std_adapter<ObjT, AllocatorT>::allocate(size_type a_count, std::allocator<void>::const_pointer /*a_hint*/)
{
    __SDLT_ASSERT(m_allocator != nullptr);
    return reinterpret_cast<pointer>(m_allocator->allocate(sizeof(ObjT)*a_count));
}

template <class ObjT, class AllocatorT>
void
std_adapter<ObjT, AllocatorT>::deallocate(pointer a_pointer, size_type a_count)
{
    __SDLT_ASSERT(m_allocator != nullptr);
    m_allocator->free(a_pointer, sizeof(ObjT)*a_count);
}

template <class ObjT, class AllocatorT>
typename std_adapter<ObjT, AllocatorT>::size_type
std_adapter<ObjT, AllocatorT>::max_size() const
{
    // Overly optimistic, but sure an allocation greater than this would fail
    return std::numeric_limits<size_type>::max()/sizeof(ObjT);
}

template <class ObjT, class AllocatorT>
void
std_adapter<ObjT, AllocatorT>::construct(pointer a_pointer, const_reference a_value)
{
    ::new((void *)(a_pointer)) value_type(a_value);
}

template <class ObjT, class AllocatorT>
void
std_adapter<ObjT, AllocatorT>::destroy(pointer a_pointer)
{
    a_pointer->~ObjT();
}


#if __SDLT_ENABLE_CPP11_STD_ALLOCATOR
    template <class ObjT, class AllocatorT>
    template< class OtherObjT, class... ArgsT >
    void
    std_adapter<ObjT, AllocatorT>::construct(OtherObjT* a_pointer, ArgsT&&... args)
    {
        ::new((void *)(a_pointer)) OtherObjT(std::forward<ArgsT>(args)...);
    }

    template <class ObjT, class AllocatorT>
    template< class OtherObjT >
    void
    std_adapter<ObjT, AllocatorT>::destroy(OtherObjT* a_pointer)
    {
        a_pointer->~OtherObjT();
    }
#endif


} // namepace __SDLT_IMPL
using __SDLT_IMPL::std_adapter;
} // namepace allocator
} // namespace sdlt

#endif // SDLT_ALLOCATOR_STD_ADAPTER_H
