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
 *
 * fftwnd_create_plan_specific - FFTW2 wrapper to Intel(R) Math Kernel Library (Intel(R) MKL).
 *
 ******************************************************************************
 */

#include "fftw2_mkl.h"

fftwnd_plan
fftwnd_create_plan_specific(int rank, const int *n, fftw_direction dir,
                            int flags, fftw_complex *in, int istride,
                            fftw_complex *out, int ostride)
{
    MKL_LONG err;
    fftw_plan_mkl *plan = NULL;
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
        plan->n[i] = (MKL_LONG)n[i];
    }

    if (rank == 1)
    {
        err = DftiCreateDescriptor(&(plan->mkl_desc), MKL_PRECISION,
                 DFTI_COMPLEX, rank, plan->n[0]);
    }
    else
    {
        err = DftiCreateDescriptor(&(plan->mkl_desc), MKL_PRECISION,
                 DFTI_COMPLEX, rank, plan->n);
    }
    if (err) goto cannot_commit;

    if (!(plan->inplace))
    {
        err = DftiSetValue(plan->mkl_desc, DFTI_PLACEMENT, DFTI_NOT_INPLACE);
        if (err) goto cannot_commit;
    }
    
    /* Set input strides per row-major format */
    {
        MKL_LONG strides[1+DFTI_MAX_RANK];
        
        strides[0] = 0;
        strides[rank] = istride;
        for (i = rank - 1; i > 0; i--)
        {
            strides[i] = plan->n[i] * strides[i + 1];
        }

        err = DftiSetValue(plan->mkl_desc, DFTI_INPUT_STRIDES, strides);
        if (err) goto cannot_commit;
    }

    /* Set output strides per row-major format */
    if (!(plan->inplace))
    {
        MKL_LONG strides[1+DFTI_MAX_RANK];
        
        strides[0] = 0;
        strides[rank] = ostride;
        for (i = rank - 1; i > 0; i--)
        {
            strides[i] = plan->n[i] * strides[i + 1];
        }

        err = DftiSetValue(plan->mkl_desc, DFTI_OUTPUT_STRIDES, strides);
        if (err) goto cannot_commit;
    }


    err = DftiCommitDescriptor(plan->mkl_desc);
    if (err) goto cannot_commit;

    mkl_memory_layout_init(plan);

    return (fftwnd_plan)plan;

cannot_commit:
    fftwnd_destroy_plan( (fftwnd_plan)plan );
    return NULL;
}

#define CHECK_DFTI_CALL(dfti_call, error_handler) do {       \
        if ((dfti_call) != DFTI_NO_ERROR) { error_handler; } \
    } while (0)

/// Naive mutex lock
static void naive_mutex_lock(volatile unsigned int *lock);

/// Naive mutex unlock
static void naive_mutex_unlock(volatile unsigned int *lock);

/// Match memory layout
static int mkl_memory_layout_match(const mkl_memory_layout *A, const mkl_memory_layout *B, int inplace)
{
    return A->istride == B->istride && A->idist == B->idist &&
        (inplace || (A->ostride == B->ostride && A->odist == B->odist)) &&
        A->howmany == B->howmany && A->nthreads == B->nthreads;
}

DFTI_DESCRIPTOR_HANDLE mkl_memory_layout_get(fftw_plan_mkl *P, const mkl_memory_layout *L)
{
    int i;
    DFTI_DESCRIPTOR_HANDLE H = NULL;
    enum DFTI_CONFIG_VALUE precision, domain;
    mkl_memory_layout plan_layout = {P->istride, P->idist, P->ostride, P->odist, P->howmany, P->nthreads};

    if (mkl_memory_layout_match(L, &plan_layout, P->inplace)) return P->mkl_desc;
    for (i = 0; i < MKL_MEMORY_LAYOUT_CACHE_MAX; ++i)
    {
        if (P->cache[i].mkl_desc == NULL)
            break;
        if (mkl_memory_layout_match(L, &P->cache[i].layout, P->inplace))
            return P->cache[i].mkl_desc;
    }
    // Create descriptor like P->mkl_desc
    CHECK_DFTI_CALL(DftiGetValue(P->mkl_desc, DFTI_PRECISION     , &precision), goto cannot_create_descriptor);
    CHECK_DFTI_CALL(DftiGetValue(P->mkl_desc, DFTI_FORWARD_DOMAIN, &domain   ), goto cannot_create_descriptor);
    if (P->rank == 1)
        CHECK_DFTI_CALL(DftiCreateDescriptor(&H, precision, domain, 1      , P->n[0]), goto cannot_create_descriptor);
    else
        CHECK_DFTI_CALL(DftiCreateDescriptor(&H, precision, domain, P->rank, P->n   ), goto cannot_create_descriptor);

    if (domain == DFTI_REAL)
        CHECK_DFTI_CALL(DftiSetValue(H, DFTI_CONJUGATE_EVEN_STORAGE, DFTI_COMPLEX_COMPLEX), goto cannot_create_descriptor);
    if (!P->inplace)
        CHECK_DFTI_CALL(DftiSetValue(H, DFTI_PLACEMENT             , DFTI_NOT_INPLACE    ), goto cannot_create_descriptor);

    {
        // Set strides and distances
        MKL_LONG is[DFTI_MAX_RANK+1] = {0}, os[DFTI_MAX_RANK+1] = {0}, idist = 0, odist = 0;
        if (domain == DFTI_COMPLEX)
        {
            is[P->rank] = L->istride;
            os[P->rank] = P->inplace ? L->istride : L->ostride;
            for (i = P->rank-1; i > 0; --i)
            {
                is[i] = is[i+1]*P->n[i];
                os[i] = os[i+1]*P->n[i];
            }
            idist = L->idist;
            odist = P->inplace ? L->idist : L->odist;
        }
        else
        {
            MKL_LONG nd2 = P->n[P->rank-1]/2 + 1;
            MKL_LONG ic  = P->sign == FFTW_FORWARD ? (P->inplace ? 2*nd2 : 0) : nd2;
            MKL_LONG oc  = P->sign == FFTW_FORWARD ?                      nd2 : (P->inplace ? 2*nd2 : 0);
            is[P->rank] = L->istride;
            os[P->rank] = P->inplace ? L->istride : L->ostride;
            for (i = P->rank - 1; i > 0; --i)
            {
                is[i] = is[i+1] * (ic != 0 ? ic : P->n[i]);
                os[i] = os[i+1] * (oc != 0 ? oc : P->n[i]);
                ic = 0;
                oc = 0;
            }
            if (L->howmany > 1)
            {
                idist = L->idist;
                if (P->inplace)
                    odist = (idist == 1 && idist < L->istride) ? 1 : (P->sign == FFTW_FORWARD ? idist/2 : idist*2);
                else
                    odist = L->odist;
            }
        }
        CHECK_DFTI_CALL(DftiSetValue(H, DFTI_INPUT_STRIDES  , is   ), goto cannot_create_descriptor);
        CHECK_DFTI_CALL(DftiSetValue(H, DFTI_OUTPUT_STRIDES , os   ), goto cannot_create_descriptor);
        CHECK_DFTI_CALL(DftiSetValue(H, DFTI_INPUT_DISTANCE , idist), goto cannot_create_descriptor);
        CHECK_DFTI_CALL(DftiSetValue(H, DFTI_OUTPUT_DISTANCE, odist), goto cannot_create_descriptor);
    }
    CHECK_DFTI_CALL(DftiSetValue(H, DFTI_NUMBER_OF_TRANSFORMS, L->howmany ), goto cannot_create_descriptor);
    CHECK_DFTI_CALL(DftiSetValue(H, DFTI_THREAD_LIMIT        , L->nthreads), goto cannot_create_descriptor);

    CHECK_DFTI_CALL(DftiCommitDescriptor(H), goto cannot_create_descriptor);

    naive_mutex_lock(&P->cache_lock);
    for (i = 0; i < MKL_MEMORY_LAYOUT_CACHE_MAX; ++i)
        if (mkl_memory_layout_match(&P->cache[i].layout, L, P->inplace))
        {
            DftiFreeDescriptor(&H);
            H = P->cache[i].mkl_desc;
            break;
        }
        else if (P->cache[i].mkl_desc == NULL)
        {
            P->cache[i].layout = L[0];
            P->cache[i].mkl_desc = H;
            break;
        }
    naive_mutex_unlock(&P->cache_lock);
    return H;
cannot_create_descriptor:
    if (H != NULL) DftiFreeDescriptor(&H);
    return NULL;
}

void mkl_memory_layout_recycle(fftw_plan_mkl *P, DFTI_DESCRIPTOR_HANDLE H)
{
    int i;
    if (P->mkl_desc == H) return;
    for (i = 0; i < MKL_MEMORY_LAYOUT_CACHE_MAX; ++i)
        if (P->cache[i].mkl_desc == NULL)
        {
            DftiFreeDescriptor(&H);
            return;
        }
        else if (P->cache[i].mkl_desc == H)
            return;
}

void mkl_memory_layout_init(fftw_plan_mkl *P)
{
    int i;
    for (i = 0; i < MKL_MEMORY_LAYOUT_CACHE_MAX; ++i)
    {
        mkl_memory_layout zero = {0};
        P->cache[i].layout = zero;
        P->cache[i].mkl_desc = NULL;
    }
    P->cache_lock = 0;
}

void mkl_memory_layout_free(fftw_plan_mkl *P)
{
    int i;
    for (i = 0; i < MKL_MEMORY_LAYOUT_CACHE_MAX; ++i)
    {
        mkl_memory_layout zero = {0};
        P->cache[i].layout = zero;
        if (P->cache[i].mkl_desc != NULL)
        {
            DftiFreeDescriptor(&P->cache[i].mkl_desc);
            P->cache[i].mkl_desc = NULL;
        }
    }
}

/// Atomic compare-and-swap
static int naive_atomic_cas(volatile unsigned int *x, int c, int y)
{
#ifdef _WIN32
    extern long _InterlockedCompareExchange(long volatile *, long, long);
    return _InterlockedCompareExchange(x, y, c);
#elif defined __APPLE__
    #ifdef DEPRECATED_OSAtomic
        return atomic_compare_exchange_strong_explicit((volatile atomic_int*)x, &c, y, memory_order_relaxed, memory_order_relaxed) ? c : y;
    #else
        return OSAtomicCompareAndSwapInt(c, y, (volatile int*)x) ? c : y;
    #endif
#elif defined __PGI
    // Linux, PGI compiler
    return __sync_val_compare_and_swap_4(x, c, y);
#else
    // Linux, regular compiler
    return __sync_val_compare_and_swap(x, c, y);
#endif
}

static void naive_mutex_lock(volatile unsigned int *lock)
{
    while ( naive_atomic_cas(lock, 0, 1) != 0 ) ;
}
static void naive_mutex_unlock(volatile unsigned int *lock)
{
    *lock = 0;
}

