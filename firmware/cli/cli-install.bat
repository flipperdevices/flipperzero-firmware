@echo off
setlocal EnableDelayedExpansion

set CLI_TEMP=%TEMP%\arduino-cli
set CONFIG_FILE=--config-file .\arduino-cli.yaml
set DEFAULT_BOARD_FQBN=esp32:esp32:esp32cam
set SELECTED_BOARD=%DEFAULT_BOARD_FQBN%
set CLI_FOUND_FOLLOW_UP=0
set COMPILE_FLAG=compile.flag

echo Initializing...

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

echo.
echo -------------------------------------------------------------------------
echo Make sure your Flipper Zero is plugged in via a transfer capable USB.
echo Then on your Flipper Zero, open the GPIO menu and select USB-UART Bridge.
echo If you have already done this, you may skip this step.
echo -------------------------------------------------------------------------
echo.
pause

echo.
echo Ready for installation...

if not exist "%COMPILE_FLAG%" (
    set /p USE_DEFAULT_BOARD="Install to default AI-Thinker ESP32-CAM board with FQBN '%DEFAULT_BOARD_FQBN%'? (Y/N): "
    if /i "%USE_DEFAULT_BOARD%"=="N" (
        echo Warning - This script has not been tested with other boards. Please use at your own risk.
        set /p SHOW_BOARDS="Display all possible ESP32 board names and FQBN's? (Y/N): "
        if /i "!SHOW_BOARDS!"=="Y" (
            echo.
            arduino-cli board listall
        )
        set /p SELECTED_BOARD="Please enter your board FQBN. For example '%DEFAULT_BOARD_FQBN%' with no quotes: "
    )
    goto :compileFirmware
)

if exist "%COMPILE_FLAG%" (
    set /p RECOMPILE="A previous firmware build was found, would you like to use it? (Y/N): "
    if /i "!RECOMPILE!"=="N" (
        goto :compileFirmware
    )
)

:continue
echo.
arduino-cli board list
echo Please find your Flipper Zero USB port from the list above (may show as unknown).
set /p PORT_NUMBER="Enter it here. For example 'COM3' capitalized with no quotes: "

echo.
echo Your ESP32-CAM is ready to be flashed. Please follow the instructions below.

:uploadFirmware
echo.
echo 1. Make sure you've grounded your IO0 pin on your ESP32-CAM module to the correct GND pin.
echo 2. Plug in your ESP32-CAM module with the reset button pressed a few seconds before continuing to the next step.
echo 3. When continuing to the next step, simultaneously release the reset button.
echo 4. Your ESP32-CAM should now be in flash mode. Allow the firmware to upload, this will take a moment.
echo 5. It's not uncommon for this to fail many times, keep trying and double check your connections.
echo.
pause

set RETRY_COUNT=1

:uploadLoop
echo.
echo Preparing firmware upload... Attempt number !RETRY_COUNT!...
arduino-cli %CONFIG_FILE% upload -p %PORT_NUMBER% --fqbn !SELECTED_BOARD! ..\firmware.ino
if !ERRORLEVEL! EQU 0 (
    goto :uploadSuccess
) else (
    if !RETRY_COUNT! lss 3 (
        set /a RETRY_COUNT+=1
        goto :uploadLoop
    ) else (
        echo.
        set /p UPLOAD_TRY_AGAIN="Upload failed after 3 attempts, would you like to retry? (Y/N): "
        if /i "!UPLOAD_TRY_AGAIN!"=="Y" (
            set RETRY_COUNT=1
            goto :uploadFirmware
        ) else (
            echo.
            echo If you're still having issues, feel free to open a ticket at the following link:
            echo https://github.com/CodyTolene/Flipper-Zero-Camera-Suite/issues
        )
    )
)

:uploadSuccess
echo.
echo Firmware upload was successful.
echo.
echo Fin. Happy programming friend.
echo.
pause
exit /b

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

goto :continue
