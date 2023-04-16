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

#ifndef SDLT_ALLOCATOR_MEM_ALIGN_H
#define SDLT_ALLOCATOR_MEM_ALIGN_H

#include "../common.h"

#if __SDLT_ON_WINDOWS
    #error sdlt::allocator::mem_align is not supported on Windows, do not include it
#endif // __SDLT_ON_WINDOWS

#include "../min_max_val.h"

#include <stdlib.h>
#include <stdio.h>

#include "../internal/is_aligned.h"
#include "../internal/power_of_two.h"

#ifdef __MIC__
    #include <errno.h>
    #include <sys/mman.h>
#endif

namespace sdlt {
namespace allocator {
namespace __SDLT_IMPL {

template <int BoundaryT, bool AllowHugePagesT = false>
struct mem_align
{
    static_assert(BoundaryT >= sizeof(void *), "posix_memalign requires BoundaryT to be a multiple of sizeof(void *)");
    static_assert(sdlt::__SDLT_IMPL::internal::power_of_two<BoundaryT>::exists, "BoundaryT must be a power of two");

    static SDLT_INLINE void* allocate(size_t byte_count);
    static SDLT_INLINE void free(void *pointer, size_t byte_count);
};

// Implementation
template <int BoundaryT, bool AllowHugePagesT>
void*
mem_align<BoundaryT, AllowHugePagesT>::allocate(size_t bcount)
{
    size_t block_count = (bcount + size_t(BoundaryT - 1))/BoundaryT;
    size_t byte_count = block_count*BoundaryT;
    __SDLT_ASSERT(byte_count >= bcount);
    void * pointer;
    int return_value = posix_memalign(&pointer, BoundaryT, byte_count);
    if ((return_value != 0) || (pointer == nullptr))
    {
        fprintf(stderr, "%s failed to allocate %zu bytes using posix_memalign with a boundary of %d\n",
            __SDLT_FUNCTION_SIG, byte_count, BoundaryT);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
    __SDLT_ASSERT(sdlt::__SDLT_IMPL::internal::is_aligned<BoundaryT>(pointer));
    #ifdef __MIC__
        if (AllowHugePagesT == false) {
            // We can only advise for pointers on page boundaries
            if (BoundaryT%4096 == 0) {
                return_value = madvise(pointer, byte_count, MADV_NOHUGEPAGE );
                if (return_value != 0) {
                    // Hitting this on MIC, believe that pages get reused when going through
                    // posix_memalign, and if we have previously advised MADV_NOHUGEPAGE,
                    // to do so again would be an EINVAL error
                    if (errno != EINVAL)
                    {
                        #if SDLT_DEBUG
                            fprintf(stderr, "%s failed to madvise MADV_NOHUGEPAGE with errno=%d\n",
                                __SDLT_FUNCTION_SIG, errno);
                            fflush(stderr);
                            // Just a hint, no need to exit, the allocation still succeeded
                        #endif
                    }
                }
            }
        }
        else {
            // We can only advise for pointers on page boundaries
            if (BoundaryT%2097152 == 0) {
                return_value = madvise(pointer, byte_count, MADV_HUGEPAGE);
                if (return_value != 0)
                {
                    // Hitting this on MIC, believe that pages get reused when going through
                    // posix_memalign, and if we have previously advised MADV_HUGEPAGE,
                    // to do so again would be an EINVAL error
                    if (errno != EINVAL) {
                        #if SDLT_DEBUG
                            fprintf(stderr, "%s failed to madvise MADV_HUGEPAGE with errno=%d\n",
                                __SDLT_FUNCTION_SIG, errno);
                            fflush(stderr);
                            printf("failed to madvise MADV_HUGEPAGE with errno=%d\n", errno);
                            // Just a hint, no need to exit, the allocation still succeeded
                        #endif
                    }
                }
            }
        }

    #endif

    return pointer;
}

template <int BoundaryT, bool AllowHugePagesT>
void
mem_align<BoundaryT, AllowHugePagesT>::free(
    void *a_pointer,
    size_t // byte_count
)
{
    ::free(a_pointer);
}

} // namepace __SDLT_IMPL
using __SDLT_IMPL::mem_align;
} // namepace allocator
} // namespace sdlt

#endif // SDLT_ALLOCATOR_MEM_ALIGN_H
