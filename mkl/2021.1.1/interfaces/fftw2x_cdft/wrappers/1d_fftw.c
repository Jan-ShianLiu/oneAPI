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
!       Wrapper library building. Function fftw_mpi.
!       This library allows to use Intel(R) Math Kernel Library (Intel(R) MKL)
!       CDFT routines through MPI FFTW interface.
!******************************************************************************/

#include "common.h"

void fftw_mpi(fftw_mpi_plan p,int n,void *local,void *work) {

	MKL_LONG err = DFTI_INVALID_CONFIGURATION;
	if (p == NULL) goto error;
	if (work==NULL)
		if (p->dir==FFTW_FORWARD) err=DftiComputeForwardDM(p->h,local); else err=DftiComputeBackwardDM(p->h,local);
	else {
		err=DftiSetValueDM(p->h,CDFT_WORKSPACE,work);
		if (err!=DFTI_NO_ERROR) goto error;
		err=DftiCommitDescriptorDM(p->h);
		if (err!=DFTI_NO_ERROR) goto error;
		if (p->dir==FFTW_FORWARD) err=DftiComputeForwardDM(p->h,local); else err=DftiComputeBackwardDM(p->h,local);
		if (err!=DFTI_NO_ERROR) goto error;
		err=DftiSetValueDM(p->h,CDFT_WORKSPACE,NULL);
		if (err!=DFTI_NO_ERROR) goto error;
		err=DftiCommitDescriptorDM(p->h);
	}
	if (err!=DFTI_NO_ERROR) goto error;
	return;
error:
	fprintf(stderr,"CDFT error "PRINT_LI" in fftw_mpi wrapper\n",err);
	return;
}

void fftw_f77_mpi(fftw_mpi_plan *p, int *n, void *local, void *work, int *use_work) {
     if (p == NULL || n == NULL || use_work == NULL) return;
     fftw_mpi(*p, *n, local, (*use_work)?work:NULL);
}
