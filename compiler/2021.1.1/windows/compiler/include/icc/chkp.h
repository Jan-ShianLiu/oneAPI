/* Copyright (C) 2010-2013 Intel Corporation. All rights reserved.
 *
 *        INTEL CORPORATION PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license
 * agreement or nondisclosure agreement with Intel Corp.
 * and may not be copied or disclosed except in accordance
 * with the terms of that agreement.
 */

/* chkp.h */
/* Header file for MPX (checked pointers). */

#ifndef _INCLUDED_CHKP
#define _INCLUDED_CHKP

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32)
#ifdef __COMPILING_CHKP_C
#define _DLLSYM __declspec(dllexport)
#else
#define _DLLSYM __declspec(dllimport)
#endif
#define __ICL_INTRINCC __cdecl
#else
#define _DLLSYM
#define _DLLSYM
#define __ICL_INTRINCC
#endif

/* This function removes the bounds information from a pointer, giving
 * it bounds that allow access to all of memory.  This can be used when a
 * pointer is created by code that is not Point Lookout Soft enabled, to
 * ensure that the pointer does not inherit bounds from some previous pointer
 * that existed at the same memory location.  The return value is a pointer
 * that has the new bounds.
 */
void * __ICL_INTRINCC __chkp_kill_bounds(void *);

/* This function creates bounds for a pointer, replacing any bounds that might
 * have been associated with it previously.  The new bounds are:
 *    lower bound: ptr
 *    upper bound: ptr + size - 1
 */
void * __ICL_INTRINCC __chkp_make_bounds(void *, /* ptr */
                                        size_t);


/*
 * This function returns the lower bound associated with the pointer stored
 * in the passed address.
 */
_DLLSYM void * __ICL_INTRINCC __chkp_lower_bound(void **);


/*
 * This function returns the upper bound associated with the pointer stored
 * in the passed address.
 */
_DLLSYM void * __ICL_INTRINCC __chkp_upper_bound(void **);


/*
 * This enum defines the action to take when reporting a bounds check error.
 */
typedef enum
{
    __CHKP_REPORT_NONE,           /* Do nothing */
    __CHKP_REPORT_BPT,            /* Execute a breakpoint interrupt */
    __CHKP_REPORT_LOG,            /* Log the error and continue */
    __CHKP_REPORT_TERM,           /* Log the error and exit the program */
    __CHKP_REPORT_CALLBACK,       /* Call a user defined function */
    __CHKP_REPORT_TRACE_BPT,      /* Print a traceback and emit BPT */
    __CHKP_REPORT_TRACE_LOG,      /* Print a traceback and continue */
    __CHKP_REPORT_TRACE_TERM,     /* Print a traceback and exit the program */
    __CHKP_REPORT_TRACE_CALLBACK, /* Print a traceback and call user function */
    __CHKP_REPORT_OOB_STATS,      /* Emit statics on OOB errors (currently */
                                  /* this is just a count of the OOB errors) */
    __CHKP_REPORT_USE_ENV_VAR     /* Use value in environment variable  */
                                  /* INTEL_CHKP_REPORT_MODE to set the report */
                                  /* mode.  If the environment variable is not */
                                  /* set, the default report mode is used */
} __chkp_report_option_t;

/*
 * This function type is the type of a user defined function that is called
 * when the __CHKP_REPORT_CALLBACK option is chosen.
 */
typedef void (*__chkp_callback_t)(char *, /* program address */
                                  char *, /* lower bound */
                                  char *, /* upper bound */
                                  char *, /* referenced address */
                                  size_t  /* size of reference */);

/*
 * This function sets the action to take when a bounds check error occurs.
 *     The option is from the above enum.
 *
 *     The callback function argument is only used when the option
 *     indicates it, and is the address of the function to call.
 */
_DLLSYM void __ICL_INTRINCC __chkp_report_control(
    __chkp_report_option_t,
    __chkp_callback_t /* callback function */);

/* This function invalidates pointers to a freed block of memory.
 *      ptr is the pointer to the begining of the block
 *      size is the size in bytes of the block.
 *
 * All pointers that point to this block are given invalid bounds
 * so that use of them an an indirection will cause a bounds violation.
 */
_DLLSYM void __ICL_INTRINCC __chkp_invalidate_dangling(void *, size_t);

/*
 * This function can turn on and off checking and dangling pointer
 * handling at runtime.
 */
_DLLSYM void __ICL_INTRINCC __chkp_control(int, int);

/*
 * Create an error file for pointer checker errors.
 */
_DLLSYM int __ICL_INTRINCC __chkp_create_error_file(char *);

#ifdef __cplusplus
}
#endif

#endif  /* _INCLUDED_CHKP */
