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

## Content:
##      Build standalone library of FFTW3 Fortran wrappers to Intel(R) MKL.
##*****************************************************************************

## Usage examples:
##
## nmake libia32
##        Use Intel C++ compiler (as default) to build the library
##        for 32-bit applications compiled by Intel Fortran compiler.
##
## nmake libintel64 compiler=msvs INTERFACE=ilp64
##        Use Microsoft C++ compiler to build the library for use with
##        Intel Fortran compiler, set default INTEGER size 8 bytes.

help:
	@type <<
  Usage: nmake {libia32|libintel64} [option...]

  Options:
  compiler=msvs|pgi|intel
        Build the library with Microsoft C compiler, PGI C compiler, or
        Intel(R) C Compiler.
        Default value: intel

  INTERFACE=lp64|ilp64 selects kind of MKL_INT type for %intel64 targets:"
        INTERFACE=lp64  - 32-bit integers (DEFAULT)"
        INTERFACE=ilp64 - 64-bit integers"

  fname=a_name|a_name_|a_name__|A_NAME
        Select the pattern to decorate wrapper names for Fortran.
        For example, with no special options
            g77 uses pattern a_name__
            ifort and gfortran use pattern a_name_
            ifort on Windows uses pattern A_NAME
        Default value: A_NAME

  Additional macros:
  MKLROOT=<path>
        Path to Intel(R) MKL root directory with header files and libraries.
        Default value: ..\..

  INSTALL_DIR=<path>
        Install the library to the specified location, which must exist.
        Default value: . (that is, the current directory)

  INSTALL_LIBNAME=<name>
        Specify the name of the library.
        Default value depends on compiler used.

<<NOKEEP

##-----------------------------------------------------------------------------
## Default values

!include fftw3xf.lst

compiler = intel

!ifndef MKLROOT
MKLROOT = ..\..
!endif

INTERFACE = lp64
fname = A_NAME

!ifndef INSTALL_DIR
!ifdef install_to
INSTALL_DIR = $(install_to)
!else
INSTALL_DIR = .
!endif
!endif

obj_path = $(INSTALL_DIR)\obj_$(compiler)

!ifndef INSTALL_LIBNAME
!ifdef install_as
INSTALL_LIBNAME = $(install_as)
!else
INSTALL_LIBNAME = fftw3xf_$(compiler).lib
!endif
!endif

_passdown = compiler=$(compiler) MKLROOT="$(MKLROOT:^"=)" INTERFACE=$(INTERFACE) fname=$(fname) \
            INSTALL_DIR="$(INSTALL_DIR:^"=)" INSTALL_LIBNAME=$(INSTALL_LIBNAME)

##-----------------------------------------------------------------------------
## Main targets

libia32 lib32:
	nmake lib _IA=ia32 $(_passdown) tmpdir="$(tmpdir:^"=)_ia32"

libintel64 libem64t:
	nmake lib _IA=intel64 $(_passdown) tmpdir="$(tmpdir:^"=)_intel64"

!ifdef _IA
##-----------------------------------------------------------------------------
## Supporting _macros

!if ("$(compiler)" == "msvs")
CC = cl
!else if ("$(compiler)" == "pgi")
CC = pgcc
!else
CC = icl
!endif

_cflags = $(_cflags_1) $(_cflags_2) $(CFLAGS)
_cppflags = -I"$(MKLROOT:^"=)"\include -I"$(MKLROOT:^"=)"\include\fftw \
	$(_cppflags_1) $(_cppflags_2) $(CPPFLAGS)

!if ("$(compiler)" != "pgi")
_cflags_1 = /MT
!endif

!if ("$(compiler)" == "intel" && "$(_IA)" == "ia32")
## This option is required by Intel(R) compiler to produce
## Pentium(R) III compatible executables.
_cflags_2 = /arch:ia32
!endif

!if ("$(INTERFACE)" == "ilp64")
_cppflags_1 = -DMKL_ILP64
!endif

!if ("$(fname)" == "A_NAME")
_cppflags_2 = -D_FNAME_UPPERCASE
!else if ("$(fname)" == "a_name")
_cppflags_2 = -D_FNAME_NOUNDERSCORE
!else if ("$(fname)" == "a_name__")
_cppflags_2 = -D_FNAME_SECOND_UNDERSCORE
!endif

_objs = $(WRAP:+=.obj)

##-----------------------------------------------------------------------------
## Rules

.SUFFIXES:
.SUFFIXES: .c .obj

lib: clean "$(INSTALL_DIR:^"=)\$(INSTALL_LIBNAME)" cleanup

# NMAKE provides no way to add prefix $(obj_path)\ to $(_objs),
# so we use $(_objs) as pseudotargets
"$(INSTALL_DIR:^"=)\$(INSTALL_LIBNAME)": $(_objs)
	lib /out:$@ "$(obj_path)\*.obj"

# Nmake doesn't allow spaces in {$(obj_path)}.obj, use fake {}.obj instead.
!if ("$(compiler)" == "pgi")
{wrappers}.c{}.obj:
	$(CC) $(_cflags) $(_cppflags) -c -o"$(obj_path)"\$(@F) $<
!else
{wrappers}.c{}.obj:
	$(CC) $(_cflags) $(_cppflags) -c /Fo"$(obj_path)"\ $<
!endif

clean:
	-rd /s/q "$(obj_path)"
	md "$(obj_path)"
	-del /f/q "$(INSTALL_DIR:^"=)\$(INSTALL_LIBNAME)"

cleanup:
	-rd /s/q "$(obj_path)"

##-----------------------------------------------------------------------------
!endif
