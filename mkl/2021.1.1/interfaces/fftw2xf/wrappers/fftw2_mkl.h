/*******************************************************************************
* Copyright 2006-2020 Intel Corporation.
*
* This software and the related documents are Intel copyrighted  materials,  and
* your use of  them is  governed by the  express license  under which  they were
* provided to you (License).  Unless the License provides otherwise, you may not
* use, modify, copy, publish, distribute,  disclose or transmit this software or
* the related documents without Intel's prior written permission.
*
* This software and the related documents  are provided as  is,  with no express
* or implied  warranties,  other  than those  that are  expressly stated  in the
* License.
*******************************************************************************/

/*
 *
 * Definitions for FFTW2 wrappers to Intel(R) Math Kernel Library (Intel(R) MKL).
 *
 ******************************************************************************
 */

#ifndef FFTW2_MKL_H
#define FFTW2_MKL_H

#include <stdlib.h>
#include <stdio.h>
#include "mkl_dfti.h"
#include "mkl_service.h"
#include "fftw_threads.h"
#include "rfftw_threads.h"
#if defined __APPLE__

#include <AvailabilityMacros.h>

#if ( MAC_OS_X_VERSION_MAX_ALLOWED >= 101200 && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_ATOMICS__) )
#   define DEPRECATED_OSAtomic
#   include <stdatomic.h>
#else
#   include <libkern/OSAtomic.h>
#endif

#endif

#define DFTI_MAX_RANK 7

#ifdef MKL_DOUBLE
    #define TYPE_PRECISION double
    #define MKL_PRECISION DFTI_DOUBLE
#else
    #define TYPE_PRECISION float
    #define MKL_PRECISION DFTI_SINGLE
    #define FFTW_ENABLE_FLOAT 1
#endif

typedef struct
{
    MKL_LONG istride, idist;
    MKL_LONG ostride, odist;
    MKL_LONG howmany;
    int nthreads;
} mkl_memory_layout;

/// Maximum number of memory layouts that can be cached for same plan
#define MKL_MEMORY_LAYOUT_CACHE_MAX (16)

typedef struct fftw_plan_mkl
{
    DFTI_DESCRIPTOR_HANDLE mkl_desc;
    int sign;
    int inplace;
    int readonly;
    int rank;
    MKL_LONG istride, idist;
    MKL_LONG ostride, odist;
    MKL_LONG *n;
    MKL_LONG howmany;
    int nthreads;
    struct {
        DFTI_DESCRIPTOR_HANDLE mkl_desc;
        mkl_memory_layout layout;
    } cache[MKL_MEMORY_LAYOUT_CACHE_MAX];
    volatile unsigned int cache_lock;
} fftw_plan_mkl;

#ifndef UNUSED
#define UNUSED(p) (void)p
#endif

/// Get DFTI descriptor handle for memory layout
/// If return value is NULL, creation of handle failed
/// To recycle handle returned by mkl_memory_layout_get,
/// call mkl_memory_layout_recycle
DFTI_DESCRIPTOR_HANDLE mkl_memory_layout_get(fftw_plan_mkl *P, const mkl_memory_layout *L);

/// Recycle DFTI descriptor handle returned by mkl_memory_layout_get
void mkl_memory_layout_recycle(fftw_plan_mkl *P, DFTI_DESCRIPTOR_HANDLE H);

/// Initialize list of memory layouts
void mkl_memory_layout_init(fftw_plan_mkl *P);

/// Free list of memory layouts
void mkl_memory_layout_free(fftw_plan_mkl *P);


#endif /* FFTW2_MKL_H */
