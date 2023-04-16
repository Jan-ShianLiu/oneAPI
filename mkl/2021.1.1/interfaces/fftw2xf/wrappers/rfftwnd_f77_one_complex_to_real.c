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
 * rfftwnd_f77_one_complex_to_real - FFTW2 Fortran 77 wrapper to
 * Intel(R) Math Kernel Library (Intel(R) MKL).
 *
 ******************************************************************************
 */

#include "rfftw.h"
#include "fftw2_mkl.h"
#include "fftw2_f77_mkl.h"

void
rfftwnd_f77_one_complex_to_real(rfftwnd_plan *plan_f77, fftw_complex *in,
                                fftw_real *out)
{
    if (plan_f77 == NULL) return;
    rfftwnd_complex_to_real( *plan_f77, 1, in, 1, 0, out, 1, 0 );
}
