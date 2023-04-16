/*===---- x86intrin.h - X86 intrinsics -------------------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */

#ifndef __X86INTRIN_H
#define __X86INTRIN_H

/* Moved to immintrin.h */
/*#include <ia32intrin.h>*/

#include <immintrin.h>

#if !(defined(_MSC_VER) || defined(__SCE__)) || __has_feature(modules) ||      \
    defined(__3dNOW__) || defined(__M_INTRINSIC_PROMOTE__)
#include <mm3dnow.h>
#endif

#if !(defined(_MSC_VER) || defined(__SCE__)) || __has_feature(modules) ||      \
    defined(__PRFCHW__) || defined(__M_INTRINSIC_PROMOTE__)
#include <prfchwintrin.h>
#endif

#if !(defined(_MSC_VER) || defined(__SCE__)) || __has_feature(modules) ||      \
    defined(__SSE4A__) || defined(__M_INTRINSIC_PROMOTE__)
#include <ammintrin.h>
#endif

#if !(defined(_MSC_VER) || defined(__SCE__)) || __has_feature(modules) ||      \
    defined(__FMA4__) || defined(__M_INTRINSIC_PROMOTE__)
#include <fma4intrin.h>
#endif

#if !(defined(_MSC_VER) || defined(__SCE__)) || __has_feature(modules) ||      \
    defined(__XOP__) || defined(__M_INTRINSIC_PROMOTE__)
#include <xopintrin.h>
#endif

#if !(defined(_MSC_VER) || defined(__SCE__)) || __has_feature(modules) ||      \
    defined(__TBM__) || defined(__M_INTRINSIC_PROMOTE__)
#include <tbmintrin.h>
#endif

#if !(defined(_MSC_VER) || defined(__SCE__)) || __has_feature(modules) ||      \
    defined(__LWP__) || defined(__M_INTRINSIC_PROMOTE__)
#include <lwpintrin.h>
#endif

#if !(defined(_MSC_VER) || defined(__SCE__)) || __has_feature(modules) ||      \
    defined(__MWAITX__) || defined(__M_INTRINSIC_PROMOTE__)
#include <mwaitxintrin.h>
#endif

#if !(defined(_MSC_VER) || defined(__SCE__)) || __has_feature(modules) ||      \
    defined(__CLZERO__) || defined(__M_INTRINSIC_PROMOTE__)
#include <clzerointrin.h>
#endif


#endif /* __X86INTRIN_H */
