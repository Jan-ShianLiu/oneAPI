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

#include "fftw3-mpi_mkl.h"
#include "mpi_transpose.h"

FFTW_EXTERN void FFTW_MPI_MANGLE(execute_dft)
     (FFTW_MANGLE(plan) p, complex_t *in, complex_t *out)
{
    if (p != NULL) {
        struct fftw_mkl_plan_s tmp_plan_s = ((fftw_mkl_plan)p)[0];
        tmp_plan_s.io[0] = in;
        tmp_plan_s.io[1] = out;
        tmp_plan_s.execute(&tmp_plan_s);
    }
}

FFTW_EXTERN void FFTW_MPI_MANGLE(execute_dft_r2c)
     (FFTW_MANGLE(plan) p, real_t *in, complex_t *out)
{
    if (p != NULL) {
        struct fftw_mkl_plan_s tmp_plan_s = ((fftw_mkl_plan)p)[0];
        tmp_plan_s.io[0] = in;
        tmp_plan_s.io[1] = out;
        tmp_plan_s.execute(&tmp_plan_s);
    }
}

FFTW_EXTERN void FFTW_MPI_MANGLE(execute_dft_c2r)
     (FFTW_MANGLE(plan) p, complex_t *in, real_t *out)
{
    if (p != NULL) {
        struct fftw_mkl_plan_s tmp_plan_s = ((fftw_mkl_plan)p)[0];
        tmp_plan_s.io[0] = in;
        tmp_plan_s.io[1] = out;
        tmp_plan_s.execute(&tmp_plan_s);
    }
}

FFTW_EXTERN void FFTW_MPI_MANGLE(execute_dft_r2r)
     (FFTW_MANGLE(plan) p, real_t *in, real_t *out)
{
    fftw_mkl_plan plan = (fftw_mkl_plan)p;
    if (is_mpi_transpose_plan(plan)) {
        struct fftw_mkl_plan_s tmp_plan_s = plan[0];
        tmp_plan_s.io[0] = in;
        tmp_plan_s.io[1] = out;
        tmp_plan_s.execute(&tmp_plan_s);
        return;
    }

    return; /* not implemented */
}
