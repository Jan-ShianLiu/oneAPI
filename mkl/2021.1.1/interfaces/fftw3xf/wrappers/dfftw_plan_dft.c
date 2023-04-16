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
 * dfftw_plan_dft - FFTW3 Fortran 77 wrapper to Intel(R) Math Kernel Library (Intel(R) MKL).
 *
 ******************************************************************************
 */

#include "fftw3_mkl_f77.h"

void
dfftw_plan_dft(PLAN *p, INTEGER *rank, INTEGER *n, COMPLEX16 *in,
               COMPLEX16 *out, INTEGER *sign, INTEGER *flags)
{
    fftw_iodim64 dims64[MKL_MAXRANK];
    int i;

    if (p == NULL || rank == NULL || n == NULL || sign == NULL || flags == NULL)
        return;

    *(MKL_INT64 *)p = 0;
    if (*rank > MKL_MAXRANK) return;

    /* Reverse dimensions for column-major layout */
    for (i = 0; i < *rank; i++)
    {
        int j = *rank - i - 1;
        dims64[j].n = n[i];
        if (i == 0)
        {
            dims64[j].is = 1;
            dims64[j].os = 1;
        }
        else
        {
            dims64[j].is = dims64[j + 1].is * dims64[j + 1].n;
            dims64[j].os = dims64[j + 1].os * dims64[j + 1].n;
        }
    }

    *(fftw_plan *)p =
        fftw_plan_guru64_dft(*rank, dims64, 0, NULL, in, out, *sign, *flags);
}
