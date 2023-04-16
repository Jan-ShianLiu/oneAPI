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
 * dfftw_forget_wisdom - FFTW3 Fortran 77 wrapper to Intel(R) Math Kernel Library (Intel(R) MKL).
 *
 ******************************************************************************
 */

#include "fftw3_mkl_f77.h"

void
dfftw_forget_wisdom(void)
{
    fftw_forget_wisdom();
}
