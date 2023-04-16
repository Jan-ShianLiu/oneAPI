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
!       Wrapper library building. Function rfftwnd_mpi.
!       This library allows to use Intel(R) Math Kernel Library (Intel(R) MKL)
!       CDFT routines through MPI FFTW interface.
!******************************************************************************/

#include "mkl.h"
#include "common.h"

#ifdef MKL_SINGLE
	#define Comp     MKL_Complex8
	#define XOMATCOPY mkl_comatcopy
#else
	#define Comp     MKL_Complex16
	#define XOMATCOPY mkl_zomatcopy
#endif

#define CLEANUP() do { if ((work==NULL) && (pwork!=NULL)) mkl_free(pwork); } while(0)

static MKL_LONG local_transpose_fftw2cdft(DFTI_DESCRIPTOR_DM_HANDLE h, void* src, void* dst)	// !BAC..Z -> !BC..ZA
{
	MKL_LONG err;
	MKL_LONG i,b,B,A,CODIM,dim;
	MKL_LONG lengths[MKL_CDFT_MAXRANK];
	Comp alpha;

	err=DftiGetValueDM(h,DFTI_DIMENSION,&dim);
	if (err!=DFTI_NO_ERROR) return err;

	err=DftiGetValueDM(h,CDFT_LOCAL_OUT_NX,&B);
	if (err!=DFTI_NO_ERROR) return err;

	err=DftiGetValueDM(h,DFTI_LENGTHS,lengths);
	if (err!=DFTI_NO_ERROR) return err;

	A=lengths[0];
	CODIM=1;
	for (i=2;i<dim-1;++i) CODIM*=lengths[i];
	CODIM*=(lengths[dim-1]/2+1);

	alpha.real=1.; alpha.imag=0.;

	for (b=0;b<B;++b)
		XOMATCOPY('R', 'T', A, CODIM, alpha, (Comp*)src+b*CODIM*A, CODIM, (Comp*)dst+b*CODIM*A, A);
	
	return DFTI_NO_ERROR;
}

static MKL_LONG local_transpose_cdft2fftw(DFTI_DESCRIPTOR_DM_HANDLE h, void* src, void* dst)	// !BC..ZA -> !BAC..Z
{
	MKL_LONG err;
	MKL_LONG i,b,B,A,CODIM,dim;
	MKL_LONG lengths[MKL_CDFT_MAXRANK];
	Comp alpha;

	err=DftiGetValueDM(h,DFTI_DIMENSION,&dim);
	if (err!=DFTI_NO_ERROR) return err;

	err=DftiGetValueDM(h,CDFT_LOCAL_OUT_NX,&B);
	if (err!=DFTI_NO_ERROR) return err;

	err=DftiGetValueDM(h,DFTI_LENGTHS,lengths);
	if (err!=DFTI_NO_ERROR) return err;

	A=lengths[0];
	CODIM=1;
	for (i=2;i<dim-1;++i) CODIM*=lengths[i];
	CODIM*=(lengths[dim-1]/2+1);

	alpha.real=1.; alpha.imag=0.;

	for (b=0;b<B;++b)
		XOMATCOPY('R', 'T', CODIM, A, alpha, (Comp*)src+b*CODIM*A, A, (Comp*)dst+b*CODIM*A, CODIM);
	
	return DFTI_NO_ERROR;
}

void rfftwnd_mpi(rfftwnd_mpi_plan p, int n, void *local, void *work, fftwnd_mpi_output_order output_order)
{
	MKL_LONG err = DFTI_INVALID_CONFIGURATION;
	MKL_LONG local_size, dim;
	void *pwork = NULL;
	if (p == NULL) goto error;

	err=DftiGetValueDM(p->h,DFTI_DIMENSION,&dim);
	if (err!=DFTI_NO_ERROR) goto error;

	if ((output_order==FFTW_NORMAL_ORDER) || ((output_order==FFTW_TRANSPOSED_ORDER) && (dim==2)) || (dim==1)) {

		if (dim != 1) {
			err=DftiSetValueDM(p->h,DFTI_TRANSPOSE,(output_order==FFTW_NORMAL_ORDER)?DFTI_NONE:DFTI_ALLOW);
			err=DftiCommitDescriptorDM(p->h);
			if (err!=DFTI_NO_ERROR) goto error;
		}

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
	} else if (output_order==FFTW_TRANSPOSED_ORDER) {

		err =DftiSetValueDM(p->h,DFTI_TRANSPOSE,DFTI_ALLOW);
		err|=DftiCommitDescriptorDM(p->h);
		if (err!=DFTI_NO_ERROR) goto error;

		err =DftiSetValueDM(p->h,DFTI_PLACEMENT,DFTI_NOT_INPLACE);
		err|=DftiCommitDescriptorDM(p->h);
		if (err!=DFTI_NO_ERROR) goto error;

		if (work==NULL) {
			err=DftiGetValueDM(p->h,CDFT_LOCAL_SIZE,&local_size);
			if (err!=DFTI_NO_ERROR) goto error;

			pwork=(void*)mkl_malloc(sizeof(Comp)*local_size, 4096);
			if (pwork==NULL) { fprintf(stderr,"CDFT error in rfftwnd_mpi wrapper: not enough memory\n"); return; }
		} else pwork=work;
		switch (p->dir) {
			case FFTW_FORWARD:
				err=DftiComputeForwardDM(p->h,local,pwork);
				if (err!=DFTI_NO_ERROR) goto error;
				err=local_transpose_cdft2fftw(p->h,pwork,local);
				if (err!=DFTI_NO_ERROR) goto error;
				break;
			case FFTW_BACKWARD:
				err=local_transpose_fftw2cdft(p->h,local,pwork);
				if (err!=DFTI_NO_ERROR) goto error;
				err=DftiComputeBackwardDM(p->h,pwork,local);
				if (err!=DFTI_NO_ERROR) goto error;
				break;
			default:
				CLEANUP();
				fprintf(stderr,"CDFT error in rfftwnd_mpi wrapper: unknown direction\n");
				return;
		}

		err =DftiSetValueDM(p->h,DFTI_PLACEMENT,DFTI_INPLACE);
		err|=DftiCommitDescriptorDM(p->h);
		if (err!=DFTI_NO_ERROR) goto error;

		CLEANUP();
	} else goto error2; // !FFTW_NORMAL_ORDER & !FFTW_TRANSPOSED_ORDER

	return;
error:
	CLEANUP();
	fprintf(stderr,"CDFT error "PRINT_LI" in rfftwnd_mpi wrapper\n",err);
	return;
error2:
	fprintf(stderr,"CDFT error "PRINT_LI" in rfftwnd_mpi wrapper: unknown output_order\n",err);
	return;
}

void rfftwnd_f77_mpi(rfftwnd_mpi_plan *p, int *n, void *local, void *work, int *use_work, fftwnd_mpi_output_order *output_order)
{
	if (p == NULL || n == NULL || use_work == NULL || output_order == NULL) return;
	rfftwnd_mpi(*p, *n, local, (*use_work)?work:NULL, *output_order);
}
