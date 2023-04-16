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

/*
*   Content:
*       Wrapper library building. Source file.
*       This library allows to use custom MPI implementation MKLMPI interface.
*****************************************************************************/

#define MKLMPI_OMIT_MPI_VIEW 1
#include "mklmpi.h"
#include "mpi.h"

#include "mkl_service.h"

static void* xmalloc(size_t sz)                 { return MKL_malloc(sz,0); }
static void* xcalloc(size_t nmemb, size_t size) { return MKL_calloc(nmemb, size,0); }
static void  xfree  (void*  p )                 { MKL_free(p); return;   }

/** VOID - remove constness off a pointer.
 * Different MPIs may define same arg const or not.
 */
#define VOID(ptr) ((void*)(ptr))

/** STATIC_ASSERT - make sure a compile-time assumption is valid. */
#define CONCAT(a,b) a##b
#define STATIC_ASSERT(expr) STATIC_ASSERt(expr,__LINE__)
#define STATIC_ASSERt(expr,line) typedef char CONCAT(static_assert_,line)[2*!!(expr)-1]

STATIC_ASSERT(sizeof(MKLMPI_Status)-offsetof(MKLMPI_Status,pad) >= sizeof(MPI_Status));

/** Translate an MKLMPI_Comm into MPI_Comm. */
static MPI_Comm X2COMM(MKLMPI_Comm xcomm)
{
    if (xcomm == MKLMPI_COMM_WORLD) return MPI_COMM_WORLD;
    if (xcomm == MKLMPI_COMM_NULL)  return MPI_COMM_NULL;
    return (MPI_Comm)xcomm;
}

/** Compute MKLMPI_Comm for MPI_Comm. */
static MKLMPI_Comm X4COMM(MPI_Comm comm)
{
    if (comm == MPI_COMM_WORLD) return MKLMPI_COMM_WORLD;
    if (comm == MPI_COMM_NULL)  return MKLMPI_COMM_NULL;
    return (MKLMPI_Comm)comm;
}

/** Transle MPI result to MKLMPI result. */
static int X4COMPARERESULT(int res)
{
    if (res == MPI_SIMILAR) {
        return MKLMPI_SIMILAR;
    }

    /*NOTE: that is not correct in general case*/
    return MKLMPI_UNEQUAL;
}

/** Translate MKLMPI_Datatype into MPI_Datatype. */
static MPI_Datatype X2DATATYPE(MKLMPI_Datatype xdatatype)
{
#define RETURN_IF(v,sym) if (v == MKL##sym) return sym
    RETURN_IF(xdatatype,MPI_BYTE);
    RETURN_IF(xdatatype,MPI_COMPLEX);
    RETURN_IF(xdatatype,MPI_DOUBLE);
    RETURN_IF(xdatatype,MPI_DOUBLE_INT);
    RETURN_IF(xdatatype,MPI_DOUBLE_COMPLEX);
    RETURN_IF(xdatatype,MPI_DOUBLE_PRECISION);
    RETURN_IF(xdatatype,MPI_FLOAT);
    RETURN_IF(xdatatype,MPI_INT);
    RETURN_IF(xdatatype,MPI_INTEGER);
    RETURN_IF(xdatatype,MPI_LONG_LONG_INT);
    RETURN_IF(xdatatype,MPI_PACKED);
    RETURN_IF(xdatatype,MPI_REAL);
    RETURN_IF(xdatatype,MPI_UNSIGNED_SHORT);
#undef RETURN_IF
    return (MPI_Datatype) xdatatype;
}

/** Compute MKLMPI_Datatype for MPI_Datatype. */
static MKLMPI_Datatype X4DATATYPE(MPI_Datatype datatype)
{
#define RETURN_IF(v,sym) if (v == sym) return MKL##sym
    RETURN_IF(datatype,MPI_BYTE);
    RETURN_IF(datatype,MPI_COMPLEX);
    RETURN_IF(datatype,MPI_DOUBLE);
    RETURN_IF(datatype,MPI_DOUBLE_INT);
    RETURN_IF(datatype,MPI_DOUBLE_COMPLEX);
    RETURN_IF(datatype,MPI_DOUBLE_PRECISION);
    RETURN_IF(datatype,MPI_FLOAT);
    RETURN_IF(datatype,MPI_INT);
    RETURN_IF(datatype,MPI_INTEGER);
    RETURN_IF(datatype,MPI_LONG_LONG_INT);
    RETURN_IF(datatype,MPI_PACKED);
    RETURN_IF(datatype,MPI_REAL);
    RETURN_IF(datatype,MPI_UNSIGNED_SHORT);
#undef RETURN_IF
    return (MKLMPI_Datatype) datatype;
}

/** Compute MKLMPI_ERR_xxx for MPI_ERR_xxx. */
static int X4ERR(int mpi_err)
{
#define RETURN_IF(v,sym) if (v == sym) return MKL##sym
    RETURN_IF(mpi_err,MPI_SUCCESS);
    RETURN_IF(mpi_err,MPI_ERR_INTERN);
    RETURN_IF(mpi_err,MPI_ERR_NO_MEM);
    RETURN_IF(mpi_err,MPI_ERR_OTHER);
    RETURN_IF(mpi_err,MPI_ERR_UNKNOWN);
#undef RETURN_IF
    return MKLMPI_ERR_INTERN;
}

/** Translate an MKLMPI_Group into MPI_Group. */
static MPI_Group X2GROUP(MKLMPI_Group xgroup)
{
#define RETURN_IF(sym) if (xgroup == MKL##sym) return sym
    /* Not used yet */
    /* RETURN_IF(MPI_GROUP_EMPTY); */
    /* RETURN_IF(MPI_GROUP_NULL); */
#undef RETURN_IF
    return (MPI_Group)xgroup;
}

/** Compute MKLMPI_Group for MPI_Group. */
static MKLMPI_Group X4GROUP(MPI_Group group)
{
#define RETURN_IF(sym) if (group == sym) return MKL##sym
    /* Not used yet */
    /* RETURN_IF(MPI_GROUP_EMPTY); */
    /* RETURN_IF(MPI_GROUP_NULL); */
#undef RETURN_IF
    return (MKLMPI_Group)group;
}

/** Translate a keyval from MKLMPI into MPI. */
static int X2KEYVAL(int xkeyval)
{
#define RETURN_IF(sym) if (xkeyval == MKL##sym) return sym
    RETURN_IF(MPI_TAG_UB);
#undef RETURN_IF
    return xkeyval;
}

/** Translate MKLMPI_Op into MPI_Op. */
static MPI_Op X2OP(MKLMPI_Op xop)
{
#define RETURN_IF(sym) if (xop == MKL##sym) return sym;
    RETURN_IF(MPI_MIN);
    RETURN_IF(MPI_MAX);
    RETURN_IF(MPI_SUM);
    RETURN_IF(MPI_MAXLOC);
#undef RETURN_IF
    return (MPI_Op) xop;
}

/** Make MKLMPI_Op for MPI_Op. */
static MKLMPI_Op X4OP(MPI_Op op)
{
#define RETURN_IF(sym) if (op == sym) return MKL##sym;
    RETURN_IF(MPI_MIN);
    RETURN_IF(MPI_MAX);
    RETURN_IF(MPI_SUM);
    RETURN_IF(MPI_MAXLOC);
#undef RETURN_IF
    return (MKLMPI_Op) op;
}

/** Translate MKLMPI_Request into MPI_Request. */
static void X2REQUEST(const MKLMPI_Request *xrequest,MPI_Request *request)
{
    if (xrequest && request)
    {
        if (*xrequest == MKLMPI_REQUEST_NULL)
        {
            request[0] = MPI_REQUEST_NULL;
        }
        else
        {
            request[0] = (MPI_Request) xrequest[0];
        }
    }
}

/** Make MKLMPI_Request for MPI_Request. */
static void X4REQUEST(MKLMPI_Request *xrequest,const MPI_Request *request)
{
    if (xrequest && request)
    {
        if (*request == MPI_REQUEST_NULL)
        {
            xrequest[0] = MKLMPI_REQUEST_NULL;
        }
        else
        {
            xrequest[0] = (MKLMPI_Request) request[0];
        }
    }
}

/** Translate array of MKLMPI_Requests into NEW array of MPI_Requests. */
static int X2REQUESTS(int N, const MKLMPI_Request *xrequests, MPI_Request **prequests)
{
    *prequests = 0;
    if (N > 0 && xrequests)
    {
        int n;
        *prequests = (MPI_Request*)xcalloc(N,sizeof(MPI_Request));
        if (!*prequests) return MPI_ERR_NO_MEM;

        for (n = 0; n < N; ++n)
        {
            if (xrequests[n] == MKLMPI_REQUEST_NULL)
            {
                (*prequests)[n] = MPI_REQUEST_NULL;
            }
            else
            {
                (*prequests)[n] = (MPI_Request) xrequests[n];
            }
        }
    }
    return 0;
}

/** Make \a xrequests translated from \a requests and then DELETE \a requests. */
static void X4REQUESTS(int N, MKLMPI_Request *xrequests, const MPI_Request *requests)
{
    if (N > 0 && xrequests && requests)
    {
        int n;
        for (n = 0; n < N; ++n)
        {
            if (requests[n] == MPI_REQUEST_NULL)
            {
                xrequests[n] = MKLMPI_REQUEST_NULL;
            }
            else
            {
                xrequests[n] = (MKLMPI_Request) requests[n];
            }
        }
    }
    if (requests) xfree( (void*)requests );
}

/** Translate xsource into MPI source. */
static int X2SOURCE(int xsource)
{
#define RETURN_IF(sym) if (xsource == MKL##sym) return sym;
    RETURN_IF(MPI_ANY_SOURCE);
#undef RETURN_IF
    return xsource;
}

/** Translate MKLMPI_Status into MPI_Status. */
#if 0
static void UNUSED_X2STATUS(const MKLMPI_Status *xstatus,MPI_Status *status)
{
    /* Intel(R) MKL doesn't use STATUS as IN parameter */
}
#endif

/** Make MKLMPI_Status for MPI_Status. */
static void X4STATUS(MKLMPI_Status *xstatus,const MPI_Status *status)
{
    if (xstatus == (MKLMPI_Status*)MKLMPI_STATUS_IGNORE) return;
    if (xstatus && status)
    {
        xstatus->MPI_SOURCE = status->MPI_SOURCE;
        xstatus->MPI_TAG    = status->MPI_TAG;
        xstatus->MPI_ERROR  = status->MPI_ERROR;
        *(MPI_Status*)(&xstatus->pad) = *status;
    }
}

/** Make NEW array of MPI_Status and translate array of MKLMPI_Status into it. */
static int X2STATUSES(int N, const MKLMPI_Status *xstatuses, MPI_Status **pstatuses)
{
    *pstatuses = MPI_STATUSES_IGNORE;
    if (xstatuses == (MKLMPI_Status*)MKLMPI_STATUSES_IGNORE) return 0;
    *pstatuses = NULL;
    if (N > 0 && xstatuses)
    {
        int n;
        *pstatuses = (MPI_Status*)xcalloc(N,sizeof(MPI_Status));
        if (!*pstatuses) return MPI_ERR_NO_MEM;
        for (n = 0; n < N; ++n)
        {
            (*pstatuses)[n] = *(MPI_Status*)&xstatuses[n].pad[0];
        }
    }
    return 0;
}

/** Make \a xstatuses translated from \a statuses, and DELETE \a statuses. */
static void X4STATUSES(int N, MKLMPI_Status *xstatuses, const MPI_Status *statuses)
{
    if (xstatuses == (MKLMPI_Status*)MKLMPI_STATUSES_IGNORE) return;
    if (N > 0 && xstatuses && statuses)
    {
        int n;
        for (n = 0; n < N; ++n)
        {
            xstatuses[n].MPI_SOURCE = statuses[n].MPI_SOURCE;
            xstatuses[n].MPI_TAG    = statuses[n].MPI_TAG;
            xstatuses[n].MPI_ERROR  = statuses[n].MPI_ERROR;
            *(MPI_Status*)(&xstatuses[n].pad[0]) = statuses[n];
        }
    }
    if (statuses) xfree( (void*)statuses );
}

/**********************************************************************/
/* MPI WRAPPERS */
/**********************************************************************/

int MKLMPI_Abort(MKLMPI_Comm comm,int errorcode)
{
    int res = MPI_Abort( X2COMM(comm), errorcode );
    return X4ERR(res);
}

int MKLMPI_Allgather(const void *sendbuf,int sendcount,MKLMPI_Datatype sendtype,
                   /***/ void *recvbuf,int recvcount,MKLMPI_Datatype recvtype,
                   MKLMPI_Comm comm)
{
    int res = MPI_Allgather(VOID(sendbuf),sendcount,X2DATATYPE(sendtype),
                            /***/recvbuf ,recvcount,X2DATATYPE(recvtype),
                            X2COMM(comm));
    return X4ERR(res);
}

int MKLMPI_Allgatherv(const void *sendbuf,int sendcount,
                      MKLMPI_Datatype sendtype,/***/ void *recvbuf,
                      const int recvcounts[], const int displs[],
                      MKLMPI_Datatype recvtype,MKLMPI_Comm comm)
{
    int res = MPI_Allgatherv(VOID(sendbuf),sendcount,X2DATATYPE(sendtype),
                        /***/recvbuf ,(int*)recvcounts,(int*)displs,
                        X2DATATYPE(recvtype), X2COMM(comm));
    return X4ERR(res);
}

int MKLMPI_Alltoall(const void* sendbuf, int sendcount, MKLMPI_Datatype sendtype,
                  /***/ void* recvbuf, int recvcount, MKLMPI_Datatype recvtype,
                  MKLMPI_Comm comm)
{
    int res = MPI_Alltoall(VOID(sendbuf),sendcount,X2DATATYPE(sendtype),
                           /***/recvbuf ,recvcount,X2DATATYPE(recvtype),
                           X2COMM(comm));
    return X4ERR(res);
}

int MKLMPI_Allreduce(void* sendbuf,void* recvbuf,int count,MKLMPI_Datatype datatype,
                   MKLMPI_Op op,MKLMPI_Comm comm)
{
    int err = MPI_Allreduce(sendbuf, recvbuf, count, X2DATATYPE(datatype),
                         X2OP(op), X2COMM(comm));
    return X4ERR(err);
}

int MKLMPI_Alltoallv(const void* sendbuf, const int sendcounts[],
                   const int sdispls[], MKLMPI_Datatype sendtype,
                   /***/ void* recvbuf, const int recvcounts[],
                   const int rdispls[], MKLMPI_Datatype recvtype,
                   MKLMPI_Comm comm)
{
    int res = MPI_Alltoallv(VOID(sendbuf),(int*)(sendcounts),
                            (int*)(sdispls),X2DATATYPE(sendtype),
                            /***/recvbuf ,(int*)(recvcounts),
                            (int*)(rdispls),X2DATATYPE(recvtype),
                            X2COMM(comm));
    return X4ERR(res);
}


int MKLMPI_Barrier(MKLMPI_Comm comm)
{
    int res = MPI_Barrier(X2COMM(comm));
    return X4ERR(res);
}

int MKLMPI_Bcast(void* buffer, int count, MKLMPI_Datatype datatype, int root,
               MKLMPI_Comm comm)
{
    int res = MPI_Bcast(buffer,count,X2DATATYPE(datatype),root,X2COMM(comm));
    return X4ERR(res);
}

int MKLMPI_Cart_create(MKLMPI_Comm comm_old, int ndims, const int dims[],
                    const int periods[], int reorder, MKLMPI_Comm *comm_cart)
{
    MPI_Comm mpi_comm;
    int res = MPI_Cart_create(X2COMM(comm_old),ndims, (int*)dims, (int*)periods,
                    reorder, &mpi_comm);
    comm_cart[0] = X4COMM(mpi_comm);
    return X4ERR(res);
}

int MKLMPI_Cart_sub(MKLMPI_Comm comm, const int remain_dims[], MKLMPI_Comm *newcomm)
{
    MPI_Comm mpi_comm;
    int res = MPI_Cart_sub(X2COMM(comm), (int*)remain_dims, &mpi_comm);
    newcomm[0] = X4COMM(mpi_comm);
    return X4ERR(res);
}

MKLMPI_Fint MKLMPI_Comm_c2f(MKLMPI_Comm xcomm)
{
    return (MKLMPI_Fint)MPI_Comm_c2f( X2COMM(xcomm) );
}

int MKLMPI_Comm_compare(MKLMPI_Comm comm1, MKLMPI_Comm comm2, int *result)
{
    int res = MPI_Comm_compare(X2COMM(comm1),X2COMM(comm2),result);
    *result = X4COMPARERESULT(*result);
    return X4ERR(res);
}

int MKLMPI_Comm_create(MKLMPI_Comm comm, MKLMPI_Group group,
                     /*OUT*/ MKLMPI_Comm *xnewcomm)
{
    MPI_Comm mpi_comm;
    int res = MPI_Comm_create(X2COMM(comm),X2GROUP(group),&mpi_comm);
    xnewcomm[0] = X4COMM(mpi_comm);
    return X4ERR(res);
}

int MKLMPI_Comm_dup(MKLMPI_Comm comm, /*OUT*/ MKLMPI_Comm *xnewcomm)
{
    MPI_Comm mpi_comm;
    int res = MPI_Comm_dup(X2COMM(comm),&mpi_comm);
    xnewcomm[0] = X4COMM(mpi_comm);
    return X4ERR(res);
}

MKLMPI_Comm MKLMPI_Comm_f2c(MKLMPI_Fint xfcomm)
{
    return X4COMM( MPI_Comm_f2c((MPI_Fint)xfcomm) );
}

int MKLMPI_Comm_free(/*INOUT*/ MKLMPI_Comm* xcomm)
{
    MPI_Comm comm = X2COMM(xcomm[0]);
    int err = MPI_Comm_free(&comm);
    xcomm[0] = X4COMM(comm);
    return X4ERR(err);
}

int MKLMPI_Comm_get_attr(MKLMPI_Comm comm,int keyval,void* attribute_val,int *flag)
{
    int res = MPI_Comm_get_attr(X2COMM(comm),X2KEYVAL(keyval),attribute_val,flag);
    return X4ERR(res);
}


int MKLMPI_Comm_group(MKLMPI_Comm comm, MKLMPI_Group *xgroup)
{
    MPI_Group mpi_group;
    int res = MPI_Comm_group(X2COMM(comm),&mpi_group);
    xgroup[0] = X4GROUP(mpi_group);
    return X4ERR(res);
}

int MKLMPI_Comm_rank(MKLMPI_Comm comm, int *rank)
{
    int res = MPI_Comm_rank(X2COMM(comm),rank);
    return X4ERR(res);
}

int MKLMPI_Comm_size(MKLMPI_Comm comm, int *size)
{
    int res = MPI_Comm_size(X2COMM(comm),size);
    return X4ERR(res);
}

int MKLMPI_Comm_split(MKLMPI_Comm comm, int color, int key, MKLMPI_Comm *newcomm)
{
    MPI_Comm mpi_comm;
    int res = MPI_Comm_split(X2COMM(comm),color,key,&mpi_comm);
    newcomm[0] = X4COMM(mpi_comm);
    return X4ERR(res);
}

int MKLMPI_Error_class(int xerrorcode, int* xerrorclass)
{
    /* this function is trivial because of X4ERR function */
    *xerrorclass = xerrorcode;
    return MKLMPI_SUCCESS;
}

int MKLMPI_Finalize(void)
{
    int err = MPI_Finalize();
    return X4ERR(err);
}

int MKLMPI_Gatherv(const void *sendbuf,int sendcount,MKLMPI_Datatype sendtype,
                 /***/ void *recvbuf,const int recvcounts[],const int displs[],
                 MKLMPI_Datatype recvtype,int root,MKLMPI_Comm comm)
{
    int res = MPI_Gatherv(VOID(sendbuf),sendcount,X2DATATYPE(sendtype),
                          /***/recvbuf ,(int*)(recvcounts),(int*)(displs),
                          X2DATATYPE(recvtype),root,X2COMM(comm));
    return X4ERR(res);
}

int MKLMPI_Get_address(void *location, MKLMPI_Aint *address)
{
       int err = MPI_Get_address(location, address);
       return X4ERR(err);
}

int MKLMPI_Group_free(/*INOUT*/ MKLMPI_Group* xgroup)
{
    MPI_Group group = X2GROUP(xgroup[0]);
    int err = MPI_Group_free(&group);
    xgroup[0] = X4GROUP(group);
    return X4ERR(err);
}

int MKLMPI_Group_incl(MKLMPI_Group xgroup, int n, const int* ranks,
                    /*OUT*/ MKLMPI_Group* xnewgroup)
{
    MPI_Group newgroup;
    int err = MPI_Group_incl(X2GROUP(xgroup), n, (int*)(ranks), &newgroup);
    xnewgroup[0] = X4GROUP(newgroup);
    return X4ERR(err);
}

int MKLMPI_Init(int *argc, char ***argv)
{
    int err = MPI_Init( argc, argv );
    return X4ERR(err);
}

int MKLMPI_Initialized(int *flag)
{
    int res = MPI_Initialized(flag);
    return X4ERR(res);
}

int MKLMPI_Iprobe(int source, int tag, MKLMPI_Comm comm, int *flag,
                /*OUT*/ MKLMPI_Status *xstatus)
{
    MPI_Status mpi_status;
    int res = MPI_Iprobe(X2SOURCE(source), tag, X2COMM(comm), flag, &mpi_status);
    if (*flag) X4STATUS( xstatus, &mpi_status );
    return X4ERR(res);
}

int MKLMPI_Irecv(void* buf, int count, MKLMPI_Datatype datatype, int source,
               int tag, MKLMPI_Comm comm, /*OUT*/ MKLMPI_Request *xrequest)
{
    MPI_Request mpi_request;
    int res = MPI_Irecv(buf, count, X2DATATYPE(datatype), X2SOURCE(source), tag,
                    X2COMM(comm), &mpi_request);
    X4REQUEST( xrequest, &mpi_request );
    return X4ERR(res);
}

int MKLMPI_Isend(const void *buf, int count, MKLMPI_Datatype datatype,
               int dest, int tag, MKLMPI_Comm comm,
               /*OUT*/ MKLMPI_Request *xrequest)
{
    MPI_Request mpi_request;
    int res = MPI_Isend(VOID(buf),count,X2DATATYPE(datatype),
                    dest,tag,X2COMM(comm),&mpi_request);
    X4REQUEST(xrequest,&mpi_request);
    return X4ERR(res);
}

MKLMPI_Fint MKLMPI_Op_c2f(MKLMPI_Op xop)
{
    return (MKLMPI_Fint)MPI_Op_c2f( X2OP(xop) );
}

int MKLMPI_Op_create(MKLMPI_User_function *user_fn, int commute,
                   /*OUT*/ MKLMPI_Op* xop)
{
    MPI_Op op;
    int err = MPI_Op_create((MPI_User_function*)user_fn,commute,&op);
    xop[0] = X4OP(op);
    return X4ERR(err);
}

MKLMPI_Op MKLMPI_Op_f2c(MKLMPI_Fint xfop)
{
    return X4OP( MPI_Op_f2c((MPI_Fint)xfop) );
}

int MKLMPI_Op_free(/*INOUT*/ MKLMPI_Op *xop)
{
    MPI_Op op = X2OP(xop[0]);
    int err = MPI_Op_free(&op);
    xop[0] = X4OP(op);
    return X4ERR(err);
}

int MKLMPI_Pack(const void* inbuf, int incount, MKLMPI_Datatype xdatatype,
              void *outbuf, int outsize, int *position, MKLMPI_Comm xcomm)
{
    int err = MPI_Pack(VOID(inbuf), incount, X2DATATYPE(xdatatype),
                       outbuf, outsize, position, X2COMM(xcomm));
    return X4ERR(err);
}

int MKLMPI_Pack_size(int incount, MKLMPI_Datatype datatype, MKLMPI_Comm comm,
                   int *size)
{
    int res = MPI_Pack_size(incount,X2DATATYPE(datatype),X2COMM(comm),size);
    return X4ERR(res);
}

int MKLMPI_Recv(void *buf, int count, MKLMPI_Datatype datatype,
              int source, int tag, MKLMPI_Comm comm,
              /*OUT*/ MKLMPI_Status *xstatus)
{
    MPI_Status mpi_status;
    int res = MPI_Recv(buf,count,X2DATATYPE(datatype),
                       X2SOURCE(source),tag,X2COMM(comm),&mpi_status);
    X4STATUS(xstatus,&mpi_status);
    return X4ERR(res);
}

int MKLMPI_Reduce(const void* sendbuf, void* recvbuf, int count,
                MKLMPI_Datatype xdatatype, MKLMPI_Op xop, int root, MKLMPI_Comm xcomm)
{
    int err = MPI_Reduce(VOID(sendbuf), recvbuf, count,
                         X2DATATYPE(xdatatype), X2OP(xop), root, X2COMM(xcomm));
    return X4ERR(err);
}

int MKLMPI_Rsend(const void* buf, int count, MKLMPI_Datatype datatype,
               int dest, int tag, MKLMPI_Comm comm)
{
    int res = MPI_Rsend(VOID(buf),count,X2DATATYPE(datatype),
                        dest,tag,X2COMM(comm));
    return X4ERR(res);
}

int MKLMPI_Scatterv(const void* sendbuf, const int* sendcounts,
                  const int*  displs,  MKLMPI_Datatype xsendtype,
                  /***/ void* recvbuf, int recvcount,
                  MKLMPI_Datatype xrecvtype, int root, MKLMPI_Comm xcomm)
{
    int err = MPI_Scatterv(VOID(sendbuf), (int*)(sendcounts),
                           (int*)(displs),  X2DATATYPE(xsendtype),
                           /***/recvbuf , recvcount,
                           X2DATATYPE(xrecvtype), root, X2COMM(xcomm));
    return X4ERR(err);
}

int MKLMPI_Send(const void* buf, int count, MKLMPI_Datatype datatype, int dest,
              int tag, MKLMPI_Comm comm)
{
    int res = MPI_Send(VOID(buf),count,X2DATATYPE(datatype),dest,
                       tag,X2COMM(comm));
    return X4ERR(res);
}

int MKLMPI_Sendrecv(const void *sendbuf, int sendcount, MKLMPI_Datatype sendtype,
                  int dest, int sendtag,
                  /***/ void *recvbuf, int recvcount, MKLMPI_Datatype recvtype,
                  int source, int recvtag, MKLMPI_Comm comm,
                  /*OUT*/ MKLMPI_Status *xstatus)
{
    MPI_Status mpi_status;
    int res = MPI_Sendrecv(VOID(sendbuf), sendcount, X2DATATYPE(sendtype),
                       dest,   sendtag,
                       /***/recvbuf , recvcount, X2DATATYPE(recvtype),
                       X2SOURCE(source), recvtag,
                       X2COMM(comm), &mpi_status);
    X4STATUS(xstatus,&mpi_status);
    return X4ERR(res);
}

int MKLMPI_Test(/*INOUT*/ MKLMPI_Request *xrequest,
              /*OUT*/ int *flag, /*OUT*/ MKLMPI_Status *xstatus)
{
    MPI_Request request;
    MPI_Status status;
    int err;

    X2REQUEST(xrequest,&request);
    err = MPI_Test(&request,flag,&status);
    X4REQUEST(xrequest,&request);
    if (flag)
    {
        X4STATUS(xstatus,&status);
    }
    return X4ERR(err);
}

int MKLMPI_Testall(int count, MKLMPI_Request* xrequests, int* flag,
        MKLMPI_Status* xstatuses)
{
    int err, c;
    MPI_Request *mpi_requests;
    MPI_Status  *mpi_statuses;

    mpi_requests = (MPI_Request*)xmalloc(sizeof(MPI_Request)*count);
    mpi_statuses = (MPI_Status*)xmalloc(sizeof(MPI_Status)*count);
    if (NULL == mpi_requests || NULL == mpi_statuses)
    {
        err = MPI_ERR_NO_MEM;
        goto done;
    }

    for (c = 0; c < count; ++c)
    {
        X2REQUEST(&xrequests[c],&mpi_requests[c]);
    }
    err = MPI_Testall(count,mpi_requests,flag,mpi_statuses);
    if (flag)
    {
        for (c = 0; c < count; ++c)
        {
            X4REQUEST(&xrequests[c],&mpi_requests[c]);
            X4STATUS(&xstatuses[c],&mpi_statuses[c]);
        }
    }

 done:
    xfree(mpi_requests);
    xfree(mpi_statuses);
    return X4ERR(err);
}

MKLMPI_Fint MKLMPI_Type_c2f(MKLMPI_Datatype xdatatype)
{
    return (MKLMPI_Fint)MPI_Type_c2f( X2DATATYPE(xdatatype) );
}

int MKLMPI_Type_commit(/*INOUT*/ MKLMPI_Datatype *xdatatype)
{
    MPI_Datatype mpi_datatype = X2DATATYPE(xdatatype[0]);
    int res = MPI_Type_commit(&mpi_datatype);
    xdatatype[0] = X4DATATYPE(mpi_datatype);
    return X4ERR(res);
}

int MKLMPI_Type_contiguous(int count, MKLMPI_Datatype oldtype,
                         /*OUT*/ MKLMPI_Datatype *xnewtype)
{
    MPI_Datatype mpi_datatype;
    int res = MPI_Type_contiguous(count, X2DATATYPE(oldtype), &mpi_datatype);
    xnewtype[0] = X4DATATYPE(mpi_datatype);
    return X4ERR(res);
}

int MKLMPI_Type_create_struct(int count, const int array_of_blocklengths[],
                            const MKLMPI_Aint xarray_of_displacements[],
                            const MKLMPI_Datatype xarray_of_types[],
                            MKLMPI_Datatype *xnewtype)
{
    MPI_Aint *array_of_displacements = NULL;
    MPI_Datatype *array_of_types = NULL;
    MPI_Datatype newtype;
    int err, n;

    array_of_displacements = (MPI_Aint*)xcalloc(count,sizeof(MPI_Aint));
    array_of_types = (MPI_Datatype*)xcalloc(count,sizeof(MPI_Datatype));
    if (NULL==array_of_displacements || NULL==array_of_types)
    {
        err = MPI_ERR_NO_MEM;
        goto done;
    }
    for (n = 0; n < count; ++n)
    {
        array_of_displacements[n] = (MPI_Aint)xarray_of_displacements[n];
        array_of_types[n] = X2DATATYPE(xarray_of_types[n]);
    }
    err = MPI_Type_create_struct( count, (int*)(array_of_blocklengths),
                                  array_of_displacements,
                                  array_of_types, &newtype );
    xnewtype[0] = X4DATATYPE(newtype);
 done:
    xfree(array_of_types);
    xfree(array_of_displacements);
    return X4ERR(err);
}

MKLMPI_Datatype MKLMPI_Type_f2c(MKLMPI_Fint xftype)
{
    return X4DATATYPE( MPI_Type_f2c((MPI_Fint)xftype) );
}

int MKLMPI_Type_free(MKLMPI_Datatype *xdatatype)
{
    MPI_Datatype datatype = X2DATATYPE( xdatatype[0] );
    int err = MPI_Type_free( &datatype );
    xdatatype[0] = X4DATATYPE( datatype );
    return X4ERR(err);
}

int MKLMPI_Type_indexed(int count,
                      const int array_of_blocklengths[],
                      const int array_of_displacements[],
                      MKLMPI_Datatype xoldtype,
                      MKLMPI_Datatype *xnewtype)
{
    MPI_Datatype newtype;
    int err = MPI_Type_indexed(count,
                               (int*)(array_of_blocklengths),
                               (int*)(array_of_displacements),
                               X2DATATYPE(xoldtype), &newtype);
    if (!err) xnewtype[0] = X4DATATYPE(newtype);
    return X4ERR(err);
}

/*DEPRECATED*/

int MKLMPI_Type_vector(int count, int blocklength, int stride,
                     MKLMPI_Datatype xoldtype, MKLMPI_Datatype *xnewtype)
{
    MPI_Datatype newtype;
    int err = MPI_Type_vector( count, blocklength, stride,
                               X2DATATYPE(xoldtype), &newtype );
    if (!err) xnewtype[0] = X4DATATYPE(newtype);
    return X4ERR(err);
}

int MKLMPI_Type_create_resized(MKLMPI_Datatype xoldtype, MKLMPI_Aint lb,
                    MKLMPI_Aint extent, MKLMPI_Datatype *xnewtype)
{
    MPI_Datatype newtype;
    int err = MPI_Type_create_resized(X2DATATYPE(xoldtype), lb, extent,
                                      &newtype);
    if (!err) xnewtype[0] = X4DATATYPE(newtype);
    return X4ERR(err);
}


int MKLMPI_Unpack(const void *inbuf, int insize, int *position,
                /***/ void *outbuf, int outcount, MKLMPI_Datatype datatype,
                MKLMPI_Comm comm)
{
    int res = MPI_Unpack(VOID(inbuf),insize,position,outbuf,outcount,
                         X2DATATYPE(datatype),X2COMM(comm));
    return X4ERR(res);
}

int MKLMPI_Wait(/*INOUT*/ MKLMPI_Request *xrequest, /*OUT*/ MKLMPI_Status *xstatus)
{
    MPI_Request mpi_request;
    MPI_Status mpi_status;
    int res;
    X2REQUEST(xrequest,&mpi_request);
    res = MPI_Wait(&mpi_request,&mpi_status);
    X4REQUEST(xrequest,&mpi_request);
    X4STATUS(xstatus,&mpi_status);
    return X4ERR(res);
}

int MKLMPI_Waitall(int count,
                 /*INOUT*/MKLMPI_Request array_of_xrequests[],
                 /*OUT*/  MKLMPI_Status  array_of_xstatuses[])
{
    MPI_Request *array_of_mpi_requests = NULL;
    MPI_Status  *array_of_mpi_statuses = NULL;
    int res;

    res = X2REQUESTS(count, array_of_xrequests, &array_of_mpi_requests);
    if (res) goto bail_out;

    res = X2STATUSES(count, array_of_xstatuses, &array_of_mpi_statuses);
    if (res) goto bail_out;

    res = MPI_Waitall( count, array_of_mpi_requests, array_of_mpi_statuses );

 bail_out:
    X4REQUESTS(count, array_of_xrequests, array_of_mpi_requests);
    X4STATUSES(count, array_of_xstatuses, array_of_mpi_statuses);
    return X4ERR(res);
}

int MKLMPI_Waitany(int count,
                 /*INOUT*/ MKLMPI_Request* xrequests, /*OUT*/ int* index,
                 /*OUT*/   MKLMPI_Status*  xstatus)
{
    MPI_Request *mpi_requests = NULL;
    MPI_Status  mpi_status;
    int res, c;

    mpi_requests = (MPI_Request*)xcalloc(count,sizeof(MPI_Request));
    if (NULL==mpi_requests && 0 != count)
    {
        res = MPI_ERR_NO_MEM;
        goto done;
    }
    for (c = 0; c < count; ++c)
    {
        X2REQUEST(&xrequests[c], &mpi_requests[c]);
    }
    res = MPI_Waitany(count, mpi_requests, index, &mpi_status);
    if (res == MPI_SUCCESS)
    {
        for (c = 0; c < count; ++c)
        {
            X4REQUEST(&xrequests[c], &mpi_requests[c]);
        }
        X4STATUS(xstatus, &mpi_status);
    }
 done:
    xfree(mpi_requests);
    return X4ERR(res);
}

double MKLMPI_Wtime(void)
{
    return MPI_Wtime();
}

MKLMPI_Comm MKLMPI_Comm_wrap(const void *xcomm)
{
    return X4COMM(*((const MPI_Comm *)xcomm));
}

int MKLMPI_Comm_unwrap(void *xcomm, const void *comm)
{
    *((MPI_Comm *)xcomm) = X2COMM(*((const MKLMPI_Comm *)comm));
    return MKLMPI_SUCCESS;
}

MKLMPI_Comm MKLMPI_Comm_wrap_f(const void *xfcomm)
{
    MPI_Comm comm = MPI_Comm_f2c(*((const MPI_Fint *)xfcomm));
    return X4COMM(comm);
}

int MKLMPI_Comm_unwrap_f(void *xfcomm, const void *comm)
{
    MPI_Comm xcomm = X2COMM(*((const MKLMPI_Comm *)comm));
    *((MPI_Fint *)xfcomm) = MPI_Comm_c2f(xcomm);
	return MKLMPI_SUCCESS;
}
