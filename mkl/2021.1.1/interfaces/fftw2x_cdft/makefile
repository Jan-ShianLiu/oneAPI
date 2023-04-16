#===============================================================================
# Copyright 2006-2020 Intel Corporation.
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

##  Content:
##      Makefile for Intel(R) MKL FFTW3X CDFT wrapper library.
##      This library allows to use Intel(R) MKL CDFT routines through MPI FFTW interface.
##******************************************************************************
help:
	@echo Usage: nmake libintel64 
	@echo 		   [mpi=intelmpi^|mpich2^|mshpc] [mpidir=path]
	@echo 		   [fname=a_name^|a_name_^|a_name__^|A_NAME]
	@echo 		   [precision=MKL_DOUBLE^|MKL_SINGLE]
	@echo.
	@echo mpi=intelmpi - using Intel(R) MPI Library, default
	@echo mpi=mpich2   - using MPICH2
	@echo mpi=mshpc    - using Microsoft HPC Pack SDK
	@echo.
	@echo mpidir=path  - path to MPI installation directory.
	@echo.
	@echo fname=A_NAME   - upper case wrapper names for Fortran, default
	@echo fname=a_name   - no trailing underscore wrapper names for Fortran
	@echo fname=a_name_  - one trailing underscore wrapper names for Fortran
	@echo fname=a_name__ - two trailing underscore wrapper names for Fortran
	@echo.
	@echo precision=MKL_DOUBLE - for double precision data, default
	@echo precision=MKL_SINGLE - for float precision data
	@echo.
	@echo Make sure that environment variables such as PATH are set
	@echo properly before testing. Intel(R) C Compiler must be
	@echo included in PATH

##------------------------------------------------------------------------------
## examples of using:
##
## nmake libintel64
##      - compile using Intel(R) MPI Library, Intel(R) C Compiler
##        for double precision data and build library for
##        Intel(R) 64 based applications
##
## nmake libintel64 precision=MKL_SINGLE mpi=mpich2 mpidir=c:\mpich2x64
##      - compile using MPICH Library from "C:\mpich2x64", Intel(R) C Compiler
##        for single precision data and build library for
##        Intel(R) 64 based applications
##------------------------------------------------------------------------------

!include fftw2x_cdft.lst

sufobj   = obj
suflib   = lib
obj_path = obj
LIBR     = lib -out:
CFLAGS   = -Qstd=c99 /Wall /W2 /WX
fname    = A_NAME

IA = intel64

!ifndef INSTALL_DIR
INSTALL_DIR = ..\..\lib\$(IA)
obj_path = obj
!else
obj_path = "$(INSTALL_DIR:^"=)\obj"
!endif

objs = $(WRAP:+=.obj)

!ifndef precision
precision = MKL_DOUBLE
!endif

!if "$(precision)" == "MKL_DOUBLE"
PRE = _DOUBLE
!else
PRE = _SINGLE
!endif

!if "$(interface)" == "ilp64"
ILP_OPTS = /DMKL_ILP64
ILP_EXT = _ilp64
!else
ILP_OPTS =
ILP_EXT =
!endif

!ifndef mpi
mpi = intelmpi
!endif

!ifdef mpidir
!if "$(mpi)" == "intelmpi"
MPI_INCLUDE = $(mpidir:^"=)\intel64\include
!else
MPI_INCLUDE = $(mpidir:^"=)\include
!endif
!endif

!if "$(mpi)" == "intelmpi"
MPICC=mpiicc.bat
!else
MPICC=icl.exe
!endif

!if ("$(fname)" == "a_name")
_DEF_FNAME = -D_FNAME_NOUNDERSCORE
!else if ("$(fname)" == "a_name__")
_DEF_FNAME = -D_FNAME_SECOND_UNDERSCORE
!else if ("$(fname)" == "A_NAME")
_DEF_FNAME = -D_FNAME_UPPERCASE
!endif

!ifdef INSTALL_LIBNAME
WRAPLIB_FFTW2X_CDFT = $(INSTALL_LIBNAME).lib
!else
WRAPLIB_FFTW2X_CDFT = fftw2x_cdft$(PRE)$(ILP_EXT).lib
!endif

##-----------------------------------------------------------------------------
.SUFFIXES: .c .obj

libintel64: clean lib

lib: makedir $(objs) library  clean_obj

makedir:
	if not exist $(obj_path) md $(obj_path)

{wrappers}.c.obj:
	set include=$(MPI_INCLUDE);..\..\include;..\..\include\fftw;$(INCLUDE)
	call $(MPICC) $(CFLAGS) $(SPEC_OPT) /c /w /MD $(ILP_OPTS) $(_DEF_FNAME) /D$(precision) /Fo$(obj_path)\ $<

library:
	$(LIBR)"$(INSTALL_DIR:^"=)\$(WRAPLIB_FFTW2X_CDFT)" $(obj_path)\*.$(sufobj)

clean:
	-del /f/q "$(INSTALL_DIR:^"=)\$(WRAPLIB_FFTW2X_CDFT)"
	-del /f/q $(obj_path)

clean_obj:
	-del /f/q $(obj_path)

##-----------------------------------------------------------------------------
