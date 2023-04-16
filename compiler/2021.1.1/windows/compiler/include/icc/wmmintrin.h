/*  Copyright (c) 2020 Intel Corporation.
 *
 *  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception.
 */

/*
 * Principal header file for AES and PCLMULQDQ intrinsics.
 */


#ifndef _INCLUDED_WMM
#define _INCLUDED_WMM

#include <nmmintrin.h>


#ifdef __cplusplus
extern "C" {
#endif


#ifdef __INTEL_COMPILER_USE_INTRINSIC_PROTOTYPES
/*
 * Performs 1 round of AES decryption of the first m128i using
 * the second m128i as a round key.
 */
extern __m128i __ICL_INTRINCC _mm_aesdec_si128(__m128i, __m128i);

/*
 * Performs the last round of AES decryption of the first m128i
 * using the second m128i as a round key.
 */
extern __m128i __ICL_INTRINCC _mm_aesdeclast_si128(__m128i, __m128i);

/*
 * Performs 1 round of AES encryption of the first m128i using
 * the second m128i as a round key.
 */
extern __m128i __ICL_INTRINCC _mm_aesenc_si128(__m128i, __m128i);

/*
 * Performs the last round of AES encryption of the first m128i
 * using the second m128i as a round key.
 */
extern __m128i __ICL_INTRINCC _mm_aesenclast_si128(__m128i, __m128i);

/*
 * Performs the InverseMixColumn operation on the source m128i
 * and stores the result into m128i destination.
 */
extern __m128i __ICL_INTRINCC _mm_aesimc_si128(__m128i);

/*
 * Generates a m128i round key for the input m128i
 * AES cipher key and byte round constant.
 * The second parameter must be a compile time constant.
 */
extern __m128i __ICL_INTRINCC _mm_aeskeygenassist_si128(__m128i, const int);

/*
 * Performs carry-less integer multiplication of 64-bit halves
 * of 128-bit input operands.
 * The third parameter inducates which 64-bit haves of the input parameters
 * v1 and v2 should be used. It must be a compile time constant.
 */
extern __m128i __ICL_INTRINCC _mm_clmulepi64_si128(__m128i, __m128i,
                                                   const int);

#endif /* __INTEL_COMPILER_USE_INTRINSIC_PROTOTYPES */

#ifdef __cplusplus
}
#endif

#endif  /* _INCLUDED_WMM */
