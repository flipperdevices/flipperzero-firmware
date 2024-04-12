@echo off
set loopCount=10
:loop
pytest --port="COM18" -m smoke
set /a loopCount=%loopCount%-1
if %loopCount%==0 GOTO:EOF
GOTO :loop
