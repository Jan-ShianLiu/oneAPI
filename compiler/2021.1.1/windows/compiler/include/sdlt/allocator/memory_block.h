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

#ifndef SDLT_ALLOCATOR_MEMORY_BLOCK_H
#define SDLT_ALLOCATOR_MEMORY_BLOCK_H

#include <atomic>
#include <cstdio>
#include <cstdlib>

#include "../common.h"
#include "../internal/is_aligned.h"


namespace sdlt {
namespace allocator {
namespace __SDLT_IMPL {

// Trivial, low overhead allocation out of an existing block of memory
// caller has the option of not bothering to call free
// and instead just using a new instance of memory_block.
// It allows 0 framentation, and will simply hand out
// allocations until the memory block is consumed.
// If free is called, it must be in the exact reverse order from allocate
// If the ordering is not done properly, then an assert will fire exiting
// the program
struct memory_block
{
private:
    // disallow default constructor
    memory_block();
public:
    SDLT_INLINE memory_block(void *i64ByteAlignedBuffer, size_t byte_count);

    SDLT_INLINE void* allocate(size_t byte_count) const;
    SDLT_INLINE void free(void *pointer, size_t byte_count) const;

    SDLT_INLINE long remaining() const;
protected:
    void * m64ByteAlignedBuffer;
    size_t m_byte_count;
    mutable std::atomic<size_t> m_bytes_allocated;
};

// Implementation

memory_block::memory_block(void *i64ByteAlignedBuffer, size_t byte_count)
: m64ByteAlignedBuffer(i64ByteAlignedBuffer)
, m_byte_count(byte_count)
, m_bytes_allocated(0)
{
    __SDLT_ASSERT(sdlt::__SDLT_IMPL::internal::is_aligned<sizeof(int)>(i64ByteAlignedBuffer));

    //printf("\n\n------------>memory_block::construct at %p of size %lu \n",
    //            m64ByteAlignedBuffer, m_byte_count);


}

long memory_block::remaining() const
{
    return m_byte_count - m_bytes_allocated;
}


void*
memory_block::allocate(size_t byte_count) const
{
    size_t bytes_required_for_alignment = 64*((byte_count + (64-1))/64);
    __SDLT_ASSERT(bytes_required_for_alignment >= byte_count);


    size_t expected_bytes_allocated = m_bytes_allocated;
    //printf("\n\n------------>memory_block::allocate at %p of size %lu only has %lu remaining and cannot satisfy request for %lu bytes\nABORTING PROGRAM, segfault may occur if this is not the main thread\n",
                //m64ByteAlignedBuffer, m_byte_count, m_byte_count-expected_bytes_allocated, bytes_required_for_alignment);
    size_t next_bytes_allocated_value;
    do
    {

        // Don't allow us to run out memory and experience undefined behavior
        // choose to exit vs return nullptr which may or may not be handled
        // by the caller
        if (size_t(m_byte_count-expected_bytes_allocated) < bytes_required_for_alignment) {
            fprintf(stderr, "%s at %p of size %zu only has %zu remaining and cannot satisfy request for %zu bytes\nABORTING PROGRAM, segfault may occur if this is not the main thread\n",
                __SDLT_FUNCTION_SIG, m64ByteAlignedBuffer, m_byte_count,
                m_byte_count-expected_bytes_allocated, bytes_required_for_alignment);
            fflush(stderr);
            exit(EXIT_FAILURE);
        }
        next_bytes_allocated_value = expected_bytes_allocated + bytes_required_for_alignment;
        __SDLT_ASSERT(m_bytes_allocated%64 == 0);
    }
    while(m_bytes_allocated.compare_exchange_weak(expected_bytes_allocated, next_bytes_allocated_value) == false);

    void * pointer = reinterpret_cast<char *>(m64ByteAlignedBuffer) + expected_bytes_allocated;

    return pointer;
}

void
memory_block::free(
    void * a_pointer,
    size_t byte_count
) const
{
    size_t bytes_required_for_alignment = 64*((byte_count + (64-1))/64);
    __SDLT_ASSERT(bytes_required_for_alignment >= byte_count);

    long byte_to_end_of_allocation = (reinterpret_cast<char *>(a_pointer) - reinterpret_cast<char *>(m64ByteAlignedBuffer))
                                  + long(bytes_required_for_alignment);
    // Only supports free in the exact reverse order of allocation
    // otherwise nothing gets freed and reuse will eventually run out
    // of memory. That's fine as the expected use case as the buffer this
    // allocator runs against will go out of scope
    size_t expected_bytes_allocated = m_bytes_allocated;
    if (byte_to_end_of_allocation == static_cast<long>(expected_bytes_allocated))
    {
        size_t newBytesAllocated = expected_bytes_allocated - bytes_required_for_alignment;
        m_bytes_allocated.compare_exchange_strong(expected_bytes_allocated, newBytesAllocated);
        __SDLT_ASSERT(m_bytes_allocated <= m_byte_count);
    }
}

} // namepace __SDLT_IMPL
using __SDLT_IMPL::memory_block;
} // namepace allocator
} // namespace sdlt

#endif // SDLT_ALLOCATOR_MEMORY_BLOCK_H
