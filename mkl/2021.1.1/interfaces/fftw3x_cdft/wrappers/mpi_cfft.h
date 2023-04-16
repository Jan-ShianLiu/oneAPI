/*******************************************************************************
* Copyright 2015-2020 Intel Corporation.
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
 * MPI FFTW3 wrappers to Intel(R) Math Kernel Library (Intel(R) MKL).
 *
 ******************************************************************************
 */

#ifndef _MPI_CFFT_H_
#define _MPI_CFFT_H_

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "mpi.h"
#include "fftw3.h"
#include "fftw3-mpi.h"
#include "fftw3-mpi_mkl.h"
#include "mkl_trans.h"

#define SWAP(t,x,y) do { t z = (x); (x) = (y); (y) = z; } while(0)

#ifdef MKL_SINGLE
#define MKL_Complex MKL_Complex8
#define XOMATCOPY   mkl_comatcopy
#else
#define MKL_Complex MKL_Complex16
#define XOMATCOPY   mkl_zomatcopy
#endif

typedef enum {
    REAL, COMPLEX
} type_t;

typedef struct {
    DFTI_DESCRIPTOR_DM_HANDLE h;
    int sign;

    int do_trans;
    size_t batch;
    size_t n, codim;

    complex_t *buf;
} cfft_t;

#endif
