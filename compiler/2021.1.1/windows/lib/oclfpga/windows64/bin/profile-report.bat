@rem Generate Profile Utility Script 

@echo off
@if not exist "%WINDIR%\system32\chcp.com" goto after_chcp
for /f "tokens=2 delims=:." %%x in ('chcp') do set saved_chcp=%%x
@rem Change console code to US english
@chcp 437>nul
:after_chcp
@echo on
@if not "%INTELFPGAOCLSDKROOT%" == "" goto check_bin
@echo Error: You must set environment variable INTELFPGAOCLSDKROOT to the absolute path of the root of the Intel(R) FPGA SDK for OpenCL(TM) software installation
@set ERRORLEVEL=1
@goto end
:check_bin
@if exist "%INTELFPGAOCLSDKROOT%"\llvm\aocl-bin/aocl-clang.exe goto clang_ok
@echo Error: You must set environment variable INTELFPGAOCLSDKROOT to the absolute path of the root of the Intel(R) FPGA SDK for OpenCL(TM) software installation
@set ERRORLEVEL=1
@goto end
:clang_ok
@if not "%QUARTUS_ROOTDIR%" == "" goto check_qtcl
@echo Error: You must set environment variable QUARTUS_ROOTDIR to the absolute path of the quartus subdirectory inside ACDS
@set ERRORLEVEL=2
@goto end
:check_qtcl
@if exist "%QUARTUS_ROOTDIR%/common/tcl/internal" goto preamble_ok
@echo Error: You must set environment variable QUARTUS_ROOTDIR to the absolute path of the quartus subdirectory inside ACDS
@set ERRORLEVEL=2
@goto end
:preamble_ok
@set PERL5LIB=%INTELFPGAOCLSDKROOT%\share\lib\perl;%INTELFPGAOCLSDKROOT%\share\lib\perl\5.8.8;%PERL5LIB%


@echo OFF
rem set USAGE='UsageX:%~n0 <sourcename>.bc'
rem set NAME = %0 


if "x%1" == "x" (
    echo Usage:%~n0 sourcename.bc
    goto end
)

if not "x%2" == "x" (
    echo Usage:%~n0 sourcename.bc
    goto end
)

if not exist %1 (
    echo "%~n0: Can't find input file %1"
    goto end
)

%QUARTUS_ROOTDIR%\bin\quartus_stp -t "%INTELFPGAOCLSDKROOT%"\share\lib\tcl\get-profile-data.tcl
if not exist "profile.out" (
    echo %~n0: No profile data found in profile.out
    goto end
)

"%INTELFPGAOCLSDKROOT%"\windows64\aocl-bin\aocl-opt -digest %1.bc > nul

if errorlevel 1 (
	name=${file%\.*}
	echo Report generated in file profile.%~nNAME.html
)

@if not exist "%WINDIR%\system32\chcp.com" goto end
@rem Restore console settings
@chcp %saved_chcp%>nul
:end
