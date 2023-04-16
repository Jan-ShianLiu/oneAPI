@echo off

set "FPGA_SCRIPTDIR=%~dp0"
set "FPGA_SCRIPTDIR=%FPGA_SCRIPTDIR:~0,-1%"

IF "%INTELFPGAOCLSDKROOT%"=="" (
  echo Warning: INTELFPGAOCLSDKROOT is not set.
  echo          Please run oneAPI setvars.bat to set it.
) ELSE (
  IF NOT "%INTELFPGAOCLSDKROOT%"=="%FPGA_SCRIPTDIR%" (
    echo Warning: INTELFPGAOCLSDKROOT is set to "%INTELFPGAOCLSDKROOT%".
    echo          oneAPI DPC++ Compiler FPGA backend's INTELFPGAOCLSDKROOT is "%FPGA_SCRIPTDIR%".
  )
)

exit /b 0
