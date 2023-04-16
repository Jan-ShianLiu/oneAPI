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
 * rfftw_create_plan_specific - FFTW2 wrapper to Intel(R) Math Kernel Library (Intel(R) MKL).
 *
 ******************************************************************************
 */

#include "fftw2_mkl.h"

/* This function is not implemented because
 * Intel MKL DFTI doesn't support half-complex data layout.
 */
rfftw_plan
rfftw_create_plan_specific(int n, fftw_direction dir, int flags, fftw_real *in,
                           int istride, fftw_real *out, int ostride)
{
    UNUSED(n);
    UNUSED(dir);
    UNUSED(flags);
    UNUSED(in);
    UNUSED(istride);
    UNUSED(out);
    UNUSED(ostride);
    return NULL;
}
