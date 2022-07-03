@echo off


if not exist ".git" (
    echo ".git" directory not found, please clone repo via "git clone --recursive"
    exit /B 1
)
git submodule update --init

set "SCONS_DEFAULT_FLAGS=-Q --warn=target-not-built"
python lib\scons\scripts\scons.py %SCONS_DEFAULT_FLAGS% %*
