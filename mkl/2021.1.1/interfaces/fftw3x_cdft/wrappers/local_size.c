/*******************************************************************************
* Copyright 2010-2020 Intel Corporation.
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

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_many)
     (int rnk, const ptrdiff_t *n, ptrdiff_t howmany,
      ptrdiff_t block0, MPI_Comm comm,
      ptrdiff_t *local_n0, ptrdiff_t *local_0_start)
{
    return 0; /* not implemented */
}

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size)
     (int rnk, const ptrdiff_t *n, MPI_Comm comm,
      ptrdiff_t *local_n0, ptrdiff_t *local_0_start)
{
    MKL_LONG dims[MKL_MAXRANK] = {0};
    DFTI_DESCRIPTOR_DM_HANDLE desc = NULL;
    MKL_LONG status, value;
    ptrdiff_t ret = 0;
    int i;

    if (n == NULL || local_n0 == NULL || local_0_start == NULL) return 0;

    for (i=0; i<rnk; i++)
        dims[i] = n[i];

    if (rnk == 1)
        status = DftiCreateDescriptorDM(comm, &desc, MKL_PREC, DFTI_COMPLEX,
                (MKL_LONG)rnk, dims[0]);
    else
        status = DftiCreateDescriptorDM(comm, &desc, MKL_PREC, DFTI_COMPLEX,
                (MKL_LONG)rnk, dims);

    if (DFTI_NO_ERROR != status) return ret;
    status = DftiSetValueDM(desc, DFTI_PLACEMENT, DFTI_NOT_INPLACE);
    if (DFTI_NO_ERROR != status) goto cleanup;
    status = DftiCommitDescriptorDM(desc);
    if (DFTI_NO_ERROR != status) goto cleanup;

    status = DftiGetValueDM(desc, CDFT_LOCAL_NX, &value);
    if (DFTI_NO_ERROR != status) goto cleanup;
    *local_n0 = value;

    status = DftiGetValueDM(desc, CDFT_LOCAL_X_START, &value);
    if (DFTI_NO_ERROR != status) goto cleanup;
    *local_0_start = value;

    status = DftiGetValueDM(desc, CDFT_LOCAL_SIZE, &value);
    if (DFTI_NO_ERROR != status) goto cleanup;
    ret = value;

cleanup:
    DftiFreeDescriptorDM(&desc);
    return ret;
}

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_many_1d)(
     ptrdiff_t n0, ptrdiff_t howmany,
     MPI_Comm comm, int sign, unsigned flags,
     ptrdiff_t *local_ni, ptrdiff_t *local_i_start,
     ptrdiff_t *local_no, ptrdiff_t *local_o_start)
{
    return 0; /* not implemented */
}

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_1d)(
     ptrdiff_t n0, MPI_Comm comm, int sign, unsigned flags,
     ptrdiff_t *local_ni, ptrdiff_t *local_i_start,
     ptrdiff_t *local_no, ptrdiff_t *local_o_start)
{
    DFTI_DESCRIPTOR_DM_HANDLE desc;
    MKL_LONG status, value;
    ptrdiff_t ret = 0;

    if (local_ni == NULL || local_i_start == NULL || local_no == NULL || local_o_start == NULL) return 0;

    status = DftiCreateDescriptorDM(comm, &desc, MKL_PREC, DFTI_COMPLEX, (MKL_LONG)1, (MKL_LONG)n0);
    if (DFTI_NO_ERROR != status) return ret;
    status = DftiSetValueDM(desc, DFTI_PLACEMENT, DFTI_NOT_INPLACE);
    if (DFTI_NO_ERROR != status) goto cleanup;
    status = DftiCommitDescriptorDM(desc);
    if (DFTI_NO_ERROR != status) goto cleanup;

    status = DftiGetValueDM(desc, CDFT_LOCAL_NX, &value);
    if (DFTI_NO_ERROR != status) goto cleanup;
    *local_ni = value;

    status = DftiGetValueDM(desc, CDFT_LOCAL_X_START, &value);
    if (DFTI_NO_ERROR != status) goto cleanup;
    *local_i_start = value;

    status = DftiGetValueDM(desc, CDFT_LOCAL_OUT_NX, &value);
    if (DFTI_NO_ERROR != status) goto cleanup;
    *local_no = value;

    status = DftiGetValueDM(desc, CDFT_LOCAL_OUT_X_START, &value);
    if (DFTI_NO_ERROR != status) goto cleanup;
    *local_o_start = value;

    status = DftiGetValueDM(desc, CDFT_LOCAL_SIZE, &value);
    if (DFTI_NO_ERROR != status) goto cleanup;
    ret = value;

cleanup:
    DftiFreeDescriptorDM(&desc);
    return ret;
}

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_2d)(
     ptrdiff_t n0, ptrdiff_t n1, MPI_Comm comm,
     ptrdiff_t *local_n0, ptrdiff_t *local_0_start)
{
    const ptrdiff_t n[2] = {n0, n1};
    return FFTW_MPI_MANGLE(local_size)(2, n, comm, local_n0, local_0_start);
}

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_3d)(
     ptrdiff_t n0, ptrdiff_t n1, ptrdiff_t n2, MPI_Comm comm,
     ptrdiff_t *local_n0, ptrdiff_t *local_0_start)
{
    const ptrdiff_t n[3] = {n0, n1, n2};
    return FFTW_MPI_MANGLE(local_size)(3, n, comm, local_n0, local_0_start);
}
