@echo off
setlocal EnableDelayedExpansion

rem λ

set CLI_TEMP=%TEMP%\arduino-cli
set ARDUINO_CLI_CONFIG_FILE=--config-file %CD%\firmware\arduino-cli.yaml
set CLI_FOUND_FOLLOW_UP=0
set GITHUB_HOOKS_FOLDER=%CD%\.github\hooks
set GIT_HOOKS_FOLDER=%CD%\.git\hooks

chcp 65001 > nul
echo ┏┓   ┓    ┏┳┓  ┓
echo ┃ ┏┓┏┫┓┏   ┃ ┏┓┃┏┓┏┓┏┓
echo ┗┛┗┛┗┻┗┫   ┻ ┗┛┗┗ ┛┗┗
echo        ┛  https://github.com/CodyTolene
echo.
echo Flipper Zero - ESP32-CAM Development Assets - Windows 10+
echo https://github.com/CodyTolene/Flipper-Zero-Camera-Suite
echo.
echo ------------------------------------------------------------------------------
echo This will install all assets needed to get you started with ESP32-CAM firmware
echo development. These files will be installed to the following directory:
echo.
echo "%CLI_TEMP%"
echo.
echo Once installed you can add them to the "Include path" in your IDE of choice.
echo.
echo Notes:
echo - Development asset files will take up approx. 3.5GB of storage space.
echo - You can reinstall or delete the asset files by re-running this script.
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

echo Checking and setting arduino-cli config...
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
    :installAssets
    arduino-cli %ARDUINO_CLI_CONFIG_FILE% core update-index
    arduino-cli %ARDUINO_CLI_CONFIG_FILE% core install esp32:esp32
    arduino-cli %ARDUINO_CLI_CONFIG_FILE% lib install --git-url https://github.com/me-no-dev/ESPAsyncWebServer.git
    arduino-cli %ARDUINO_CLI_CONFIG_FILE% lib install --git-url https://github.com/me-no-dev/AsyncTCP.git
    arduino-cli %ARDUINO_CLI_CONFIG_FILE% lib install --git-url https://github.com/me-no-dev/ESPAsyncTCP.git
    goto :wrapUp
) else (
    set /p SHOULD_REINSTALL="Assets already installed. Reinstall? (Y/N): "
    if /i "!SHOULD_REINSTALL!"=="Y" (
        rmdir /s /q %CLI_TEMP%
        goto :installAssets
    )
    set /p DELETE_TEMP="Would you like to remove the previously installed dependencies? (Y/N): "
    if /i "!DELETE_TEMP!"=="Y" (
        rmdir /s /q %CLI_TEMP%
        goto :end
    )
)

:wrapUp
echo Configuring Git pre-commit hook...
copy /Y "%GITHUB_HOOKS_FOLDER%" "%GIT_HOOKS_FOLDER%"
echo Resetting arduino-cli config back to defaults...
arduino-cli %ARDUINO_CLI_CONFIG_FILE% config set directories.data C:\temp\arduino-cli\data
arduino-cli %ARDUINO_CLI_CONFIG_FILE% config set directories.downloads C:\temp\arduino-cli\staging
arduino-cli %ARDUINO_CLI_CONFIG_FILE% config set directories.user C:\temp\arduino-cli\user
arduino-cli %ARDUINO_CLI_CONFIG_FILE% config set library.enable_unsafe_install false

echo.
echo The ESP32-CAM development dependencies were installed successfully.
echo.
echo ------------------------------------------------------------------------------
echo.
echo You can now add the following path to your IDEs "Include path" setting:
echo.
echo "%CLI_TEMP%\**"
echo.

:end
echo ------------------------------------------------------------------------------
echo.
echo Fin - Happy programming friend.
echo.
pause
exit /b
