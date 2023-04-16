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

#ifndef SDLT_ISA_H
#define SDLT_ISA_H

namespace sdlt {
namespace __SDLT_IMPL {

enum class isa : int
{
    sse2,
    sse3,
    sse4_1,
    sse4_2,
    avx,
    avx2,
    avx512, // 128, 256 bit byte and word vector operations
    avx512bw, // 128, 256, 512 bit byte and word vector operations
    mic_ni
};

static const isa compiler_defined_isa
#ifdef __INTEL_COMPILER
    #if defined(__MIC__)
      = isa::mic_ni;
    #else
        #if defined(__AVX512F__)
            #if defined(__AVX512BW__)
                = isa::avx512bw;
            #else
                = isa::avx512;
            #endif // __AVX512BW__
        #else
            #if defined(__AVX2__)
                = isa::avx2;
            #elif defined(__AVX__)
                = isa::avx;
            #elif defined(__SSE4_2__)
                = isa::sse4_2;
            #elif defined(__SSE4_1__)
                = isa::sse4_1;
            #elif defined(__SSE3__)
                = isa::sse3;
            #else
                #if !defined(__SSE2__)
                    #error unsupported architecture
                #endif
                = isa::sse2;
            #endif
        #endif
    #endif
#else
    // Not __INTEL_COMPILER so we will default to lowest possible architecture
    // TODO: add code to detect target architecture for other compilers
    = isa::sse2;
#endif // __INTEL_COMPILER


} // namepace __SDLT_IMPL
using __SDLT_IMPL::isa;
using __SDLT_IMPL::compiler_defined_isa;

} // namespace sdlt

#endif // SDLT_ISA_H
