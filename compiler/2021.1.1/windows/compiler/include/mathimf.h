/* file: mathimf.h */

/*
** Copyright  (C) 1985-2018 Intel Corporation. All rights reserved.
**
** The information and source code contained herein is the exclusive property
** of Intel Corporation and may not be disclosed, examined, or reproduced in
** whole or in part without explicit written authorization from the Company.
**
*/

/*
 * mathimf.h - Intel math library definitions
 *
 * Usage notes:
 * 1. This header file is for use with only  the Intel compilers!
 * 2. The 'long double' prototypes require the -Qlong_double (icl/ecl)
 *    or the -long_double (icc/ecc) compiler option.
 * 3. The 'complex' prototypes are for use only with "C" (not "C++").
 * 4. Under icl/ecl, the 'complex' prototypes require the -Qstd=c99 compiler option.
 * 5. The C99 _Complex and GNU __complex__ types are fully compatible.
 *
 */


#ifndef __MATHIMF_H_INCLUDED
#define __MATHIMF_H_INCLUDED

#include <math.h>

#include <math_common_define.h>

/* GNU functions */
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI exp10( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI exp10f( float __x );

_LIBIMF_EXTERN_C void     _LIBIMF_PUBAPI sincos( double __x, double *__psin, double *__pcos );
_LIBIMF_EXTERN_C void     _LIBIMF_PUBAPI sincosf( float __x, float *__psin, float *__pcos );

/* Manipulation functions */
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI significand( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI significandf( float __x );

/* Intel specific functions */
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI invsqrt( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI invsqrtf( float __x );

_LIBIMF_EXTERN_C void     _LIBIMF_PUBAPI sinhcosh( double __x, double *__psinh, double *__pcosh );
_LIBIMF_EXTERN_C void     _LIBIMF_PUBAPI sinhcoshf( float __x, float *__psinh, float *__pcosh );

_LIBIMF_EXTERN_C void     _LIBIMF_PUBAPI sincospi( double __x, double *__psin, double *__pcos );
_LIBIMF_EXTERN_C void     _LIBIMF_PUBAPI sincospif( float __x, float *__psin, float *__pcos );

/* Degree and pi- argument trigonometric functions */
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI cosd( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI cosdf( float __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI cospi( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI cospif( float __x );

_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI sind( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI sindf( float __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI sinpi( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI sinpif( float __x );

_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI tand( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI tandf( float __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI tanpi( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI tanpif( float __x );

_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI acosd( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI acosdf( float __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI acospi( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI acospif( float __x );

_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI asind( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI asindf( float __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI asinpi( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI asinpif( float __x );

_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI atand( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI atandf( float __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI atanpi( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI atanpif( float __x );

_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI atand2( double __y, double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI atand2f( float __y, float __x );

_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI atan2d( double __y, double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI atan2df( float __y, float __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI atan2pi( double __y, double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI atan2pif( float __y, float __x );

_LIBIMF_EXTERN_C void     _LIBIMF_PUBAPI sincosd( double __x, double *__psin, double *__pcos );
_LIBIMF_EXTERN_C void     _LIBIMF_PUBAPI sincosdf( float __x, float *__psin, float *__pcos );

/* cotangeant functions */
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI cot( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI cotf( float __x );

_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI cotd( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI cotdf( float __x );

/* cumulative normal distribution functions */
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI cdfnorm( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI cdfnormf( float __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI cdfnorminv( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI cdfnorminvf( float __x );

/* Error functions */
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI erfinv( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI erfinvf( float __x );

_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI erfcinv( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI erfcinvf( float __x );

_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI erfcx( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI erfcxf( float __x );

/* Bessel functions */
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI j0f( float __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI j1f( float __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI jnf( int __n, float __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI y0f( float __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI y1f( float __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI ynf( int __n, float __x );

_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI j0( double __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI j1( double __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI jn( int __n, double __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI y0( double __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI y1( double __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI yn( int __n, double __x );

#if !defined(__linux__) && !defined(__APPLE__) && !defined(__NetBSD__) && !defined(__QNX__) && !defined(__VXWORKS__)
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI _hypot( double __x, double __y );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI _j0( double __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI _j1( double __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI _jn( int __n, double __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI _y0( double __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI _y1( double __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI _yn( int __n, double __x );
#endif

/* maxmag/minmag functions */
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI maxmag( double __x, double __y );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI maxmagf( float __x, float __y );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI minmag( double __x, double __y );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI minmagf( float __x, float __y );

/* Other power functions */
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI powr( double __x, double __y );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI powrf( float __x, float __y );

_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI pow2o3( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI pow2o3f( float __x );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI pow3o2( double __x );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI pow3o2f( float __x );

/* Other - special functions */
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI annuity( double __x, double __y );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI annuityf( float __x, float __y );
_LIBIMF_EXTERN_C double   _LIBIMF_PUBAPI compound( double __x, double __y );
_LIBIMF_EXTERN_C float    _LIBIMF_PUBAPI compoundf( float __x, float __y );


/* handling ABI incompatibility when long doubles are 64 bits */
#if (__IMFLONGDOUBLE == 64) && !defined(__ANDROID__)
_LIBIMF_FORCEINLINE long double  _LIBIMF_PUBAPI_INL exp10l( long double __x )
{ return (long double) exp10((double) __x);}
_LIBIMF_FORCEINLINE void         _LIBIMF_PUBAPI_INL sincosl( long double __x, long double *__psin, long double *__pcos )
{ return sincos((double) __x, (double *) __psin, (double *)__pcos);}
_LIBIMF_FORCEINLINE void         _LIBIMF_PUBAPI_INL sinhcoshl( long double __x, long double *__psinh, long double *__pcosh )
{ return sinhcosh((double) __x, (double *) __psinh, (double *)__pcosh);}
_LIBIMF_FORCEINLINE long double  _LIBIMF_PUBAPI_INL invsqrtl( long double __x )
{ return (long double) invsqrt((double) __x);}
_LIBIMF_FORCEINLINE long double  _LIBIMF_PUBAPI_INL cosdl( long double __x )
{ return (long double) cosd((double) __x);}
_LIBIMF_FORCEINLINE long double  _LIBIMF_PUBAPI_INL sindl( long double __x )
{ return (long double) sind((double) __x);}
_LIBIMF_FORCEINLINE long double  _LIBIMF_PUBAPI_INL tandl( long double __x )
{ return (long double) tand((double) __x);}
_LIBIMF_FORCEINLINE long double  _LIBIMF_PUBAPI_INL acosdl( long double __x )
{ return (long double) acosd((double) __x);}
_LIBIMF_FORCEINLINE long double  _LIBIMF_PUBAPI_INL asindl( long double __x )
{ return (long double) asind((double) __x);}
_LIBIMF_FORCEINLINE long double  _LIBIMF_PUBAPI_INL atandl( long double __x )
{ return (long double) atand((double) __x);}
_LIBIMF_FORCEINLINE long double  _LIBIMF_PUBAPI_INL atand2l( long double __y, long double __x )
{ return (long double) atand2((double) __y, (double) __x);}
_LIBIMF_FORCEINLINE long double  _LIBIMF_PUBAPI_INL atan2dl( long double __y, long double __x )
{ return (long double) atan2d((double) __y, (double) __x);}
_LIBIMF_FORCEINLINE void     _LIBIMF_PUBAPI_INL sincosdl( long double __x, long double *__psin, long double *__pcos )
{ return sincosd((double) __x, (double *) __psin, (double *)__pcos);}

_LIBIMF_FORCEINLINE long double  _LIBIMF_PUBAPI_INL cotl( long double __x )
{return (long double) cot((double) __x);}
_LIBIMF_FORCEINLINE long double  _LIBIMF_PUBAPI_INL cotdl( long double __x )
{return (long double) cotd((double) __x);}
_LIBIMF_FORCEINLINE long double  _LIBIMF_PUBAPI_INL erfinvl( long double __x )
{return (long double) erfinv((double) __x);}
_LIBIMF_FORCEINLINE long double  _LIBIMF_PUBAPI_INL significandl( long double __x )
{return (long double) significand((double) __x);}
_LIBIMF_FORCEINLINE long double  _LIBIMF_PUBAPI_INL annuityl( long double __x, long double __y )
{return (long double) annuity((double) __x, (double) __y);}
_LIBIMF_FORCEINLINE long double  _LIBIMF_PUBAPI_INL compoundl( long double __x, long double __y )
{return (long double) compound((double) __x, (double) __y);}
_LIBIMF_FORCEINLINE long double   _LIBIMF_PUBAPI_INL j0l( long double __x )
{return (long double) j0((double) __x);}
_LIBIMF_FORCEINLINE long double   _LIBIMF_PUBAPI_INL j1l( long double __x )
{return (long double) j1((double) __x);}
_LIBIMF_FORCEINLINE long double   _LIBIMF_PUBAPI_INL jnl( int __n, long double __x )
{return (long double) jn(__n, (double) __x);}
_LIBIMF_FORCEINLINE long double   _LIBIMF_PUBAPI_INL y0l( long double __x )
{return (long double) y0((double) __x);}
_LIBIMF_FORCEINLINE long double   _LIBIMF_PUBAPI_INL y1l( long double __x )
{return (long double) y1((double) __x);}
_LIBIMF_FORCEINLINE long double   _LIBIMF_PUBAPI_INL ynl( int __n, long double __x )
{return (long double) yn(__n, (double) __x);}

#else
_LIBIMF_EXTERN_C long double  _LIBIMF_PUBAPI exp10l( long double __x );
_LIBIMF_EXTERN_C void         _LIBIMF_PUBAPI sincosl( long double __x, long double *__psin, long double *__pcos );
_LIBIMF_EXTERN_C void         _LIBIMF_PUBAPI sinhcoshl( long double __x, long double *__psinh, long double *__pcosh );
_LIBIMF_EXTERN_C long double  _LIBIMF_PUBAPI invsqrtl( long double __x );
_LIBIMF_EXTERN_C long double  _LIBIMF_PUBAPI cosdl( long double __x );
_LIBIMF_EXTERN_C long double  _LIBIMF_PUBAPI sindl( long double __x );
_LIBIMF_EXTERN_C long double  _LIBIMF_PUBAPI tandl( long double __x );
_LIBIMF_EXTERN_C long double  _LIBIMF_PUBAPI acosdl( long double __x );
_LIBIMF_EXTERN_C long double  _LIBIMF_PUBAPI asindl( long double __x );
_LIBIMF_EXTERN_C long double  _LIBIMF_PUBAPI atandl( long double __x );
_LIBIMF_EXTERN_C long double  _LIBIMF_PUBAPI atand2l( long double __y, long double __x );
_LIBIMF_EXTERN_C long double  _LIBIMF_PUBAPI atan2dl( long double __y, long double __x );
_LIBIMF_EXTERN_C void     _LIBIMF_PUBAPI sincosdl( long double __x, long double *__psin, long double *__pcos );

_LIBIMF_EXTERN_C long double  _LIBIMF_PUBAPI cotl( long double __x );
_LIBIMF_EXTERN_C long double  _LIBIMF_PUBAPI cotdl( long double __x );
_LIBIMF_EXTERN_C long double  _LIBIMF_PUBAPI erfinvl( long double __x );
_LIBIMF_EXTERN_C long double  _LIBIMF_PUBAPI significandl( long double __x );
_LIBIMF_EXTERN_C long double  _LIBIMF_PUBAPI annuityl( long double __x, long double __y );
_LIBIMF_EXTERN_C long double  _LIBIMF_PUBAPI compoundl( long double __x, long double __y );

_LIBIMF_EXTERN_C long double   _LIBIMF_PUBAPI j0l( long double __x );
_LIBIMF_EXTERN_C long double   _LIBIMF_PUBAPI j1l( long double __x );
_LIBIMF_EXTERN_C long double   _LIBIMF_PUBAPI jnl( int __n, long double __x );
_LIBIMF_EXTERN_C long double   _LIBIMF_PUBAPI y0l( long double __x );
_LIBIMF_EXTERN_C long double   _LIBIMF_PUBAPI y1l( long double __x );
_LIBIMF_EXTERN_C long double   _LIBIMF_PUBAPI ynl( int __n, long double __x );
#endif


#include <math_common_undefine.h>

/*-- Complex functions --*/

#if ((defined(__unix__) || defined(__APPLE__) || defined(__QNX__) || defined(__VXWORKS__)) && (!defined(__cplusplus))) || ((defined(_WIN32) || defined(_WIN64)) && defined (__STDC_VERSION__) &&  (__STDC_VERSION__ >= 199901L))  /* No _Complex or GNU __complex__ types available for C++ */
#include <complex.h>
#endif  /*!__cplusplus*/

/*-- Decimal functions --*/
#if defined(__INTEL_CLANG_COMPILER)
/* Decimal FP is not supported with clang-compatible Intel C++ Compiler */
#else
#include <dfp754.h>
#endif

#if defined(__TOGGLED_PURE_INTEL_C99_HEADERS__)
#undef __TOGGLED_PURE_INTEL_C99_HEADERS__
#undef __PURE_INTEL_C99_HEADERS__
#endif

#endif  /* __MATHIMF_H_INCLUDED */