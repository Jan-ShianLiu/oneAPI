/**
*** Copyright (C) 1985-2011 Intel Corporation.  All rights reserved.
***
*** The information and source code contained herein is the exclusive
*** property of Intel Corporation and may not be disclosed, examined
*** or reproduced in whole or in part without explicit written authorization
*** from the company.
***
**/

/**
*** Provided for use in the instrumentation compilation phase of
*** profile-guided optimizations. Interface to the instrumentation
*** run-time.
**/

#if !defined(_PGO_USER_H)
#define _PGO_USER_H

#if defined __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PGOPTI_COVERAGE_INVALID_NUM_FILES ((int) -1)

typedef struct PGOPTI_COVERAGE_SUMMARY_S  PGOPTI_COVERAGE_SUMMARY_NODE;
typedef struct PGOPTI_COVERAGE_SUMMARY_S  *PGOPTI_COVERAGE_SUMMARY;

// Structure type that is used to return the coverage summary information.
struct PGOPTI_COVERAGE_SUMMARY_S {
    char *file_name;
    unsigned int coverage_percent;
    unsigned int line_coverage_percent;
};

#if defined _PGO_INSTRUMENT

/** 
*** Direct the profiling module to create .dyn file(s) containing the 
*** data collected since the start of the application run, or the last reset
*** (see _PGOPTI_Prof_Reset*).
***
*** On Linux applications that involve shared objects, each shared object
*** may have its own memory region for profiling data. Separate .dyn files 
*** will be created for each loaded shared object that contains instrumented 
*** routines, as well as the main application executable. 
***
*** This routine may be called from either the main application or from a 
*** shared object to dump the data of the main executable and all the shared 
*** objects.
***
*** On Windows or applications that do not involve shared objects, this
*** routine is equivalent to _PGOPTI_Prof_Dump 
**/
extern void _PGOPTI_Prof_Dump_All(void);


/** 
*** Direct the profiling module to create a .dyn file containing the 
*** data collected since the start of the application run, or the last reset
*** (see _PGOPTI_Prof_Reset*) for the module performing the call. 
*** 
*** On Linux applications that involve shared objects, each shared object
*** may have its own memory region for profiling data. If this routine is
*** called by a shared object, only the data for the routines that are part
*** of that shared object will be output to the .dyn file. If this routine is 
*** called by the main executable, only the data for the routines of the main
*** executable will be output to the .dyn file.
**/
extern void _PGOPTI_Prof_Dump(void);


/** 
*** Direct the profiling module to clear the profiling data collected.
***
*** On Linux applications that involve shared objects, each shared object
*** may have its own memory region for profiling data. This routine will
*** reset the data for all the shared objects that have instrumentation, as 
*** well as the main application executable. 
***
*** This routine may be called from either the main application or from a 
*** shared object to clear the data of the main executable and all the shared 
*** objects.
***
*** On Windows or applications that do not involve shared objects, this 
*** routine is equivalent to _PGOPTI_Prof_Reset 
**/
extern void _PGOPTI_Prof_Reset_All(void);


/** 
*** Direct the profiling module to clear the profiling data collected.
*** 
*** On Linux applications that involve shared objects, each shared object
*** may have its own memory region for profiling data. If this routine is
*** called on a shared object, only the data for the routines that are part
*** of that shared object will be reset. If this routine is called on the 
*** main executable, only the data for the routines of the main
*** executable will be reset.
**/
extern void _PGOPTI_Prof_Reset(void);

/**
*** The equivalent of calling _PGOPTI_Prof_Dump followed by _PGOPTI_Prof_Reset
*** for the executable or shared object.
**/
extern void _PGOPTI_Prof_Dump_And_Reset(void);


/** Activates 'Interval Profile Dumping' and sets the approximate frequency at 
*** which dumps occur. The interval is specified in milliseconds.
***
*** This function is used in non-terminating applications to cause data files
*** be to created periodically.
**/
extern void _PGOPTI_Set_Interval_Prof_Dump(int interval);

// Return the coverage summary information in a dynamically allocated
// array of PGOPTI_COVERAGE_SUMMARY_NODE structures in coverage_array.
// The function returns the number of elements in this array if the 
// coverage summary collection is successful. A value of 
// PGOPTI_COVERAGE_INVALID_NUM_FILES  is returned otherwise.
extern int _PGOPTI_Get_Coverage_Info(PGOPTI_COVERAGE_SUMMARY *coverage_array);
#else
/**
*** An empty version of the routines is provided to support building and 
*** linking of applications containing the PGO API routines when not 
*** performing instrumentation without the need to modify the source
*** routines.
**/
static void _PGOPTI_Prof_Dump_All(void) {}
static void _PGOPTI_Prof_Dump(void) {}
static void _PGOPTI_Prof_Reset_All(void) {}
static void _PGOPTI_Prof_Reset(void) {}
static void _PGOPTI_Prof_Dump_And_Reset(void) {}
static void _PGOPTI_Set_Interval_Prof_Dump(int interval) {}
#define _PGOPTI_GET_COVERAGE_USE_WITHOUT_PROFILING -100
static int  _PGOPTI_Get_Coverage_Info(PGOPTI_COVERAGE_SUMMARY *coverage_array)
{ return _PGOPTI_GET_COVERAGE_USE_WITHOUT_PROFILING; }

#endif /* _PGO_INSTRUMENT */


#if defined __cplusplus
}
#endif /* __cplusplus */

#endif /* _PGO_USER_H */
