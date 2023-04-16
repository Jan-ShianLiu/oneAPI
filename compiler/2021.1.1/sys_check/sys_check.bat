@echo off
::#===============================================================================
::# Copyright 1985-2019 Intel Corporation All Rights Reserved.
::#
::# The source code,  information  and material  ("Material") contained  herein is
::# owned by Intel Corporation or its  suppliers or licensors,  and  title to such
::# Material remains with Intel  Corporation or its  suppliers or  licensors.  The
::# Material  contains  proprietary  information  of  Intel or  its suppliers  and
::# licensors.  The Material is protected by  worldwide copyright  laws and treaty
::# provisions.  No part  of  the  Material   may  be  used,  copied,  reproduced,
::# modified, published,  uploaded, posted, transmitted,  distributed or disclosed
::# in any way without Intel's prior express written permission.  No license under
::# any patent,  copyright or other  intellectual property rights  in the Material
::# is granted to  or  conferred  upon  you,  either   expressly,  by implication,
::# inducement,  estoppel  or  otherwise.  Any  license   under such  intellectual
::# property rights must be express and approved by Intel in writing.
::#
::# Unless otherwise agreed by Intel in writing,  you may not remove or alter this
::# notice or  any  other  notice   embedded  in  Materials  by  Intel  or Intel's
::# suppliers or licensors in any way.
::#===============================================================================
set VARSDIR=%~dp0
if not defined CMPLR_ROOT call :GetFullPath "%VARSDIR%.." CMPLR_ROOT
set INTEL_TARGET_PLATFORM=windows
call common.bat :speak   This Is A Message You Will See In Verbose Mode
:: OpenCL FPGA runtime
if exist "%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\lib\oclfpga\fpga_sys_check.bat" (
    call "%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\lib\oclfpga\fpga_sys_check.bat"
)

:: every syscheck script should set up an ERRORSTATE variable and return it on completion.
setlocal
set /A ERRORSTATE=0

@echo off

CMD.EXE /C "python --version" 2>&1|findstr version
if %errorlevel% == 0 (
  set /A ERRORSTATE=%errorlevel%
) else (
  set /A ERRORSTATE=%errorlevel%
  echo "The OneAPI DPC++ requires the python to be installed."
  exit /B 1
)

if %ERRORSTATE% == 0 (
echo "OK"
)

::exit with the %ERRORSTATE%
:: use /B flag, or the exit will prevent other sys_checks from running.
exit /B %ERRORSTATE%
