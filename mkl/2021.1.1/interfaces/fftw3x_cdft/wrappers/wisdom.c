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

FFTW_EXTERN void FFTW_MPI_MANGLE(gather_wisdom)(MPI_Comm comm_)
{
  /* not implemented */
}

FFTW_EXTERN void FFTW_MPI_MANGLE(broadcast_wisdom)(MPI_Comm comm_)
{
  /* not implemented */
}
