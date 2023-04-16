//
// Copyright (C) 2015-2016 Intel Corporation. All Rights Reserved.
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

#ifndef SDLT_LAYOUT_H
#define SDLT_LAYOUT_H

#include "n_extent.h"

namespace sdlt {
namespace layout {
namespace __SDLT_IMPL {

    // Suggest using soa_per_row for most algorithms with unit-stride 
    template<int AlignOnColumnIndexT = 0>
    struct soa_per_row {};

    template<int AlignOnColumnIndexT = 0>
    struct soa {};

    struct aos_by_struct {};
    struct aos_by_stride {};

    // Reserved for future use
    //template<typename TileExtentsT, int AlignOnColumnIndexT = 0>
    //struct tiled_soa
    //{
    //    static_assert(std::is_default_constructible<TileExtentsT>::value, "ExtentsT must be defined in terms of only fixed<int NumberT> where NumberT is a power of 2")
    //    typedef ExtentsT tile_extents;
    //};

    // Reserved for future use
    //template<typename TileExtentsT, int AlignOnColumnIndexT = 0>
    //struct tiled_soa_per_row
    //{
    //    static_assert(std::is_default_constructible<TileExtentsT>::value, "ExtentsT must be defined in terms of only fixed<int NumberT> where NumberT is a power of 2")
    //    typedef ExtentsT tile_extents;
    //};

} // namepace __SDLT_IMPL

using __SDLT_IMPL::soa;
using __SDLT_IMPL::soa_per_row;
using __SDLT_IMPL::aos_by_struct;
using __SDLT_IMPL::aos_by_stride;

// Reserved for future use
//using __SDLT_IMPL::tiled_soa;
//using __SDLT_IMPL::tiled_soa_per_row;

} // namepace layout
} // namespace sdlt

#endif // SDLT_LAYOUT_H
