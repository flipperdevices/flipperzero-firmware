wolfSSL for Microsoft Azure Sphere Devices in MSVS 2019
==========================================

## Description
This directory contains the Visual Studio CMake Project that is targeted for
the visual studio 2019 Azure Sphere plugin.

## Limitations
The example includes a client and server executable but the main.c app
only runs the server by default. main.c can be manually modified to run the
client instead if desired.

By default the main.c app include the hardware definitions for MT3620-mini. It
can be manually modified to include hardware definitions for MT3620, or
avnet MT3620 instead.

The app relies on a wolfSSL static library project, app and library  are built
by the CMakeLists.txt located in
IDE/MSVS-2019-AZSPHERE/wolfssl_new_azsphere/CMakeLists.txt. The library and app
use `user_settings.h` to enable and disable features.

### Set Up Steps
0. Setup your Azure Sphere device.
   [Install Azure Sphere](https://docs.microsoft.com/en-us/azure-sphere/install/install)
   [Set up an account](https://docs.microsoft.com/en-us/azure-sphere/install/azure-directory-account)
   [Claim your device](https://docs.microsoft.com/en-us/azure-sphere/install/claim-device)
   [Configure networking](https://docs.microsoft.com/en-us/azure-sphere/install/configure-wifi)

1. Open the local folder IDE/MSVS-2019-AZSPHERE/wolfssl_new_azsphere in MSVS 2019

2. From the file menu select "Build" dropdown
    + Click: `Rebuild All` option

3. Connect your Azure Sphere MT3620-mini Development Board using USB.

4. Test the server.
    + Run server(Azure Sphere) using: `Debug->'Start new instance'`
    + Run the following wolfSSL example client command inside wolfssl directory.

```
./examples/client/client -h "Server IP Address" -p 11111 -A ./certs/ca-cert.pem
```

For other information please refer to the README in IDE/VS-AZURE-SPHERE
