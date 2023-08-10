@echo off
REM Expect the script at /path/to/wolfssl/IDE/Espressif/ESP-IDF/

::******************************************************************************************************
::******************************************************************************************************
echo;
echo wolfSSL Windows Setup. Version 1.0a
echo;
echo This utility will copy a static snapshot of wolfSSL files to the ESP32-IDF component directory.
echo;
echo If you wish to keep your component library fresh with git pull, consider hard link with mklink.
echo;
echo    "mklink [[/d] | [/h] | [/j]] <link> <target>"
echo;
::******************************************************************************************************
::******************************************************************************************************
SET COPYERROR=false

:: if there's a setup.sh, we are probably starting in the right place.
if NOT EXIST "setup.sh" (
  echo Please run this script at /path/to/wolfssl/IDE/Espressif/ESP-IDF/
  goto :ERR
)

:: if there's also a default user_settings.h, we are very likely starting in the right place.
if NOT EXIST "user_settings.h" (
  echo Please run this script at /path/to/wolfssl/IDE/Espressif/ESP-IDF/
  goto :ERR
)

:: see if there was a parameter passed for a specific EDP-IDF directory
:: this may be different than the standard ESP-IDF environment (e.g. VisualGDB)
if not "%1" == "" (
	if not exist "%1" (
		echo "ERROR: optional directory was specified, but not found: %1"
		goto :ERR
	)

	SET IDF_PATH=%1
	echo Using specified IDF_PATH: %IDF_PATH%
)

:: if no IDF_PATH is found, we don't know what to do. Go exit with error.
if "%IDF_PATH%" == "" (
  echo Please launch the script from ESP-IDF command prompt,
  echo or set your desired IDF_PATH environment variable,
  echo or pass a parameter to your directory, such as for VisualGDB with ESP-IDF 4.4:
  echo;
  echo   .\setup_win.bat C:\SysGCC\esp32\esp-idf\v4.4
  echo;
  goto :ERR
)

:: Here we go!
:: setup some path variables
echo;

set SCRIPTDIR=%CD%
set BASEDIR=%SCRIPTDIR%\..\..\..
set WOLFSSL_ESPIDFDIR=%BASEDIR%\IDE\Espressif\ESP-IDF
set WOLFSSLLIB_TRG_DIR=%IDF_PATH%\components\wolfssl
set WOLFSSLEXP_TRG_DIR=%IDF_PATH%\examples\protocols

echo Using SCRIPTDIR          = %SCRIPTDIR%
echo Using BASEDIR            = %BASEDIR%
echo Using WOLFSSL_ESPIDFDIR  = %WOLFSSL_ESPIDFDIR%
echo Using WOLFSSLLIB_TRG_DIR = %WOLFSSLLIB_TRG_DIR%
echo Using WOLFSSLEXP_TRG_DIR = %WOLFSSLEXP_TRG_DIR%

echo;
echo Equivalalent destination path:
dir %WOLFSSL_ESPIDFDIR%\*.xyzzy 2> nul | findstr  \

echo;
echo Equivalalent source directory paths:
:: show the path of the equivalent  %VALUE% (search for files that don't exist, suppress error, and look for string with "\")
dir %BASEDIR%\*.xyzzy 2> nul | findstr  \
dir %WOLFSSLLIB_TRG_DIR%\*.xyzzy 2> nul | findstr  \
dir %WOLFSSLEXP_TRG_DIR%\*.xyzzy 2> nul | findstr  \

:: set the FileStamp variable to the current date:  YYMMYY_HHMMSS
:: the simplest method, to use existing TIME ad DATE variables:
:: date = Thu 09/17/2015
:: time = 11:13:15.47
::        012345678901234567890
::
:: There is no leading zero for single digit hours (e.g. 9:00am), so we need to manually include the zero
::                                                                      here  |
if     "%TIME:~0,1%" == " "  set FileStamp=%DATE:~12,2%%DATE:~7,2%%DATE:~4,2%_0%TIME:~1,1%%TIME:~3,2%%TIME:~6,2%

:: otherwise, if a space not found before the digit, it is a 2 digit hour, so no extract zero is needed
if NOT "%TIME:~0,1%" == " "  set FileStamp=%DATE:~12,2%%DATE:~7,2%%DATE:~4,2%_%TIME:~0,2%%TIME:~3,2%%TIME:~6,2%

:: Backup existing user settings
if exist %WOLFSSLLIB_TRG_DIR%\include\config.h (
  echo;
  echo Saving: %WOLFSSLLIB_TRG_DIR%\include\config.h
  echo     to: %SCRIPTDIR%\config_h_%FileStamp%.bak
  copy         %WOLFSSLLIB_TRG_DIR%\include\config.h      %SCRIPTDIR%\config_h_%FileStamp%.bak
  echo;
)

if exist %WOLFSSL_ESPIDFDIR%\user_settings.h (
  echo Saving: %WOLFSSLLIB_TRG_DIR%\include\user_settings.h
  echo     to: %SCRIPTDIR%\user_settings_h_%FileStamp%.bak
  copy         %WOLFSSLLIB_TRG_DIR%\include\user_settings.h      %SCRIPTDIR%\user_settings_h_%FileStamp%.bak
  echo;
)


::******************************************************************************************************
:: check if there's already an existing %WOLFSSLLIB_TRG_DIR% and confirm removal
::******************************************************************************************************
if exist %WOLFSSLLIB_TRG_DIR% (
    echo;
    echo WARNING: Existing files found in %WOLFSSLLIB_TRG_DIR%
    echo;

    :: clear any prior errorlevel
    call;
    choice /c YN /m "Delete files and proceed with install in %WOLFSSLLIB_TRG_DIR%  "
    if errorlevel 2 GOTO :NODELETE
    GOTO :PURGE


    echo;
    echo Ready to copy files into %IDF_PATH%


::******************************************************************************************************
:NODELETE
::******************************************************************************************************
  :: clear any prior errorlevel
    echo;
    call;
    choice /c YN /m "Refresh files %WOLFSSLLIB_TRG_DIR%   (there will be a prompt to keep or overwrite user_settings and config)  "
    if errorlevel 2 GOTO :NOCOPY
    GOTO :REFRESH
)


::******************************************************************************************************
:PURGE
::******************************************************************************************************
:: purge existing directory

if exist %WOLFSSLLIB_TRG_DIR% (
    echo;
    echo Removing %WOLFSSLLIB_TRG_DIR%
    rmdir %WOLFSSLLIB_TRG_DIR% /S /Q
    if exist %WOLFSSLLIB_TRG_DIR% (
        SET COPYERROR=true
        echo;
	    echo WARNING: Failed to remove %WOLFSSLLIB_TRG_DIR%
	    echo;
	    echo Check permissions, open files, read-only attributes, etc.
	    echo;
    )
    echo;
) else (
    echo;
    echo Prior %WOLFSSLLIB_TRG_DIR% not found, installing fresh.
    echo;
)

::******************************************************************************************************
:REFRESH
::******************************************************************************************************
if not exist %WOLFSSLLIB_TRG_DIR%                 mkdir      %WOLFSSLLIB_TRG_DIR%
if not exist %WOLFSSLLIB_TRG_DIR%\src             mkdir      %WOLFSSLLIB_TRG_DIR%\src
if not exist %WOLFSSLLIB_TRG_DIR%\wolfcrypt\src   mkdir      %WOLFSSLLIB_TRG_DIR%\wolfcrypt\src
if not exist %WOLFSSLLIB_TRG_DIR%\wolfssl         mkdir      %WOLFSSLLIB_TRG_DIR%\wolfssl
if not exist %WOLFSSLLIB_TRG_DIR%\wolfssl\openssl mkdir      %WOLFSSLLIB_TRG_DIR%\wolfssl\openssl
if not exist %WOLFSSLLIB_TRG_DIR%\test            mkdir      %WOLFSSLLIB_TRG_DIR%\test
if not exist %WOLFSSLLIB_TRG_DIR%\include         mkdir      %WOLFSSLLIB_TRG_DIR%\include


rem copying ... files in src/ into $WOLFSSLLIB_TRG_DIR%/src
echo;
echo Copying files to %WOLFSSLLIB_TRG_DIR%\src\
xcopy %BASEDIR%\src\*.c                                      %WOLFSSLLIB_TRG_DIR%\src\                        /S /E /Q /Y
if %errorlevel% NEQ 0 SET COPYERROR=true

echo;
echo Copying src\*.c files to %WOLFSSLLIB_TRG_DIR%\wolfcrypt\src
xcopy %BASEDIR%\wolfcrypt\src\*.c                            %WOLFSSLLIB_TRG_DIR%\wolfcrypt\src               /S /E /Q /Y
if %errorlevel% NEQ 0 SET COPYERROR=true

echo;
echo Copying src\*.i files to %WOLFSSLLIB_TRG_DIR%\wolfcrypt\src
xcopy %BASEDIR%\wolfcrypt\src\*.i                            %WOLFSSLLIB_TRG_DIR%\wolfcrypt\src               /S /E /Q /Y
if %errorlevel% NEQ 0 SET COPYERROR=true

echo;
echo Copying files to %WOLFSSLLIB_TRG_DIR%\wolfcrypt\src\port\
xcopy %BASEDIR%\wolfcrypt\src\port                           %WOLFSSLLIB_TRG_DIR%\wolfcrypt\src\port\         /S /E /Q /Y
if %errorlevel% NEQ 0 SET COPYERROR=true

echo;
echo Copying files to %WOLFSSLLIB_TRG_DIR%\wolfcrypt\test\
xcopy %BASEDIR%\wolfcrypt\test                               %WOLFSSLLIB_TRG_DIR%\wolfcrypt\test\             /S /E /Q /Y
if %errorlevel% NEQ 0 SET COPYERROR=true

:: Copy dummy test_paths.h to handle the case configure hasn't yet executed
echo;
echo Copying dummy_test_paths.h to %WOLFSSLLIB_TRG_DIR%\wolfcrypt\test\test_paths.h
echo new config                                            > %WOLFSSLLIB_TRG_DIR%\wolfcrypt\test\test_paths.h
if %errorlevel% NEQ 0 SET COPYERROR=true
xcopy %WOLFSSL_ESPIDFDIR%\dummy_test_paths.h                 %WOLFSSLLIB_TRG_DIR%\wolfcrypt\test\test_paths.h  /S /E /Y
if %errorlevel% NEQ 0 SET COPYERROR=true

echo;
echo Copying files to %WOLFSSLLIB_TRG_DIR%\wolfcrypt\benchmark\
xcopy %BASEDIR%\wolfcrypt\benchmark                          %WOLFSSLLIB_TRG_DIR%\wolfcrypt\benchmark\          /S /E /Q /Y
if %errorlevel% NEQ 0 SET COPYERROR=true

echo;
echo Copying files to %WOLFSSLLIB_TRG_DIR%\wolfssl\
xcopy %BASEDIR%\wolfssl\*.h                                  %WOLFSSLLIB_TRG_DIR%\wolfssl\                    /S /E /Q /Y
if %errorlevel% NEQ 0 SET COPYERROR=true

echo;
echo Copying files to%WOLFSSLLIB_TRG_DIR%\wolfssl\openssl\
xcopy %BASEDIR%\wolfssl\openssl\*.h                          %WOLFSSLLIB_TRG_DIR%\wolfssl\openssl\            /S /E /Q /Y
if %errorlevel% NEQ 0 SET COPYERROR=true

echo;
echo Copying files to %WOLFSSLLIB_TRG_DIR%\wolfssl\wolfcrypt\
xcopy %BASEDIR%\wolfssl\wolfcrypt                            %WOLFSSLLIB_TRG_DIR%\wolfssl\wolfcrypt\            /S /E /Q /Y
if %errorlevel% NEQ 0 SET COPYERROR=true


::******************************************************************************************************
:: user_settings and config defaults
::******************************************************************************************************
:: user_settings.h (default, may be overwritten by prior file)
echo;
echo Copying default user_settings.h to %WOLFSSLLIB_TRG_DIR%\include\
xcopy %WOLFSSL_ESPIDFDIR%\user_settings.h                    %WOLFSSLLIB_TRG_DIR%\include\                     /F
if %errorlevel% NEQ 0 SET COPYERROR=true

:: echo Creating new config file: %WOLFSSLLIB_TRG_DIR%\include\config.h (default, may be overwritten by prior file)
echo new config                                            > %WOLFSSLLIB_TRG_DIR%\include\config.h
xcopy  %WOLFSSL_ESPIDFDIR%\dummy_config_h.                   %WOLFSSLLIB_TRG_DIR%\include\config.h             /F /Y
if %errorlevel% NEQ 0 SET COPYERROR=true


:: Check if operator wants to keep prior config.h
if EXIST config_h_%FileStamp%.bak (
    echo;
    :: clear any prior errorlevel
    call;
    choice /c YN /m "Use your prior config.h  "
    if errorlevel 2 GOTO :NO_CONFIG_RESTORE
    xcopy config_h_%FileStamp%.bak   %WOLFSSLLIB_TRG_DIR%\include\config.h /Y
    if %errorlevel% NEQ 0 SET COPYERROR=true

) else (
    echo;
    echo Prior config.h not found. Using default file.
)
::******************************************************************************************************
:NO_CONFIG_RESTORE
::******************************************************************************************************

:: Check if operator wants to keep prior config.h
if EXIST user_settings_h_%FileStamp%.bak (
    echo;
    :: clear any prior errorlevel
    call;
    choice /c YN /m "User your prior user_settings.h  "
    if errorlevel 2 GOTO :NO_USER_SETTINGS_RESTORE
    xcopy user_settings_h_%FileStamp%.bak    %WOLFSSLLIB_TRG_DIR%\include\user_settings.h /Y
    if %errorlevel% NEQ 0 SET COPYERROR=true

) else (
    echo;
    echo Prior user_settings.h not found. Using default file.
)

::******************************************************************************************************
:NO_USER_SETTINGS_RESTORE
::******************************************************************************************************


:: unit test app
echo;
echo Copying unit files to %WOLFSSLLIB_TRG_DIR%\test\
xcopy %WOLFSSL_ESPIDFDIR%\test                               %WOLFSSLLIB_TRG_DIR%\test\                        /S /E /Q /Y
if %errorlevel% NEQ 0 GOTO :COPYERR

echo;
echo Copying CMakeLists.txt to %WOLFSSLLIB_TRG_DIR%\
xcopy %WOLFSSL_ESPIDFDIR%\libs\CMakeLists.txt                %WOLFSSLLIB_TRG_DIR%\                             /F
if %errorlevel% NEQ 0 GOTO :COPYERR

echo;
echo Copying component.mk to %WOLFSSLLIB_TRG_DIR%\
xcopy %WOLFSSL_ESPIDFDIR%\libs\component.mk                  %WOLFSSLLIB_TRG_DIR%\                             /F
if %errorlevel% NEQ 0 GOTO :COPYERR

:: Benchmark program
echo;
echo Removing %WOLFSSLEXP_TRG_DIR%\wolfssl_benchmark\
rmdir %WOLFSSLEXP_TRG_DIR%\wolfssl_benchmark\          /S /Q
if %errorlevel% NEQ 0 GOTO :COPYERR

echo;
echo Copying %WOLFSSLEXP_TRG_DIR%\wolfssl_benchmark\main\
mkdir %WOLFSSLEXP_TRG_DIR%\wolfssl_benchmark\main\

xcopy %BASEDIR%\wolfcrypt\benchmark\benchmark.c              %WOLFSSLEXP_TRG_DIR%\wolfssl_benchmark\main\                                 /F /Y
if %errorlevel% NEQ 0 GOTO :COPYERR

xcopy %WOLFSSL_ESPIDFDIR%\examples\wolfssl_benchmark         %WOLFSSLEXP_TRG_DIR%\wolfssl_benchmark\                                      /Q /Y
if %errorlevel% NEQ 0 GOTO :COPYERR

:: Crypt Test program
echo;
echo Copying %WOLFSSLEXP_TRG_DIR%\wolfssl_test\
rmdir %WOLFSSLEXP_TRG_DIR%\wolfssl_test\               /S /Q
mkdir %WOLFSSLEXP_TRG_DIR%\wolfssl_test\main\

xcopy %BASEDIR%\wolfcrypt\test\test.c                        %WOLFSSLEXP_TRG_DIR%\wolfssl_test\main\           /S /E /Q /Y
if %errorlevel% NEQ 0 GOTO :COPYERR

xcopy %WOLFSSL_ESPIDFDIR%\examples\wolfssl_test              %WOLFSSLEXP_TRG_DIR%\wolfssl_test\                /S /E /Q /Y
if %errorlevel% NEQ 0 GOTO :COPYERR

:: TLS Client program
echo;
echo Copying %WOLFSSLEXP_TRG_DIR%\wolfssl_client\
rmdir %WOLFSSLEXP_TRG_DIR%\wolfssl_client\            /S /Q
mkdir %WOLFSSLEXP_TRG_DIR%\wolfssl_client\main\

xcopy %WOLFSSL_ESPIDFDIR%\examples\wolfssl_client            %WOLFSSLEXP_TRG_DIR%\wolfssl_client\              /S /E /Q /Y
if %errorlevel% NEQ 0 GOTO :COPYERR

:: TLS Server program
echo;
echo Copying %WOLFSSLEXP_TRG_DIR%\wolfssl_server\
rmdir %WOLFSSLEXP_TRG_DIR%\wolfssl_server\            /S /Q
mkdir %WOLFSSLEXP_TRG_DIR%\wolfssl_server\main\

xcopy %WOLFSSL_ESPIDFDIR%\examples\wolfssl_server             %WOLFSSLEXP_TRG_DIR%\wolfssl_server\             /S /E /Q /Y
if %errorlevel% NEQ 0 GOTO :COPYERR

goto :DONE

:: error during copy encountered
::******************************************************************************************************
:COPYERR
::******************************************************************************************************
echo;
echo Error during copy.
echo
echo Please ensure none of the target files are flagged as read-only, open, etc.
goto :ERR

:: abort at user request
::******************************************************************************************************
:NOCOPY
::******************************************************************************************************
echo;
echo Setup did not copy any files.
goto :ERR

:: ERROR
::******************************************************************************************************
:ERR
::******************************************************************************************************
exit /B 1

:: Success
::******************************************************************************************************
:DONE
::******************************************************************************************************
echo;
echo;
echo Edit config file in  "%WOLFSSLLIB_TRG_DIR%\include\user_settings.h" before trying to compile.
echo;
echo See Component files for wolfSSL in %WOLFSSLLIB_TRG_DIR%
echo;
echo See additional examples at  https://github.com/wolfSSL/wolfssl-examples
echo;
echo REMINDER: Ensure any wolfSSL #include definitions occur BEFORE include files in your source code.
echo;
if "%COPYERROR%" == "true" (
    echo;
	echo WARNING: Copy completed with errors! Check for files in use, permissions, symbolic links, etc.
    echo;
)
echo setup_win.bat for ESP-IDF completed.
