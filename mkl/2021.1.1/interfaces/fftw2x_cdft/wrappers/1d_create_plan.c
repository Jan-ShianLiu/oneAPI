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
!       Wrapper library building. Function fftw_mpi_create_plan.
!       This library allows to use Intel(R) Math Kernel Library (Intel(R) MKL)
!       CDFT routines through MPI FFTW interface.
!******************************************************************************/
#include "common.h"

#ifdef MKL_SINGLE
    #define PREC DFTI_SINGLE
#else
    #define PREC DFTI_DOUBLE
#endif

fftw_mpi_plan fftw_mpi_create_plan(MPI_Comm comm,MKL_INT n,fftw_direction dir,int flags) {

	MKL_LONG err;
	fftw_mpi_plan p;
	if ((flags!=FFTW_ESTIMATE)&&(flags!=FFTW_MEASURE))
	    if ((!(flags & FFTW_SCRAMBLED_OUTPUT))&&(!(flags & FFTW_SCRAMBLED_INPUT))) goto error2;

	p=(fftw_mpi_plan)malloc(sizeof(fftw_mpi_plan_struct));
	if (NULL == p)
	{
	    goto error3;
	}
	p->dir=dir;
	err=DftiCreateDescriptorDM(comm,&p->h,PREC,DFTI_COMPLEX,1,(MKL_LONG)n);
	if (err!=DFTI_NO_ERROR) goto error;

	if ((flags & FFTW_SCRAMBLED_OUTPUT)||(flags & FFTW_SCRAMBLED_INPUT) )
	{
	    err = DftiSetValueDM(p->h,DFTI_ORDERING, DFTI_ORDERED);
	    if (err!=DFTI_NO_ERROR) goto error;
	}

	err=DftiCommitDescriptorDM(p->h);
	if (err!=DFTI_NO_ERROR) goto error;
	return p;
error:
	fprintf(stderr,"CDFT error "PRINT_LI" in fftw_mpi_create_plan wrapper\n",err);
	free(p);
	return NULL;
error2:
	fprintf(stderr,"CDFT error in fftw_mpi_create_plan wrapper: unknown flags\n");
	return NULL;
error3:
	fprintf(stderr,"Error in fftw_mpi_create_plan wrapper: memory allocation error for fftw_mpi_plan\n");
	return NULL;
}

void fftw_f77_mpi_create_plan(fftw_mpi_plan *plan, MPI_Fint *comm, MKL_INT *n, int *idir, int *flags) {
	if (plan == NULL) return;
	*plan = NULL;
	if (comm == NULL || n == NULL || idir == NULL || flags == NULL) return;
	*plan = fftw_mpi_create_plan(MPI_Comm_f2c(*comm), *n, (*idir)<0 ? FFTW_FORWARD : FFTW_BACKWARD, *flags);
}
