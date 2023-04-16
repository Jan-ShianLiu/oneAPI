/*******************************************************************************
* Copyright 2005-2020 Intel Corporation.
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
 * fftw_malloc - FFTW2 wrapper to Intel(R) Math Kernel Library (Intel(R) MKL).
 *
 ******************************************************************************
 */

#include "fftw2_mkl.h"

#ifndef MKL_FFTW_MALLOC_ALIGNMENT
#define MKL_FFTW_MALLOC_ALIGNMENT (64)
#endif

void *(*fftw_malloc_hook) (size_t n);

void *
fftw_malloc(size_t n)
{
    void *p;
    
    if (fftw_malloc_hook)
        return fftw_malloc_hook(n);

    p = MKL_malloc(n, MKL_FFTW_MALLOC_ALIGNMENT);
    if (!p)
        fftw_die("fftw_malloc() failed");
    return p;
}
