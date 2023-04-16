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

#ifndef SDLT_AOS_CONTAINER_BASE_H
#define SDLT_AOS_CONTAINER_BASE_H

#include <cstdlib>

#include "../buffer_offset_in_cachelines.h"
#include "../common.h"
#include "../primitive.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {


template<class DerivedT, class AllocatorT>
class aos_container_base
{
public:
    int get_size_d1() const { return this->m_size_d1; }

    unsigned char * & get_offload_array() { return this->m_buffer; };

protected:
    const int m_size_d1;
    size_t m_size_in_bytes;

    struct offload_digest
    {
        size_t array_size;
        size_t data_offset_in_array;
        int size_d1;
    };

    void buildOffloadDigest(offload_digest &a_digest) {
        a_digest.array_size = this->m_size_in_bytes;
        a_digest.data_offset_in_array = this->m_data - this->m_buffer;
        a_digest.size_d1 = this->m_size_d1;
    }

public:
    unsigned char * m_buffer;
protected:
    unsigned char * m_data; // can be offset into the buffer for alignment purposes
    const AllocatorT * m_allocator;

    aos_container_base(const int size_d1, const AllocatorT * an_allocator)
    : m_size_d1(size_d1)
    , m_size_in_bytes(0)
    , m_buffer(nullptr)
    , m_data(nullptr)
    , m_allocator(an_allocator)
    {
    }

    ~aos_container_base()
    {
        m_allocator->free(m_buffer, m_size_in_bytes);
    }

    DerivedT & derived() { return static_cast<DerivedT &>(*this); }

    void create(buffer_offset_in_cachelines a_padding)
    {
        size_t required_size_in_bytes = derived().calc_required_size();

        m_size_in_bytes = required_size_in_bytes;

        // Add additional cachelines based on the requested padding units
        // to avoid cache set associativity conflicts
        size_t padding_offset = a_padding.units()*64;
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
    }
};

} // namepace internal
} // namepace __SDLT_IMPL
} // namespace sdlt


#endif // SDLT_AOS_CONTAINER_BASE_H
