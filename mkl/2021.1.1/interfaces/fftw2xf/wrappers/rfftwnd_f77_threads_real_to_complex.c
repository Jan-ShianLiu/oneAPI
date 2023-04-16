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
 * rfftwnd_f77_threads_real_to_complex - FFTW2 Fortran 77 wrapper to
 * Intel(R) Math Kernel Library (Intel(R) MKL).
 *
 ******************************************************************************
 */

#include "rfftw.h"
#include "fftw2_mkl.h"
#include "fftw2_f77_mkl.h"

void
rfftwnd_f77_threads_real_to_complex(int *nthreads, rfftwnd_plan *plan,
                                    int *howmany, fftw_real *in, int *istride,
                                    int *idist, fftw_complex *out,
                                    int *ostride, int *odist)
{
    if (plan == NULL || howmany == NULL || nthreads == NULL) return;
    if (istride == NULL || idist == NULL) return;
    if (ostride == NULL || odist == NULL) return;
    rfftwnd_threads_real_to_complex(*nthreads, *plan, *howmany, in, *istride,
                                    *idist, out, *ostride, *odist);
}
