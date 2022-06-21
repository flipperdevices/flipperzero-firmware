@echo off
set "SCONS_DEFAULT_FLAGS=-Q --warn=target-not-built"
python lib/scons/scripts/scons.py %SCONS_DEFAULT_FLAGS% %*
