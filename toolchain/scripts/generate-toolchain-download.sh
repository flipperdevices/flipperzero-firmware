#!/bin/bash
SCRIPT_PATH="$(dirname -- "$(readlink -f -- "$0")")";
PROJECT_ROOT="$(cd "$SCRIPT_PATH/../../" && pwd)";
printf "echo >/dev/null # >nul & GOTO WINDOWS & rem ^\r\n" > "$PROJECT_ROOT/toolchain-download.sh.bat"
printf "# ***********************************************************\n" >> "$PROJECT_ROOT/toolchain-download.sh.bat"
printf "# * NOTE: If you modify this content, be sure to remove carriage returns (\\\r)\\\n\n" >> "$PROJECT_ROOT/toolchain-download.sh.bat"
printf "# *       from the Linux part and leave them in together with the line feeds\n" >> "$PROJECT_ROOT/toolchain-download.sh.bat"
printf "# *       (\\\n) for the Windows part. In summary:\n" >> "$PROJECT_ROOT/toolchain-download.sh.bat"
printf "# *           New lines in Linux: \\\n\\\n\n" >> "$PROJECT_ROOT/toolchain-download.sh.bat"
printf "# *           New lines in Windows: \\\r\\\n\n" >> "$PROJECT_ROOT/toolchain-download.sh.bat"
printf "# ***********************************************************\n" >> "$PROJECT_ROOT/toolchain-download.sh.bat"
printf "toolchain/scripts/unix-toolchain-download.sh\n" >> "$PROJECT_ROOT/toolchain-download.sh.bat"
printf "exit 0\n" >> "$PROJECT_ROOT/toolchain-download.sh.bat"
printf ":WINDOWS\r\n" >> "$PROJECT_ROOT/toolchain-download.sh.bat"
printf "@echo off\r\n" >> "$PROJECT_ROOT/toolchain-download.sh.bat"
printf "cls\r\n" >> "$PROJECT_ROOT/toolchain-download.sh.bat"
printf "powershell -ExecutionPolicy Bypass -File toolchain\\\scripts\\\windows-toolchain-download.ps1\r\n" >> "$PROJECT_ROOT/toolchain-download.sh.bat"

chmod +x "$PROJECT_ROOT/toolchain-download.sh.bat"
