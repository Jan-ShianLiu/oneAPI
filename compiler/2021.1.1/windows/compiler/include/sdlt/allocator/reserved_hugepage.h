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

#ifndef SDLT_ALLOCATOR_RESERVED_HUGE_PAGE_H
#define SDLT_ALLOCATOR_RESERVED_HUGE_PAGE_H

#include "../common.h"

#if (SDLT_ON_LINUX == 0)
    #error sdlt::allocator::reserved_hugepage is supported only on Linux, do not include it
#endif // (SDLT_ON_LINUX == 0)

#include "../internal/is_aligned.h"
#include "../internal/pointer_or_number.h"

#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <sys/mman.h>

namespace sdlt {
namespace allocator {
namespace __SDLT_IMPL {

// utilize mmap to get buffer using 2MB pages
// meant for use on MIC architecture

// read /usr/share/doc/kernel-doc-2.6.32/Documentation/vm/hugetlbpage.txt
// for internals.
//
// Assumes caller has already set /proc/sys/vm/nr_hugepages large enough
// to handle the requests
//
// NOTE: newer MIC MPSS appears to automatically change nr_hugepages
// on demand so no need to do it manually
struct reserved_hugepage
{
    static SDLT_INLINE bool is_dynamic();
    static SDLT_INLINE int current_count();
    static SDLT_INLINE int required_count_for(size_t byte_count);

    static SDLT_INLINE void* allocate(size_t byte_count);
    static SDLT_INLINE void free(void *pointer, size_t byte_count);
};

// Implementation

bool reserved_hugepage::is_dynamic()
{
    #ifdef __MIC__
        return true;
    #else
        return false;
    #endif
}

int reserved_hugepage::required_count_for(size_t byte_count)
{
    int requiredPageCount = static_cast<int>((byte_count + (2*1024*1024) -1)/(2*1024*1024));
    return requiredPageCount;
}

int reserved_hugepage::current_count()
{
    FILE *f = fopen("/proc/sys/vm/nr_hugepages", "r");
    char line[1024];
    fgets(line,1024,f);
    fclose(f);

    int number_of_reserved_hugepages = atoi(line);
    //printf("\nnumberOfReservedHugepages=%d\n", number_of_reserved_hugepages);
    return number_of_reserved_hugepages;
}


void*
reserved_hugepage::allocate(size_t byte_count)
{
    if (byte_count == 0)
    {
        // Just return a dummy pointer for an allocation of 0 bytes
        // no one should be ever dereference it.
        // However, we want any calling code to be able to check
        // correct pointer alignment, etc.
        sdlt::__SDLT_IMPL::internal::pointer_or_number pon;
        pon.number = 2*1024*1024;
        return pon.pointer;
    }
    // MAP_HUGETLB pages will give us 2MB page size
    #ifndef MAP_HUGETLB
        #define MAP_HUGETLB 0x40000
    #endif

    void * buffer_backed_by_2mb_pages =
        mmap(
            nullptr,
            byte_count,
            PROT_READ | PROT_WRITE,
            MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB,
            -1,
            0);
    if (buffer_backed_by_2mb_pages == reinterpret_cast<void *>(-1)) {
        int save_error = errno;
        fprintf(stderr, "%s failed to mmap %zu bytes from 2MB pages,\n\
                        errno=%d, you need to manually reserve enough 2MB pages before running.  i.e.\n\
                        echo 128 > /proc/sys/vm/nr_hugepages\n",
                        __SDLT_FUNCTION_SIG, byte_count, save_error);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
    __SDLT_ASSERT(sdlt::__SDLT_IMPL::internal::is_aligned<2*1024*1024>(buffer_backed_by_2mb_pages));

    return buffer_backed_by_2mb_pages;
}

void
reserved_hugepage::free(
    void *a_pointer,
    size_t byte_count
)
{
    if (byte_count == 0)
    {
        // Verify dummy pointer
        #if SDLT_DEBUG
            sdlt::__SDLT_IMPL::internal::pointer_or_number pon;
            pon.pointer = a_pointer;
            __SDLT_ASSERT(pon.number == 2*1024*1024);
        #endif
        return;
    }

    __SDLT_ASSERT(sdlt::__SDLT_IMPL::internal::is_aligned<2*1024*1024>(a_pointer));
    size_t bytes_to_unmap = required_count_for(byte_count)*(2*1024*1024);
    // Need to unmap the entire 2mb pages, not just the bytes allocated
    int success = munmap(a_pointer, bytes_to_unmap);
    if (success != 0) {
        fprintf(stderr, "%s failed to munmap with errno=%d for %zu bytes from 2MB pages with address %p\n",
                __SDLT_FUNCTION_SIG, errno, byte_count, a_pointer);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
}

} // namepace __SDLT_IMPL
using __SDLT_IMPL::reserved_hugepage;
} // namepace allocator
} // namespace sdlt

#endif // SDLT_ALLOCATOR_RESERVED_HUGE_PAGE_H
