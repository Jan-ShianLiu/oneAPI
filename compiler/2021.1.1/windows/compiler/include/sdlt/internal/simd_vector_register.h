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

#ifndef SDLT_SIMD_VECTOR_REGISTER_H
#define SDLT_SIMD_VECTOR_REGISTER_H

#include <stdint.h>

#include "../common.h"
#include "../internal/is_builtin.h"
#include "../isa.h"

namespace sdlt {
namespace __SDLT_IMPL {
namespace internal {


// sse2, sse3, IsaSSE4.1, IsaSSE4.2
// defined by the default template
template <
    typename BuiltInT,
    isa IsaT = compiler_defined_isa
>
struct simd_vector_register
{
    static_assert(is_builtin<BuiltInT>::value, "simd_vector_register is only valid for built-in types");
    static const int width_in_bytes = 128/8;
};

// However, not all instructions sets offer full width instructions
// for all data types, so we will need to define specializations for them


// Specialize for avx512
template <typename BuiltInT>
struct simd_vector_register<BuiltInT, isa::avx512>
{
    static_assert(is_builtin<BuiltInT>::value, "simd_vector_register is only valid for built-in types");
    static const int width_in_bytes = 512/8;
};

// Specialize for avx512
template <>
struct simd_vector_register<uint8_t, isa::avx512>
{
    // Can only use YMM registers for < 32 bit operations
    static const int width_in_bytes = 256/8;
};

// Specialize for avx512
template <>
struct simd_vector_register<int8_t, isa::avx512>
{
    // Can only use YMM registers for < 32 bit operations
    static const int width_in_bytes = 256/8;
};

// Specialize for avx512
template <>
struct simd_vector_register<uint16_t, isa::avx512>
{
    // Can only use YMM registers for < 32 bit operations
    static const int width_in_bytes = 256/8;
};

// Specialize for avx512
template <>
struct simd_vector_register<int16_t, isa::avx512>
{
    // Can only use YMM registers for < 32 bit operations
    static const int width_in_bytes = 256/8;
};

// Specialize for avx512bw
template <typename BuiltInT>
struct simd_vector_register<BuiltInT, isa::avx512bw>
{
    static_assert(is_builtin<BuiltInT>::value, "simd_vector_register is only valid for built-in types");
    static const int width_in_bytes = 512/8;
};

// Specialize for AVX
template <typename BuiltInT>
struct simd_vector_register<BuiltInT, isa::avx>
{
    static_assert(is_builtin<BuiltInT>::value, "simd_traits is only valid for built-in types");
    static const int width_in_bytes = 128/8;
};
template <>
struct simd_vector_register<float, isa::avx>
{
    static const int width_in_bytes = 256/8;
};
template <>
struct simd_vector_register<double, isa::avx>
{
    static const int width_in_bytes = 256/8;
};


// Specialize for AVX2
template <typename BuiltInT>
struct simd_vector_register<BuiltInT, isa::avx2>
{
    static_assert(is_builtin<BuiltInT>::value, "simd_traits is only valid for built-in types");
    static const int width_in_bytes = 256/8;
};

// Specialize for IsaKNCi
template <typename BuiltInT>
struct simd_vector_register<BuiltInT, isa::mic_ni>
{
    static_assert(is_builtin<BuiltInT>::value, "simd_traits is only valid for built-in types");
    static const int width_in_bytes = 512/8;
};
template <>
struct simd_vector_register<uint8_t, isa::mic_ni>
{
    // Must promote to 32 bit integer
    static const int width_in_bytes = (512/8)/(sizeof(uint32_t)/sizeof(uint8_t));
};
template <>
struct simd_vector_register<int8_t, isa::mic_ni>
{
    // Must promote to 32 bit integer
    static const int width_in_bytes = (512/8)/(sizeof(int32_t)/sizeof(int8_t));
};
template <>
struct simd_vector_register<uint16_t, isa::mic_ni>
{
    // Must promote to 32 bit integer
    static const int width_in_bytes = (512/8)/(sizeof(uint32_t)/sizeof(uint16_t));
};
template <>
struct simd_vector_register<int16_t, isa::mic_ni>
{
    // Must promote to 32 bit integer
    static const int width_in_bytes = (512/8)/(sizeof(int32_t)/sizeof(int16_t));
};

} // namespace internal
} // namepace __SDLT_IMPL
} // namespace sdlt

#endif // SDLT_SIMD_VECTOR_REGISTER_H
