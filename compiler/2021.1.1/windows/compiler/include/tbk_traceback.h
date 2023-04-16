/*
**********************************************************************************
** Copyright (C) 1985-2015 Intel Corporation. All Rights Reserved.               **
** The source code contained or described herein and all documents related to   **
** the source code ("Material") are owned by Intel Corporation or its suppliers **
** or licensors. Title to the Material remains with Intel Corporation or its    **
** suppliers and licensors. The Material contains trade secrets and proprietary **
** and confidential information of Intel or its suppliers and licensors. The    **
** Material is protected by worldwide copyright and trade secret laws and       **
** treaty provisions. No part of the Material may be used, copied, reproduced,  **
** modified, published, uploaded, posted, transmitted, distributed, or          **
** disclosed in any way without Intel's prior express written permission.       **
**                                                                              **
** No license under any patent, copyright, trade secret or other intellectual   **
** property right is granted to or conferred upon you by disclosure or delivery **
** of the Materials, either expressly, by implication, inducement, estoppel or  **
** otherwise. Any license under such intellectual property rights must be       **
** express and approved by Intel in writing.                                    **
**********************************************************************************
*/

#ifndef __TBK_BACKTRACE_H_INCLUDED

#define __TBK_BACKTRACE_H_INCLUDED


#if defined(_WIN32) || defined(_WIN64)

  #ifndef _UINTPTR_T_DEFINED
    #if defined (_M_IA64) || defined(_M_AMD64) || defined(__ia64) || defined(__x86_64)
      typedef unsigned __int64    uintptr_t;
    #else
      typedef unsigned   int      uintptr_t;
    #endif
  #define _UINTPTR_T_DEFINED
  #endif /*#ifndef _UINTPTR_T_DEFINED*/

  #include <stdlib.h>
  #define TBK_PATH_MAX _MAX_PATH

#else
  /* Linux, Mac.
  */
  #include <stdint.h>
  #include <sys/param.h>

  #define TBK_PATH_MAX PATH_MAX
#endif
      /* defined(_WIN32) || defined(_WIN64) */


typedef struct _tagPC_CORRELATION_INFO {
	char routine_name [TBK_PATH_MAX];
	char src_file_name[TBK_PATH_MAX];
	char src_line_num [33];

#if defined(_WIN32) || defined(_WIN64)
        int ilk_flag ;          /* incremental linked image flag */
#endif

} PC_CORRELATION_INFO;


/* The callback routine type.
**
** The callback routine will be called once for each stack frame during
** the two unwind passes.  Users of this package should provide a callback
** routine.
**
** If the buffer pointer is NULL, just calculate the size of buffer desired.
** Else if there isn't room for this frame's data in the buffer, return
** TBK_PREMATURE_END_TRACE. Else if there isn't more stack to walk,
** return TBL_END_TRACE.  Otherwise return TBK_WALK_CONTINUE.
*/
typedef  int (*TBK_STACK_CALLBACK) (
                 void*,          /* Pointer to stack frame data                 */
                 void*,          /* Pointer to user data                        */
                 int );          /* 1 means skip this frame, 0 means don't skip */

#define TBK_WALK_CONTINUE        0
#define TBK_END_TRACE           -1
#define TBK_PREMATURE_END_TRACE -2

#ifdef __cplusplus
extern "C" {
#endif

/*  Function Prototypes
*/

/* The "context" used below is the "ucontext_t" that is passed
** to a signal handler as the third argument.
*/

/* Passed to the per-frame routine; "What kind of output do you want?"
*/
typedef struct _tagOutputInfo {
    char  *buff_ptr;
    size_t buff_size;
    size_t curr_len;
    int    brief_format;
    int    frame_number;
} __OutputInfo;

/*  The main function for walking the stack.
**
** It returns:
**
**    CALLBACK_TRACE_OK if all went well.
**    CALLBACK_TRACE_ERR_<n> for various 'abnormal' ends.
**    CALLBACK_TRACE_NOSPACE if we ran out of buffer space.
*/
extern int tbk_trace_stack( 
              void*,              /* Pointer to context      */
              void*,              /* Pointer to __OutPutInfo */
              TBK_STACK_CALLBACK, /* call-back routine       */
              int );              /* Always pass 1           */

#define CALLBACK_TRACE_OK        0
#define CALLBACK_TRACE_ERR       1
#define CALLBACK_TRACE_ERR_1     1
#define CALLBACK_TRACE_ERR_2     2
#define CALLBACK_TRACE_ERR_3     3
#define CALLBACK_TRACE_ERR_4     4
#define CALLBACK_TRACE_ERR_5     5
#define CALLBACK_TRACE_ERR_6     6
#define CALLBACK_TRACE_NO_SPACE  7

/* Functions to retrieve information for a given stack frame.
*/
extern uintptr_t  tbk_getPC (                       /* Program counter             */
                             void* );               /* Pointer to stack frame data */

extern uintptr_t  tbk_getRetAddr (                  /* Return address              */
                             void*);                /* Pointer to stack frame data */

extern void       tbk_getModuleName(                /* Module name                 */
                             uintptr_t,             /* Input instruction address   */
                             char*,                 /* Place to put the name       */
                             size_t,                /* How big the place is        */
                             uintptr_t* );          /* Pointer to base address     */

extern int        tbk_get_pc_info (                 /* Get program counter info    */
                             PC_CORRELATION_INFO*,  /* Pointer to block to fill    */
                             uintptr_t,             /* The program counter value   */
                             uintptr_t,             /* The base address to use     */
                             char* );               /* Module name or NULL         */

extern char*      tbk_geterrorstring(void);         /* Error message string        */  

#if defined(_WIN32) || defined(_WIN64)
  extern uintptr_t  tbk_getFramePtr(                /* Frame pointer               */
                             void*);                /* Pointer to stack frame data */

  extern uintptr_t  tbk_getSP(                      /* Stack pointer               */
                             void*);                /* Pointer to stack frame data */

 #ifdef _M_IA64
  extern uintptr_t  tbk_getBStore(                  /* Intel(R) Itanium(R) architecture backing store */
                             void*);                /* Pointer to stack frame data */
 #endif
#endif

/* This routine works in two ways:
**
** If called with a NULL buffer pointer, it calculates how big the output
** buffer would have to be to hold the entire stack dump.
**
** If called with a non-NULL buffer pointer it dumps the stack to the buffer,
** up to the limit of the size of the buffer.
**
** Thus this routine should be called twice: once to get the buffer size and
** then again to fill a buffer of that size newly-allocated by the caller.
*/
extern size_t tbk_string_stack_signal(
                              void*,                /* Pointer to context        */
                              char*,                /* Pointer to buffer         */
                              size_t,               /* Size of buffer            */
                              int,                  /* Sets "brief_format" above */
                              int);                 /* Always pass 1             */


#ifdef __cplusplus
}
#endif


#endif
