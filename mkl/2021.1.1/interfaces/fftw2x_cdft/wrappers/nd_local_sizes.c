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
!       Wrapper library building. Function fftwnd_mpi_local_sizes.
!       This library allows to use Intel(R) Math Kernel Library (Intel(R) MKL)
!       CDFT routines through MPI FFTW interface.
!******************************************************************************/

#include "common.h"

void fftwnd_mpi_local_sizes(fftwnd_mpi_plan p,MKL_INT *n,MKL_INT *start,MKL_INT *n_out,MKL_INT *start_out, MKL_INT *size)
{
	MKL_LONG err = DFTI_INVALID_CONFIGURATION,v;
	if (p == NULL || n == NULL || start == NULL || n_out == NULL || start_out == NULL || size == NULL) goto error;

	err=DftiSetValueDM(p->h,DFTI_TRANSPOSE,DFTI_ALLOW);
	if (err!=DFTI_NO_ERROR) goto error;
	err=DftiCommitDescriptorDM(p->h);
	if (err!=DFTI_NO_ERROR) goto error;

	err=DftiGetValueDM(p->h,CDFT_LOCAL_NX,&v);
	if (err!=DFTI_NO_ERROR) goto error;
	*n=v;
	err=DftiGetValueDM(p->h,CDFT_LOCAL_X_START,&v);
	if (err!=DFTI_NO_ERROR) goto error;
	*start=v;
	err=DftiGetValueDM(p->h,CDFT_LOCAL_OUT_NX,&v);
	if (err!=DFTI_NO_ERROR) goto error;
	*n_out=v;
	err=DftiGetValueDM(p->h,CDFT_LOCAL_OUT_X_START,&v);
	if (err!=DFTI_NO_ERROR) goto error;
	*start_out=v;
	err=DftiGetValueDM(p->h,CDFT_LOCAL_SIZE,&v);
	if (err!=DFTI_NO_ERROR) goto error;
	*size=v;
	return;
error:
	fprintf(stderr,"CDFT error "PRINT_LI" in fftwnd_mpi_local_sizes wrapper\n",err);
	return;
}

void fftwnd_f77_mpi_local_sizes(fftwnd_mpi_plan *p,MKL_INT *n,MKL_INT *start,MKL_INT *n_out,MKL_INT *start_out, MKL_INT *size) {
	if (p == NULL) return;
	fftwnd_mpi_local_sizes(*p, n, start, n_out, start_out, size);
}
