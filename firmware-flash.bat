@echo off
setlocal EnableDelayedExpansion

rem λ

set CLI_FOUND_FOLLOW_UP=0
set CLI_TEMP=%TEMP%\arduino-cli
set COMPILE_FLAG=%CLI_TEMP%\.compile.flag
set ARDUINO_CLI_CONFIG_FILE=--config-file %CD%\firmware\arduino-cli.yaml
set DEFAULT_BOARD_FQBN=esp32:esp32:esp32cam
set FIRMWARE_SRC=%CD%\firmware\firmware.ino
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
echo Then on your Flipper Zero open the GPIO menu and select USB-UART Bridge. In
echo the USB-UART Bridge config menu make sure the following configuration is set:
echo - USB Channel = 1 (on newer firmware)
echo - Baudrate = Host
echo - UART Pins = 13 and 14
echo - RTS/DTR Pins = None
echo.
echo Notes:
echo - Temporary installation files will take up approx. 3.5GB of storage space.
echo - You will have the option to delete the temporary files on completion.
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
    echo Extract the "arduino-cli.exe" file to the same directory as this script.
    echo.
    echo When the file is ready press any key to check again.
    set /a CLI_FOUND_FOLLOW_UP+=1
    if %CLI_FOUND_FOLLOW_UP% geq 2 (
        echo If you are still having issues feel free to open a ticket at the following link:
        echo https://github.com/CodyTolene/Flipper-Zero-Camera-Suite/issues
    )
    pause
    goto :checkCLI
)
if %CLI_FOUND_FOLLOW_UP% geq 1 (
    echo File "arduino-cli.exe" found successfully. Continuing...
)

echo Checking and setting arduino-cli configs...
arduino-cli %ARDUINO_CLI_CONFIG_FILE% config set directories.data %CLI_TEMP%\data
arduino-cli %ARDUINO_CLI_CONFIG_FILE% config set directories.downloads %CLI_TEMP%\downloads
arduino-cli %ARDUINO_CLI_CONFIG_FILE% config set directories.user %CLI_TEMP%\user %*
rem Enable for Git installations (ie `arduino-cli lib install --git-url`).
rem @See "https://arduino.github.io/arduino-cli/0.35/configuration/#configuration-keys"
arduino-cli %ARDUINO_CLI_CONFIG_FILE% config set library.enable_unsafe_install true

echo Fetching assets...

set DATA_FLAG=0
if not exist "%CLI_TEMP%\data" (
    set /a "DATA_FLAG+=1"
)
if not exist "%CLI_TEMP%\downloads" (
    set /a "DATA_FLAG+=1"
)
if not exist "%CLI_TEMP%\user" (
    set /a "DATA_FLAG+=1"
)
if %DATA_FLAG% gtr 0 (
    arduino-cli %ARDUINO_CLI_CONFIG_FILE% core update-index
    arduino-cli %ARDUINO_CLI_CONFIG_FILE% core install esp32:esp32
    arduino-cli %ARDUINO_CLI_CONFIG_FILE% lib install --git-url https://github.com/me-no-dev/ESPAsyncWebServer.git
    arduino-cli %ARDUINO_CLI_CONFIG_FILE% lib install --git-url https://github.com/me-no-dev/AsyncTCP.git
    arduino-cli %ARDUINO_CLI_CONFIG_FILE% lib install --git-url https://github.com/me-no-dev/ESPAsyncTCP.git
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
echo 1. Remove ESP32-CAM. Ensure IO0 pin on ESP32-CAM is grounded to the proper GND pin.
echo 2. Hold reset and insert your ESP32-CAM; hold for a few seconds and release.
echo 3. Try to time your release simultaneously with continuing to the next step.
echo 4. ESP32-CAM should now be in flash mode; allow some time for firmware upload.
echo 5. Failure is common; verify all connections if errors persist and try again.
echo 6. Disconnecting and reconnecting USB between attempts may sometimes work.
echo.
pause

set RETRY_COUNT=1

:uploadLoop
echo.
echo Preparing firmware upload... Attempt number !RETRY_COUNT!...
arduino-cli %ARDUINO_CLI_CONFIG_FILE% upload -p %PORT_NUMBER% --fqbn !SELECTED_BOARD! %FIRMWARE_SRC%
if !ERRORLEVEL! EQU 0 (
    goto :uploadSuccess
) else (
    if !RETRY_COUNT! lss 5 (
        set /a RETRY_COUNT+=1
        goto :uploadLoop
    ) else (
        echo.
        set /p UPLOAD_TRY_AGAIN="Upload failed after 5 attempts, dont give up friend. Would you like to try again? (Y/N): "
        if /i "!UPLOAD_TRY_AGAIN!"=="Y" (
            set RETRY_COUNT=1
            goto :uploadFirmware
        ) else (
            echo.
            echo If you are still having issues feel free to open a ticket at the following link:
            echo https://github.com/CodyTolene/Flipper-Zero-Camera-Suite/issues
            echo.
            set /p DELETE_TEMP="Would you like to delete the temporary files? (Y/N): "
            if /i "!DELETE_TEMP!"=="Y" (
                rmdir /s /q %CLI_TEMP%
            )
            echo.
            pause
            exit /b
        )
    )
)

:uploadSuccess
echo.
echo Firmware upload was successful.
echo Cleaning up...
echo Resetting arduino-cli config back to defaults...
arduino-cli %ARDUINO_CLI_CONFIG_FILE% config set directories.data C:\temp\arduino-cli\data
arduino-cli %ARDUINO_CLI_CONFIG_FILE% config set directories.downloads C:\temp\arduino-cli\staging
arduino-cli %ARDUINO_CLI_CONFIG_FILE% config set directories.user C:\temp\arduino-cli\user
arduino-cli %ARDUINO_CLI_CONFIG_FILE% config set library.enable_unsafe_install false
set /p DELETE_TEMP="Would you like to delete the temporary files? (Y/N): "
if /i "!DELETE_TEMP!"=="Y" (
    rmdir /s /q %CLI_TEMP%
)
echo.
echo Fin - Happy programming friend.
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
echo Compiling firmware - this will take a moment...
echo.
arduino-cli %ARDUINO_CLI_CONFIG_FILE% compile --fqbn !SELECTED_BOARD! %FIRMWARE_SRC%
if %ERRORLEVEL% EQU 0 (
    echo.
    echo Firmware compiled successfully.
    type nul > %COMPILE_FLAG%
) else (
    echo.
    set /p TRY_COMPILE_AGAIN="Firmware failed to compile. Please see the error log above. Try again? (Y/N): "
    echo.
    if /i "!TRY_COMPILE_AGAIN!"=="Y" (
        goto :compileFirmware
    )
    echo Cleaning up...
    echo Resetting arduino-cli config back to defaults...
    arduino-cli %ARDUINO_CLI_CONFIG_FILE% config set directories.data C:\temp\arduino-cli\data
    arduino-cli %ARDUINO_CLI_CONFIG_FILE% config set directories.downloads C:\temp\arduino-cli\staging
    arduino-cli %ARDUINO_CLI_CONFIG_FILE% config set directories.user C:\temp\arduino-cli\user
    arduino-cli %ARDUINO_CLI_CONFIG_FILE% config set library.enable_unsafe_install false
    set /p DELETE_TEMP="Would you like to delete the temporary files? (Y/N): "
    if /i "!DELETE_TEMP!"=="Y" (
        rmdir /s /q %CLI_TEMP%
    )
    echo Cleanup completed - press any key to exit.
    echo.
    pause
    exit /b
)

goto :exitCompileFirmware
