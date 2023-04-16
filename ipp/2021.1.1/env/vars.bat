@echo off
::
:: Copyright 1999-2020 Intel Corporation All Rights Reserved.
:: 
:: The source code, information and material ("Material") contained herein is
:: owned by Intel Corporation or its suppliers or licensors, and title
:: to such Material remains with Intel Corporation or its suppliers or
:: licensors. The Material contains proprietary information of Intel
:: or its suppliers and licensors. The Material is protected by worldwide
:: copyright laws and treaty provisions. No part of the Material may be used,
:: copied, reproduced, modified, published, uploaded, posted, transmitted,
:: distributed or disclosed in any way without Intel's prior express written
:: permission. No license under any patent, copyright or other intellectual
:: property rights in the Material is granted to or conferred upon you,
:: either expressly, by implication, inducement, estoppel or otherwise.
:: Any license under such intellectual property rights must be express and
:: approved by Intel in writing.
:: 
:: Unless otherwise agreed by Intel in writing,
:: you may not remove or alter this notice or any other notice embedded in
:: Materials by Intel or Intel's suppliers or licensors in any way.
::

:: Cache some environment variables.
set "IPPROOT=%~d0%~p0"
set "IPPROOT=%IPPROOT:\env\=%"
set "SCRIPT_NAME=%~nx0"

:: Set the default arguments
set IPP_TARGET_ARCH=intel64
set IPP_TARGET_PLATFORM=

:ParseArgs
:: Parse the incoming arguments
if not "%1"=="" (
	if "%1"=="ia32" (
		set "IPP_TARGET_ARCH=ia32"
		goto Build
	)
	if "%1"=="intel64" (
		set "IPP_TARGET_ARCH=intel64"
		goto Build
	)
	shift
	goto ParseArgs
)

:Build
:: main actions
set "LIB=%IPPROOT%\lib\%IPP_TARGET_ARCH%;%LIB%"
set "LIBRARY_PATH=%IPPROOT%\lib\%IPP_TARGET_ARCH%;%LIBRARY_PATH%"
if Exist "%IPPROOT%\redist\%IPP_TARGET_ARCH%" set "PATH=%IPPROOT%\redist\%IPP_TARGET_ARCH%;%PATH%"

set "INCLUDE=%IPPROOT%\include;%INCLUDE%"
set "CPATH=%IPPROOT%\include;%CPATH%"

:End
exit /B 0
