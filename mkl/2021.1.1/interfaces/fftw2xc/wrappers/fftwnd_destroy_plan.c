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
 * fftwnd_destroy_plan - FFTW2 wrapper to Intel(R) Math Kernel Library (Intel(R) MKL).
 *
 ******************************************************************************
 */

#include "fftw2_mkl.h"

void
fftwnd_destroy_plan(fftwnd_plan _plan)
{
    fftw_plan_mkl *plan = (fftw_plan_mkl *)_plan;

    if (plan == NULL)
        return;

    DftiFreeDescriptor(&(plan->mkl_desc));
    mkl_memory_layout_free(plan);
    fftw_free(plan->n);
    fftw_free(plan);
}
