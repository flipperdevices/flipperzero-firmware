@echo off
setlocal EnableDelayedExpansion

set CLI_FOUND_FOLLOW_UP=0
set CLI_TEMP=%TEMP%\arduino-cli
set COMPILE_FLAG=firmware\compile.flag
set CONFIG_FILE=--config-file .\arduino-cli.yaml
set DEFAULT_BOARD_FQBN=esp32:esp32:esp32cam
set FIRMWARE_SRC=firmware\firmware.ino
set SELECTED_BOARD=%DEFAULT_BOARD_FQBN%

chcp 65001 > nul
echo ┏┓   ┓    ┏┳┓  ┓      
echo ┃ ┏┓┏┫┓┏   ┃ ┏┓┃┏┓┏┓┏┓
echo ┗┛┗┛┗┻┗┫   ┻ ┗┛┗┗ ┛┗┗ 
echo        ┛  https://github.com/CodyTolene
echo.
echo Flipper Zero - ESP32-CAM Firmware Flasher - Windows 10+
echo https://github.com/CodyTolene/Flipper-Zero-Camera-Suite
echo.
echo ------------------------------------------------------------------------------
echo Before you begin please make sure your Flipper Zero is plugged into your PC.
echo Then on your Flipper Zero, open the GPIO menu and select USB-UART Bridge.
echo ------------------------------------------------------------------------------
echo.
pause
echo.
echo Initializing...

:checkCLI
if not exist "arduino-cli.exe" (
    echo.
    echo The "arduino-cli.exe" file cannot be found. Please download it manually from the following link: 
    echo https://arduino.github.io/arduino-cli/latest/installation/#download
    echo Extract the "arduino-cli.exe" file to the same directory as this script, root of the project.
    echo.
    echo When the file is ready, press any key to check again.
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
set DATA_FLAG=0
if not exist "%CLI_TEMP%\data" (
    set /a "DATA_FLAG+=1"
)
if not exist "%CLI_TEMP%\downloads" (
    set /a "DATA_FLAG+=1"
)
if %DATA_FLAG% gtr 0 (
    arduino-cli %CONFIG_FILE% core update-index
    arduino-cli %CONFIG_FILE% core install esp32:esp32
) else (
    echo Assets already installed. Skipping...
)

if not exist "%COMPILE_FLAG%" (
    goto :compileFirmware
)

if exist "%COMPILE_FLAG%" (
    echo.
    set /p RECOMPILE="A previous firmware build was found, would you like to use it? (Y/N): "
    if /i "!RECOMPILE!"=="N" (
        goto :compileFirmware
    )
)
:exitCompileFirmware

echo.
echo Firmware ready for serial installation...
echo Fetching and displaying USB devices for upload...
echo.
arduino-cli board list
echo Please find your Flipper Zero USB port name from the list above (may show as unknown).
set /p PORT_NUMBER="Enter the port name here. For example 'COM3' capitalized with no quotes: "
echo.
echo Your ESP32-CAM is ready to be flashed. Please follow the instructions below:

:uploadFirmware
echo.
echo 1. Ensure IO0 pin on ESP32-CAM is grounded to the proper GND pin.
echo 2. Hold reset, and plug in your ESP32-CAM; release reset a few seconds later.
echo 3. As you proceed, release the reset button simultaneously with the next step.
echo 4. ESP32-CAM should now be in flash mode; allow some time for firmware upload.
echo 5. Failure is common; persist and verify all connections if errors persist.
echo 6. Disconnecting and reconnecting USB between attempts may sometimes work.
echo.
pause

set RETRY_COUNT=1

:uploadLoop
echo.
echo Preparing firmware upload... Attempt number !RETRY_COUNT!...
arduino-cli %CONFIG_FILE% upload -p %PORT_NUMBER% --fqbn !SELECTED_BOARD! %FIRMWARE_SRC%
if !ERRORLEVEL! EQU 0 (
    goto :uploadSuccess
) else (
    if !RETRY_COUNT! lss 3 (
        set /a RETRY_COUNT+=1
        goto :uploadLoop
    ) else (
        echo.
        set /p UPLOAD_TRY_AGAIN="Upload failed after 3 attempts, don't give up! Would you like to try again? (Y/N): "
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
echo.
echo Compiling firmware, this will take a moment...
echo.
arduino-cli %CONFIG_FILE% compile --fqbn !SELECTED_BOARD! %FIRMWARE_SRC%
if %ERRORLEVEL% EQU 0 (
    echo.
    echo Firmware compiled successfully.
    type nul > %COMPILE_FLAG%
) else (
    echo Firmware failed to compile. Please see the error log and try again.
    exit /b
)

goto :exitCompileFirmware
