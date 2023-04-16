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

#ifndef SDLT_ALLOCATOR_HYBRID_H
#define SDLT_ALLOCATOR_HYBRID_H

#include "../common.h"

namespace sdlt {
namespace allocator {
namespace __SDLT_IMPL {

// use SmallAllocatorT for all allocations < threshold, otherwise use BigAllocatorT
template<size_t ByteCountThresholdT, typename SmallAllocatorT, typename BigAllocatorT>
struct hybrid
{
    SDLT_INLINE hybrid(const SmallAllocatorT & small_alloc = SmallAllocatorT(), const BigAllocatorT & big_alloc = BigAllocatorT());

    SDLT_INLINE void* allocate(size_t byte_count) const;
    SDLT_INLINE void free(void *pointer, size_t byte_count) const;
protected:
    const SmallAllocatorT * m_small_alloc;
    const BigAllocatorT * m_big_alloc;
};


// Implementation
template<size_t ByteCountThresholdT, typename SmallAllocatorT, typename BigAllocatorT>
hybrid<ByteCountThresholdT, SmallAllocatorT, BigAllocatorT>::hybrid(
  const SmallAllocatorT & small_alloc,
  const BigAllocatorT & big_alloc)
: m_small_alloc(&small_alloc)
, m_big_alloc(&big_alloc)
{
}

template<size_t ByteCountThresholdT, typename SmallAllocatorT, typename BigAllocatorT>
void*
hybrid<ByteCountThresholdT, SmallAllocatorT, BigAllocatorT>::allocate(size_t byte_count) const
{
if ( byte_count < ByteCountThresholdT) {
    return m_small_alloc->allocate(byte_count);
}
else
{
    return m_big_alloc->allocate(byte_count);
}
}

template<size_t ByteCountThresholdT, typename SmallAllocatorT, typename BigAllocatorT>
void
hybrid<ByteCountThresholdT, SmallAllocatorT, BigAllocatorT>::free(
    void *a_pointer,
    size_t byte_count
) const
{
if ( byte_count < ByteCountThresholdT) {
    return m_small_alloc->free(a_pointer, byte_count);
}
else
{
    return m_big_alloc->free(a_pointer, byte_count);
}
}

} // __SDLT_IMPL
using __SDLT_IMPL::hybrid;
} // namepace allocator
} // namespace sdlt

#endif // SDLT_ALLOCATOR_HYBRID_H
