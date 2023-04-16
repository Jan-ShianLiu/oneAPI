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
 * rfftw_executor_simple - FFTW2 wrapper to Intel(R) Math Kernel Library (Intel(R) MKL).
 *
 ******************************************************************************
 */

#include "fftw2_mkl.h"

/* FFTW2 documentation doesn't cover this function. */
void
rfftw_executor_simple(int n, fftw_real *in, fftw_real *out,
        fftw_plan_node *plan, int istride, int ostride,
        fftw_recurse_kind recurse_kind)
{
    UNUSED(n);
    UNUSED(in);
    UNUSED(out);
    UNUSED(plan);
    UNUSED(istride);
    UNUSED(ostride);
    UNUSED(recurse_kind);
    fftw_die("rfftw_executor_simple() unimplemented");
}
