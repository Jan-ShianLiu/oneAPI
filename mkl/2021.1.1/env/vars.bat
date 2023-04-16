@echo off
rem ============================================================================
rem Copyright 2003-2020 Intel Corporation.
rem
rem This software  and the related  documents  are Intel  copyrighted materials,
rem and your use  of them is  governed by the express  license under  which they
rem were provided to you (License).  Unless the License provides otherwise,  you
rem may not use,  modify,  copy, publish,  distribute, disclose or transmit this
rem software or the related documents without Intel's prior written permission.
rem
rem This software and the related documents are provided as is,  with no express
rem or implied warranties,  other  than those that  are expressly  stated in the
rem License.
rem ============================================================================

set MKLROOT=

setlocal

:: Cache some environment variables.
set CPRO_PATH=%~dp0
call :GetFullPath "%CPRO_PATH%\.." TMP_MKLROOT

set SCRIPT_NAME=%~nx0
set MOD_NAME=mod

:: Set the default arguments
set MKL_TARGET_ARCH=intel64
set MKL_LP64_ILP64=lp64
set MKL_MOD=

:ParseArgs
:: Parse the incoming arguments
if /i "%1"==""           goto Build
if /i "%1"=="help"       goto Syntax
if /i "%1"=="ia32"       (set MKL_TARGET_ARCH=ia32)    & shift & goto ParseArgs
if /i "%1"=="intel64"    (set MKL_TARGET_ARCH=intel64) & shift & goto ParseArgs
if /i "%1"=="lp64"       (set MKL_LP64_ILP64=lp64)     & shift & goto ParseArgs
if /i "%1"=="ilp64"      (set MKL_LP64_ILP64=ilp64)    & shift & goto ParseArgs
if /i "%1"=="%MOD_NAME%" (set MKL_MOD=%MOD_NAME%)      & shift & goto ParseArgs
shift & goto ParseArgs

:Build
:: main actions
set "LIB=%TMP_MKLROOT%\lib\%MKL_TARGET_ARCH%;%LIB%"
set "PATH=%TMP_MKLROOT%\redist\%MKL_TARGET_ARCH%;%TMP_MKLROOT%\bin\%MKL_TARGET_ARCH%;%PATH%"

if /i "%MKL_MOD%"=="%MOD_NAME%" (
    if /i "%MKL_TARGET_ARCH%"=="intel64" (
        set "INCLUDE=%TMP_MKLROOT%\include\%MKL_TARGET_ARCH%\%MKL_LP64_ILP64%;%INCLUDE%"
    ) else (
        set "INCLUDE=%TMP_MKLROOT%\include\%MKL_TARGET_ARCH%;%INCLUDE%"
    )
) 
set "INCLUDE=%TMP_MKLROOT%\include;%INCLUDE%"

set "CPATH=%TMP_MKLROOT%\include;%CPATH%"
set "PKG_CONFIG_PATH=%TMP_MKLROOT%\tools\pkgconfig;%PKG_CONFIG_PATH%"
set "NLSPATH=%TMP_MKLROOT%\lib\%MKL_TARGET_ARCH%;%NLSPATH%"
endlocal& ^
set MKLROOT=%TMP_MKLROOT%& ^
set PATH=%PATH%& ^
set LIB=%LIB%& ^
set INCLUDE=%INCLUDE%& ^
set CPATH=%CPATH%& ^
set NLSPATH=%NLSPATH%& ^
set PKG_CONFIG_PATH=%PKG_CONFIG_PATH%

goto End

:Syntax
echo.
echo    oneAPI MKL vars.bat syntax: ^[^<arch^>^] ^[^<MKL_interface^>^] ^[mod^]
echo.
echo    ^<arch^> (optional) - oneAPI MKL architecture
echo        ia32         : Setup for IA-32 architecture
echo        intel64      : Setup for Intel(R) 64 architecture (default)
echo.
echo    ^<MKL_interface^> (optional) - oneAPI MKL programming interface for intel64
echo                                 Not applicable without "mod"
echo        lp64         : 4 bytes integer (default)
echo        ilp64        : 8 bytes integer
echo.
echo    mod (optional) - set path to oneAPI MKL F95 modules
echo.
echo.
exit /B 1

:GetFullPath
set %2=%~f1
goto End

:End
exit /B 0
