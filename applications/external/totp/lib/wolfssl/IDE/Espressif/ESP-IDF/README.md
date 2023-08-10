# ESP-IDF port

NOTICE: These Espressif examples have been created and tested with the latest stable release branch of 
[ESP-IDF V4](https://docs.espressif.com/projects/esp-idf/en/v4.4.1/esp32/get-started/index.html)
and have not yet been upgraded to the master branch V5. 
See the latest [migration guides](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/migration-guides/index.html).

## Overview
 ESP-IDF development framework with wolfSSL by setting *WOLFSSL_ESPIDF* definition

Including the following examples:

* Simple [TLS client](./examples/wolfssl_client/)/[server](./examples/wolfssl_server/)
* Cryptographic [test](./examples/wolfssl_test/)
* Cryptographic [benchmark](./examples/wolfssl_benchmark/)

 The *user_settings.h* file enables some of the hardened settings.

## Requirements
 1. [ESP-IDF development framework](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/)

## Setup for Linux
 1. Run `setup.sh` at _/path/to_`/wolfssl/IDE/Espressif/ESP-IDF/` to deploy files into ESP-IDF tree  
 2. Find Wolfssl files at _/path/to/esp_`/esp-idf/components/wolfssl/`
 3. Find [Example programs](https://github.com/wolfSSL/wolfssl/tree/master/IDE/Espressif/ESP-IDF/examples) under _/path/to/esp_`/esp-idf/examples/protocols/wolfssl_xxx` (where xxx is the project name)

## Setup for Windows
 1. Run ESP-IDF Command Prompt (cmd.exe) or Run ESP-IDF PowerShell Environment
 2. Run `setup_win.bat` at `.\IDE\Espressif\ESP-IDF\`
 3. Find Wolfssl files at _/path/to/esp_`/esp-idf/components/wolfssl/`
 4. Find [Example programs](https://github.com/wolfSSL/wolfssl/tree/master/IDE/Espressif/ESP-IDF/examples) under _/path/to/esp_`/esp-idf/examples/protocols/wolfssl_xxx` (where xxx is the project name)

## Configuration
 1. The `user_settings.h` can be found in _/path/to/esp_`/esp-idf/components/wolfssl/include/user_settings.h`

## Build examples
 1. See README in each example folder

## Support
 For question please email [support@wolfssl.com]

 Note: This is tested with :  
   - OS: Ubuntu 20.04.3 LTS and Microsoft Windows 10 Pro 10.0.19041 and well as WSL Ubuntu
   - ESP-IDF: ESP-IDF v4.3.2
   - Module : ESP32-WROOM-32
