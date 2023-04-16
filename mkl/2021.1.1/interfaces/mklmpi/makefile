#===============================================================================
# Copyright 2015-2020 Intel Corporation.
#
# This software and the related documents are Intel copyrighted  materials,  and
# your use of  them is  governed by the  express license  under which  they were
# provided to you (License).  Unless the License provides otherwise, you may not
# use, modify, copy, publish, distribute,  disclose or transmit this software or
# the related documents without Intel's prior written permission.
#
# This software and the related documents  are provided as  is,  with no express
# or implied  warranties,  other  than those  that are  expressly stated  in the
# License.
#===============================================================================

#  Content:
#      Makefile for Intel(R) MKL MPI wrapper library.
#      This library allows using custom MPI in Cluster Intel(R) MKL.
#*******************************************************************************

help:
	@type <<
  Usage: nmake {libintel64|dllintel64} [interface=interface_name] MPICC=mpi_comp

  mpidir=path - path to MPI installation directory.

  interface_name - {lp64|ilp64}
                   Default value: lp64

  mpi_comp       - C compiler and MPI specific libs (e.g. mpicc.bat)

  Additional macros:
     MKLROOT     - path to Intel(R) MKL root, default is ../..

     INSTALL_DIR - location where Intel(R) MKL MPI wrap library will be installed
                   Default is <MKLROOT>/lib/<arch>

     INSTALL_LIBNAME
                 - name of Intel(R) MKL MPI wrap library will be installed
                   Default is libmkl_blacs_custom

     TARGET_ARCH=<flags>
                 - defines additional compiler flags
                   Refer to the compiler documentation about CPU specific flags.
                   Some compilers may need these flags to enable optimizations
                   for modern CPUs

  N.B. Make sure that environment variables such as PATH are set
  properly before testing.

<<NOKEEP

#-------------------------------------------------------------------------------
# examples of using:
#
# nmake libintel64 MPICC=mpiicc.bat
#      - compile and build static Intel(R) MKL MPI wrapper library named
#        mkl_blacs_custom and place it into <MKLROOT>/lib/intel64
#        using Intel(R) C Compiler for Intel(R) MPI.
#-------------------------------------------------------------------------------

MKLROOT = ..\..

!if "$(interface)" == "ilp64"
ILP_OPTS = /DMKL_ILP64
ILP_EXT = _ilp64
!else
ILP_OPTS =
ILP_EXT = _lp64
!endif

!ifndef INSTALL_DIR
INSTALL_DIR = "$(MKLROOT:"=)"\lib\intel64
obj_path = obj_intel64$(ILP_EXT)
!else
obj_path = "$(INSTALL_DIR:^"=)\obj_intel64$(ILP_EXT)"
!endif

MPICC   = mpicc.bat
LIBMAN  = lib.exe
WRAP    = mklmpi-impl
objs    = $(WRAP).obj
objects = $(obj_path)\$(objs)

INSTALL_LIBNAME = mkl_blacs_custom$(ILP_EXT)

#-------------------------------------------------------------------------------
# Rules

.SUFFIXES:
.SUFFIXES: .c .obj

dllintel64 dllem64t: dll

libintel64 libem64t: library

$(objects): $(WRAP).c
	set include=$(MPI_INCLUDE);..\..\include;%%include%%
	call $(MPICC) /c /w /MT /LD $(ILP_OPTS) /Fo$(objects) $(WRAP).c

library: clean mkobjdir $(objects)
	copy "$(MKLROOT:"=)"\lib\intel64\mkl_blacs_intelmpi$(ILP_EXT).lib $(obj_path)\mkl_blacs_intelmpi$(ILP_EXT).lib
	$(LIBMAN) /remove:"intelmpi$(ILP_EXT)/$(objs)" $(obj_path)\mkl_blacs_intelmpi$(ILP_EXT).lib
	$(LIBMAN) /out:"$(INSTALL_DIR:"=)\$(INSTALL_LIBNAME).lib" $(objects) $(obj_path)\mkl_blacs_intelmpi$(ILP_EXT).lib

dll: clean mkobjdir $(objects)
	copy "$(MKLROOT:"=)"\lib\intel64\mkl_blacs_intelmpi$(ILP_EXT).lib $(obj_path)\mkl_blacs_intelmpi$(ILP_EXT).lib
	$(LIBMAN) /remove:"intelmpi$(ILP_EXT)/$(objs)" $(obj_path)\mkl_blacs_intelmpi$(ILP_EXT).lib
	call $(MPICC) $(objects) /link /machine:x64 /dynamicbase /incremental:no /subsystem:console /dll \
		"$(MKLROOT:"=)"\lib\intel64\mkl_intel$(ILP_EXT).lib \
		"$(MKLROOT:"=)"\lib\intel64\mkl_core_dll.lib \
		$(obj_path)\mkl_blacs_intelmpi$(ILP_EXT).lib /def:mklmpi.def /out:"$(INSTALL_DIR:"=)\$(INSTALL_LIBNAME).dll"

mkobjdir:
	if not exist $(obj_path) md $(obj_path)
	if not exist "$(INSTALL_DIR:"=)" md "$(INSTALL_DIR:"=)"

clean:
	-del /f/q "$(INSTALL_DIR:"=)\$(INSTALL_LIBNAME)" 2>nul
	-del /f/q $(obj_path) 2>nul

clean_obj:
	-del /f/q $(obj_path) 2>nul

#-------------------------------------------------------------------------------
