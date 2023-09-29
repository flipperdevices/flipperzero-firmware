@echo off
setlocal EnableDelayedExpansion

set CLI_TEMP=%TEMP%\arduino-cli
set CONFIG_FILE=--config-file .\arduino-cli.yaml
set DEFAULT_BOARD_FQBN=esp32:esp32:esp32cam
set SELECTED_BOARD=%DEFAULT_BOARD_FQBN%
set CLI_FOUND_FOLLOW_UP=0
set COMPILE_FLAG=..\compile.flag

echo.

:checkCLI
if not exist "arduino-cli.exe" (
    echo The "arduino-cli.exe" file cannot be found. Please download it manually from the following link: 
    echo https://arduino.github.io/arduino-cli/latest/installation/#download
    set /a CLI_FOUND_FOLLOW_UP+=1
    if %CLI_FOUND_FOLLOW_UP% geq 2 (
        echo If you're still having issues, feel free to open a ticket at the following link:
        echo https://github.com/CodyTolene/Flipper-Zero-Camera-Suite/issues
    )
    pause
    goto :checkCLI
)
if %CLI_FOUND_FOLLOW_UP% geq 1 (
    echo File "arduino-cli.exe" found. Continuing...
)

echo Checking configs...
arduino-cli %CONFIG_FILE% config set directories.data %CLI_TEMP%\data
arduino-cli %CONFIG_FILE% config set directories.downloads %CLI_TEMP%\downloads
arduino-cli %CONFIG_FILE% config set directories.user %CLI_TEMP%\user %*

echo Fetching assets...
if not exist "%CLI_TEMP%" (
    arduino-cli %CONFIG_FILE% core update-index
    arduino-cli %CONFIG_FILE% core install esp32:esp32
) else (
    echo Assets already installed. Skipping...
)

echo Ready for installation...

set /p USE_DEFAULT_BOARD="Install to default AI-Thinker ESP32-CAM board with FQBN '%DEFAULT_BOARD_FQBN%'? (Y/N): "
if /i "%USE_DEFAULT_BOARD%"=="N" (
    set /p SHOW_BOARDS="Display all possible ESP32 board names and FQBN's? (Y/N): "
    if /i "!SHOW_BOARDS!"=="Y" (
        echo.
        arduino-cli board listall
    )
    set /p SELECTED_BOARD="Please enter your board FQBN. For example '%DEFAULT_BOARD_FQBN%' with no quotes: "
)

if exist "%COMPILE_FLAG%" (
    set /p RECOMPILE="A previous firmware build was found, would you like to use it? (Y/N): "
    if /i "!RECOMPILE!"=="N" (
        goto :compileFirmware
    )
) else (
    :compileFirmware
    echo Compiling firmware, this will take a moment...
    arduino-cli %CONFIG_FILE% compile --fqbn !SELECTED_BOARD! ..\firmware.ino
    if %ERRORLEVEL% EQU 0 (
        echo Compile complete. Ready to upload.
        type nul > %COMPILE_FLAG%
    ) else (
        echo Compilation failed. Please correct the errors and try again.
        exit /b
    )
)

echo.
arduino-cli board list
echo Please find your Flipper Zero USB port from the list above (may show as unknown).
set /p PORT_NUMBER="Enter it here. For example 'COM3' capitalized with no quotes: "

echo.
echo Your ESP32-CAM is ready to be flashed. Please follow the instructions below.

:uploadFirmware
echo.
echo 1. Make sure your ESP32-CAM module is unplugged from your Flipper Zero.
echo 2. Make sure you've grounded your IO0 pin on your ESP32-CAM module to the correct GND pin.
echo 3. Plug your ESP32-CAM module with the reset button pressed right after going to the next step.
echo 4. When "Connecting..." is displayed unpress the reset button.
echo 5. It is common for this to fail a few times, keep trying and double check your connections.
echo.
pause

echo.
echo Preparing firmware upload...
arduino-cli %CONFIG_FILE% upload -p %PORT_NUMBER% --fqbn !SELECTED_BOARD! ..\firmware.ino
if %ERRORLEVEL% NEQ 0 (
    echo.
    set /p UPLOAD_TRY_AGAIN="Upload failed, would you like to retry? (Y/N): "
    if /i "!UPLOAD_TRY_AGAIN!"=="Y" (
        goto :uploadFirmware
    )
)

echo.
echo Fin. Happy programming friend.
echo.
pause
exit /b
