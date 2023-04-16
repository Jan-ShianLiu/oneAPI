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
 *
 * MPI FFTW3 wrappers to Intel(R) Math Kernel Library (Intel(R) MKL).
 *
 ******************************************************************************
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "mpi.h"
#include "fftw3-mpi.h"
#include "mpi_transpose.h"

/* ================ prototypes ================ */

static void set_local_size(ptrdiff_t n, int irank, int nrank, ptrdiff_t block,
        ptrdiff_t *local_n, ptrdiff_t *local_start);
static void work_distribution(ptrdiff_t N, ptrdiff_t P, ptrdiff_t block,
        ptrdiff_t ln[2], int p[2]);
static void get_buffers(fftw_mkl_plan plan, real_t *buf[4]);
static void ft_execute(fftw_mkl_plan plan);
static int  ft_create(ptrdiff_t n0, ptrdiff_t n1, ptrdiff_t howmany,
        ptrdiff_t block0, ptrdiff_t block1,
        int inplace, MPI_Comm comm, unsigned flags, ft_t *ft);
static void ft_destroy(fftw_mkl_plan plan);

/* ================  wrappers  ================ */

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_many_transposed)(
        int rnk, const ptrdiff_t *n, ptrdiff_t howmany,
        ptrdiff_t block0, ptrdiff_t block1, MPI_Comm comm,
        ptrdiff_t *local_n0, ptrdiff_t *local_0_start,
        ptrdiff_t *local_n1, ptrdiff_t *local_1_start)
{
    if (rnk <= 1) return 0;
    if (n == NULL) return 0;
    if (local_n0 == NULL || local_0_start == NULL) return 0;
    if (local_n1 == NULL || local_1_start == NULL) return 0;

    int irank, nrank;
    MPI_Comm_size(comm, &nrank);
    MPI_Comm_rank(comm, &irank);

    set_local_size(n[0], irank, nrank, block0, local_n0, local_0_start);
    set_local_size(n[1], irank, nrank, block1, local_n1, local_1_start);

    ptrdiff_t local_size = MAX((*local_n0)*n[1], (*local_n1)*n[0])*howmany;
    for (int i = 2; i < rnk; ++i)
        local_size *= n[i];

    return MAX(local_size, 1);
}

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_transposed)
     (int rnk, const ptrdiff_t *n, MPI_Comm comm,
      ptrdiff_t *local_n0, ptrdiff_t *local_0_start,
      ptrdiff_t *local_n1, ptrdiff_t *local_1_start)
{
    return FFTW_MPI_MANGLE(local_size_many_transposed)(
            rnk, n, 1, FFTW_MPI_DEFAULT_BLOCK, FFTW_MPI_DEFAULT_BLOCK,
            comm, local_n0, local_0_start, local_n1, local_1_start);
}

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_2d_transposed)(
        ptrdiff_t n0, ptrdiff_t n1, MPI_Comm comm,
        ptrdiff_t *local_n0, ptrdiff_t *local_0_start,
        ptrdiff_t *local_n1, ptrdiff_t *local_1_start)
{
    ptrdiff_t n[2] = { n0, n1 };
    return FFTW_MPI_MANGLE(local_size_transposed)(2, n, comm,
            local_n0, local_0_start, local_n1, local_1_start);
}

FFTW_EXTERN ptrdiff_t FFTW_MPI_MANGLE(local_size_3d_transposed)(
     ptrdiff_t n0, ptrdiff_t n1, ptrdiff_t n2, MPI_Comm comm,
     ptrdiff_t *local_n0, ptrdiff_t *local_0_start,
     ptrdiff_t *local_n1, ptrdiff_t *local_1_start)
{
    const ptrdiff_t n[3] = {n0, n1, n2};
    return FFTW_MPI_MANGLE(local_size_transposed)(3, n, comm,
            local_n0, local_0_start, local_n1, local_1_start);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_many_transpose)(
        ptrdiff_t n0, ptrdiff_t n1, ptrdiff_t howmany,
        ptrdiff_t block0, ptrdiff_t block1,
        real_t *in, real_t *out, MPI_Comm comm, unsigned flags)
{
    fftw_mkl_plan p = fftw_malloc(sizeof(*p));
    if (p == NULL) return NULL;
    ft_t *ft = (ft_t*)fftw_malloc(sizeof(*ft));
    if (ft == NULL) {
        fftw_free(p);
        return NULL;
    }

    int success = ft_create(n0, n1, howmany, block0, block1, in == out, comm,
            flags, ft);
    if (success) {
        /* save plan */
        p->io[0] = in;
        p->io[1] = out;
        p->execute = ft_execute;
        p->destroy = ft_destroy;
        p->mpi_plan = (void*)ft;
    } else {
        fftw_free(ft);
        fftw_free(p);
        p = NULL;
    }
    return (FFTW_MANGLE(plan))p;
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_transpose)(
        ptrdiff_t n0, ptrdiff_t n1,
        real_t *in, real_t *out, MPI_Comm comm, unsigned flags)
{
    return FFTW_MPI_MANGLE(plan_many_transpose)(
            n0, n1, 1,
            FFTW_MPI_DEFAULT_BLOCK, FFTW_MPI_DEFAULT_BLOCK,
            in, out, comm, flags);
}

/* ================ implementation ================ */

/**
 * @brief return local size and local start(offset) across chosen dimension
 *
 * @param n[in] the length of the dimension
 * @param irank[in] number of current MPI rank
 * @param nrank[in] number of MPI ranks
 * @param block[in] block size, zero means default block size
 * @param local_n[out] local length on current MPI rank
 * @param local_start[out] local offset on current MPI rank
 */
static void set_local_size(ptrdiff_t n, int irank, int nrank, ptrdiff_t block,
        ptrdiff_t *local_n, ptrdiff_t *local_start)
{
    int  p[2];
    ptrdiff_t ln[2];

    work_distribution(n, nrank, block, ln, p);

    if (irank < p[0]) {
        *local_n = ln[0];
        *local_start = irank*ln[0];
    } else if (irank < p[0] + p[1]) {
        *local_n = ln[1];
        *local_start = p[0]*ln[0] + (irank - p[0])*ln[1];
    } else {
        *local_n = 0; *local_start = 0;
    }
}

/**
 * @brief return work distribution for P MPI processes
 *
 * @param N[in] the length (total amount of work)
 * @param P[in] number of MPI ranks
 * @param block[in] block size, zero means default block size
 * @param ln[out] amount of work: ln[0] - for the first p[0] processes, and
 *                  ln[1] for the next p[1] <= P - p[0] processes
 * @param p[out] number of process in each group
 */
static void work_distribution(ptrdiff_t N, ptrdiff_t P, ptrdiff_t block,
        ptrdiff_t ln[2], int p[2])
{
    if (block <= 0) {
        block = DIVUP(N, P);

        p[1]  = (int)(block * P - N);
        ln[0] = block;
        p[0]  = P - p[1];
        ln[1] = ln[0] - 1;
    } else {
        if (block > N)  block = N;

        p[0] = DIVUP(N, block);
        ln[0] = block;

        if (p[0] > P || p[0]*ln[0] > N) {
            p[0] = P - 1;
            p[1] = 1;
        } else {
            p[1] = 0;
        }
        ln[1] = p[1] ? N - p[0]*ln[0] : 0;
    }
    assert(ln[0] >= 0 && ln[1] >= 0);
    assert(p[0] + p[1] <= P);
}

/**
 * @brief set pointers in array buf for consequent workflow
 *
 * @param plan[in] plan structure
 * @param buf[out] array of pointers which are used in the transposition kernel
 */
static void get_buffers(fftw_mkl_plan plan, real_t *buf[4])
{
    ft_t *ft = (ft_t*)plan->mpi_plan;
    int inplace = plan->io[0] == plan->io[1];

    /* buffer for stage 0 and 3 */
    buf[0] = plan->io[0];
    buf[3] = plan->io[1];

    /* buffer for stage 1 */
    if (!inplace && !ft->tr_out) buf[1] = plan->io[1];
    else buf[1] = ft->allocated_buf;

    /* buffer for stage 2 */
    if (ft->tr_out) buf[2] = plan->io[1];
    else {
        if (inplace) buf[2] = ft->allocated_buf + ft->local_size;
        else buf[2] = ft->allocated_buf;
    }
}

/**
 * @brief copy a block of real numbers (non-overlapping)
 *
 * @param out[out] pointer to output memory
 * @param in[in] pointer to input memory
 * @param sz[in] size in number of elements
 */
static void copy_block(real_t *out, real_t *in, size_t sz)
{
    for (size_t i = 0; i < sz; ++i)
            out[i] = in[i];
}

/**
 * @brief generic out-of-place transposition
 *
 * @param in[in] input matrix
 * @param out[out] output matrix
 * @param dir[in] type of matrix representation (row or col major)
 * @param n[in] number of rows
 * @param m1[in] number of columns with elements' size h1
 * @param m2[in] number of columns with elements' size h2
 *
 * Pseudo-algorithm for this transposition:
 *  ROW_M: out([m1][n][h1] + [m2][n][h2]) <-- in[n][m1*h1 + m2*h2]
 *  COL_M: out[n][m1*h1 + m2*h2] <-- in([m1][n][h1] + [m2][n][h2])
 */
static void transpose(real_t *in, real_t *out, dir_t dir,
        size_t n, size_t m1, size_t m2, size_t h1, size_t h2)
{
    size_t ldR  = m1 * h1 + m2 * h2;
    size_t ldC1 = h1 * n;
    size_t ldC2 = h2 * n;

    if (dir == ROW_MAJOR) {
        real_t *in1  = in,  *in2  = in  + m1 * h1;
        real_t *out1 = out, *out2 = out + m1 * h1 * n;
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < m1; ++j)
                copy_block(&out1[j*ldC1 + i*h1], &in1[i*ldR + j*h1], h1);
            for (size_t j = 0; j < m2; ++j)
                copy_block(&out2[j*ldC2 + i*h2], &in2[i*ldR + j*h2], h2);
        }
    } else {
        real_t *in1  = in,  *in2  = in  + m1 * h1 * n;
        real_t *out1 = out, *out2 = out + m1 * h1;
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < m1; ++j)
                copy_block(&out1[i*ldR + j*h1], &in1[j*ldC1 + i*h1], h1);
            for (size_t j = 0; j < m2; ++j)
                copy_block(&out2[i*ldR + j*h2], &in2[j*ldC2 + i*h2], h2);
        }
    }
}

/**
 * @brief transposition execution code
 *
 * @param plan[in] plan structure
 */
static void ft_execute(fftw_mkl_plan plan)
{
    ft_t *ft = (ft_t*)plan->mpi_plan;
    ptrdiff_t h = ft->howmany;

    real_t *buf[4];
    get_buffers(plan, buf);

    /* stage 0:
     * !tr_in: [P0][ln0][ P1][ln1][h] --> [P0][P1][ln0][ln1][h]
     *  tr_in: [P0][ P1][ln1][ln0][h] --> [P0][P1][ln0][ln1][h]
     */
    if (!ft->tr_in) {
        transpose(buf[0], buf[1], ROW_MAJOR, ft->local_n0,
                ft->p[1][0], ft->p[1][1], ft->ln[1][0]*h, ft->ln[1][1]*h);
    } else {
        for (int p = 0; p < ft->p[1][0]; ++p) {
            size_t off = p * ft->local_n0 * ft->ln[1][0] * h;
            transpose(buf[0] + off, buf[1] + off, ROW_MAJOR,
                    ft->ln[1][0], ft->local_n0, 0, h, 0);
        }
        size_t main_off = ft->p[1][0] * ft->local_n0 * ft->ln[1][0] * h;
        real_t *buf0 = buf[0] + main_off;
        real_t *buf1 = buf[1] + main_off;
        for (int p = 0; p < ft->p[1][1]; ++p) {
            size_t off = p * ft->local_n0 * ft->ln[1][1] * h;
            transpose(buf0 + off, buf1 + off, ROW_MAJOR,
                    ft->ln[1][1], ft->local_n0, 0, h, h);
        }
    }

    /* stage 1:
     *  A2A: [P0][P1][ln0][ln1][h] --> [P1][P0][ln0][ln1][h]
     */
    MPI_Alltoallv(buf[1], ft->scounts, ft->sdispls, MPI_PREC,
            buf[2], ft->rcounts, ft->rdispls, MPI_PREC, ft->comm);

    /* stage 2 :
     * !tr_out: [P1][P0][ln0][ln1][h] --> [P1][ln1][P0][ln0][h]
     */
    if (ft->tr_out) return;
    transpose(buf[2], buf[3], COL_MAJOR, ft->local_n1,
            ft->p[0][0]*ft->ln[0][0], ft->p[0][1]*ft->ln[0][1], h, h);
}

/**
 * @brief create transposition plan for specified parameters
 *
 * @param n0[in] size of the first dimension
 * @param n1[in] size of the second dimension
 * @param howmany[in] size of each element in real numbers
 * @param block0[in] the amount of desirable work across first dimension
 * @param block1[in] the amount of desirable work across second dimension
 * @param inplace[in] flag for inplace transform (0 stands for out-of-place)
 * @param comm[in] MPI communicator
 * @param flags[in] FFTW3 MPI flags
 * @param ft[out] created transposition plan
 */
static int ft_create(ptrdiff_t n0, ptrdiff_t n1, ptrdiff_t howmany,
        ptrdiff_t block0, ptrdiff_t block1,
        int inplace, MPI_Comm comm, unsigned flags, ft_t *ft)
{
    ft_t ft_zero = {MPI_TRANSPOSE_MARK, 0};
    *ft = ft_zero;

    ptrdiff_t n[2] = { n0, n1 };
    ptrdiff_t dummy;
    size_t local_size = FFTW_MPI_MANGLE(local_size_many_transposed)(2, n,
            howmany, block0, block1, comm,
            &ft->local_n0, &dummy, &ft->local_n1, &dummy);

    int irank, nrank;
    MPI_Comm_size(comm, &nrank);
    MPI_Comm_rank(comm, &irank);

    work_distribution(n0, nrank, block0, ft->ln[0], ft->p[0]);
    work_distribution(n1, nrank, block1, ft->ln[1], ft->p[1]);

    ft->comm = comm;
    ft->local_size = local_size;
    ft->howmany = howmany;
    ptrdiff_t h = howmany;

    /* set buffers */
    ft->tr_in  = !!(flags & FFTW_MPI_TRANSPOSED_IN);
    ft->tr_out = !!(flags & FFTW_MPI_TRANSPOSED_OUT);

    size_t nbuf = (inplace && !ft->tr_out) ? 2 : 1;
    ft->allocated_buf = fftw_malloc(sizeof(real_t)*local_size*nbuf);
    if (ft->allocated_buf == NULL) goto bail_out;

    /* set MPI parameters for A2A */
    ft->mpi_buf = fftw_malloc(sizeof(int)*nrank*4);
    if (ft->mpi_buf == NULL) goto bail_out;
    ft->scounts = ft->mpi_buf + 0*nrank;
    ft->sdispls = ft->mpi_buf + 1*nrank;
    ft->rcounts = ft->mpi_buf + 2*nrank;
    ft->rdispls = ft->mpi_buf + 3*nrank;

    ft->scounts[0] = ft->local_n0*ft->ln[1][0]*h;
    ft->sdispls[0] = 0;
    for (int p = 1; p < ft->p[1][0] + ft->p[1][1]; ++p) {
        ft->scounts[p] = ft->local_n0*ft->ln[1][p < ft->p[1][0] ? 0 : 1]*h;
        ft->sdispls[p] = ft->sdispls[p-1] + ft->scounts[p-1];
    }
    for (int p = ft->p[1][0] + ft->p[1][1]; p < nrank; ++p) {
        ft->scounts[p] = ft->sdispls[p] = 0;
    }

    ft->rcounts[0] = ft->local_n1*ft->ln[0][0]*h;
    ft->rdispls[0] = 0;
    for (int p = 1; p < ft->p[0][0] + ft->p[0][1]; ++p) {
        ft->rcounts[p] = ft->local_n1*ft->ln[0][p < ft->p[0][0] ? 0 : 1]*h;
        ft->rdispls[p] = ft->rdispls[p-1] + ft->rcounts[p-1];
    }
    for (int p = ft->p[0][0] + ft->p[0][1]; p < nrank; ++p) {
        ft->rcounts[p] = ft->rdispls[p] = 0;
    }

    return 1; /* success */
bail_out:
    if (ft->allocated_buf) fftw_free(ft->allocated_buf);
    if (ft->mpi_buf) fftw_free(ft->mpi_buf);
    return 0;
}

/**
 * @brief transposition plan destruction
 *
 * @param plan[in,out] plan structure
 */
static void ft_destroy(fftw_mkl_plan plan)
{
    if (plan) {
        ft_t *ft = (ft_t*)plan->mpi_plan;
        if (ft != NULL) {
            if (ft->allocated_buf) fftw_free(ft->allocated_buf);
            if (ft->mpi_buf) fftw_free(ft->mpi_buf);
            fftw_free(ft);
        }
        fftw_free(plan);
    }
}
