// File: istrconv.h
// =============================================================================
//
// Copyright (C) 2018 Intel Corporation. All Rights Reserved.
//
// The information and source code contained herein is the exclusive property
// of Intel Corporation and may not be disclosed, examined, or reproduced in
// whole or in part without explicit written authorization from the Company.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//
// =============================================================================
//
// Abstract:
//
// External header file for libistrconv.
//
// =============================================================================

#ifndef _ISTRCONV_H_
#define _ISTRCONV_H_

#include <stddef.h>

#if defined(__cplusplus)
#define _ISTRCONV_EXTERN_C extern "C"
#else
#define _ISTRCONV_EXTERN_C extern
#endif

#define __IML_ISTRCONV_VERSION 4

#ifdef __SSE4_2__
    /* Rename API's to use optimized code for sse4.2 */
    #define  __IML_u_to_str             __IML_u_to_str_sse4
    #define  __IML_i_to_str             __IML_i_to_str_sse4
    #define  __IML_ull_to_str           __IML_ull_to_str_sse4
    #define  __IML_ll_to_str            __IML_ll_to_str_sse4
    #define  __IML_uint_to_string       __IML_uint_to_string_sse4
    #define  __IML_int_to_string        __IML_int_to_string_sse4
    #define  __IML_uint64_to_string     __IML_uint64_to_string_sse4
    #define  __IML_int64_to_string      __IML_int64_to_string_sse4
    #define  __IML_str_to_u             __IML_str_to_u_sse4
    #define  __IML_str_to_i             __IML_str_to_i_sse4
    #define  __IML_str_to_ull           __IML_str_to_ull_sse4
    #define  __IML_str_to_ll            __IML_str_to_ll_sse4
    #define  __IML_string_to_uint       __IML_string_to_uint_sse4
    #define  __IML_string_to_int        __IML_string_to_int_sse4
    #define  __IML_string_to_uint64     __IML_string_to_uint64_sse4
    #define  __IML_string_to_int64      __IML_string_to_int64_sse4
#endif

/******************************************************************************/
/**********************  List of Optimzed Routines ****************************/
/******************************************************************************/

/*
 * Routines to convert floating-point numbers to ASCII strings
 * Similar to snprintf in stdio.h:
 * int snprintf(char * str, size_t n, %.*[g|f|e], int prec, ...);
 */
/* print floating-point number in mixed notation (%.*g) */
_ISTRCONV_EXTERN_C int __IML_float_to_string(char * str,
                                             size_t n,
                                             int prec,
                                             float x);

_ISTRCONV_EXTERN_C int __IML_double_to_string(char * str,
                                              size_t n,
                                              int prec,
                                              double x);

/* non null-terminated variant */
_ISTRCONV_EXTERN_C int __IML_f_to_str(char * str,
                                      size_t n,
                                      int prec,
                                      float x);

_ISTRCONV_EXTERN_C int __IML_d_to_str(char * str,
                                      size_t n,
                                      int prec,
                                      double x);


/* print floating-point number in fixed point notation (%.*f) */
_ISTRCONV_EXTERN_C int __IML_float_to_string_f(char * str,
                                               size_t n,
                                               int prec,
                                               float x);

_ISTRCONV_EXTERN_C int __IML_double_to_string_f(char * str,
                                                size_t n,
                                                int prec,
                                                double x);

/* non null-terminated variant */
_ISTRCONV_EXTERN_C int __IML_f_to_str_f(char * str,
                                        size_t n,
                                        int prec,
                                        float x);

_ISTRCONV_EXTERN_C int __IML_d_to_str_f(char * str,
                                        size_t n,
                                        int prec,
                                        double x);


/* print floating-point number in scientific notation  (%.*e) */
_ISTRCONV_EXTERN_C int __IML_float_to_string_e(char * str,
                                               size_t n,
                                               int prec,
                                               float x);

_ISTRCONV_EXTERN_C int __IML_double_to_string_e(char * str,
                                                size_t n,
                                                int prec,
                                                double x);

/* non null-terminated variant */
_ISTRCONV_EXTERN_C int __IML_f_to_str_e(char * str,
                                        size_t n,
                                        int prec,
                                        float x);

_ISTRCONV_EXTERN_C int __IML_d_to_str_e(char * str,
                                        size_t n,
                                        int prec,
                                        double x);

/*
 * Routines to convert ASCII strings to floating-point numbers
 * Similar to strtof, strtod in stdlib.h:
 * float strtof(const char * nptr, char ** endptr);
 * double strtod(const char * nptr, char ** endptr);
 * long double strtold(const char * nptr, char ** endptr);
 * Only handle strings of decimal digits.
 */
_ISTRCONV_EXTERN_C float __IML_string_to_float(const char * nptr,
                                               char ** endptr);

_ISTRCONV_EXTERN_C double __IML_string_to_double(const char * nptr,
                                                 char ** endptr);

_ISTRCONV_EXTERN_C long double __IML_string_to_long_double(const char * nptr,
                                                           char ** endptr);
/*
 * Routines to convert the initial 'n' decimal digit characters of
 * positive 'significand' multiplied by 10 raised to power of 'exponent' to
 * floating-point number.
 * endptr points to the object that stores the final part of 'significand'
 * string , provided that endptr is not a null pointer.
 */
_ISTRCONV_EXTERN_C float __IML_str_to_f(const char * significand,
                                        size_t n,
                                        int exponent,
                                        char ** endptr);

_ISTRCONV_EXTERN_C double __IML_str_to_d(const char * significand,
                                         size_t n,
                                         int exponent,
                                         char ** endptr);

_ISTRCONV_EXTERN_C long double __IML_str_to_ld(const char * significand,
                                               size_t n,
                                               int exponent,
                                               char ** endptr);
/*
 * Convert integer values to string representations.
 *
 * These routines are designed to behave similarly to snprintf with the
 * following mappings,
 *
 * __IML_int_to_string(str, n, x)    ~ snprintf(str, n, "%d", x)
 * __IML_uint_to_string(str, n, x)   ~ snprintf(str, n, "%u", x)
 * __IML_int64_to_string(str, n, x)  ~ snprintf(str, n, "%lld", x)
 * __IML_uint64_to_string(str, n, x) ~ snprintf(str, n, "%llu", x)
 *
 * Unlike snprintf these implementations do not return a negative value
 * on encoding errors.
 */
/* Integer to decimal string */
_ISTRCONV_EXTERN_C int __IML_int_to_string(char * str,
                                           size_t n,
                                           int x);

_ISTRCONV_EXTERN_C int __IML_uint_to_string(char * str,
                                            size_t n,
                                            unsigned int x);

_ISTRCONV_EXTERN_C int __IML_int64_to_string(char * str,
                                             size_t n,
                                             long long x);

_ISTRCONV_EXTERN_C int __IML_uint64_to_string(char * str,
                                              size_t n,
                                              unsigned long long x);

/* generic versions that do not use SSE4.2 instructions */
_ISTRCONV_EXTERN_C int __IML_int_to_string_generic(char * str,
                                                   size_t n,
                                                   int x);

_ISTRCONV_EXTERN_C int __IML_uint_to_string_generic(char * str,
                                                    size_t n,
                                                    unsigned int x);

_ISTRCONV_EXTERN_C int __IML_int64_to_string_generic(char * str,
                                                     size_t n,
                                                     long long x);

_ISTRCONV_EXTERN_C int __IML_uint64_to_string_generic(char * str,
                                                      size_t n,
                                                      unsigned long long x);

/* Integer to octal string */
_ISTRCONV_EXTERN_C int __IML_int_to_oct_string(char * str,
                                               size_t n,
                                               int x);

_ISTRCONV_EXTERN_C int __IML_uint_to_oct_string(char * str,
                                                size_t n,
                                                unsigned int x);

_ISTRCONV_EXTERN_C int __IML_int64_to_oct_string(char * str,
                                                 size_t n,
                                                 long long x);

_ISTRCONV_EXTERN_C int __IML_uint64_to_oct_string(char * str,
                                                  size_t n,
                                                  unsigned long long x);

/* Integer to hexadecimal string */
_ISTRCONV_EXTERN_C int __IML_int_to_hex_string(char * str,
                                               size_t n,
                                               int x);

_ISTRCONV_EXTERN_C int __IML_uint_to_hex_string(char * str,
                                                size_t n,
                                                unsigned int x);

_ISTRCONV_EXTERN_C int __IML_int64_to_hex_string(char * str,
                                                 size_t n,
                                                 long long x);

_ISTRCONV_EXTERN_C int __IML_uint64_to_hex_string(char * str,
                                                  size_t n,
                                                  unsigned long long x);

/* non null-terminated variant */
_ISTRCONV_EXTERN_C int __IML_i_to_str(char * str,
                                      size_t n,
                                      int x);

_ISTRCONV_EXTERN_C int __IML_u_to_str(char * str,
                                      size_t n,
                                      unsigned int x);

_ISTRCONV_EXTERN_C int __IML_ll_to_str(char * str,
                                       size_t n,
                                       long long x);

_ISTRCONV_EXTERN_C int __IML_ull_to_str(char * str,
                                        size_t n,
                                        unsigned long long x);

_ISTRCONV_EXTERN_C int __IML_i_to_str_generic(char * str,
                                              size_t n,
                                              int x);

_ISTRCONV_EXTERN_C int __IML_u_to_str_generic(char * str,
                                              size_t n,
                                              unsigned int x);

_ISTRCONV_EXTERN_C int __IML_ll_to_str_generic(char * str,
                                               size_t n,
                                               long long x);

_ISTRCONV_EXTERN_C int __IML_ull_to_str_generic(char * str,
                                                size_t n,
                                                unsigned long long x);

_ISTRCONV_EXTERN_C int __IML_i_to_oct_str(char * str,
                                          size_t n,
                                          int x);

_ISTRCONV_EXTERN_C int __IML_u_to_oct_str(char * str,
                                          size_t n,
                                          unsigned int x);

_ISTRCONV_EXTERN_C int __IML_ll_to_oct_str(char * str,
                                           size_t n,
                                           long long x);

_ISTRCONV_EXTERN_C int __IML_ull_to_oct_str(char * str,
                                            size_t n,
                                            unsigned long long x);

_ISTRCONV_EXTERN_C int __IML_i_to_hex_str(char * str,
                                          size_t n,
                                          int x);

_ISTRCONV_EXTERN_C int __IML_u_to_hex_str(char * str,
                                          size_t n,
                                          unsigned int x);

_ISTRCONV_EXTERN_C int __IML_ll_to_hex_str(char * str,
                                           size_t n,
                                           long long x);

_ISTRCONV_EXTERN_C int __IML_ull_to_hex_str(char * str,
                                            size_t n,
                                            unsigned long long x);

/*
 * Convert string representations to integer values.
 *
 * These routines are designed to behave similarly to strto[u]l and strto[u]ll.
 *
 * The second variant of these functions with an additional size_t argument only
 * processes the initial 'n' digits of the string to determine the integer value.
 */

/* Decimal string to integer */
_ISTRCONV_EXTERN_C int __IML_string_to_int(const char * nptr,
                                           char ** endptr);

_ISTRCONV_EXTERN_C unsigned int __IML_string_to_uint(const char * nptr,
                                                     char ** endptr);

_ISTRCONV_EXTERN_C long long __IML_string_to_int64(const char * nptr,
                                                   char ** endptr);

_ISTRCONV_EXTERN_C unsigned long long __IML_string_to_uint64(const char * nptr,
                                                             char ** endptr);

_ISTRCONV_EXTERN_C int __IML_str_to_i(const char * nptr,
                                      size_t n,
                                      char ** endptr);

_ISTRCONV_EXTERN_C unsigned int __IML_str_to_u(const char * nptr,
                                               size_t n,
                                               char ** endptr);

_ISTRCONV_EXTERN_C long long __IML_str_to_ll(const char * nptr,
                                             size_t n,
                                             char ** endptr);

_ISTRCONV_EXTERN_C unsigned long long __IML_str_to_ull(const char * nptr,
                                                       size_t n,
                                                       char ** endptr);

_ISTRCONV_EXTERN_C int __IML_string_to_int_generic(const char * nptr,
                                                   char ** endptr);

_ISTRCONV_EXTERN_C unsigned int __IML_string_to_uint_generic(const char * nptr,
                                                             char ** endptr);

_ISTRCONV_EXTERN_C long long __IML_string_to_int64_generic(const char * nptr,
                                                           char ** endptr);

_ISTRCONV_EXTERN_C unsigned long long __IML_string_to_uint64_generic(const char * nptr,
                                                                     char ** endptr);

_ISTRCONV_EXTERN_C int __IML_str_to_i_generic(const char * nptr,
                                              size_t n,
                                              char ** endptr);

_ISTRCONV_EXTERN_C unsigned int __IML_str_to_u_generic(const char * nptr,
                                                       size_t n,
                                                       char ** endptr);

_ISTRCONV_EXTERN_C long long __IML_str_to_ll_generic(const char * nptr,
                                                     size_t n,
                                                     char ** endptr);

_ISTRCONV_EXTERN_C unsigned long long __IML_str_to_ull_generic(const char * nptr,
                                                               size_t n,
                                                               char ** endptr);

/* Octal string to integer */
_ISTRCONV_EXTERN_C int __IML_oct_string_to_int(const char * nptr,
                                               char ** endptr);

_ISTRCONV_EXTERN_C unsigned int __IML_oct_string_to_uint(const char * nptr,
                                                         char ** endptr);

_ISTRCONV_EXTERN_C long long __IML_oct_string_to_int64(const char * nptr,
                                                       char ** endptr);

_ISTRCONV_EXTERN_C unsigned long long __IML_oct_string_to_uint64(const char * nptr,
                                                                 char ** endptr);

_ISTRCONV_EXTERN_C int __IML_oct_str_to_i(const char * nptr,
                                          size_t n,
                                          char ** endptr);

_ISTRCONV_EXTERN_C unsigned int __IML_oct_str_to_u(const char * nptr,
                                                   size_t n,
                                                   char ** endptr);

_ISTRCONV_EXTERN_C long long __IML_oct_str_to_ll(const char * nptr,
                                                 size_t n,
                                                 char ** endptr);

_ISTRCONV_EXTERN_C unsigned long long __IML_oct_str_to_ull(const char * nptr,
                                                           size_t n,
                                                           char ** endptr);

/* Hexadecimal string to integer */
_ISTRCONV_EXTERN_C int __IML_hex_string_to_int(const char * nptr,
                                               char ** endptr);

_ISTRCONV_EXTERN_C unsigned int __IML_hex_string_to_uint(const char * nptr,
                                                         char ** endptr);

_ISTRCONV_EXTERN_C long long __IML_hex_string_to_int64(const char * nptr,
                                                       char ** endptr);

_ISTRCONV_EXTERN_C unsigned long long __IML_hex_string_to_uint64(const char * nptr,
                                                                 char ** endptr);

_ISTRCONV_EXTERN_C int __IML_hex_str_to_i(const char * nptr,
                                          size_t n,
                                          char ** endptr);

_ISTRCONV_EXTERN_C unsigned int __IML_hex_str_to_u(const char * nptr,
                                                   size_t n,
                                                   char ** endptr);

_ISTRCONV_EXTERN_C long long __IML_hex_str_to_ll(const char * nptr,
                                                 size_t n,
                                                 char ** endptr);

_ISTRCONV_EXTERN_C unsigned long long __IML_hex_str_to_ull(const char * nptr,
                                                           size_t n,
                                                           char ** endptr);


/******************************************************************************/
/**********************  List of Stub Routines ********************************/
/******************************************************************************/

/* print floating-point number in mixed notation */
_ISTRCONV_EXTERN_C int __IML_long_double_to_string(char * str,
                                                   size_t n,
                                                   int prec,
                                                   long double x);

_ISTRCONV_EXTERN_C int __IML_ld_to_str(char * str,
                                       size_t n,
                                       int prec,
                                       long double x);

/* print floating-point number in fixed point notation */
_ISTRCONV_EXTERN_C int __IML_long_double_to_string_f(char * str,
                                                     size_t n,
                                                     int prec,
                                                     long double x);

_ISTRCONV_EXTERN_C int __IML_ld_to_str_f(char * str,
                                         size_t n,
                                         int prec,
                                         long double x);

/* print floating-point number in scientific notation */
_ISTRCONV_EXTERN_C int __IML_long_double_to_string_e(char * str,
                                                     size_t n,
                                                     int prec,
                                                     long double x);

_ISTRCONV_EXTERN_C int __IML_ld_to_str_e(char * str,
                                         size_t n,
                                         int prec,
                                         long double x);

/* Use "double" routines when "long double" is "double" */
#if (__LONG_DOUBLE_SIZE__ == 64) || (defined __LONG_DOUBLE_64__)
    #define  __IML_string_to_long_double    __IML_string_to_double
    #define  __IML_str_to_ld                __IML_str_to_d
    #define  __IML_long_double_to_string    __IML_double_to_string
    #define  __IML_long_double_to_string_f  __IML_double_to_string_f
    #define  __IML_long_double_to_string_e  __IML_double_to_string_e
    #define  __IML_ld_to_str                __IML_d_to_str
    #define  __IML_ld_to_str_f              __IML_d_to_str_f
    #define  __IML_ld_to_str_e              __IML_d_to_str_e
#endif

#endif /*_ISTRCONV_H_*/
