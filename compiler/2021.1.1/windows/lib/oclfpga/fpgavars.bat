@echo off

set "FPGA_SCRIPTDIR=%~dp0"
set "FPGA_SCRIPTDIR=%FPGA_SCRIPTDIR:~0,-1%"

call "%FPGA_SCRIPTDIR%\init_opencl.bat" > NUL
IF NOT DEFINED OCL_ICD_FILENAMES (
  set "OCL_ICD_FILENAMES=intelocl64_emu.dll"
) else (
  set "OCL_ICD_FILENAMES=intelocl64_emu.dll;%OCL_ICD_FILENAMES%"
)

:: clear temp globals
set FPGA_SCRIPTDIR=
exit /B 0

