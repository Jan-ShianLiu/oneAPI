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

#ifndef SDLT_SIMD_TRAITS_H
#define SDLT_SIMD_TRAITS_H

#include <immintrin.h>
#include <stdint.h>

#include "common.h"
#include "internal/compiler_defined.h"
#include "internal/is_builtin.h"
#include "internal/simd_vector_register.h"
#include "isa.h"

namespace sdlt {
namespace __SDLT_IMPL {

template <
    typename BuiltInT,
    isa IsaT = compiler_defined_isa
>
struct simd_traits
{
    static_assert(internal::is_builtin<BuiltInT>::value, "simd_traits is only valid for built-in types");

    static const int lane_count = static_cast<int>(internal::simd_vector_register<BuiltInT, IsaT>::width_in_bytes/sizeof(BuiltInT));

private:
    static int populate_widest_supported_lane_count() {
        int vector_register_width_in_bytes;
        #ifdef __INTEL_COMPILER
            if ((true == _may_i_use_cpu_feature(_FEATURE_AVX512F)) ||
                (true == _may_i_use_cpu_feature(_FEATURE_KNCNI))) {
                vector_register_width_in_bytes = 64;
            } else {
                if ((true == _may_i_use_cpu_feature(_FEATURE_AVX)) ||
                    (true == _may_i_use_cpu_feature(_FEATURE_AVX2))) {
                    vector_register_width_in_bytes = 32;
                } else {
                    vector_register_width_in_bytes = 16;
                }
            }
        #else
            vector_register_width_in_bytes = 16;
        #endif

        return vector_register_width_in_bytes/sizeof(BuiltInT);
    }
public:

    static int widest_supported_lane_count() {
        static int lane_count = populate_widest_supported_lane_count();
        return lane_count;
    }
};


} // namepace __SDLT_IMPL
using __SDLT_IMPL::simd_traits;
} // namespace sdlt

#endif // SDLT_SIMD_TRAITS_H
