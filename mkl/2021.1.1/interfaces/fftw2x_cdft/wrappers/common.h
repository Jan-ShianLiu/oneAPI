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
!       Wrapper library building. Header file.
!       This library allows to use Intel(R) Math Kernel Library (Intel(R) MKL)
!       CDFT routines through MPI FFTW interface.
!******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include "mkl_cdft.h"

#define MKL_CDFT_MAXRANK (8)

typedef enum {
	FFTW_FORWARD	=-1,
	FFTW_BACKWARD	= 1
} fftw_direction;

#define FFTW_REAL_TO_COMPLEX FFTW_FORWARD
#define FFTW_COMPLEX_TO_REAL FFTW_BACKWARD

#if !defined(MKL_ILP64)
#define PRINT_LI "%ld"
#else
#define PRINT_LI "%lli"
#endif

typedef enum {
    FFTW_NORMAL_ORDER,
    FFTW_TRANSPOSED_ORDER
} fftwnd_mpi_output_order;

typedef struct {
	DFTI_DESCRIPTOR_DM_HANDLE h;
	fftw_direction dir;
} fftw_mpi_plan_struct;

typedef struct {
	DFTI_DESCRIPTOR_DM_HANDLE h;
	DFTI_DESCRIPTOR_DM_HANDLE ht;
	fftw_direction dir;
} fftwnd_mpi_plan_struct;

#define  FFTW_MEASURE  (1)
#define  FFTW_ESTIMATE (0)

#define  FFTW_SCRAMBLED_OUTPUT (16384)
#define  FFTW_SCRAMBLED_INPUT (8192)

#define fftw_mpi_plan fftw_mpi_plan_struct*
#define fftwnd_mpi_plan fftwnd_mpi_plan_struct*
#define rfftwnd_mpi_plan fftwnd_mpi_plan_struct*

#define REVERSE_INT_ARRAY(p,n) do {  \
int __i;                         \
for (__i=0; __i<(n)/2; __i++) { \
int __tmp = (p)[__i];        \
(p)[__i] = (p)[n-__i-1];         \
(p)[n-__i-1] = __tmp; }          \
} while(0)

#if defined(_FNAME_UPPERCASE)

#define fftw_f77_mpi_create_plan     FFTW_F77_MPI_CREATE_PLAN
#define fftw_f77_mpi_destroy_plan    FFTW_F77_MPI_DESTROY_PLAN
#define fftw_f77_mpi                 FFTW_F77_MPI
#define fftw_f77_mpi_local_sizes     FFTW_F77_MPI_LOCAL_SIZES
#define fftw2d_f77_mpi_create_plan   FFTW2D_F77_MPI_CREATE_PLAN
#define fftw3d_f77_mpi_create_plan   FFTW3D_F77_MPI_CREATE_PLAN
#define fftwnd_f77_mpi_create_plan   FFTWND_F77_MPI_CREATE_PLAN
#define fftwnd_f77_mpi_destroy_plan  FFTWND_F77_MPI_DESTROY_PLAN
#define fftwnd_f77_mpi               FFTWND_F77_MPI
#define fftwnd_f77_mpi_local_sizes   FFTWND_F77_MPI_LOCAL_SIZES
#define rfftw2d_f77_mpi_create_plan  RFFTW2D_F77_MPI_CREATE_PLAN
#define rfftw3d_f77_mpi_create_plan  RFFTW3D_F77_MPI_CREATE_PLAN
#define rfftwnd_f77_mpi_create_plan  RFFTWND_F77_MPI_CREATE_PLAN
#define rfftwnd_f77_mpi_destroy_plan RFFTWND_F77_MPI_DESTROY_PLAN
#define rfftwnd_f77_mpi              RFFTWND_F77_MPI
#define rfftwnd_f77_mpi_local_sizes  RFFTWND_F77_MPI_LOCAL_SIZES

#else

#if defined(_FNAME_SECOND_UNDERSCORE)
#define N(n) n##__
#elif defined(_FNAME_NOUNDERSCORE)
#define N(n) n
#else
#define N(n) n##_
#endif

#define fftw_f77_mpi_create_plan     N(fftw_f77_mpi_create_plan)
#define fftw_f77_mpi_destroy_plan    N(fftw_f77_mpi_destroy_plan)
#define fftw_f77_mpi                 N(fftw_f77_mpi)
#define fftw_f77_mpi_local_sizes     N(fftw_f77_mpi_local_sizes)
#define fftw2d_f77_mpi_create_plan   N(fftw2d_f77_mpi_create_plan)
#define fftw3d_f77_mpi_create_plan   N(fftw3d_f77_mpi_create_plan)
#define fftwnd_f77_mpi_create_plan   N(fftwnd_f77_mpi_create_plan)
#define fftwnd_f77_mpi_destroy_plan  N(fftwnd_f77_mpi_destroy_plan)
#define fftwnd_f77_mpi               N(fftwnd_f77_mpi)
#define fftwnd_f77_mpi_local_sizes   N(fftwnd_f77_mpi_local_sizes)
#define rfftw2d_f77_mpi_create_plan  N(rfftw2d_f77_mpi_create_plan)
#define rfftw3d_f77_mpi_create_plan  N(rfftw3d_f77_mpi_create_plan)
#define rfftwnd_f77_mpi_create_plan  N(rfftwnd_f77_mpi_create_plan)
#define rfftwnd_f77_mpi_destroy_plan N(rfftwnd_f77_mpi_destroy_plan)
#define rfftwnd_f77_mpi              N(rfftwnd_f77_mpi)
#define rfftwnd_f77_mpi_local_sizes  N(rfftwnd_f77_mpi_local_sizes)

#endif // _FNAME_UPPERCASE
