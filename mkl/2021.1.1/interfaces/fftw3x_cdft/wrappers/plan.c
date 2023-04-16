/*******************************************************************************
* Copyright 2010-2020 Intel Corporation.
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

#include "fftw3-mpi_mkl.h"
#include "mpi_cfft.h"

/* ================ prototypes ================ */

static FFTW_MANGLE(plan) create_plan(int rnk, const ptrdiff_t *n,
        complex_t *in, complex_t *out,
        MPI_Comm comm, int sign, unsigned flags, type_t type);
static void execute_fwd(fftw_mkl_plan p);
static void execute_bwd(fftw_mkl_plan p);
static void execute_trans_fwd(fftw_mkl_plan p);
static void execute_trans_bwd(fftw_mkl_plan p);
static void fftw_mpi_delete_plan(fftw_mkl_plan p);

static void local_transpose(cfft_t *cfft, complex_t *src, complex_t *dst);

/* ================  wrappers  ================ */

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_many_dft)
     (int rnk, const ptrdiff_t *n, ptrdiff_t howmany,
      ptrdiff_t block, ptrdiff_t tblock, complex_t *in, complex_t *out,
      MPI_Comm comm, int sign, unsigned flags)
{
    return 0; /* not implemented */
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft)
     (int rnk, const ptrdiff_t *n, complex_t *in, complex_t *out,
      MPI_Comm comm, int sign, unsigned flags)
{
    return create_plan(rnk, n, in, out, comm, sign, flags, COMPLEX);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_1d)
     (ptrdiff_t n0, complex_t *in, complex_t *out,
      MPI_Comm comm, int sign, unsigned flags)
{
    const ptrdiff_t n[1] = {n0};
    return FFTW_MPI_MANGLE(plan_dft)(1, n, in, out, comm, sign, flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_2d)
     (ptrdiff_t n0, ptrdiff_t n1, complex_t *in, complex_t *out,
      MPI_Comm comm, int sign, unsigned flags)
{
    const ptrdiff_t n[2] = {n0, n1};
    return FFTW_MPI_MANGLE(plan_dft)(2, n, in, out, comm, sign, flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_3d)
     (ptrdiff_t n0, ptrdiff_t n1, ptrdiff_t n2, complex_t *in, complex_t *out,
      MPI_Comm comm, int sign, unsigned flags)
{
    const ptrdiff_t n[3] = {n0, n1, n2};
    return FFTW_MPI_MANGLE(plan_dft)(3, n, in, out, comm, sign, flags);
}


FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_many_r2r)
     (int rnk, const ptrdiff_t *n, ptrdiff_t howmany,
      ptrdiff_t iblock, ptrdiff_t oblock, real_t *in, real_t *out,
      MPI_Comm comm, const FFTW_MANGLE(r2r_kind) *kind, unsigned flags)
{
    return 0; /* not implemented */
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_r2r)
     (int rnk, const ptrdiff_t *n, real_t *in, real_t *out,
      MPI_Comm comm, const FFTW_MANGLE(r2r_kind) *kind, unsigned flags)
{
    return 0; /* not implemented */
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_r2r_2d)
     (ptrdiff_t n0, ptrdiff_t n1, real_t *in, real_t *out, MPI_Comm comm,
      FFTW_MANGLE(r2r_kind) kind0, FFTW_MANGLE(r2r_kind) kind1, unsigned flags)
{
    return 0; /* not implemented */
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_r2r_3d)
     (ptrdiff_t n0, ptrdiff_t n1, ptrdiff_t n2,
      real_t *in, real_t *out, MPI_Comm comm, FFTW_MANGLE(r2r_kind) kind0,
      FFTW_MANGLE(r2r_kind) kind1, FFTW_MANGLE(r2r_kind) kind2, unsigned flags)
{
    return 0; /* not implemented */
}


FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_many_dft_r2c)
     (int rnk, const ptrdiff_t *n, ptrdiff_t howmany,
      ptrdiff_t iblock, ptrdiff_t oblock, real_t *in, complex_t *out,
      MPI_Comm comm, unsigned flags)
{
    return 0; /* not implemented */
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_r2c)
     (int rnk, const ptrdiff_t *n, real_t *in, complex_t *out,
      MPI_Comm comm, unsigned flags)
{
    return create_plan(rnk, n, (complex_t*)in, out,
            comm, FFTW_FORWARD, flags, REAL);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_r2c_2d)
     (ptrdiff_t n0, ptrdiff_t n1, real_t *in, complex_t *out,
      MPI_Comm comm, unsigned flags)
{
    const ptrdiff_t n[2] = {n0, n1};
    return FFTW_MPI_MANGLE(plan_dft_r2c)(2, n, in, out, comm, flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_r2c_3d)
     (ptrdiff_t n0, ptrdiff_t n1, ptrdiff_t n2, real_t *in, complex_t *out,
      MPI_Comm comm, unsigned flags)
{
    const ptrdiff_t n[3] = {n0, n1, n2};
    return FFTW_MPI_MANGLE(plan_dft_r2c)(3, n, in, out, comm, flags);
}


FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_many_dft_c2r)
     (int rnk, const ptrdiff_t *n, ptrdiff_t howmany,
      ptrdiff_t iblock, ptrdiff_t oblock, complex_t *in, real_t *out,
      MPI_Comm comm, unsigned flags)
{
    return 0; /* not implemented */
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_c2r)
     (int rnk, const ptrdiff_t *n, complex_t *in, real_t *out,
      MPI_Comm comm, unsigned flags)
{
    return create_plan(rnk, n, in, (complex_t*)out,
            comm, FFTW_BACKWARD, flags, REAL);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_c2r_2d)
     (ptrdiff_t n0, ptrdiff_t n1, complex_t *in, real_t *out,
      MPI_Comm comm, unsigned flags)
{
    const ptrdiff_t n[2] = {n0, n1};
    return FFTW_MPI_MANGLE(plan_dft_c2r)(2, n, in, out, comm, flags);
}

FFTW_EXTERN FFTW_MANGLE(plan) FFTW_MPI_MANGLE(plan_dft_c2r_3d)
     (ptrdiff_t n0, ptrdiff_t n1, ptrdiff_t n2, complex_t *in, real_t *out,
      MPI_Comm comm, unsigned flags)
{
    const ptrdiff_t n[3] = {n0, n1, n2};
    return FFTW_MPI_MANGLE(plan_dft_c2r)(3, n, in, out, comm, flags);
}

/* ================ implementation ================ */

/**
 * @brief create cluster FFT plan for specified parameters
 *
 * @param rnk[in] problem rnk
 * @param n[in] array of size rnk with array lenghts
 * @param in[in] pointer to input data
 * @param out[in] pointer to output data (out == in for inplace transform)
 * @param comm[in] MPI communicator
 * @param sign[in] sign of transform (FFTW_FORWARD or FFTW_BACKWARD)
 * @param flags[in] FFTW3 MPI flags
 * @param type[in] type of transform (REAL or COMPLEX)
 */
static FFTW_MANGLE(plan) create_plan(int rnk, const ptrdiff_t *n,
        complex_t *in, complex_t *out,
        MPI_Comm comm, int sign, unsigned flags, type_t type)
{
    MKL_LONG dims[MKL_MAXRANK];
    MKL_LONG status;
    ptrdiff_t n_copy[MKL_MAXRANK];
    fftw_mkl_plan p = NULL;

    if (rnk > MKL_MAXRANK) return NULL; /* rank is too high */
    if (n == NULL) return NULL;

    int tr_in  = !!(rnk > 1 && (flags & FFTW_MPI_TRANSPOSED_IN));
    int tr_out = !!(rnk > 1 && (flags & FFTW_MPI_TRANSPOSED_OUT));

    int cdft_inplace; /* to be determined later */

    for (int i = 0; i < rnk; i++)
        n_copy[i] = n[i];

    cfft_t *cfft = (cfft_t*)fftw_malloc(sizeof(*cfft));
    if (NULL == cfft) return NULL;
    cfft_t cfft_zero = {0};
    *cfft = cfft_zero;
    cfft->sign = sign;

    cfft->do_trans = (tr_in + tr_out) == 1;
    if (cfft->do_trans) {
        if ((cfft->sign == FFTW_FORWARD && tr_in) ||
                (cfft->sign == FFTW_BACKWARD && tr_out))
            SWAP(MKL_LONG, n_copy[0], n_copy[1]);

        ptrdiff_t local_size, local_n0, local_n1, local_0_start, local_1_start;
        if (type == REAL) n_copy[rnk-1] = n_copy[rnk-1]/2 + 1;
        local_size = FFTW_MPI_MANGLE(local_size_transposed)(rnk, n_copy, comm,
                &local_n0, &local_0_start, &local_n1, &local_1_start);

        cfft->buf = fftw_malloc(local_size*sizeof(complex_t));
        if (cfft->buf == NULL) goto cleanup;

        cfft->batch = local_n1;
        cfft->n = n_copy[0];
        cfft->codim = 1;
        for (int i = 2; i < rnk; ++i)
            cfft->codim *= n_copy[i];

        n_copy[rnk-1] = n[rnk-1]; /* restore to the original value */

        cdft_inplace = 0;
    } else {
        if (tr_in && tr_out)
            SWAP(ptrdiff_t, n_copy[0], n_copy[1]);
        cdft_inplace = (in == out);

        if (WANT_FAST_INPLACE_CLUSTER_FFT || !cdft_inplace) {
            ptrdiff_t local_size, local_n, local_start;
            local_size = FFTW_MPI_MANGLE(local_size)(rnk, n, comm,
                    &local_n, &local_start);
            cfft->buf = fftw_malloc(local_size*sizeof(complex_t));
            if (!cdft_inplace && cfft->buf == NULL) goto cleanup;
        }
    }

    for (int i = 0; i < rnk; i++)
        dims[i] = n_copy[i];

    if (rnk == 1) {
        status = DftiCreateDescriptorDM(comm, &cfft->h, MKL_PREC,
                (type == COMPLEX) ? DFTI_COMPLEX : DFTI_REAL,
                (MKL_LONG)rnk, dims[0]);
    } else {
        status = DftiCreateDescriptorDM(comm, &cfft->h, MKL_PREC,
                (type == COMPLEX) ? DFTI_COMPLEX : DFTI_REAL,
                (MKL_LONG)rnk, dims);
    }
    if (DFTI_NO_ERROR != status) goto cleanup;
    status = DftiSetValueDM(cfft->h, DFTI_PLACEMENT,
            cdft_inplace ? DFTI_INPLACE : DFTI_NOT_INPLACE);
    status |= DftiSetValueDM(cfft->h, CDFT_WORKSPACE, cfft->buf);
    status |= DftiSetValueDM(cfft->h, DFTI_TRANSPOSE,
            cfft->do_trans ? DFTI_ALLOW : DFTI_NONE);
    status |= DftiCommitDescriptorDM(cfft->h);
    if (DFTI_NO_ERROR != status) goto cleanup;

    p = (fftw_mkl_plan)fftw_malloc(sizeof(*p));
    if (NULL == p) goto cleanup;
    p->io[0] = in;
    p->io[1] = out;
    if (cfft->do_trans) {
        p->execute = (FFTW_FORWARD == sign) ?
            execute_trans_fwd : execute_trans_bwd;
    } else {
        p->execute = (FFTW_FORWARD == sign) ?
            execute_fwd : execute_bwd;
    }
    p->destroy = fftw_mpi_delete_plan;
    p->mpi_plan = (void*)cfft;

    return (FFTW_MANGLE(plan))p;

cleanup:
    if (cfft->buf) fftw_free(cfft->buf);
    if (cfft->h) DftiFreeDescriptorDM(&cfft->h);
    fftw_free(cfft);
    return NULL;
}

/**
 * @brief cluster FFT execution code (forward case)
 *
 * @param plan[in] plan structure
 */
static void execute_fwd(fftw_mkl_plan p)
{
    if (p != NULL && p->mpi_plan != NULL) {
        cfft_t *cfft = (cfft_t*)p->mpi_plan;
        DftiComputeForwardDM(cfft->h, p->io[0], p->io[1]);
    }
}

/**
 * @brief cluster FFT execution code (backward case)
 *
 * @param plan[in] plan structure
 */
static void execute_bwd(fftw_mkl_plan p)
{
    if (p != NULL && p->mpi_plan != NULL) {
        cfft_t *cfft = (cfft_t*)p->mpi_plan;
        DftiComputeBackwardDM(cfft->h, p->io[0], p->io[1]);
    }
}

/**
 * @brief cluster FFT execution code (forward transposed case)
 *
 * @param plan[in] plan structure
 */
static void execute_trans_fwd(fftw_mkl_plan p)
{
    if (p != NULL && p->mpi_plan != NULL) {
        cfft_t *cfft = (cfft_t*)p->mpi_plan;

        if (cfft->buf) {
            DftiSetValueDM(cfft->h, CDFT_WORKSPACE, p->io[1]);  /* replace buffer */
            DftiCommitDescriptorDM(cfft->h);
        }
        DftiComputeForwardDM(cfft->h, p->io[0], cfft->buf);
        if (cfft->buf) {
            DftiSetValueDM(cfft->h, CDFT_WORKSPACE, cfft->buf); /* restore buffer */
            DftiCommitDescriptorDM(cfft->h);
        }
        local_transpose(cfft, cfft->buf, p->io[1]);
    }
}

/**
 * @brief cluster FFT execution code (backward transposed case)
 *
 * @param plan[in] plan structure
 */
static void execute_trans_bwd(fftw_mkl_plan p)
{
    if (p != NULL && p->mpi_plan != NULL) {
        cfft_t *cfft = (cfft_t*)p->mpi_plan;

        local_transpose(cfft, p->io[0], cfft->buf);
        DftiComputeBackwardDM(cfft->h, cfft->buf, p->io[1]);
    }
}

/**
 * @brief cluster FFT plan destruction
 *
 * @param plan[in,out] plan structure
 */
static void fftw_mpi_delete_plan(fftw_mkl_plan p)
{
    if (p && p->mpi_plan != NULL)
    {
        cfft_t *cfft = (cfft_t*)p->mpi_plan;
        if (cfft) {
            if (cfft->h) DftiFreeDescriptorDM(&cfft->h);
            if (cfft->buf) fftw_free(cfft->buf);
            fftw_free(cfft);
        }
        fftw_free(p);
    }
}

/**
 * @brief data movement between Intel MKL CDFT and FFTW3 MPI formats
 *
 * @param cfft[in] cfft plan
 * @param src[in] input array
 * @param dst[out] output array
 *
 * Data movement according to the following chart:
 *      FFTW3             <--> Intel MKL CDFT
 *      n1 x n0 x n2 x n3 <--> n1 x n2 x n3 x n0
 */
static void local_transpose(cfft_t *cfft, complex_t *src, complex_t *dst)
{
    MKL_Complex alpha;
    alpha.real=1.; alpha.imag=0.;

    ptrdiff_t m0 = (cfft->sign == FFTW_FORWARD) ? cfft->codim : cfft->n;
    ptrdiff_t m1 = (cfft->sign == FFTW_FORWARD) ? cfft->n : cfft->codim;

    for (ptrdiff_t b = 0; b < cfft->batch; ++b)
        XOMATCOPY('R', 'T', m0, m1, alpha,
                (MKL_Complex*)src + b*m0*m1, m1,
                (MKL_Complex*)dst + b*m0*m1, m0);
}
