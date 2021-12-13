@echo off
build_win.bat && python3 ../../scripts/storage.py -p COM25 send build/plugin-striped.elf /ext/plugin.elf -f