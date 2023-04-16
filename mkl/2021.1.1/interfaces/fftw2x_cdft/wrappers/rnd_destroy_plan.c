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
!   Content:
!       Wrapper library building. Function rfftwnd_mpi_destroy_plan.
!       This library allows to use Intel(R) Math Kernel Library (Intel(R) MKL)
!       CDFT routines through MPI FFTW interface.
!******************************************************************************/

#include "common.h"

void rfftwnd_mpi_destroy_plan(rfftwnd_mpi_plan p)
{
	MKL_LONG err;

	if (p == NULL) return;
	err=DftiFreeDescriptorDM(&p->h);
	if (err!=DFTI_NO_ERROR) {
		fprintf(stderr,"CDFT error "PRINT_LI" in rfftwnd_mpi_destroy_plan wrapper\n",err);
		return;
	}
	free(p);
}

void rfftwnd_f77_mpi_destroy_plan(rfftwnd_mpi_plan* p)
{
	if (p == NULL) return;
	rfftwnd_mpi_destroy_plan(*p);
}
