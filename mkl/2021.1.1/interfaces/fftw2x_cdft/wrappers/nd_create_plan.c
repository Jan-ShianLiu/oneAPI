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
!       Wrapper library building. Functions fftw2d_mpi_create_plan,
!	fftw3d_mpi_create_plan, fftwnd_mpi_create_plan.
!       This library allows to use Intel(R) Math Kernel Library (Intel(R) MKL)
!       CDFT routines through MPI FFTW interface.
!******************************************************************************/
#include "common.h"

#ifdef MKL_SINGLE
	#define PREC DFTI_SINGLE
#else
	#define PREC DFTI_DOUBLE
#endif


fftwnd_mpi_plan fftw2d_mpi_create_plan(MPI_Comm comm,MKL_INT nx, MKL_INT ny,fftw_direction dir, int flags){
	MKL_LONG err;
	fftwnd_mpi_plan p;
	MKL_LONG n[2];
	if ((flags!=FFTW_ESTIMATE)&&(flags!=FFTW_MEASURE))
		if ((!(flags & FFTW_SCRAMBLED_OUTPUT))&&(!(flags & FFTW_SCRAMBLED_INPUT)))  goto error2;

	p=(fftwnd_mpi_plan)malloc(sizeof(fftwnd_mpi_plan_struct));
	if (p == NULL) return NULL;
	p->dir=dir;

	n[0]=nx; n[1]=ny;
	err=DftiCreateDescriptorDM(comm,&p->h,PREC,DFTI_COMPLEX,2,n);
	if (err!=DFTI_NO_ERROR) goto error;

	if (dir==FFTW_BACKWARD) {
		n[0]=ny; n[1]=nx;
		err=DftiCreateDescriptorDM(comm,&p->ht,PREC,DFTI_COMPLEX,2,n);
		if (err!=DFTI_NO_ERROR) p->ht=NULL;
	} else 	p->ht=NULL;

	if ((flags & FFTW_SCRAMBLED_OUTPUT)||(flags & FFTW_SCRAMBLED_INPUT) )
	{
	    err = DftiSetValueDM(p->h,DFTI_ORDERING, DFTI_ORDERED);
	    if (err!=DFTI_NO_ERROR) goto error;
	}
	err=DftiCommitDescriptorDM(p->h);
    if (err!=DFTI_NO_ERROR) goto error;

    if (p->ht!=NULL) {
        err=DftiCommitDescriptorDM(p->ht);
        if (err!=DFTI_NO_ERROR) {
            DftiFreeDescriptorDM(&p->ht);
            p->ht=NULL;
        }
    }

	return p;

error:
	fprintf(stderr,"CDFT error "PRINT_LI" in fftw2d_mpi_create_plan wrapper\n",err);
	free(p);
	return NULL;
error2:
	fprintf(stderr,"CDFT error  in fftw2d_mpi_create_plan wrapper: unknown flags\n");
	return NULL;
}

void fftw2d_f77_mpi_create_plan(fftwnd_mpi_plan *plan, MPI_Fint *comm, MKL_INT *nx, MKL_INT *ny, fftw_direction *dir, int *flags)
{
	if (plan == NULL) return;
	*plan = NULL;
	if (comm == NULL || nx == NULL || ny == NULL || dir == NULL || flags == NULL) return;
	*plan = fftw2d_mpi_create_plan(MPI_Comm_f2c(*comm), *ny, *nx, *dir, *flags);
}

fftwnd_mpi_plan fftw3d_mpi_create_plan(MPI_Comm comm,MKL_INT nx, MKL_INT ny,MKL_INT nz,fftw_direction dir, int flags){
	MKL_LONG err;
	fftwnd_mpi_plan p;
	MKL_LONG n[3];
	if ((flags!=FFTW_ESTIMATE)&&(flags!=FFTW_MEASURE))
		if ((!(flags & FFTW_SCRAMBLED_OUTPUT))&&(!(flags & FFTW_SCRAMBLED_INPUT)))  goto error2;

	p=(fftwnd_mpi_plan)malloc(sizeof(fftwnd_mpi_plan_struct));
	if (p == NULL) return NULL;
	p->dir=dir;

	n[0]=nx; n[1]=ny; n[2]=nz;
	err=DftiCreateDescriptorDM(comm,&p->h,PREC,DFTI_COMPLEX,3,n);
	if (err!=DFTI_NO_ERROR) goto error;

	if (dir==FFTW_BACKWARD) {
		n[0]=ny; n[1]=nx; n[2]=nz;
		err=DftiCreateDescriptorDM(comm,&p->ht,PREC,DFTI_COMPLEX,3,n);
		if (err!=DFTI_NO_ERROR) p->ht=NULL;
	} else 	p->ht=NULL;

	if ((flags & FFTW_SCRAMBLED_OUTPUT)||(flags & FFTW_SCRAMBLED_INPUT) )
	{
	    err = DftiSetValueDM(p->h,DFTI_ORDERING, DFTI_ORDERED);
	    if (err!=DFTI_NO_ERROR) goto error;
	}

	err=DftiCommitDescriptorDM(p->h);
	if (err!=DFTI_NO_ERROR) goto error;

    if (p->ht!=NULL) {
        err=DftiCommitDescriptorDM(p->ht);
        if (err!=DFTI_NO_ERROR) {
            DftiFreeDescriptorDM(&p->ht);
            p->ht=NULL;
        }
    }

	return p;

error:
	fprintf(stderr,"CDFT error "PRINT_LI" in fftw3d_mpi_create_plan wrapper\n",err);
	free(p);
	return NULL;
error2:
	fprintf(stderr,"CDFT error  in fftw3d_mpi_create_plan wrapper: unknown flags\n");
	return NULL;
}

void fftw3d_f77_mpi_create_plan(fftwnd_mpi_plan *plan, MPI_Fint *comm, MKL_INT *nx, MKL_INT *ny, MKL_INT *nz, fftw_direction *dir, int *flags)
{
	if (plan == NULL) return;
	*plan = NULL;
	if (comm == NULL || nx == NULL || ny == NULL || nz == NULL || dir == NULL || flags == NULL) return;
	*plan = fftw3d_mpi_create_plan(MPI_Comm_f2c(*comm), *nz, *ny, *nx, *dir, *flags);
}

fftwnd_mpi_plan fftwnd_mpi_create_plan(MPI_Comm comm,int dim, const MKL_INT *nn,fftw_direction dir,int flags){
	MKL_LONG err = DFTI_INVALID_CONFIGURATION;
	fftwnd_mpi_plan p = NULL;
	MKL_LONG n[MKL_CDFT_MAXRANK];
	int i;
	if (nn == NULL) goto error;
	if ((flags!=FFTW_ESTIMATE)&&(flags!=FFTW_MEASURE))
		if ((!(flags & FFTW_SCRAMBLED_OUTPUT))&&(!(flags & FFTW_SCRAMBLED_INPUT)))  goto error2;

	p=(fftwnd_mpi_plan)malloc(sizeof(fftwnd_mpi_plan_struct));
	if (p == NULL) return NULL;
	p->dir=dir;

	for( i=0; i<dim; i++) n[i]=nn[i];

	err=DftiCreateDescriptorDM(comm,&p->h,PREC,DFTI_COMPLEX,dim,n);
	if (err!=DFTI_NO_ERROR) goto error;

	if (dir==FFTW_BACKWARD) {
		n[0]=nn[1]; n[1]=nn[0];
		for(i=2; i<dim; i++) n[i]=nn[i];  // redundant copy
		err=DftiCreateDescriptorDM(comm,&p->ht,PREC,DFTI_COMPLEX,dim,n);
		if (err!=DFTI_NO_ERROR) p->ht=NULL;
	} else p->ht=NULL;

	if ((flags & FFTW_SCRAMBLED_OUTPUT)||(flags & FFTW_SCRAMBLED_INPUT) )
	{
	    err = DftiSetValueDM(p->h,DFTI_ORDERING, DFTI_ORDERED);
	    if (err!=DFTI_NO_ERROR) goto error;
	}

	err=DftiCommitDescriptorDM(p->h);
	if (err!=DFTI_NO_ERROR) goto error;

    if (p->ht!=NULL) {
        err=DftiCommitDescriptorDM(p->ht);
        if (err!=DFTI_NO_ERROR) {
            DftiFreeDescriptorDM(&p->ht);
            p->ht=NULL;
        }
    }

	return p;

error:
	fprintf(stderr,"CDFT error "PRINT_LI" in fftwnd_mpi_create_plan wrapper\n",err);
	free(p);
	return NULL;

error2:
	fprintf(stderr,"CDFT error in fftwnd_mpi_create_plan wrapper: unknown flags\n");
	return NULL;
}

void fftwnd_f77_mpi_create_plan(fftwnd_mpi_plan *plan, MPI_Fint *comm, int *dim, MKL_INT *nn, fftw_direction *dir, int *flags)
{
	if (plan == NULL) return;
	*plan = NULL;
	if (comm == NULL || dim == NULL || nn == NULL || dir == NULL || flags == NULL) return;
	REVERSE_INT_ARRAY(nn,*dim);
	*plan = fftwnd_mpi_create_plan(MPI_Comm_f2c(*comm), *dim, nn, *dir, *flags);
	REVERSE_INT_ARRAY(nn,*dim);
}
