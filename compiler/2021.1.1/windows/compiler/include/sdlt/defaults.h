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

#ifndef SDLT_DEFAULTS_H
#define SDLT_DEFAULTS_H

#include "common.h"
#include "internal/simd_vector_register.h"
#include "primitive.h"
#include "simd_traits.h"

namespace sdlt {
namespace __SDLT_IMPL {

template<typename PrimitiveT>
struct defaults
{
    static_assert(primitive<PrimitiveT>::declared, "You must declare SDLT_PRIMITIVE(STRUCT_NAME, MEMBER1, MEMBER2, ..., MemberN) before trying to access a defaults<STRUCT_NAME>");

    static constexpr int lane_count = simd_traits<typename primitive<PrimitiveT>::largest_builtin_type>::lane_count;
    static constexpr int byte_alignment = internal::simd_vector_register<typename primitive<PrimitiveT>::largest_builtin_type>::width_in_bytes;
};

} // namepace __SDLT_IMPL
using __SDLT_IMPL::defaults;
} // namespace sdlt

#endif // SDLT_DEFAULTS_H
