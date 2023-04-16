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

/*
 * wrappers for local_size.c
 */

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_many_transposed_f03)
     (int rnk, const ptrdiff_t *n, ptrdiff_t howmany,
      ptrdiff_t block0, ptrdiff_t block1, MPI_Fint fcomm,
      ptrdiff_t *local_n0, ptrdiff_t *local_0_start,
      ptrdiff_t *local_n1, ptrdiff_t *local_1_start)
{
    return FFTW_MPI_MANGLE(local_size_many_transposed)(rnk, n, howmany,
            block0, block1,
            MPI_Comm_f2c(fcomm),
            local_n0, local_0_start, local_n1, local_1_start);
}


FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_many_f03)
     (int rnk, const ptrdiff_t *n, ptrdiff_t howmany,
      ptrdiff_t block0, MPI_Fint fcomm,
      ptrdiff_t *local_n0, ptrdiff_t *local_0_start)
{
    return FFTW_MPI_MANGLE(local_size_many)(rnk, n, howmany,
            block0,
            MPI_Comm_f2c(fcomm),
            local_n0, local_0_start);
}

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_transposed_f03)
     (int rnk, const ptrdiff_t *n, MPI_Fint fcomm,
      ptrdiff_t *local_n0, ptrdiff_t *local_0_start,
      ptrdiff_t *local_n1, ptrdiff_t *local_1_start)
{
    return FFTW_MPI_MANGLE(local_size_transposed)(rnk, n,
            MPI_Comm_f2c(fcomm),
            local_n0, local_0_start, local_n1, local_1_start);
}

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_f03)
     (int rnk, const ptrdiff_t *n, MPI_Fint fcomm,
      ptrdiff_t *local_n0, ptrdiff_t *local_0_start)
{
    return FFTW_MPI_MANGLE(local_size)(rnk, n,
            MPI_Comm_f2c(fcomm),
            local_n0, local_0_start);
}

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_many_1d_f03)(
     ptrdiff_t n0, ptrdiff_t howmany,
     MPI_Fint fcomm, int sign, unsigned flags,
     ptrdiff_t *local_ni, ptrdiff_t *local_i_start,
     ptrdiff_t *local_no, ptrdiff_t *local_o_start)
{
    return FFTW_MPI_MANGLE(local_size_many_1d)(n0, howmany,
            MPI_Comm_f2c(fcomm),
            sign, flags,
            local_ni, local_i_start, local_no, local_o_start);
}

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_1d_f03)(
     ptrdiff_t n0, MPI_Fint fcomm, int sign, unsigned flags,
     ptrdiff_t *local_ni, ptrdiff_t *local_i_start,
     ptrdiff_t *local_no, ptrdiff_t *local_o_start)
{
    return FFTW_MPI_MANGLE(local_size_1d)(n0,
            MPI_Comm_f2c(fcomm),
            sign, flags,
            local_ni, local_i_start, local_no, local_o_start);
}

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_2d_f03)(
     ptrdiff_t n0, ptrdiff_t n1, MPI_Fint fcomm,
     ptrdiff_t *local_n0, ptrdiff_t *local_0_start)
{
    return FFTW_MPI_MANGLE(local_size_2d)(n0, n1,
            MPI_Comm_f2c(fcomm),
            local_n0, local_0_start);
}

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_2d_transposed_f03)(
     ptrdiff_t n0, ptrdiff_t n1, MPI_Fint fcomm,
     ptrdiff_t *local_n0, ptrdiff_t *local_0_start,
     ptrdiff_t *local_n1, ptrdiff_t *local_1_start)
{
    return FFTW_MPI_MANGLE(local_size_2d_transposed)(n0, n1,
            MPI_Comm_f2c(fcomm),
            local_n0, local_0_start, local_n1, local_1_start);
}

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_3d_f03)(
     ptrdiff_t n0, ptrdiff_t n1, ptrdiff_t n2, MPI_Fint fcomm,
     ptrdiff_t *local_n0, ptrdiff_t *local_0_start)
{
    return FFTW_MPI_MANGLE(local_size_3d)(n0, n1, n2,
            MPI_Comm_f2c(fcomm),
            local_n0, local_0_start);
}

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_3d_transposed_f03)(
     ptrdiff_t n0, ptrdiff_t n1, ptrdiff_t n2, MPI_Fint fcomm,
     ptrdiff_t *local_n0, ptrdiff_t *local_0_start,
     ptrdiff_t *local_n1, ptrdiff_t *local_1_start)
{
    return FFTW_MPI_MANGLE(local_size_3d_transposed)(n0, n1, n2,
            MPI_Comm_f2c(fcomm),
            local_n0, local_0_start, local_n1, local_1_start);
}

/*
 * wrappers for plan.c
 */

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_many_transpose_f03)
     (ptrdiff_t n0, ptrdiff_t n1,
      ptrdiff_t howmany, ptrdiff_t block0, ptrdiff_t block1,
      real_t *in, real_t *out, MPI_Fint fcomm, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_many_transpose)(n0, n1, howmany,
            block0, block1, in, out, MPI_Comm_f2c(fcomm), flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_transpose_f03)
     (ptrdiff_t n0, ptrdiff_t n1,
      real_t *in, real_t *out, MPI_Fint fcomm, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_transpose)(n0, n1, in, out,
            MPI_Comm_f2c(fcomm), flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_many_dft_f03)
     (int rnk, const ptrdiff_t *n, ptrdiff_t howmany,
      ptrdiff_t block, ptrdiff_t tblock, complex_t *in, complex_t *out,
      MPI_Fint fcomm, int sign, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_many_dft)(rnk, n, howmany, block, tblock,
            in, out, MPI_Comm_f2c(fcomm), sign, flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_f03)
     (int rnk, const ptrdiff_t *n, complex_t *in, complex_t *out,
      MPI_Fint fcomm, int sign, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_dft)(rnk, n,
            in, out, MPI_Comm_f2c(fcomm), sign, flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_1d_f03)
     (ptrdiff_t n0, complex_t *in, complex_t *out,
      MPI_Fint fcomm, int sign, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_dft_1d)(n0,
            in, out, MPI_Comm_f2c(fcomm), sign, flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_2d_f03)
     (ptrdiff_t n0, ptrdiff_t n1, complex_t *in, complex_t *out,
      MPI_Fint fcomm, int sign, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_dft_2d)(n0, n1,
            in, out, MPI_Comm_f2c(fcomm), sign, flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_3d_f03)
     (ptrdiff_t n0, ptrdiff_t n1, ptrdiff_t n2, complex_t *in, complex_t *out,
      MPI_Fint fcomm, int sign, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_dft_3d)(n0, n1, n2,
            in, out, MPI_Comm_f2c(fcomm), sign, flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_many_r2r_f03)
     (int rnk, const ptrdiff_t *n, ptrdiff_t howmany,
      ptrdiff_t iblock, ptrdiff_t oblock, real_t *in, real_t *out,
      MPI_Fint fcomm, const FFTW_MANGLE(r2r_kind) *kind, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_many_r2r)(rnk, n, howmany, iblock, oblock,
            in, out, MPI_Comm_f2c(fcomm), kind, flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_r2r_f03)
     (int rnk, const ptrdiff_t *n, real_t *in, real_t *out,
      MPI_Fint fcomm, const FFTW_MANGLE(r2r_kind) *kind, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_r2r)(rnk, n,
            in, out, MPI_Comm_f2c(fcomm), kind, flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_r2r_2d_f03)
     (ptrdiff_t n0, ptrdiff_t n1, real_t *in, real_t *out, MPI_Fint fcomm,
      FFTW_MANGLE(r2r_kind) kind0, FFTW_MANGLE(r2r_kind) kind1, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_r2r_2d)(n0, n1,
            in, out, MPI_Comm_f2c(fcomm), kind0, kind1, flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_r2r_3d_f03)
     (ptrdiff_t n0, ptrdiff_t n1, ptrdiff_t n2,
      real_t *in, real_t *out, MPI_Fint fcomm, FFTW_MANGLE(r2r_kind) kind0,
      FFTW_MANGLE(r2r_kind) kind1, FFTW_MANGLE(r2r_kind) kind2, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_r2r_3d)(n0, n1, n2,
            in, out, MPI_Comm_f2c(fcomm), kind0, kind1, kind2, flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_many_dft_r2c_f03)
     (int rnk, const ptrdiff_t *n, ptrdiff_t howmany,
      ptrdiff_t iblock, ptrdiff_t oblock, real_t *in, complex_t *out,
      MPI_Fint fcomm, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_many_dft_r2c)(rnk, n, howmany, iblock, oblock,
            in, out, MPI_Comm_f2c(fcomm), flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_r2c_f03)
     (int rnk, const ptrdiff_t *n, real_t *in, complex_t *out,
      MPI_Fint fcomm, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_dft_r2c)(rnk, n,
            in, out, MPI_Comm_f2c(fcomm), flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_r2c_2d_f03)
     (ptrdiff_t n0, ptrdiff_t n1, real_t *in, complex_t *out,
      MPI_Fint fcomm, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_dft_r2c_2d)(n0, n1,
            in, out, MPI_Comm_f2c(fcomm), flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_r2c_3d_f03)
     (ptrdiff_t n0, ptrdiff_t n1, ptrdiff_t n2, real_t *in, complex_t *out,
      MPI_Fint fcomm, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_dft_r2c_3d)(n0, n1, n2,
            in, out, MPI_Comm_f2c(fcomm), flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_many_dft_c2r_f03)
     (int rnk, const ptrdiff_t *n, ptrdiff_t howmany,
      ptrdiff_t iblock, ptrdiff_t oblock, complex_t *in, real_t *out,
      MPI_Fint fcomm, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_many_dft_c2r)(rnk, n, howmany, iblock, oblock,
            in, out, MPI_Comm_f2c(fcomm), flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_c2r_f03)
     (int rnk, const ptrdiff_t *n, complex_t *in, real_t *out,
      MPI_Fint fcomm, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_dft_c2r)(rnk, n,
            in, out, MPI_Comm_f2c(fcomm), flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_c2r_2d_f03)
     (ptrdiff_t n0, ptrdiff_t n1, complex_t *in, real_t *out,
      MPI_Fint fcomm, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_dft_c2r_2d)(n0, n1,
            in, out, MPI_Comm_f2c(fcomm), flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_c2r_3d_f03)
     (ptrdiff_t n0, ptrdiff_t n1, ptrdiff_t n2, complex_t *in, real_t *out,
      MPI_Fint fcomm, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_dft_c2r_3d)(n0, n1, n2,
            in, out, MPI_Comm_f2c(fcomm), flags);
}

/*
 * wrappers for wisdom.c
 */

FFTW_EXTERN void FFTW_MPI_MANGLE(gather_wisdom_f03)(MPI_Fint fcomm_)
{
    FFTW_MPI_MANGLE(gather_wisdom)(MPI_Comm_f2c(fcomm_));
}

FFTW_EXTERN void FFTW_MPI_MANGLE(broadcast_wisdom_f03)(MPI_Fint fcomm_)
{
    FFTW_MPI_MANGLE(broadcast_wisdom)(MPI_Comm_f2c(fcomm_));
}
