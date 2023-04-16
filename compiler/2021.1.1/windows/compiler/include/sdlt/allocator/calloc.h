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

#ifndef SDLT_ALLOCATOR_CALLOC_H
#define SDLT_ALLOCATOR_CALLOC_H

#include "../common.h"
#include <cstdlib>
#include "../internal/is_aligned.h"
#include "../internal/pointer_or_number.h"
#include "../internal/power_of_two.h"

namespace sdlt {
namespace allocator {

namespace __SDLT_IMPL {

// WARNING:  using the calloc could hurt performance or cause
// faults on hardware expecting aligned memory accesses
template <int BoundaryT>
struct calloc
{
    static_assert(BoundaryT >= sizeof(int), "We need enough padding to store an integer");
    static_assert(sdlt::__SDLT_IMPL::internal::power_of_two<BoundaryT>::exists, "BoundaryT must be a power of two");

    static SDLT_INLINE void* allocate(size_t byte_count);
    static SDLT_INLINE void free(void *pointer, size_t byte_count);
};

// Implementation


template <int BoundaryT>
void*
calloc<BoundaryT>::allocate(size_t byte_count)
{
    size_t padded_byte_count = byte_count + BoundaryT;
    void * base_pointer = ::calloc(1, padded_byte_count);

    __SDLT_ASSERT(sdlt::__SDLT_IMPL::internal::is_aligned<sizeof(int)>(base_pointer));

    sdlt::__SDLT_IMPL::internal::pointer_or_number pon;
    pon.pointer = base_pointer;

    int byte_count_to_cacheline_boundary = static_cast<int>(pon.number%BoundaryT);
    // Check that we will always pad enough to store an int before the data pointer
    __SDLT_ASSERT((BoundaryT - byte_count_to_cacheline_boundary)>static_cast<int>(sizeof(int)));

    void * data_pointer = reinterpret_cast<unsigned char *>(base_pointer) + BoundaryT - byte_count_to_cacheline_boundary;
    // Store how many bytes we skipped in the 1st byte before the data_pointer,
    // we will always skip 4 to BoundaryT bytes, so there should always be room
    *(reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(data_pointer) - sizeof(int))) = byte_count_to_cacheline_boundary;
    return data_pointer;
}

template <int BoundaryT>
void
calloc<BoundaryT>::free(
    void *a_data_pointer,
    size_t // byte_count
)
{
    int byte_count_to_cacheline_boundary = *(reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(a_data_pointer) - sizeof(int)));
    void * base_pointer = reinterpret_cast<unsigned char *>(a_data_pointer) - BoundaryT + byte_count_to_cacheline_boundary;
    ::free(base_pointer);
}

} // namepace __SDLT_IMPL
using __SDLT_IMPL::calloc;
} // namepace allocator
} // namespace sdlt

#endif // SDLT_ALLOCATOR_CALLOC_H
