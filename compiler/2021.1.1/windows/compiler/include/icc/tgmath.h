/* file: tgmath.h */

/*
** Copyright  (C) 1985-2018 Intel Corporation. All rights reserved.
**
** The information and source code contained herein is the exclusive property
** of Intel Corporation and may not be disclosed, examined, or reproduced in
** whole or in part without explicit written authorization from the Company.
**
*/

/*
 * tgmath.h - Intel generic math C99 header
 */

/* Include_next should be before guard macros in order to at last reach system header */
#if defined(__PURE_SYS_C99_HEADERS__) && !(defined(_WIN32) || defined(_WIN64)) && !defined (__APPLE__) && !defined(__INTEL_CLANG_COMPILER)
# include_next <tgmath.h> /* utilize sys header */
#elif defined(__INTEL_CLANG_COMPILER) && !defined(__INTEL_LLVM_COMPILER)
# include <tgmath_clang.h> /* utilize header with appropriate version */
#else

#if !defined(__PURE_INTEL_C99_HEADERS__) && !(defined(_WIN32) || defined(_WIN64))
#if !defined (__APPLE__) && !defined(__NetBSD__) && !defined(__FreeBSD__) && !defined(__QNX__) && !defined(__VXWORKS__) && !defined(__ANDROID__)
# include_next <tgmath.h> /* utilize and expand sys header */
#endif

#ifndef __TGMATH_H_INCLUDED
/* Will undef sys macro only first time Intel header is included.
 * Though do not need to define this guard macros here, we'll define it below.
 */

# undef acos(x)
# undef asin(x)
# undef atan(x)
# undef acosh(x)
# undef asinh(x)
# undef atanh(x)
# undef cos(x)
# undef sin(x)
# undef tan(x)
# undef cosh(x)
# undef sinh(x)
# undef tanh(x)
# undef exp(x)
# undef log(x)
# undef pow(x,y)
# undef sqrt(x)
# undef fabs(x)
# undef atan2(x, y)
# undef cbrt(x)
# undef ceil(x)
# undef copysign(x, y)
# undef erf(x)
# undef erfc(x)
# undef exp2(x)
# undef expm1(x)
# undef fdim(x, y)
# undef floor(x)
# undef fma(x, y, z)
# undef fmax(x, y)
# undef fmin(x, y)
# undef fmod(x, y)
# undef frexp(x, y)
# undef hypot(x, y)
# undef ilogb(x)
# undef ldexp(x, y)
# undef lgamma(x)
# undef llrint(x)
# undef llround(x)
# undef log10(x)
# undef log1p(x)
# undef log2(x)
# undef logb(x)
# undef lrint(x)
# undef lround(x)
# undef nearbyint(x)
# undef nextafter(x, y)
# undef nexttoward(x, y)
# undef remainder(x, y)
# undef remquo(x, y, z)
# undef rint(x)
# undef round(x)
# undef scalbn(x, y)
# undef scalbln(x, y)
# undef tgamma(x)
# undef trunc(x)
# undef carg(x)
# undef cimag(x)
# undef conj(x)
# undef cproj(x)
# undef creal(x)

#endif  /*__TGMATH_H_INCLUDED*/

#endif /* use and expand sys headers */

#ifndef __TGMATH_H_INCLUDED
#define __TGMATH_H_INCLUDED

/* Need to define it since tgmath.h re-define math functions and
   all required info will be got from math.h complex.h */
#ifndef __MATHIMF_H_INCLUDED
#define __MATHIMF_H_INCLUDED
#endif

/* Check usage correctness */
#if !(defined(__INTEL_COMPILER) || defined(__INTEL_LLVM_COMPILER))
# error "This Intel <tgmath.h> is for use with only the Intel compilers!"
#endif

#include <math.h>
#include <complex.h>

#if defined(__cplusplus)    /* C linkage */
extern "C" {
#endif

#if (defined(__VXWORKS__) && defined(_WRS_KERNEL))
    #define _REAL_1_ARG(x,fname)         __generic((x),   ,   ,fname,fname##f,        ,        ,           ,           )(x)
    #define _REAL_2_ARG(x,y,fname)       __generic((x),(y),   ,fname,fname##f,        ,        ,           ,           )((x),(y))
    #define _REAL_3_ARG(x,y,z,fname)     __generic((x),(y),(z),fname,fname##f,        ,        ,           ,           )((x),(y),(z))
    #define _REAL_1_2_ARG(x,y,fname)     __generic((x),   ,   ,fname,fname##f,        ,        ,           ,           )((x),(y))
    #define _REAL_2_3_ARG(x,y,z,fname)   __generic((x),(y),   ,fname,fname##f,        ,        ,           ,           )((x),(y),(z))
    #define _CMPLX_1_ARG(x,fname)        __generic((x),   ,   ,     ,        ,        ,   fname,   fname##f,           )(x)
    #define _CMPLX_2_ARG(x,y,fname)      __generic((x),(y),   ,     ,        ,        ,   fname,   fname##f,           )((x),(y))
    #define _FABS_1_ARG(x,fname)         __generic((x),   ,   ,fname,fname##f,        ,        ,           ,           )(x)
    #define _REAL_CMPLX_1_ARG(x,fname)   __generic((x),   ,   ,fname,fname##f,        ,        ,           ,           )(x)
    #define _C_CMPLX_1_ARG(x,fname)      __generic((x),   ,   ,fname,fname##f,        ,   fname,   fname##f,           )(x)
    #define _REAL_CMPLX_2_ARG(x,y,fname) __generic((x),(y),   ,fname,fname##f,        ,        ,           ,           )((x),(y))
#else
    #define _REAL_1_ARG(x,fname)         __generic((x),   ,   ,fname,fname##f,fname##l,        ,           ,           )(x)
    #define _REAL_2_ARG(x,y,fname)       __generic((x),(y),   ,fname,fname##f,fname##l,        ,           ,           )((x),(y))
    #define _REAL_3_ARG(x,y,z,fname)     __generic((x),(y),(z),fname,fname##f,fname##l,        ,           ,           )((x),(y),(z))
    #define _REAL_1_2_ARG(x,y,fname)     __generic((x),   ,   ,fname,fname##f,fname##l,        ,           ,           )((x),(y))
    #define _REAL_2_3_ARG(x,y,z,fname)   __generic((x),(y),   ,fname,fname##f,fname##l,        ,           ,           )((x),(y),(z))
    #define _CMPLX_1_ARG(x,fname)        __generic((x),   ,   ,     ,        ,        ,   fname,   fname##f,   fname##l)(x)
    #define _CMPLX_2_ARG(x,y,fname)      __generic((x),(y),   ,     ,        ,        ,   fname,   fname##f,   fname##l)((x),(y))
    #define _FABS_1_ARG(x,fname)         __generic((x),   ,   ,fname,fname##f,fname##l,   cabs ,   cabsf   ,   cabsl   )(x)
    #define _REAL_CMPLX_1_ARG(x,fname)   __generic((x),   ,   ,fname,fname##f,fname##l,c##fname,c##fname##f,c##fname##l)(x)
    #define _C_CMPLX_1_ARG(x,fname)      __generic((x),   ,   ,fname,fname##f,fname##l,   fname,   fname##f,   fname##l)(x)
    #define _REAL_CMPLX_2_ARG(x,y,fname) __generic((x),(y),   ,fname,fname##f,fname##l,c##fname,c##fname##f,c##fname##l)((x),(y))
#endif

#define acos(x)    _REAL_CMPLX_1_ARG(x, acos)
#define asin(x)    _REAL_CMPLX_1_ARG(x, asin)
#define atan(x)    _REAL_CMPLX_1_ARG(x, atan)
#if !(defined(__VXWORKS__) && defined(_WRS_KERNEL))
#define acosh(x)   _REAL_CMPLX_1_ARG(x, acosh)
#define asinh(x)   _REAL_CMPLX_1_ARG(x, asinh)
#define atanh(x)   _REAL_CMPLX_1_ARG(x, atanh)
#endif
#define cos(x)     _REAL_CMPLX_1_ARG(x, cos)
#define sin(x)     _REAL_CMPLX_1_ARG(x, sin)
#define tan(x)     _REAL_CMPLX_1_ARG(x, tan)
#define cosh(x)    _REAL_CMPLX_1_ARG(x, cosh)
#define sinh(x)    _REAL_CMPLX_1_ARG(x, sinh)
#define tanh(x)    _REAL_CMPLX_1_ARG(x, tanh)
#define exp(x)     _REAL_CMPLX_1_ARG(x, exp)
#define log(x)     _REAL_CMPLX_1_ARG(x, log)
#define pow(x, y)  _REAL_CMPLX_2_ARG(x, y, pow)
#if !(defined(__VXWORKS__) && defined(_WRS_KERNEL))
#define sqrt(x)    _REAL_CMPLX_1_ARG(x, sqrt)
#endif
#define fabs(x)          _FABS_1_ARG(x, fabs)
#if !(defined(__VXWORKS__) && defined(_WRS_KERNEL))
#define atan2(x, y)      _REAL_2_ARG(x, y, atan2)
#define cbrt(x)          _REAL_1_ARG(x, cbrt)
#endif
#define ceil(x)          _REAL_1_ARG(x, ceil)
#if !(defined(__VXWORKS__) && defined(_WRS_KERNEL))
#define copysign(x, y)   _REAL_2_ARG(x, y, copysign)
#define erf(x)           _REAL_1_ARG(x, erf)
#define erfc(x)          _REAL_1_ARG(x, erfc)
#define exp2(x)          _REAL_1_ARG(x, exp2)
#define expm1(x)         _REAL_1_ARG(x, expm1)
#define fdim(x, y)       _REAL_2_ARG(x, y, fdim)
#endif
#define floor(x)         _REAL_1_ARG(x, floor)
#if !(defined(__VXWORKS__) && defined(_WRS_KERNEL))
#define fma(x, y, z)     _REAL_3_ARG(x,y,z,fma)
#define fmax(x, y)       _REAL_2_ARG(x, y, fmax)
#define fmin(x, y)       _REAL_2_ARG(x, y, fmin)
#endif
#define fmod(x, y)       _REAL_2_ARG(x, y, fmod)
#define frexp(x, y)      _REAL_1_2_ARG(x, y, frexp)
#if !(defined(__VXWORKS__) && defined(_WRS_KERNEL))
#define hypot(x, y)      _REAL_2_ARG(x, y, hypot)
#define ilogb(x)         _REAL_1_ARG(x, ilogb)
#endif
#define ldexp(x, y)      _REAL_1_2_ARG(x, y, ldexp)
#if !(defined(__VXWORKS__) && defined(_WRS_KERNEL))
#define lgamma(x)        _REAL_1_ARG(x, lgamma)
#define llrint(x)        _REAL_1_ARG(x, llrint)
#define llround(x)       _REAL_1_ARG(x, llround)
#endif
#define log10(x)         _REAL_1_ARG(x, log10)
#if !(defined(__VXWORKS__) && defined(_WRS_KERNEL))
#define log1p(x)         _REAL_1_ARG(x, log1p)
#define log2(x)          _REAL_1_ARG(x, log2)
#define logb(x)          _REAL_1_ARG(x, logb)
#define lrint(x)         _REAL_1_ARG(x, lrint)
#define lround(x)        _REAL_1_ARG(x, lround)
#define nearbyint(x)     _REAL_1_ARG(x, nearbyint)
#define nextafter(x, y)  _REAL_2_ARG(x, y, nextafter)
#define nexttoward(x, y) _REAL_1_2_ARG(x, y, nexttoward)
#define remainder(x, y)  _REAL_2_ARG(x, y, remainder)
#define remquo(x, y, z)  _REAL_2_3_ARG(x, y, z, remquo)
#define rint(x)          _REAL_1_ARG(x, rint)
#define round(x)         _REAL_1_ARG(x, round)
#define scalbn(x, y)     _REAL_1_2_ARG(x, y, scalbn)
#define scalbln(x, y)    _REAL_1_2_ARG(x, y, scalbln)
#define tgamma(x)        _REAL_1_ARG(x, tgamma)
#define trunc(x)         _REAL_1_ARG(x, trunc)
#define carg(x)          _C_CMPLX_1_ARG(x, carg)
#define cimag(x)         _C_CMPLX_1_ARG(x, cimag)
#define conj(x)          _C_CMPLX_1_ARG(x, conj)
#define cproj(x)         _C_CMPLX_1_ARG(x, cproj)
#define creal(x)         _C_CMPLX_1_ARG(x, creal)
#endif

#if defined(__cplusplus)    /* end C linkage */
}
#endif

#endif  /*__TGMATH_H_INCLUDED */
#endif  /* usage of sys headers */