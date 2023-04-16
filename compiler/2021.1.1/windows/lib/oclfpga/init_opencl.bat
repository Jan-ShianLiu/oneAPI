@echo off
setlocal enabledelayedexpansion

REM This script updates PATH to add SDK's and board's libraries
if "x!INTELFPGAOCLSDKROOT!" == "x" (
  echo INTELFPGAOCLSDKROOT is not set
  echo Using script's current directory "%~dp0"
  set "INTELFPGAOCLSDKROOT=%~dp0"
) else (
  echo INTELFPGAOCLSDKROOT is set to "!INTELFPGAOCLSDKROOT!". Using that.
)

REM Removing the trainling backslash (if any)
if !INTELFPGAOCLSDKROOT:~-1!==\ set INTELFPGAOCLSDKROOT=!INTELFPGAOCLSDKROOT:~0,-1!

if not exist "%INTELFPGAOCLSDKROOT%" (
  echo Error: INTELFPGAOCLSDKROOT is set but is not a directory
  goto end
)

REM Detect if Full SDK is installed or the RTE
set RTE_ONLY=0
if not exist "%INTELFPGAOCLSDKROOT%/bin/aoc.exe" (
  if exist "%INTELFPGAOCLSDKROOT%/bin/aocl.exe" (
    echo aoc was not found, but aocl was found. Assuming only RTE is installed.
    set RTE_ONLY=1
  )
)

REM Check that we have Quartus in our path
if  %RTE_ONLY% == 1 goto FOUND_Q
REM If oneAPI FPGA add-on is installed and configed, set Quartus until we are in aoc.pl
if not "x!INTEL_FPGA_ROOT!" == "x" (
  echo Will use INTEL_FPGA_ROOT to find Quartus
  goto FOUND_Q
)
if not "x!QUARTUS_ROOTDIR_OVERRIDE!" == "x" (
  echo Will use QUARTUS_ROOTDIR_OVERRIDE=!QUARTUS_ROOTDIR_OVERRIDE! to find Quartus
  goto FOUND_Q
)
REM Check the peer directories for quartus
if exist "%INTELFPGAOCLSDKROOT%\..\quartus" (
  for /f %%i in ("%INTELFPGAOCLSDKROOT%\..\quartus") do set QUARTUS_ROOTDIR_OVERRIDE=%%~fi
REM set QUARTUS_ROOTDIR_OVERRIDE=!INTELFPGAOCLSDKROOT!\..\quartus
  echo Found a Quartus directory at "!QUARTUS_ROOTDIR_OVERRIDE!". Using that.
  goto FOUND_Q
)
if exist "%INTELFPGAOCLSDKROOT%\..\..\quartus" (
  for /f %%i in ("%INTELFPGAOCLSDKROOT%\..\..\quartus") do set QUARTUS_ROOTDIR_OVERRIDE=%%~fi
REM  set QUARTUS_ROOTDIR_OVERRIDE=!INTELFPGAOCLSDKROOT!\..\..\quartus
  echo Found a Quartus directory at "!QUARTUS_ROOTDIR_OVERRIDE!". Using that.
  goto FOUND_Q
)
REM Check for the next environment variable
if not "x!QUARTUS_ROOTDIR!" == "x" (
  echo Will use QUARTUS_ROOTDIR=!QUARTUS_ROOTDIR! to find Quartus
  goto FOUND_Q
)
REM No environment set check existing paths
where /Q quartus_sh.exe 2> NUL
if %errorlevel% == 0 (
  for /f "delims=" %%I in ('where quartus_sh') do (echo Found Quartus at %%~dpI)
  goto FOUND_Q
)
echo Warning: Quartus (quartus_sh^) is not on the path!  Without Quartus you will not be able to run full compiles.

:FOUND_Q
REM Find which VisualStudio is installed on the machine, if any.
REM link.exe and VS run-time libraries are needed for emulator compiles.
where /Q link.exe 2> NUL
if %errorlevel% == 0 goto FOUND_VS_PATH
echo.
echo VisualStudio's link.exe is not on the path. Guessing its location.
if not "x!VS100COMNTOOLS!" == "x" (
  echo VS100COMNTOOLS environment variable is set to !VS100COMNTOOLS!.
  echo Looks like you have VisualStudio2010 installed. Using it.
  set "OCLVSTOOLS=!VS100COMNTOOLS!\..\..\VC\bin\amd64\vcvars64.bat"
  goto FOUND_VS_PATH
)
if not "x!VS110COMNTOOLS!" == "x" (
  echo VS110COMNTOOLS environment variable is set to !VS110COMNTOOLS!.
  echo Looks like you have VisualStudio2012 installed. Using it.
  set "OCLVSTOOLS=!VS110COMNTOOLS!\..\..\VC\bin\amd64\vcvars64.bat"
  goto FOUND_VS_PATH
)
if not "x!VS120COMNTOOLS!" == "x" (
  echo VS120COMNTOOLS environment variable is set to !VS120COMNTOOLS!.
  echo Looks like you have VisualStudio2013 installed. Using it.
  set "OCLVSTOOLS=!VS120COMNTOOLS!\..\..\VC\bin\amd64\vcvars64.bat"
  goto FOUND_VS_PATH
)
if not "x!VS140COMNTOOLS!" == "x" (
  echo VS140COMNTOOLS environment variable is set to !VS140COMNTOOLS!.
  echo Looks like you have VisualStudio2015 installed. Using it.
  set "OCLVSTOOLS=!VS140COMNTOOLS!\..\..\VC\bin\amd64\vcvars64.bat"
  goto FOUND_VS_PATH
)
echo Cannot find VisualStudio installation by looking at the environment.
echo If VisualStudio is installed, use VisualStudio Command Prompt to set all
echo required environment variables.

:FOUND_VS_PATH
REM The discovery phase is done, begin setting global variables
endlocal & set "INTELFPGAOCLSDKROOT=%INTELFPGAOCLSDKROOT%" & set "OCLVSTOOLS=%OCLVSTOOLS%" & set "RTE_ONLY=%RTE_ONLY%" & set "QUARTUS_ROOTDIR_OVERRIDE=%QUARTUS_ROOTDIR_OVERRIDE%"
REM Call VisualStudio's script to find Windows SDK path, among other things.
if "x%OCLVSTOOLS%" == "x" goto DONE_VS_SETUP
call "%OCLVSTOOLS%"
set OCLVSTOOLS=
:DONE_VS_SETUP
echo.
REM The complicated PATH commands below simply first check if the variable already on the path before adding.
echo Adding %INTELFPGAOCLSDKROOT%\bin to PATH
echo %PATH%|findstr /i /c:"%INTELFPGAOCLSDKROOT%\bin">nul
if errorlevel 1 (
  set "PATH=%INTELFPGAOCLSDKROOT%\bin;%PATH%"
)

echo Adding %INTELFPGAOCLSDKROOT%\windows64\bin to PATH
echo %PATH%|findstr /i /c:"%INTELFPGAOCLSDKROOT%\windows64\bin">nul
if errorlevel 1 (
  set "PATH=%INTELFPGAOCLSDKROOT%\windows64\bin;%PATH%"
)

echo Adding %INTELFPGAOCLSDKROOT%\llvm\aocl-bin to PATH
echo %PATH%|findstr /i /c:"%INTELFPGAOCLSDKROOT%\llvm\aocl-bin">nul
if errorlevel 1 (
  set "PATH=%INTELFPGAOCLSDKROOT%\llvm\aocl-bin;%PATH%"
)

echo Adding %INTELFPGAOCLSDKROOT%\host\windows64\bin to PATH
echo %PATH%|findstr /i /c:"%INTELFPGAOCLSDKROOT%\host\windows64\bin">nul
if errorlevel 1 (
  set "PATH=%INTELFPGAOCLSDKROOT%\host\windows64\bin;%PATH%"
)

REM Silently add AOCL_BOARD_PACKAGE_ROOT to PATH if it is defined
if not "x%AOCL_BOARD_PACKAGE_ROOT%" == "x" (
  echo "%PATH%"|findstr /i /c:"%AOCL_BOARD_PACKAGE_ROOT%\windows64\bin">nul
  if errorlevel 1 (
    set "PATH=%AOCL_BOARD_PACKAGE_ROOT%\windows64\bin;%PATH%"
  )
)

REM Check that we now have aoc (or aocl) in our path
if  %RTE_ONLY% == 0 (
  set AOC_OR_AOCL="aoc"
  where /Q aoc.exe 2> NUL
) else (
  set AOC_OR_AOCL="aocl"
  where /Q aocl.exe 2> NUL
)
if %errorlevel% == 0 goto FOUND_A
  echo Error: Cannot find %AOC_OR_AOCL%. Please make sure this script is located in the installation's directory and run the script
  exit /b 1;

:FOUND_A
:end
set RTE_ONLY=
set AOC_OR_AOCL=
