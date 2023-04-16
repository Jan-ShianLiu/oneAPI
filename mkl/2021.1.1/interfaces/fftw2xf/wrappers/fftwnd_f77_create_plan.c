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
 * fftwnd_f77_create_plan - FFTW2 Fortran 77 wrapper to Intel(R) Math Kernel Library (Intel(R) MKL).
 *
 ******************************************************************************
 */

#include "fftw.h"
#include "fftw2_mkl.h"
#include "fftw2_f77_mkl.h"

void
fftwnd_f77_create_plan(fftwnd_plan *plan_f77, int *rank, const int *n,
                       fftw_direction *dir, int *flags)
{
    int nn[DFTI_MAX_RANK];
    int i;

    if (plan_f77 == NULL) return;
    *(MKL_INT *)plan_f77 = 0;
    if (rank == NULL || n == NULL || dir == NULL || flags == NULL) return;

    for (i = 0; i < *rank; i++)
    {
        nn[i] = n[*rank - i - 1];
    }

    *plan_f77 = fftwnd_create_plan(*rank, nn, *dir, *flags);
}
