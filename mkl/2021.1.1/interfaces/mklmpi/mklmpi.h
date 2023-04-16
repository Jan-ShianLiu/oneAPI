#if 0
/*******************************************************************************
* Copyright 2015-2020 Intel Corporation.
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
#endif

#if 0
/*
*   Content:
*       Wrapper library building. Header file.
*       This library allows to use custom MPI implementation MKLMPI interface.
******************************************************************************/
#endif

#ifndef MKLMPI_H_INCLUDED
#define MKLMPI_H_INCLUDED

#if !defined(MKLMPI_OMIT_MPI_VIEW)
#undef COMMENT /*typenames*/
#define MPI_Aint                MKLMPI_Aint
#define MPI_Comm                MKLMPI_Comm
#define MPI_Datatype            MKLMPI_Datatype
#define MPI_Fint                MKLMPI_Fint
#define MPI_Group               MKLMPI_Group
#define MPI_Op                  MKLMPI_Op
#define MPI_Request             MKLMPI_Request
#define MPI_Status              MKLMPI_Status
#define MPI_User_function       MKLMPI_User_function
#undef COMMENT /*constants*/
#define MPI_ANY_SOURCE          MKLMPI_ANY_SOURCE
#define MPI_BYTE                MKLMPI_BYTE
#define MPI_COMM_NULL           MKLMPI_COMM_NULL
#define MPI_COMM_WORLD          MKLMPI_COMM_WORLD
#define MPI_COMPLEX             MKLMPI_COMPLEX
#define MPI_DOUBLE              MKLMPI_DOUBLE
#define MPI_DOUBLE_COMPLEX      MKLMPI_DOUBLE_COMPLEX
#define MPI_DOUBLE_INT          MKLMPI_DOUBLE_INT
#define MPI_DOUBLE_PRECISION    MKLMPI_DOUBLE_PRECISION
#define MPI_ERR_INTERN          MKLMPI_ERR_INTERN
#define MPI_ERR_NO_MEM          MKLMPI_ERR_NO_MEM
#define MPI_ERR_OTHER           MKLMPI_ERR_OTHER
#define MPI_ERR_UNKNOWN         MKLMPI_ERR_UNKNOWN
#define MPI_FLOAT               MKLMPI_FLOAT
#define MPI_INT                 MKLMPI_INT
#define MPI_INTEGER             MKLMPI_INTEGER
#define MPI_LONG_LONG_INT       MKLMPI_LONG_LONG_INT
#define MPI_MAX                 MKLMPI_MAX
#define MPI_MAXLOC              MKLMPI_MAXLOC
#define MPI_MIN                 MKLMPI_MIN
#define MPI_PACKED              MKLMPI_PACKED
#define MPI_REAL                MKLMPI_REAL
#define MPI_REQUEST_NULL        MKLMPI_REQUEST_NULL
#define MPI_SIMILAR             MKLMPI_SIMILAR
#define MPI_STATUS_IGNORE       MKLMPI_STATUS_IGNORE
#define MPI_STATUSES_IGNORE     MKLMPI_STATUSES_IGNORE
#define MPI_SUCCESS             MKLMPI_SUCCESS
#define MPI_SUM                 MKLMPI_SUM
#define MPI_TAG_UB              MKLMPI_TAG_UB
#define MPI_UNDEFINED           MKLMPI_UNDEFINED
#define MPI_UNEQUAL             MKLMPI_UNEQUAL
#define MPI_UNSIGNED_SHORT      MKLMPI_UNSIGNED_SHORT
#undef COMMENT /*functions*/
#define MPI_Abort               MKLMPI_Abort
#define MPI_Allgather           MKLMPI_Allgather
#define MPI_Allgatherv          MKLMPI_Allgatherv
#define MPI_Allreduce           MKLMPI_Allreduce
#define MPI_Alltoall            MKLMPI_Alltoall
#define MPI_Alltoallv           MKLMPI_Alltoallv
#define MPI_Barrier             MKLMPI_Barrier
#define MPI_Bcast               MKLMPI_Bcast
#define MPI_Cart_create         MKLMPI_Cart_create
#define MPI_Cart_sub            MKLMPI_Cart_sub
#define MPI_Comm_c2f            MKLMPI_Comm_c2f
#define MPI_Comm_compare        MKLMPI_Comm_compare
#define MPI_Comm_create         MKLMPI_Comm_create
#define MPI_Comm_dup            MKLMPI_Comm_dup
#define MPI_Comm_f2c            MKLMPI_Comm_f2c
#define MPI_Comm_free           MKLMPI_Comm_free
#define MPI_Comm_get_attr       MKLMPI_Comm_get_attr
#define MPI_Comm_group          MKLMPI_Comm_group
#define MPI_Comm_rank           MKLMPI_Comm_rank
#define MPI_Comm_size           MKLMPI_Comm_size
#define MPI_Comm_split          MKLMPI_Comm_split
#define MPI_Error_class         MKLMPI_Error_class
#define MPI_Finalize            MKLMPI_Finalize
#define MPI_Gatherv             MKLMPI_Gatherv
#define MPI_Get_address         MKLMPI_Get_address
#define MPI_Group_free          MKLMPI_Group_free
#define MPI_Group_incl          MKLMPI_Group_incl
#define MPI_Init                MKLMPI_Init
#define MPI_Initialized         MKLMPI_Initialized
#define MPI_Iprobe              MKLMPI_Iprobe
#define MPI_Irecv               MKLMPI_Irecv
#define MPI_Isend               MKLMPI_Isend
#define MPI_Op_c2f              MKLMPI_Op_c2f
#define MPI_Op_create           MKLMPI_Op_create
#define MPI_Op_f2c              MKLMPI_Op_f2c
#define MPI_Op_free             MKLMPI_Op_free
#define MPI_Pack                MKLMPI_Pack
#define MPI_Pack_size           MKLMPI_Pack_size
#define MPI_Recv                MKLMPI_Recv
#define MPI_Reduce              MKLMPI_Reduce
#define MPI_Rsend               MKLMPI_Rsend
#define MPI_Scatterv            MKLMPI_Scatterv
#define MPI_Send                MKLMPI_Send
#define MPI_Sendrecv            MKLMPI_Sendrecv
#define MPI_Test                MKLMPI_Test
#define MPI_Testall             MKLMPI_Testall
#define MPI_Type_c2f            MKLMPI_Type_c2f
#define MPI_Type_create_struct  MKLMPI_Type_create_struct
#define MPI_Type_commit         MKLMPI_Type_commit
#define MPI_Type_contiguous     MKLMPI_Type_contiguous
#define MPI_Type_f2c            MKLMPI_Type_f2c
#define MPI_Type_free           MKLMPI_Type_free
#define MPI_Type_indexed        MKLMPI_Type_indexed
#define MPI_Type_vector         MKLMPI_Type_vector
#define MPI_Type_create_resized MKLMPI_Type_create_resized
#define MPI_Unpack              MKLMPI_Unpack
#define MPI_Wait                MKLMPI_Wait
#define MPI_Waitall             MKLMPI_Waitall
#define MPI_Waitany             MKLMPI_Waitany
#define MPI_Wtime               MKLMPI_Wtime
#endif /*MKLMPI_OMIT_MPI_VIEW*/

#define XMPI_BASE                 100000000

#undef COMMENT /*MKLMPI_SUCCESS must be 0*/
#define MKLMPI_SUCCESS            0
#define MKLMPI_ANY_SOURCE         (XMPI_BASE + 1)
#define MKLMPI_BYTE               (XMPI_BASE + 2)
#define MKLMPI_COMM_NULL          (XMPI_BASE + 3)
#define MKLMPI_COMM_WORLD         (XMPI_BASE + 4)
#define MKLMPI_COMPLEX            (XMPI_BASE + 5)
#define MKLMPI_DOUBLE             (XMPI_BASE + 6)
#define MKLMPI_DOUBLE_INT         (XMPI_BASE + 7)
#define MKLMPI_DOUBLE_COMPLEX     (XMPI_BASE + 8)
#define MKLMPI_DOUBLE_PRECISION   (XMPI_BASE + 9)
#define MKLMPI_ERR_INTERN         (XMPI_BASE + 10)
#define MKLMPI_ERR_NO_MEM         (XMPI_BASE + 28)
#define MKLMPI_ERR_OTHER          (XMPI_BASE + 11)
#define MKLMPI_ERR_UNKNOWN        (XMPI_BASE + 12)
#define MKLMPI_FLOAT              (XMPI_BASE + 13)
#define MKLMPI_INT                (XMPI_BASE + 14)
#define MKLMPI_INTEGER            (XMPI_BASE + 15)
#define MKLMPI_LONG_LONG_INT      (XMPI_BASE + 16)
#define MKLMPI_MAX                (XMPI_BASE + 17)
#define MKLMPI_MAXLOC             (XMPI_BASE + 18)
#define MKLMPI_MIN                (XMPI_BASE + 19)
#define MKLMPI_PACKED             (XMPI_BASE + 20)
#define MKLMPI_REAL               (XMPI_BASE + 21)
#define MKLMPI_REQUEST_NULL       (XMPI_BASE + 22)
#define MKLMPI_SIMILAR            (XMPI_BASE + 23)
#define MKLMPI_SUM                (XMPI_BASE + 24)
#define MKLMPI_TAG_UB             (XMPI_BASE + 25)
#define MKLMPI_UNDEFINED          (XMPI_BASE + 26)
#define MKLMPI_UNEQUAL            (XMPI_BASE + 27)
#define MKLMPI_UNSIGNED_SHORT     (XMPI_BASE + 28)

#if !defined(MKLMPI_OMIT_C_DECLARATIONS)

#include <stddef.h>

#define MKLMPI1DEPRECATED(msg) /*TODO*/

#if defined(__cplusplus)
extern "C" {
#endif

typedef ptrdiff_t MKLMPI_Aint;
typedef ptrdiff_t MKLMPI_Comm;
typedef ptrdiff_t MKLMPI_Datatype;
typedef ptrdiff_t MKLMPI_Fint;
typedef ptrdiff_t MKLMPI_Group;
typedef ptrdiff_t MKLMPI_Op;
typedef ptrdiff_t MKLMPI_Request;
typedef struct MKLMPI__Status_s MKLMPI_Status;

/** Caveat: user function is called with MKLMPI_Datatype being MPI_Datatype */
typedef void (MKLMPI_User_function)(void*,void*,int*,MKLMPI_Datatype*);

/** To accomodate different layouts of MPI_Status structure,
 * Intel(R) Math Kernel Library (Intel(R) MKL) requires that many 'int's for MKLMPI_Status. */
#define MKLMPI_STATUS_SIZE 16

/*Definition of this structure must be aligned with constants in mklmpi.fi*/
/*MPI_SOURCE=1,MPI_TAG=2,MPI_ERROR=3,MPI_STATUS_SIZE=16(ints)*/
struct MKLMPI__Status_s
{
    int MPI_SOURCE;
    int MPI_TAG;
    int MPI_ERROR;
    int pad[MKLMPI_STATUS_SIZE-3]; /*reserved*/
};

int MKLMPI_Abort(MKLMPI_Comm,int);
int MKLMPI_Allgather(const void*,int,MKLMPI_Datatype,void*,int,MKLMPI_Datatype,MKLMPI_Comm);
int MKLMPI_Allgatherv(const void*,int,MKLMPI_Datatype,void*,const int*,const int*,MKLMPI_Datatype,MKLMPI_Comm);
int MKLMPI_Allreduce(void*,void*,int,MKLMPI_Datatype,MKLMPI_Op,MKLMPI_Comm);
int MKLMPI_Alltoall(const void*,int,MKLMPI_Datatype,void*,int,MKLMPI_Datatype,MKLMPI_Comm);
int MKLMPI_Alltoallv(const void*,const int*,const int*,MKLMPI_Datatype,void*,const int*,const int*,MKLMPI_Datatype,MKLMPI_Comm);
int MKLMPI_Barrier(MKLMPI_Comm);
int MKLMPI_Bcast(void*,int,MKLMPI_Datatype,int,MKLMPI_Comm);
int MKLMPI_Cart_create(MKLMPI_Comm,int,const int*,const int*,int,MKLMPI_Comm*);
int MKLMPI_Cart_sub(MKLMPI_Comm, const int*, MKLMPI_Comm*);
MKLMPI_Fint MKLMPI_Comm_c2f(MKLMPI_Comm);
int MKLMPI_Comm_compare(MKLMPI_Comm,MKLMPI_Comm,int*);
int MKLMPI_Comm_create(MKLMPI_Comm,MKLMPI_Group,MKLMPI_Comm*);
int MKLMPI_Comm_dup(MKLMPI_Comm,MKLMPI_Comm*);
MKLMPI_Comm MKLMPI_Comm_f2c(MKLMPI_Fint);
int MKLMPI_Comm_free(MKLMPI_Comm*);
int MKLMPI_Comm_get_attr(MKLMPI_Comm,int,void*,int*);
int MKLMPI_Comm_group(MKLMPI_Comm,MKLMPI_Group*);
int MKLMPI_Comm_rank(MKLMPI_Comm,int*);
int MKLMPI_Comm_size(MKLMPI_Comm,int*);
int MKLMPI_Comm_split(MKLMPI_Comm,int,int,MKLMPI_Comm*);
int MKLMPI_Error_class(int,int*);
int MKLMPI_Finalize(void);
int MKLMPI_Gatherv(const void*,int,MKLMPI_Datatype,void*,const int*,const int*,MKLMPI_Datatype,int,MKLMPI_Comm);
int MKLMPI_Get_address(void *, MKLMPI_Aint *);
int MKLMPI_Group_free(MKLMPI_Group*);
int MKLMPI_Group_incl(MKLMPI_Group,int,const int*,MKLMPI_Group*);
int MKLMPI_Init(int*,char***);
int MKLMPI_Initialized(int*);
int MKLMPI_Iprobe(int,int,MKLMPI_Comm,int*,MKLMPI_Status*);
int MKLMPI_Irecv(void*,int,MKLMPI_Datatype,int,int,MKLMPI_Comm,MKLMPI_Request*);
int MKLMPI_Isend(const void*,int,MKLMPI_Datatype,int,int,MKLMPI_Comm,MKLMPI_Request*);
MKLMPI_Fint MKLMPI_Op_c2f(MKLMPI_Op);
int MKLMPI_Op_create(MKLMPI_User_function*,int,MKLMPI_Op*);
MKLMPI_Op MKLMPI_Op_f2c(MKLMPI_Fint);
int MKLMPI_Op_free( MKLMPI_Op*);
int MKLMPI_Pack(const void*,int,MKLMPI_Datatype,void*,int,int*,MKLMPI_Comm);
int MKLMPI_Pack_size(int,MKLMPI_Datatype,MKLMPI_Comm,int*);
int MKLMPI_Recv(void*,int,MKLMPI_Datatype,int,int,MKLMPI_Comm,MKLMPI_Status*);
int MKLMPI_Reduce(const void*,void*,int,MKLMPI_Datatype,MKLMPI_Op,int,MKLMPI_Comm);
int MKLMPI_Rsend(const void*,int,MKLMPI_Datatype,int,int,MKLMPI_Comm);
int MKLMPI_Scatterv(const void*,const int*,const int*,MKLMPI_Datatype,void*,int,MKLMPI_Datatype,int,MKLMPI_Comm);
int MKLMPI_Send(const void*,int,MKLMPI_Datatype,int,int,MKLMPI_Comm);
int MKLMPI_Sendrecv(const void*,int,MKLMPI_Datatype,int,int,void*,int,MKLMPI_Datatype,int,int,MKLMPI_Comm,MKLMPI_Status*);
int MKLMPI_Test(MKLMPI_Request*,int*,MKLMPI_Status*);
int MKLMPI_Testall(int,MKLMPI_Request*,int*,MKLMPI_Status*);
MKLMPI_Fint MKLMPI_Type_c2f(MKLMPI_Datatype);
int MKLMPI_Type_commit(MKLMPI_Datatype*);
int MKLMPI_Type_create_struct(int,const int*,const MKLMPI_Aint*,const MKLMPI_Datatype*,MKLMPI_Datatype*);
int MKLMPI_Type_contiguous(int,MKLMPI_Datatype,MKLMPI_Datatype*);
MKLMPI_Datatype MKLMPI_Type_f2c(MKLMPI_Fint);
int MKLMPI_Type_free(MKLMPI_Datatype*);
int MKLMPI_Type_indexed(int,const int*,const int*,MKLMPI_Datatype,MKLMPI_Datatype*);
int MKLMPI_Type_vector(int,int,int,MKLMPI_Datatype,MKLMPI_Datatype*);
int MKLMPI_Type_create_resized(MKLMPI_Datatype,MKLMPI_Aint,MKLMPI_Aint,MKLMPI_Datatype*);
int MKLMPI_Unpack(const void*,int,int*,void*,int,MKLMPI_Datatype,MKLMPI_Comm);
int MKLMPI_Wait(MKLMPI_Request*,MKLMPI_Status*);
int MKLMPI_Waitall(int,MKLMPI_Request*,MKLMPI_Status*);
int MKLMPI_Waitany(int,MKLMPI_Request*,int*,MKLMPI_Status*);
double MKLMPI_Wtime(void);

/* These functions are not a part of MPI wrappers but allow translation of real
 * MPI handles to their MKLMPI versions and back. */
MKLMPI_Comm MKLMPI_Comm_wrap(const void*);
int MKLMPI_Comm_unwrap(void*,const void*);

MKLMPI_Comm MKLMPI_Comm_wrap_f(const void*);
int MKLMPI_Comm_unwrap_f(void*,const void*);

#if defined(__cplusplus)
}
#endif

/* XXX: This should have a different meaning in FORTRAN */
#define MKLMPI_STATUS_IGNORE      (MKLMPI_Status *)1
#define MKLMPI_STATUSES_IGNORE    (MKLMPI_Status *)1

#endif /*MKLMPI_OMIT_C_DECLARATIONS*/
#endif /*MKLMPI_H_INCLUDED*/
