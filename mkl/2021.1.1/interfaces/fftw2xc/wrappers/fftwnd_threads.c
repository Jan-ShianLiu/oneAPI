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
 * fftwnd_threads - FFTW2 wrapper to Intel(R) Math Kernel Library (Intel(R) MKL).
 *
 ******************************************************************************
 */

#include "fftw2_mkl.h"

void
fftwnd_threads(int nthreads, fftwnd_plan _plan, int howmany, fftw_complex *in,
               int istride, int idist, fftw_complex *out, int ostride,
               int odist)
{
    MKL_LONG err = DFTI_NO_ERROR;
    fftw_plan_mkl *plan = (fftw_plan_mkl*)_plan;

    mkl_memory_layout layout = {istride, idist, ostride, odist, howmany, nthreads};
    DFTI_DESCRIPTOR_HANDLE mkl_desc = mkl_memory_layout_get(plan, &layout);
    if (mkl_desc == NULL) goto cannot_commit;

    if (plan->sign == FFTW_FORWARD)
    {
        err = DftiComputeForward(mkl_desc, in, out);
    }
    else
    {
        err = DftiComputeBackward(mkl_desc, in, out);
    }
    mkl_memory_layout_recycle(plan, mkl_desc);
    if (err != DFTI_NO_ERROR) fftw_die("DftiCompute returned error in fftwnd_threads()");
    return;

cannot_commit:
    fftw_die("Cannot commit plan in fftwnd_threads()");
}
