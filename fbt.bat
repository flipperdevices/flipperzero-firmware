set "toolchainRoot=%~dp0toolchain\i686-windows"
set "SCONS_DEFAULT_FLAGS=-Q --warn=target-not-built"
if not exist "%toolchainRoot%" (powershell -ExecutionPolicy Bypass -File %~dp0scripts\toolchain\windows-toolchain-download.ps1)
cmd /V /C "set "PATH=%toolchainRoot%\python;%toolchainRoot%\bin;%toolchainRoot%\protoc\bin;%PATH%" && python lib\scons\scripts\scons.py %SCONS_DEFAULT_FLAGS% %*"
