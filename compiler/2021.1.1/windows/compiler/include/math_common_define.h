/* file: math_common_define.h */

/*
** Copyright  (C) 1985-2018 Intel Corporation. All rights reserved.
**
** The information and source code contained herein is the exclusive property
** of Intel Corporation and may not be disclosed, examined, or reproduced in
** whole or in part without explicit written authorization from the Company.
**
*/

#if defined(__cplusplus)
#define _LIBIMF_EXTERN_C extern "C"
#else
#define _LIBIMF_EXTERN_C extern
#endif

#if (defined(_WIN32) || defined(_WIN64))
    #if defined(__cplusplus)
        #define _LIBIMF_FORCEINLINE extern "C" __forceinline
    #else
        #define _LIBIMF_FORCEINLINE __forceinline
    #endif
#else
    #if defined(__cplusplus)
        #define _LIBIMF_FORCEINLINE extern "C" __attribute__((always_inline))
    #else
        #define _LIBIMF_FORCEINLINE __attribute__((always_inline))
    #endif
#endif

#ifndef __IMFLONGDOUBLE
    #if defined(__LONG_DOUBLE_SIZE__) /* Compiler-predefined macros. If defined, should be 128|80|64 */
        #define __IMFLONGDOUBLE (__LONG_DOUBLE_SIZE__)
    #else
        #define __IMFLONGDOUBLE 64
    #endif
#endif

#if defined(__cplusplus) && !(defined(__FreeBSD__) || defined(__ANDROID__))
#define _LIBIMF_CPP_EXCEPT_SPEC() throw()
#else
#define _LIBIMF_CPP_EXCEPT_SPEC()
#endif

#if defined(_DLL) && (defined(_WIN32) || defined(_WIN64)) /* Windows DLL */
# define _LIBIMF_PUBAPI __declspec(dllimport) __cdecl
# define _LIBIMF_PUBAPI_INL __cdecl
# define _LIBIMF_PUBVAR __declspec(dllimport)
#elif defined(__unix__) || defined(__APPLE__) || defined(__QNX__) || defined(__VXWORKS__) /* Linux, MacOS or QNX */
# define _LIBIMF_PUBAPI /* do not change this line! */
# define _LIBIMF_PUBAPI_INL
# define _LIBIMF_PUBVAR
#else /* Windows static */
# define _LIBIMF_PUBAPI __cdecl
# define _LIBIMF_PUBAPI_INL __cdecl
# define _LIBIMF_PUBVAR
#endif

#if defined (__APPLE__)
#define _LIBIMF_DBL_XDBL    long double
#else
#define _LIBIMF_DBL_XDBL    double
#endif
