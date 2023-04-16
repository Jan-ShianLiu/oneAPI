#===============================================================================
# Copyright 2005-2020 Intel Corporation.
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
#      Intel(R) Math Kernel Library BLAS95 library creation
#******************************************************************************

help:
	@echo Usage: nmake {libia32^|libintel64} install_dir=user_dir
	@echo             [interface=interface_name] [FC=compiler_name]
	@echo Intel(R) Fortran Compiler as default
	@echo install_dir    - library and the .mod files will be built and installed
	@echo                  in subdirectories of user_dir the same way as if
	@echo                  user_dir were the Intel(R) MKL installation directory.
	@echo interface_name - can be lp64 or ilp64 for intel64. Default value is lp64.
	@echo FC             - can be ifort or pgf95. Default value is ifort.

##------------------------------------------------------------------------------
## examples of using:
##
## nmake libia32 install_dir=lib95    - build  by Intel(R) Fortran Compiler  (as default)
##                                    for 32-bit applications, static linking
##
## nmake libintel64 install_dir=lib95 - build  by Intel(R) Fortran Compiler  (as default)
##                                    for Intel(R) 64 processor family applications, static linking
##------------------------------------------------------------------------------

!include blas95.lst

iface_95 = blas.f90
iface_77 = blas_interfaces.f90

!ifndef FC
FC = ifort
!endif

!IFNDEF interface
interface = lp64
!ENDIF

opts0    = /MT /Zl
optsc    = /c $(opts0)
sufobj   = obj
suflib   = lib
sufmod   = mod
src_path = source
LIBR     = lib -out:

!IFNDEF MKLROOT
MKLROOT = ..\..
!ENDIF

obj_path = "$(install_dir)"\lib\$(IA)\obj_blas95_$(IA)_$(interface)
obj77_path = $(obj_path)\obj77

inc_path = $(MKLROOT)\include

objs = $(src_blas95:+=.obj)
mod77 = $(iface_77:.f90=.mod)

FOPTS =

!if "$(PLAT)"=="win32"
IA = ia32
mod_path = $(install_dir)\include\ia32
mkl_blas95_lib      = mkl_blas95.$(suflib)

!else if "$(PLAT)"=="win32e"
IA = intel64
!  if ("$(interface)"=="ilp64")
!IF "$(FC)"=="pgf95"
FOPTS = -i8 -mp
!ELSE
FOPTS = /4I8 /4Ya
!ENDIF
mod_path = $(install_dir)\include\intel64\ilp64
mkl_blas95_lib      = mkl_blas95_ilp64.$(suflib)
!  else
mod_path = $(install_dir)\include\intel64\lp64
mkl_blas95_lib      = mkl_blas95_lp64.$(suflib)
!  endif

!else
IA = ERROR
!endif

.SUFFIXES:: .f90 .$(sufobj) .$(suflib) .$(sufmod)

libia32 lib32: checkdir
	nmake PLAT=win32 build interface=$(interface) install_dir="$(install_dir)" MKLROOT="$(MKLROOT:^"=)" FC=$(FC)
libintel64 libem64t: checkdir
	nmake PLAT=win32e build interface=$(interface) install_dir="$(install_dir)" MKLROOT="$(MKLROOT:^"=)" FC=$(FC)

build:	check lib_mod
lib_mod:	"$(install_dir)\lib\$(IA)\$(mkl_blas95_lib)"

"$(install_dir)\lib\$(IA)\$(mkl_blas95_lib)":
	if not exist $(obj77_path) md $(obj77_path)
!IF "$(FC)"=="pgf95"
	$(FC) $(FOPTS) -module $(obj77_path) -c -o$(obj_path)\blas77.$(sufobj) "$(src_path)"\$(iface_77)
!ELSE
	$(FC) $(SPEC_OPT) $(FOPTS) /module:$(obj77_path) $(optsc) /Fo$(obj77_path)\ "$(src_path)"\$(iface_77)
!ENDIF
	nmake $(objs) interface=$(interface) install_dir="$(install_dir)"
	if not exist "$(mod_path)" md "$(mod_path)"
!IF "$(FC)"=="pgf95"
	$(FC) $(FOPTS) -module "$(mod_path)" -c -o$(obj_path)\blas95.$(sufobj) "$(inc_path)\$(iface_95)"
!ELSE
	$(FC) $(SPEC_OPT) $(FOPTS) /module:"$(mod_path)" $(optsc) /Fo$(obj77_path)\ "$(inc_path)\$(iface_95)"
!ENDIF
	$(LIBR)"$(install_dir)"\lib\$(IA)\$(mkl_blas95_lib) $(obj_path)\*.$(sufobj)
	-rmdir /s/q $(obj_path)

checkdir:
!ifndef install_dir
	@echo Not given install_dir
	@exit 1
!endif

check:
!ifndef PLAT
	@echo Undefined platform: PLAT=$(PLAT). Set one of: win32 win32e
!endif
!if "$(IA)"=="ERROR"
	@echo Unknown platform: PLAT=$(PLAT). Set one of: win32 win32e
!endif

{$(src_path)}.f90.$(sufobj):
	if not exist $(obj77_path) md $(obj77_path)
!IF "$(FC)"=="pgf95"
	$(FC) $(FOPTS) -module $(obj77_path) -c -o$(obj_path)\$@ $<
!ELSE
	$(FC) $(SPEC_OPT) $(FOPTS) /module:$(obj77_path) $(optsc) /Fo$(obj_path)\ $<
!ENDIF

clean: cleania32 cleanintel64
cleania32: checkdir
	-rmdir /s/q "$(install_dir)"\lib\ia32\obj_blas95_ia32_lp64
	-del /f/q "$(install_dir)"\lib\ia32\mkl_blas95.$(suflib)
cleanintel64: checkdir
	-rmdir /s/q "$(install_dir)"\lib\intel64\obj_blas95_intel64_lp64 "$(install_dir)"\lib\intel64\obj_blas95_intel64_ilp64
	-del /f/q "$(install_dir)"\lib\intel64\lib\mkl_blas95_lp64.$(suflib) "$(install_dir)"\lib\intel64\mkl_blas95_ilp64.$(suflib)
