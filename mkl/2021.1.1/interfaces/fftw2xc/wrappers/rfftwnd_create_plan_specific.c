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
 * rfftwnd_create_plan_specific - FFTW2 wrapper to Intel(R) Math Kernel Library (Intel(R) MKL).
 *
 ******************************************************************************
 */

#include "fftw2_mkl.h"

rfftwnd_plan
rfftwnd_create_plan_specific(int rank, const int *n,
						fftw_direction dir, int flags,
						fftw_real *in, int istride,
						fftw_real *out, int ostride)
{
    MKL_LONG err;
    MKL_LONG istrides[1 + DFTI_MAX_RANK];
    MKL_LONG ostrides[1 + DFTI_MAX_RANK];
    fftw_plan_mkl *plan = NULL;
    int nd2; /* used to keep physical size of the last dimension */
    int i;

    UNUSED(in);
    UNUSED(out);

    if (n == NULL) return NULL;

    plan = (fftw_plan_mkl *)fftw_malloc(sizeof(fftw_plan_mkl));
    if (plan == NULL)
        return NULL;

    plan->howmany = 1;
    plan->idist = 0;
    plan->inplace = flags & FFTW_IN_PLACE;
    plan->istride = istride;
    plan->mkl_desc = NULL;
    plan->n = NULL;
    plan->nthreads = MKL_Domain_Get_Max_Threads(MKL_DOMAIN_FFT);
    plan->odist = 0;
    plan->ostride = ostride;
    plan->rank = rank;
    plan->readonly = flags & FFTW_THREADSAFE;
    plan->sign = dir;

    if (rank > DFTI_MAX_RANK)
    {
        err = DFTI_UNIMPLEMENTED;
        goto cannot_commit;
    }

    plan->n = (MKL_LONG *)fftw_malloc(rank * sizeof(MKL_LONG));
    if (plan->n == NULL)
        goto cannot_commit;

    for (i = 0; i < rank; i++)
    {
        plan->n[i] = n[i];
    }

    if (rank == 1)
    {
        err = DftiCreateDescriptor(&(plan->mkl_desc), MKL_PRECISION,
                 DFTI_REAL, rank, plan->n[0]);
    }
    else
    {
        err = DftiCreateDescriptor(&(plan->mkl_desc), MKL_PRECISION,
                 DFTI_REAL, rank, plan->n);
    }
    if (err) goto cannot_commit;

    if (!(plan->inplace))
    {
        err = DftiSetValue(plan->mkl_desc, DFTI_PLACEMENT, DFTI_NOT_INPLACE);
        if (err) goto cannot_commit;
    }

    /* rfftwnd* computes real to complex-conjugate-even */
    err = DftiSetValue(plan->mkl_desc, DFTI_CONJUGATE_EVEN_STORAGE,
            DFTI_COMPLEX_COMPLEX);
    if (err) goto cannot_commit;

    /* Set strides per row-major format */
    istrides[0] = 0;
    ostrides[0] = 0;
    nd2 = plan->n[rank-1]/2 + 1;
    if (dir == FFTW_FORWARD)
    {
        if (plan->inplace)
        {
            istrides[rank] = istride;
            ostrides[rank] = istride;

            for (i = rank - 1; i > 0; --i)
            {
                istrides[i] = istrides[i+1] * (nd2 ? 2*nd2 : plan->n[i]);
                ostrides[i] = ostrides[i+1] * (nd2 ?   nd2 : plan->n[i]);
                nd2 = 0;
            }
        }
        else /* real to complex out of place */
        {
            istrides[rank] = istride;
            ostrides[rank] = ostride;

            for (i = rank - 1; i > 0; --i)
            {
                istrides[i] = istrides[i+1] * plan->n[i];
                ostrides[i] = ostrides[i+1] * (nd2 ?   nd2 : plan->n[i]);
                nd2 = 0;
            }
        }
    }
    if (dir == FFTW_BACKWARD)
    {
        if (plan->inplace)
        {
            istrides[rank] = istride;
            ostrides[rank] = istride;

            for (i = rank - 1; i > 0; --i)
            {
                istrides[i] = istrides[i+1] * (nd2 ?   nd2 : plan->n[i]);
                ostrides[i] = ostrides[i+1] * (nd2 ? 2*nd2 : plan->n[i]);
                nd2 = 0;
            }
        }
        else /* complex to real out of place */
        {
            istrides[rank] = istride;
            ostrides[rank] = ostride;

            for (i = rank - 1; i > 0; --i)
            {
                istrides[i] = istrides[i+1] * (nd2 ?   nd2 : plan->n[i]);
                ostrides[i] = ostrides[i+1] * plan->n[i];
                nd2 = 0;
            }
        }
    }
    err = DftiSetValue(plan->mkl_desc,DFTI_INPUT_STRIDES,istrides);
    if (err) goto cannot_commit;
    err = DftiSetValue(plan->mkl_desc,DFTI_OUTPUT_STRIDES,ostrides);
    if (err) goto cannot_commit;

    err = DftiCommitDescriptor(plan->mkl_desc);
    if (err) goto cannot_commit;

    mkl_memory_layout_init(plan);

    return (rfftwnd_plan)plan;

cannot_commit:
    rfftwnd_destroy_plan( (rfftwnd_plan)plan );
    return NULL;
}
