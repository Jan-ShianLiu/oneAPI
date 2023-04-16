/*  cilk.h                  -*-C++-*-
 *
 *  Copyright (C) 2010-2018 Intel Corporation.  All Rights Reserved.
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
 */
 
/** @file cilk.h
 *
 *  @brief Provides convenient aliases for Intel(R) Cilk(TM) language keywords.
 *
 *  @details
 *  Since Intel Cilk Plus is a nonstandard extension to both C and C++, the Intel
 *  Cilk language keywords all begin with "`_Cilk_`", which guarantees that they
 *  will not conflict with user-defined identifiers in properly written 
 *  programs. This way, a Cilk-enabled C or C++ compiler can safely compile 
 *  "standard" C and C++ programs.
 *
 *  However, this means that the keywords _look_ like something grafted on to
 *  the base language. Therefore, you can include this header:
 *
 *      #include "cilk/cilk.h"
 *
 *  and then write the Intel Cilk keywords with a "`cilk_`" prefix instead of
 *  "`_Cilk_`".
 *
 *  @ingroup language
 */
 
 
/** @defgroup language Language Keywords
 *  Definitions for the Intel Cilk language.
 *  @{
 */
 
#ifndef cilk_spawn
# define cilk_spawn _Cilk_spawn ///< Spawn a task that can execute in parallel.
# define cilk_sync  _Cilk_sync  ///< Wait for spawned tasks to complete.
# define cilk_for   _Cilk_for   ///< Execute iterations of a `for` loop in parallel.
#endif

/// @}
