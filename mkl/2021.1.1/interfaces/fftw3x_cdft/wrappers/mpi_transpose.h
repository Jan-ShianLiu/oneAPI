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

#ifndef _MPI_TRANSPOSE_H_
#define _MPI_TRANSPOSE_H_

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "mpi.h"
#include "fftw3.h"
#include "fftw3-mpi.h"
#include "fftw3-mpi_mkl.h"

#define DIVUP(x,y) (((x) + (y)  -  1) / (y))
#define MIN(x,y)   (((x) < (y)) ? (x) : (y))
#define MAX(x,y)   (((x) > (y)) ? (x) : (y))

enum {
    IN_PLACE       = 0x1,
    TRANSPOSED_IN  = 0x2,
    TRANSPOSED_OUT = 0x4,
};

typedef enum { ROW_MAJOR, COL_MAJOR } dir_t;

typedef struct {
    int mark;
    MPI_Comm comm;
    int *scounts, *sdispls;
    int *rcounts, *rdispls;
    int *mpi_buf;

    int       p[2][2];
    ptrdiff_t ln[2][2];
    ptrdiff_t local_n0, local_n1;
    ptrdiff_t local_size;
    ptrdiff_t howmany;

    int  tr_in, tr_out;
    real_t *allocated_buf;
} ft_t;

#define MPI_TRANSPOSE_MARK 0xBE350A1F

/**
 * @brief check whether the plan is transposition plan or not
 *
 * @param plan[in] FFTW3 Intel MKL plan structure
 */
static inline int is_mpi_transpose_plan(fftw_mkl_plan plan) {
    if (plan) {
        ft_t *ft = (ft_t*)plan->mpi_plan;
        if (ft && ft->mark == MPI_TRANSPOSE_MARK) return 1;
    }
    return 0;
}

#endif
