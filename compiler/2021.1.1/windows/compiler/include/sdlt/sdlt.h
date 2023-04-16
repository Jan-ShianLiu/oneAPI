//
// Copyright (C) 2014-2016 Intel Corporation. All Rights Reserved.
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

#ifndef SDLT_SDLT__H
#define SDLT_SDLT__H

#include "common.h"
#include "aligned_accessor.h"
#include "aligned.h"
#include "aligned_offset.h"
#include "aos1d_accessor.h"
#include "aos1d_const_accessor.h"
#include "aos1d_container.h"
#include "asa1d_container.h"
#include "buffer_offset_in_cachelines.h"
#include "const_data_member_proxy.h"
#include "data_member_proxy.h"
#include "defaults.h"
#include "fixed.h"
#include "fixed_offset.h"
#include "isa.h"
#include "layout.h"
#include "linear_index.h"
#include "min_max_val.h"
#include "n_bounds.h"
#include "n_container.h"
#include "n_extent.h"
#include "n_index.h"
#include "no_offset.h"
#include "primitive.h"
#include "simd_index.h"
#include "simd_traits.h"
#include "soa1d_container.h"
#include "allocator/calloc.h"
#include "allocator/hybrid.h"
#include "allocator/malloc.h"
#include "allocator/memory_block.h"
#include "allocator/std_adapter.h"
#if (SDLT_ON_LINUX == 1)
    #include "allocator/mem_align.h"
    #include "allocator/reserved_hugepage.h"
    #include "allocator/transparent_hugepage.h"
#endif



#endif // SDLT_SDLT__H
