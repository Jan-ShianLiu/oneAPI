/*
 *  Copyright (C) 2009-2018 Intel Corporation.  All Rights Reserved.
 *
 *  The source code contained or described herein and all documents related
 *  to the source code ("Material") are owned by Intel Corporation or its
 *  suppliers or licensors.  Title to the Material remains with Intel
 *  Corporation or its suppliers and licensors.  The Material is protected
 *  by worldwide copyright laws and treaty provisions.  No part of the
 *  Material may be used, copied, reproduced, modified, published, uploaded,
 *  posted, transmitted, distributed, or disclosed in any way without
 *  Intel's prior express written permission.
 *
 *  No license under any patent, copyright, trade secret or other
 *  intellectual property right is granted to or conferred upon you by
 *  disclosure or delivery of the Materials, either expressly, by
 *  implication, inducement, estoppel or otherwise.  Any license under such
 *  intellectual property rights must be express and approved by Intel in
 *  writing.
 *
 */

#ifndef INCLUDED_CILK_HYPEROBJECT_BASE
#define INCLUDED_CILK_HYPEROBJECT_BASE

#ifdef __cplusplus
# include <cstdlib>
# include <cstddef>
#else
# include <stdlib.h>
# include <stddef.h>
#endif

#include <cilk/common.h>

#if defined _WIN32 || defined _WIN64
# if !defined CILK_STUB && !defined IN_CILK_RUNTIME
    /* bring in the Intel(R) Cilk(TM) Plus library, which has definitions for some of these
     * functions. */
#   pragma comment(lib, "cilkrts")
# endif
#endif

/* The __CILKRTS_STRAND_PURE attribute tells the compiler that the value
 * returned by 'func' for a given argument to 'func' will remain valid until
 * the next strand boundary (spawn or sync) or until the next call to a
 * function with the __CILKRTS_STRAND_STALE attribute using the same function
 * argument.
 */
#if 0 && defined __cilk && (defined __GNUC__ && !defined _WIN32) && defined __cilkartsrev
# define __CILKRTS_STRAND_PURE(func) \
    func __attribute__((__cilk_hyper__("lookup")))
# define __CILKRTS_STRAND_STALE(func) \
    func __attribute__((__cilk_hyper__("flush")))
#else
# define __CILKRTS_STRAND_PURE(func) func
# define __CILKRTS_STRAND_STALE(func) func
#endif

/*****************************************************************************
 * C runtime interface to the hyperobject subsystem
 *****************************************************************************/

__CILKRTS_BEGIN_EXTERN_C

/* Callback function signatures.  The 'r' argument always points to the
 * reducer itself and is commonly ignored. */
typedef void (*cilk_c_reducer_reduce_fn_t)(void* r, void* lhs, void* rhs);
typedef void (*cilk_c_reducer_identity_fn_t)(void* r, void* view);
typedef void (*cilk_c_reducer_destroy_fn_t)(void* r, void* view);
typedef void* (*cilk_c_reducer_allocate_fn_t)(void* r, __STDNS size_t bytes);
typedef void (*cilk_c_reducer_deallocate_fn_t)(void* r, void* view);

/** Representation of the monoid */
typedef struct cilk_c_monoid {
    cilk_c_reducer_reduce_fn_t          reduce_fn;
    cilk_c_reducer_identity_fn_t        identity_fn;
    cilk_c_reducer_destroy_fn_t         destroy_fn;
    cilk_c_reducer_allocate_fn_t        allocate_fn;
    cilk_c_reducer_deallocate_fn_t      deallocate_fn;
} cilk_c_monoid;

/** Base of the hyperobject */
typedef struct __cilkrts_hyperobject_base
{
    cilk_c_monoid       __c_monoid;
    unsigned long long  __flags;
    __STDNS ptrdiff_t   __view_offset;  /* offset (in bytes) to leftmost view */
    __STDNS size_t      __view_size;    /* Size of each view */
} __cilkrts_hyperobject_base;


#ifndef CILK_STUB

/* Library functions. */
CILK_EXPORT
    void __cilkrts_hyper_create(__cilkrts_hyperobject_base *key);
CILK_EXPORT void __CILKRTS_STRAND_STALE(
    __cilkrts_hyper_destroy(__cilkrts_hyperobject_base *key));
CILK_EXPORT void* __CILKRTS_STRAND_PURE(
    __cilkrts_hyper_lookup(__cilkrts_hyperobject_base *key));

CILK_EXPORT
    void* __cilkrts_hyperobject_alloc(void* ignore, __STDNS size_t bytes);
CILK_EXPORT
    void __cilkrts_hyperobject_dealloc(void* ignore, void* view);

/* No-op destroy function */
CILK_EXPORT
    void __cilkrts_hyperobject_noop_destroy(void* ignore, void* ignore2);


#else // CILK_STUB

// Programs compiled with CILK_STUB are not linked with the Intel Cilk Plus runtime 
// library, so they should not have external references to cilkrts functions.
// Furthermore, they don't need the hyperobject functionality, so the
// functions can be stubbed.

#define __cilkrts_hyperobject_create __cilkrts_hyperobject_create__stub
__CILKRTS_INLINE
    void __cilkrts_hyper_create(__cilkrts_hyperobject_base *key) 
    {}

#define __cilkrts_hyperobject_destroy __cilkrts_hyperobject_destroy__stub
__CILKRTS_INLINE
    void __cilkrts_hyper_destroy(__cilkrts_hyperobject_base *key) 
    {}

#define __cilkrts_hyperobject_lookup __cilkrts_hyperobject_lookup__stub
__CILKRTS_INLINE
    void* __cilkrts_hyper_lookup(__cilkrts_hyperobject_base *key)
    { return (char*)(key) + key->__view_offset; }

// Pointers to these functions are stored into monoids, so real functions
// are needed.

#define __cilkrts_hyperobject_alloc __cilkrts_hyperobject_alloc__stub
__CILKRTS_INLINE
    void* __cilkrts_hyperobject_alloc(void* ignore, __STDNS size_t bytes)
    { assert(0); return __STDNS malloc(bytes); }

#define __cilkrts_hyperobject_dealloc __cilkrts_hyperobject_dealloc__stub
__CILKRTS_INLINE
    void __cilkrts_hyperobject_dealloc(void* ignore, void* view)
    { assert(0); __STDNS free(view); }

#define __cilkrts_hyperobject_noop_destroy \
            __cilkrts_hyperobject_noop_destroy__stub
__CILKRTS_INLINE
    void __cilkrts_hyperobject_noop_destroy(void* ignore, void* ignore2)
    {}
    
#endif

__CILKRTS_END_EXTERN_C

#endif /* INCLUDED_CILK_HYPEROBJECT_BASE */
