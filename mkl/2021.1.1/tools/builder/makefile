#===============================================================================
# Copyright 2004-2020 Intel Corporation.
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
##      Intel(R) Math Kernel Library (Intel(R) MKL) Custom DLL builder
##
##******************************************************************************

help:
 	@nmake -q print_help PRIHE=YES

print_help:
!IF ("$(PRIHE)"=="YES")
!MESSAGE *
!MESSAGE * Custom DLL builder.
!MESSAGE * Usage: nmake <target> [<options>]
!MESSAGE *   target
!MESSAGE *     libia32    - for IA-32 architecture from static Intel(R) MKL libraries
!MESSAGE *     libintel64 - for Intel(R) 64 architecture from static Intel(R) MKL libraries
!MESSAGE *     help       - prints this help
!MESSAGE *   <options>
!MESSAGE *     interface={cdecl}
!MESSAGE *         Specifies programming interface for ia32.
!MESSAGE *         Default: cdecl.
!MESSAGE *     interface={lp64|ilp64}
!MESSAGE *         Specifies programming interface for intel64.
!MESSAGE *         Default: lp64.
!MESSAGE *     export=<file_name>
!MESSAGE *         The name of the file that contains the list of entry points
!MESSAGE *         to be included to the DLL.
!MESSAGE *         Default: user_example_list (no extension).
!MESSAGE *     name=<dll_name>
!MESSAGE *         The name of the created libraries.
!MESSAGE *         Default: mkl_custom.dll and mkl_custom.lib.
!MESSAGE *     xerbla=<err_handlr>
!MESSAGE *         The name of the object file that contains the user's error handler.
!MESSAGE *         By default native Intel(R) MKL XERBLA is used.
!MESSAGE *     threading={parallel|sequential}
!MESSAGE *         Specifies whether to use Intel(R) MKL in the threaded or sequential mode.
!MESSAGE *         Default: parallel.
!MESSAGE *     cluster={yes|no}  (for libintel64 only)
!MESSAGE *         Specifies whether Intel(R) MKL cluster components (BLACS, ScaLAPACK or/and CDFT)
!MESSAGE *         are needed to build the custom shared object.
!MESSAGE *         Default: no.
!MESSAGE *     blacs_mpi={intelmpi|msmpi}
!MESSAGE *         Specifies which pre-compiled Intel(R) MKL BLACS library to use. Ignored if 'cluster=no'.
!MESSAGE *         Default: intelmpi.
!MESSAGE *     blacs_name=<lib_name>
!MESSAGE *         Specifies the name (without extension) of a custom Intel(R) MKL BLACS library to use.
!MESSAGE *         Ignored if 'cluster=no'. 'blacs_mpi' is ignored if 'blacs_name' was specified.
!MESSAGE *         Default: mkl_blacs_<blacs_mpi>_<interface>
!MESSAGE *     mpi=<lib_name>
!MESSAGE *         Specifies the name (without an extension) of MPI library used to build the custom DLL.
!MESSAGE *         Ignored if 'cluster=no'.
!MESSAGE *         Default: impi.
!MESSAGE *     manifest={yes|no|embed}
!MESSAGE *         Manages the creation of MS manifest for Custom DLL.
!MESSAGE *         Default: manifest is not specified.
!MESSAGE *     crt=<c-runtime library name>
!MESSAGE *         Specifies the name of MS C-runtime library used to build the custom DLL.
!MESSAGE *         Default: msvcrt.lib.
!MESSAGE *     uwd_compat=<yes|no>
!MESSAGE *         Build Uiversal Windows Driver compatible custom DLL with OneCore.lib
!MESSAGE *         Recommended Windows SDK version: 10.0.17134.0 or higher
!MESSAGE *         If 'uwd_compat=yes' default threading: sequential, default crt: ucrt.lib
!MESSAGE *         Default: no.
!MESSAGE *     MKLROOT=<MKL_directory>
!MESSAGE *         Specifies the location of Intel(R) MKL libraries used to build the custom shared object.
!MESSAGE *         Default: the Intel(R) MKL installation directory.
!MESSAGE *
!ENDIF

##------------------------------------------------------------------------------
## Usage examples:
##
## nmake libia32
##      Creates Intel(R) MKL Custom DLL for IA-32 architecture.
##      Function list is taken from the predefined file user_example_list.
##      The static-parallel Intel(R) MKL libraries are used to build mkl_custom.dll.
##      No special xerbla is used.
##
## nmake libintel64 export=my_blas_list interface=ilp64 name=my_blas
##      Creates my_blas.dll for Intel(R) 64 architecture.
##      Function list is taken from the user file my_blas_list.
##      The static-parallel-ilp64 Intel(R) MKL libraries are used to build my_blas.dll.
##
##------------------------------------------------------------------------------

!if "$(MKLROOT)"==""
CB_MKLROOT=..\..
!else
CB_MKLROOT="$(MKLROOT)"
!endif

mklia32_libpath=$(CB_MKLROOT)\lib\ia32
mklintel64_libpath=$(CB_MKLROOT)\lib\intel64
compileria32_libpath=$(CB_MKLROOT)\..\compiler\lib\ia32
compilerintel64_libpath=$(CB_MKLROOT)\..\compiler\lib\intel64

!if "$(export)"==""
CB_EXPORT=user_example_list
!else
CB_EXPORT="$(export)"
!endif

!if "$(name)"==""
CB_NAME=mkl_custom
!else
CB_NAME="$(name)"
!endif

IFACE_LIB_32=mkl_intel_c.lib
IFACE_LIB_32_DLL=mkl_intel_c_dll.lib

IF=lp64
!if "$(interface)"=="ilp64"
IF=ilp64
!endif

!ifndef threading
!if "$(uwd_compat)"=="yes"
threading=sequential
!else
threading=parallel
!endif
!endif

!if "$(threading)"=="sequential"
THREADING_LIB=mkl_sequential.lib
OMP_LIB_INTEL64=
OMP_LIB_IA32=
!else
THREADING_LIB=mkl_intel_thread.lib
OMP_LIB_INTEL64=libiomp5md.lib
OMP_LIB_IA32=libiomp5md.lib
!endif

!if "$(cluster)"=="yes"
!if "$(blacs_mpi)"==""
blacs_mpi=intelmpi
!endif
!if "$(mpi)"==""
MPI_LIB=impi.lib
!else
MPI_LIB=$(mpi).lib
!endif
!if "$(blacs_name)"==""
BLACS_LIB=mkl_blacs_$(blacs_mpi)_$(IF).lib
!else
BLACS_LIB=$(blacs_name).lib
!endif
SCALAPACK_LIB=mkl_scalapack_$(IF).lib
CDFT_LIB=mkl_cdft_core.lib
!else
BLACS_LIB=
SCALAPACK_LIB=
CDFT_LIB=
MPI_LIB=
!endif

manifest = embed

!if "$(manifest)"=="embed"
MNFST_OPT=/MANIFEST
MT_CMD=mt.exe -nologo -manifest $(CB_NAME).dll.manifest -outputresource:$(CB_NAME).dll;2
!else
!if "$(manifest)"=="yes"
MNFST_OPT=/MANIFEST
MT_CMD=
!else
!if "$(manifest)"=="no"
MNFST_OPT=/MANIFEST:NO
MT_CMD=
!else
MNFST_OPT=/MANIFEST
MT_CMD=mt.exe -nologo -manifest $(CB_NAME).dll.manifest -outputresource:$(CB_NAME).dll;2
!endif
!endif
!endif

!if "$(xerbla)"==""
CB_XERBLA=
!else
CB_XERBLA="$(xerbla)"
!endif

!if "$(crt)"==""
!if "$(uwd_compat)"=="yes"
CB_CRT=ucrt.lib
I_MALLOC_OBJ=i_malloc_dll.obj
!else
CB_CRT=msvcrt.lib
I_MALLOC_OBJ=i_malloc_dll_static.obj
!endif
!elseif "$(crt)"=="msvcrt.lib"
CB_CRT="$(crt)"
I_MALLOC_OBJ=i_malloc_dll.obj
!else
CB_CRT="$(crt)"
I_MALLOC_OBJ=i_malloc_dll_static.obj
!endif

!if "$(uwd_compat)"=="yes"
SYS_LIBS = OneCore.lib
NODEFLIBS = /NODEFAULTLIB:kernel32.lib /NODEFAULTLIB:user32.lib /NODEFAULTLIB:libucrt.lib
!else
SYS_LIBS = kernel32.lib user32.lib
!if "$(CB_CRT)"=="msvcrt.lib"
NODEFLIBS = /NODEFAULTLIB:libcmt.lib
!else
NODEFLIBS = /NODEFAULTLIB:msvcrt.lib
!endif
!endif

NODEFLIBS = $(NODEFLIBS) /NODEFAULTLIB:libmmt.lib /NODEFAULTLIB:libirc.lib /NODEFAULTLIB:svml_dispmt.lib /NODEFAULTLIB:libdecimal.lib

libintel64 intel64 em64t:
	set Lib=$(mklintel64_libpath);%%Lib%%;$(compilerintel64_libpath)
	@echo EXPORTS > user_def_file.def
	type $(CB_EXPORT) | findstr /V "^[;#]" >> user_def_file.def
	@if exist .\temp_cdll rd /q /s .\temp_cdll
	@md .\temp_cdll
	lib .\lib\mkl_cdll_intel64.lib /EXTRACT:"$(I_MALLOC_OBJ)" /OUT:".\temp_cdll\$(I_MALLOC_OBJ)"
	lib .\lib\mkl_cdll_intel64.lib /EXTRACT:_vml_dll_core.obj /OUT:.\temp_cdll\_vml_dll_core.obj
	link /DLL $(MNFST_OPT) /MACHINE:AMD64 $(NODEFLIBS) /def:user_def_file.def \
	.\temp_cdll\$(I_MALLOC_OBJ) .\temp_cdll\_vml_dll_core.obj $(CB_XERBLA) \
	$(BLACS_LIB) $(SCALAPACK_LIB) $(CDFT_LIB) mkl_intel_$(IF).lib $(THREADING_LIB) mkl_core.lib \
	$(OMP_LIB_INTEL64) \
	mkl_custom_vers.res \
	$(CB_CRT) $(MPI_LIB) $(SYS_LIBS) /out:$(CB_NAME).dll
	@rd /q /s .\temp_cdll
	@del user_def_file.def
	$(MT_CMD)

libia32 ia32:
	set Lib=$(mklia32_libpath);%%Lib%%;$(compileria32_libpath)
	@echo EXPORTS > user_def_file.def
	type $(CB_EXPORT) | findstr /V "^[;#]" >> user_def_file.def
	@if exist .\temp_cdll rd /q /s .\temp_cdll
	@md .\temp_cdll
	@lib .\lib\mkl_cdll_ia32.lib /EXTRACT:"$(I_MALLOC_OBJ)" /OUT:".\temp_cdll\$(I_MALLOC_OBJ)"
	@lib .\lib\mkl_cdll_ia32.lib /EXTRACT:_vml_dll_core.obj /OUT:.\temp_cdll\_vml_dll_core.obj
	link /DLL $(MNFST_OPT) /MACHINE:IX86 $(NODEFLIBS) /def:user_def_file.def \
	.\temp_cdll\$(I_MALLOC_OBJ) .\temp_cdll\_vml_dll_core.obj $(CB_XERBLA) \
	$(IFACE_LIB_32) $(THREADING_LIB) mkl_core.lib \
	$(OMP_LIB_IA32) \
	mkl_custom_vers.res \
	$(CB_CRT) $(SYS_LIBS) /out:$(CB_NAME).dll
	@rd /q /s .\temp_cdll
	@del user_def_file.def
	$(MT_CMD)
