/*  Copyright (c) 2020 Intel Corporation.
 *
 *  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception.
 */

/*
 * Principal header file for Intel(R) Pentium(R) 4 processor SSE3 intrinsics
 */

#ifndef _INCLUDED_PMM
#define _INCLUDED_PMM

#include <emmintrin.h>

/*****************************************************/
/*     INTRINSICS FUNCTION PROTOTYPES START HERE     */
/*****************************************************/

#if defined __cplusplus
extern "C" { /* Begin "C" */
  /* Intrinsics use C name-mangling. */
#endif /* __cplusplus */

#ifdef __INTEL_COMPILER_USE_INTRINSIC_PROTOTYPES
/*
 * New Single precision vector instructions.
 */

extern __m128 __ICL_INTRINCC _mm_addsub_ps(__m128, __m128);
extern __m128 __ICL_INTRINCC _mm_hadd_ps(__m128, __m128);
extern __m128 __ICL_INTRINCC _mm_hsub_ps(__m128, __m128);
extern __m128 __ICL_INTRINCC _mm_movehdup_ps(__m128);
extern __m128 __ICL_INTRINCC _mm_moveldup_ps(__m128);

/*
 * New double precision vector instructions.
 */

extern __m128d __ICL_INTRINCC _mm_addsub_pd(__m128d, __m128d);
extern __m128d __ICL_INTRINCC _mm_hadd_pd(__m128d, __m128d);
extern __m128d __ICL_INTRINCC _mm_hsub_pd(__m128d, __m128d);
extern __m128d __ICL_INTRINCC _mm_loaddup_pd(double const *);
extern __m128d __ICL_INTRINCC _mm_movedup_pd(__m128d);

/*
 * New unaligned integer vector load instruction.
 */
extern __m128i __ICL_INTRINCC _mm_lddqu_si128(__m128i const *);
#endif /* __INTEL_COMPILER_USE_INTRINSIC_PROTOTYPES */

/*
 * Miscellaneous new instructions.
 */
/*
 * For _mm_monitor p goes in eax, extensions goes in ecx, hints goes in edx.
 */
extern void __ICL_INTRINCC _mm_monitor(void const *, unsigned, unsigned);
/*
 * For _mm_mwait, extensions goes in ecx, hints goes in eax.
 */
extern void __ICL_INTRINCC _mm_mwait(unsigned, unsigned);

#if defined __cplusplus
}; /* End "C" */
#endif /* __cplusplus */
#endif /* _INCLUDED_PMM */
