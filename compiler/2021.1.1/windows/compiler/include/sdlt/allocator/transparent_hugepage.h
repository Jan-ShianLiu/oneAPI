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

#ifndef SDLT_ALLOCATOR_TRANSPARENT_HUGE_PAGE_H
#define SDLT_ALLOCATOR_TRANSPARENT_HUGE_PAGE_H

#include "mem_align.h"

#if __SDLT_ON_WINDOWS
    #error sdlt::allocator::transparent_hugepage is not supported on Windows, do not include it
#endif

namespace sdlt {
namespace allocator {
namespace __SDLT_IMPL {

__SDLT_INHERIT_IMPL_BEGIN

    // read /usr/share/doc/kernel-doc-version/Documentation/vm/transhuge.txt
    // By using pos_memalign with a 2MB boundary, it should transparently
    // place the data in anonymous huge pages
    struct transparent_hugepage :
        public mem_align<2*1024*1024, true>
    {
    };

__SDLT_INHERIT_IMPL_END

} // namepace __SDLT_IMPL
using __SDLT_IMPL::transparent_hugepage;
} // namepace allocator
} // namespace sdlt

#endif // SDLT_ALLOCATOR_TRANSPARENT_HUGE_PAGE_H
