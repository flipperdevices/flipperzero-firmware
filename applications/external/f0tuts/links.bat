@echo off
if NOT exist assets ( goto :noAssetsFolder ) 
if NOT exist applications ( goto :noApplicationsFolder )

set TUTORIAL_PATH=C:\repo\flipper\flipper-zero-tutorials

copy %TUTORIAL_PATH%\badusb\ddj-400.txt assets\resources\badusb\ddj-400.txt
copy %TUTORIAL_PATH%\badusb\demo-backspace-del.txt assets\resources\badusb\demo-backspace-del.txt 
copy %TUTORIAL_PATH%\badusb\sub-raw-to-csv.txt assets\resources\badusb\sub-raw-to-csv.txt 

pushd assets\resources
mklink /D pins %TUTORIAL_PATH%\gpio\pins
popd

pushd applications\plugins
mklink /D gpio_7segment %TUTORIAL_PATH%\gpio\gpio_7segment
mklink /D gpio_interrupt_demo %TUTORIAL_PATH%\gpio\gpio_interrupt_demo
mklink /D gpio_polling_demo %TUTORIAL_PATH%\gpio\gpio_polling_demo
mklink /D hc_sr04 %TUTORIAL_PATH%\gpio\hc_sr04
mklink /D memsic_2125 %TUTORIAL_PATH%\gpio\memsic_2125
popd

pushd applications\plugins
mklink /D basic_demo %TUTORIAL_PATH%\plugins\basic
popd

pushd applications\plugins
mklink /D rock_paper_scissors %TUTORIAL_PATH%\subghz\plugins\rock_paper_scissors
mklink /D subghz_demo %TUTORIAL_PATH%\subghz\plugins\subghz_demo
popd

ECHO.
ECHO TODO: subghz\protocols\x10


goto :EOF

:noAssetsFolder
ECHO no assets folder!
goto :EOF

:noApplicationsFolder
ECHO no applications folder!
goto :EOF