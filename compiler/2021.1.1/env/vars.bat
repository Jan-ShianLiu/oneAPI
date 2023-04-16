@echo off
:: ============================================================================
:: Copyright 1985-2019 Intel Corporation All Rights Reserved.
::
:: The source code,  information and material ("Material")  contained herein is
:: owned by Intel Corporation or its suppliers or licensors,  and title to such
:: Material remains with Intel Corporation or its suppliers  or licensors.  The
:: Material contains  proprietary  information  of Intel  or its  suppliers and
:: licensors.  The Material is protected by worldwide copyright laws and treaty
:: provisions.  No part  of the  Material   may be  used,  copied,  reproduced,
:: modified,  published,   uploaded,   posted,   transmitted,   distributed  or
:: disclosed in any way without Intel's prior  express written  permission.  No
:: license under any patent,  copyright  or other intellectual  property rights
:: in the Material is granted to or conferred upon you,  either  expressly,  by
:: implication,  inducement,  estoppel  or otherwise.  Any  license  under such
:: intellectual  property  rights must  be  express and  approved  by  Intel in
:: writing.
::
:: Unless otherwise  agreed by  Intel in writing,  you may not  remove or alter
:: this notice or  any other notice  embedded in Materials by  Intel or Intel's
:: suppliers or licensors in any way.
:: ============================================================================

set VARSDIR=%~dp0
call :GetFullPath "%VARSDIR%.." CMPLR_ROOT
call :GetFullPath "%VARSDIR%..\..\.." ONEAPI_ROOT

set SCRIPT_NAME=%~nx0
set VS_TARGET_ARCH=
set INTEL_TARGET_ARCH=
set INTEL_TARGET_PLATFORM=windows

:ParseArgs
:: Parse the incoming arguments
if /i "%1"==""              goto CheckArgs
if /i "%1"=="ia32"          (set INTEL_TARGET_ARCH=ia32)     & (set TARGET_VS_ARCH=x86)     & shift & goto ParseArgs
if /i "%1"=="intel64"       (set INTEL_TARGET_ARCH=intel64)  & (set TARGET_VS_ARCH=amd64)   & shift & goto ParseArgs
if /i "%1"=="vs2017"        (set TARGET_VS=vs2017)           & shift & goto ParseArgs
if /i "%1"=="vs2019"        (set TARGET_VS=vs2019)           & shift & goto ParseArgs
shift & goto ParseArgs

:CheckArgs
:: set correct defaults
if /i "%INTEL_TARGET_ARCH%"==""   (set INTEL_TARGET_ARCH=intel64) & (set TARGET_VS_ARCH=amd64)

:: Setup Intel Compiler environment directly if Visual Studio environment is ready.
if defined VSCMD_VER (
    if /i "%VSCMD_ARG_TGT_ARCH%"=="x86" (
        set INTEL_TARGET_ARCH=ia32
    ) else (
        set INTEL_TARGET_ARCH=intel64
    )
    goto SetIntelEnv
)

::detect installed VS
set MSVS_VAR_SCRIPT=

:: The exact installation directory depends on both the version and offering of Visual Studio,
:: according to the following pattern: C:\Program Files (x86)\Microsoft Visual Studio\<version>\<offering>.
if defined VS2019INSTALLDIR (
    goto SetVCVars
)
if defined VS2017INSTALLDIR (
    goto SetVCVars
)

if /i "%TARGET_VS%"=="" (
    call :SetVS2019INSTALLDIR
    if not defined VS2019INSTALLDIR (
        call :SetVS2017INSTALLDIR
    )
    goto SetVCVars
)

if /i "%TARGET_VS%"=="vs2019" (
    if not defined VS2019INSTALLDIR (
        call :SetVS2019INSTALLDIR
    )
    goto SetVCVars
)

if /i "%TARGET_VS%"=="vs2017" (
    if not defined VS2017INSTALLDIR (
        call :SetVS2017INSTALLDIR
    )
    goto SetVCVars
)

::default, set the latest VS in global environment
:SetVCVars
if /i "%TARGET_VS%"=="" (
    ::vs2019
    if defined VS2019INSTALLDIR (
        if exist "%VS2019INSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat" (
            goto SetVS2019
        )
    )
    ::vs2017
    if defined VS2017INSTALLDIR (
        if exist "%VS2017INSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat" (
            goto SetVS2017
        )
    )
    call :NO_VS 2017 or 2019
    goto EndWithError
)

::VS2019
if /i "%TARGET_VS%"=="vs2019" (
    if defined VS2019INSTALLDIR (
        if exist "%VS2019INSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat" (
            goto SetVS2019
        )
    )
    call :NO_VS 2019
    goto EndWithError
)

::VS2017
if /i "%TARGET_VS%"=="vs2017" (
    if defined VS2017INSTALLDIR (
        if exist "%VS2017INSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat" (
            goto SetVS2017
        )
    )
    call :NO_VS 2017
    goto EndWithError
)

:SetVS2019
set TARGET_VS=vs2019
set MSVS_VAR_SCRIPT="%VS2019INSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat"
goto Setup

:SetVS2017
set TARGET_VS=vs2017
set MSVS_VAR_SCRIPT="%VS2017INSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat"
goto Setup

:Setup

:: call visual studio VARs script
:: ============================================================================
if "%VSCMD_START_DIR%"=="" (
    if EXIST "%USERPROFILE%\Source" (
        set "VSCMD_START_DIR=%CD%"
    )
)

@call %MSVS_VAR_SCRIPT% %TARGET_VS_ARCH% 1>NUL

call :GetFullPath %MSVS_VAR_SCRIPT%\.. MSVS_VAR_SCRIPT_DIR
if /i "%INTEL_TARGET_ARCH%"=="ia32" (
    if defined VCToolsInstallDir (
        if exist "%VCToolsInstallDir%\bin\HostX64\x64" (
            set "PATH=%PATH%;%VCToolsInstallDir%\bin\HostX64\x64"
            goto set_dll_end
        )
    )
    if exist "%MSVS_VAR_SCRIPT_DIR%\bin\amd64" (
        set "PATH=%PATH%;%MSVS_VAR_SCRIPT_DIR%\bin\amd64"
        goto set_dll_end
    )
)
:set_dll_end

if defined VCToolsInstallDir (
    set "__MS_VC_INSTALL_PATH=%VCToolsInstallDir%"
)

:: setup intel compiler after visual studio environment ready
:: ============================================================================
:SetIntelEnv
if /i "%INTEL_TARGET_ARCH%"=="ia32" (
    set "INTEL_TARGET_ARCH_IA32=ia32"
) else (
    if defined INTEL_TARGET_ARCH_IA32 (set INTEL_TARGET_ARCH_IA32=)
)

:: OpenCL FPGA runtime
if exist "%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\lib\oclfpga\fpgavars.bat" (
    call "%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\lib\oclfpga\fpgavars.bat"
)

set "PATH=%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\bin\intel64;%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\redist\%INTEL_TARGET_ARCH%_win\compiler;%PATH%"
set "PATH=%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\bin;%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\lib;%PATH%"
set "PATH=%PATH%;%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\lib\oclgpu;%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\ioc\bin;%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\lib\ocloc"

set "CPATH=%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\include;%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\compiler\include;%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\compiler\include\%INTEL_TARGET_ARCH%;%CPATH%"

set "INCLUDE=%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\include;%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\compiler\include;%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\compiler\include\%INTEL_TARGET_ARCH%;%INCLUDE%"

set "LIB=%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\compiler\lib;%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\compiler\lib\%INTEL_TARGET_ARCH%_win;%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\lib;%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\lib\x64;%LIB%"

set "MIC_LIBRARY_PATH=%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\compiler\lib\mic;%MIC_LIBRARY_PATH%"

set "MIC_LD_LIBRARY_PATH=%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\compiler\lib\mic;%MIC_LD_LIBRARY_PATH%"

set "OCL_ICD_FILENAMES=%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\lib\emu\intelocl64_emu.dll;%CMPLR_ROOT%\%INTEL_TARGET_PLATFORM%\lib\x64\intelocl64.dll"

goto End

:End
exit /B 0

:: ============================================================================
:NO_VS
echo.
if /i "%*"=="2017 or 2019" (
    echo ERROR: Visual Studio %* is not found in "C:\Program Files (x86)\Microsoft Visual Studio\<2017 or 2019>\<Edition>", please set VS2017INSTALLDIR or VS2019INSTALLDIR
    goto :EOF
)
if /i "%*"=="2019" (
    echo ERROR: Visual Studio %* is not found in "C:\Program Files (x86)\Microsoft Visual Studio\2019\<Edition>", please set VS2019INSTALLDIR
    goto :EOF
)
if /i "%*"=="2017" (
    echo ERROR: Visual Studio %* is not found in "C:\Program Files (x86)\Microsoft Visual Studio\2019\<Edition>", please set VS2017INSTALLDIR
    goto :EOF
)
:EndWithError
exit /B 1

:: ============================================================================
:GetFullPath
SET %2=%~f1
GOTO :EOF

:SetVS2019INSTALLDIR
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional" (
    set "VS2019INSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional"
    goto :EOF
)
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise" (
    set "VS2019INSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise"
    goto :EOF
)
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community" (
    set "VS2019INSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community"
    goto :EOF
)
goto :EOF

:SetVS2017INSTALLDIR
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional" (
    set "VS2017INSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional"
    goto :EOF
)
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise" (
    set "VS2017INSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\2017\Enterprise"
    goto :EOF
)
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community" (
    set "VS2017INSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community"
    goto :EOF
)
goto :EOF
