echo >/dev/null # >nul & GOTO WINDOWS & rem ^
# ***********************************************************
# * NOTE: If you modify this content, be sure to remove carriage returns (\r)\n
# *       from the Linux part and leave them in together with the line feeds
# *       (\n) for the Windows part. In summary:
# *           New lines in Linux: \n\n
# *           New lines in Windows: \r\n
# ***********************************************************
toolchain/scripts/unix-toolchain-download.sh
exit 0
:WINDOWS
@echo off
cls
powershell -ExecutionPolicy Bypass -File toolchain\scripts\windows-toolchain-download.ps1
