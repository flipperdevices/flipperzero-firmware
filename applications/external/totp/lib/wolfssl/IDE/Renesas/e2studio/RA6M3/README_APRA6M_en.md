wolfSSL for Alpha Project AP-RA6M-0A Setup Guide
=================================================

## Description

This directory contains e2studio projects targeted at the Alpha Project AP-RA6M-0A board including the Renesas RA 32-bit MCUs.
The example projects include a wolfSSL TLS client and server. They also include benchmark and cryptography tests for the wolfCrypt library.

The wolfssl project contains both the wolfSSL and wolfCrypt libraries. It is built as a `Renesas RA C Library Project` and contains the Renesas RA configuration.

The other projects (benchmark, client, server and test) are built as a `Renesas RA C Project Using RA Library`, where the RA library is the wolfssl project.
The wolfssl Project Summary is listed below and is relevant for every project.

### Project Summary
|Item|Name/Version|
|:--|:----|
|e2studio|2020-07|
|Board    |AP-RA6M-0A   |
|Device   |R7FA6M3AH3CFC|
|Toolchain|GCC ARM Embedded
|FSP Version|1.3.0|

### Selected software components
|Component|Version|
|:--|:---|
|Board Support Package Common Files|v1.3.0|
|Arm CMSIS Version 5 - Core (M) |v5.7.0|
|Board support package for R7FA6M3AH3CFC |v1.3.0|
|Board support package for RA6M3|v1.3.0  |
|Board support package for RA6M3 - FSP Data|v1.3.0|
|FreeRTOS|v1.3.0|
|FreeRTOS - Buffer Allocation 2 |v1.3.0|
|FreeRTOS+TCP|v1.3.0|
|r_ether to FreeRTOS+TCP Wrapper|v1.3.0|
|Ethernet |v1.3.0   |
|Ethernet PHY|v1.3.0|
|I/O Port|v1.3.0    |
|BSP-Board|v1.2.0   |

## Setup Steps

The project directories are missing files necessary to build the project.\
These files can be generated when creating a new Renesas RA Project.\
The following steps explain how to generate the missing files and where to place them.

1.) Download Alpha project example program from [Alpha Project Home Page](https://www.apnet.co.jp/product/ra/ap-ra6m-0a.html)

+ Unzip the downloaded example project

2.) Create a 'dummy' Renesas RA C Library Project on e2studio

+ Click File->New->`RA C/C++ Project`
+ Enter `dummy_library` as the project name. Click Next.
+ Select `Board: Custom User Board`.
+ Select `R7FA6M3AH3CFC
+ Under `RTOS: No RTOS`, select `FreeRTOS`.
+ Click Next. Select `FreeRTOS - Minimal - Static Allocation`
+ Click `Renesas RA C Library Project`. Click Next
+ Click Finish.

3.) Create a 'dummy' Renesas RA C Project Using RA Library  on e2studio

+ Click File->New->`RA C/C++ Project`
+ Enter `dummy_app` as the project name. Click Next.
+ Under `RA library project`, select `dummy_library`.
+ Click `Executable Using an RA Static Library`. Click Next
+ Click Finish.
+ Enter `dummy_app` as Project name
+ Select RA library project `dummy_library`

4.) Import all the wolfSSL Projects into e2studio workspace.

+ Click File->`Open Projects from File System`
+ Click `Directory...` to the right of Import source
+ Select the RA6M3G folder location that contains the projects
   example path: wolfssl/IDE/Renesas/e2studio/RA6M3
+ Deselect the Non-Eclipse project, RA6M3G, by clicking the checkbox
   Only the folders with 'Eclipse project' under 'Import as' need to be selected.
+ Click Finish.

5.) Copy files from `dummy_library` into `wolfSSL_RA6M3G`

+ Expand the dummy_library and wolfSSL_RA6M3G projects
  (Click the drop-down arrow to the left of the project name.)
+ Select and Copy the following folders/files inside dummy_library

    `ra/`  
    `ra_gen/`  
    `ra_cfg/`  
    `script/`

+ Paste the copied folders/files into wolfSSL_RA6M3G
+ The `dummy_library` project can now be deleted.
+ Copy `APRA6M0A.pincfg` from ap_ra6m_0a_sample\sample\ap_ra6m_0a_ether_sample to wolfSSL_RA6M3G
+ Delete `R7FA6M3AH3CFC.pincfg` from wolfSSL_RA6M3G
+ Generate Project Content.

  + Click `Open RA Configuration` in the top bar (Grey Settings Cog)
  + Go to `BSP` tab and import CMSIS pack file, AP.APRA6M0A.x.x.x.pack, from ap_ra6m_0a_sample\sample folder
  + Select APRA6M0A as Board
  + Go to `Pins` tab and select APRA6M0A.pincfg
  + Go to `Stacks` tab and add Heap 4 stack from New Stack(+ Icon)
  + Click `Generate Project Content` at top right (Green Icon)
+ Build wolfSSL_RA6M3G.

6.) Copy files from `dummy_app` into `./IDE/Renesas/e2studio/RA6M3/common/ra6m3g/`
    **NOTE:** This may need to be done outside of the e2studio environment (e.g. File Explorer).

+ Select and Copy the following folder inside dummy_app

    `src/`  
    `script/`

+ Paste the copied folders into `./IDE/Renesas/e2studio/RA6M3/common/ra6m3g/`
`(The test, benchmark, client and server projects link to this folder.)`
+ The `dummy_app` project can now be deleted.

7.) Setup Network Environment

        The client and server projects have defines inside their wolfssl_thread_entry.h.
        These defines (ucIPAddress ... ucDNSServerAddress) may need to be changed
        based on your internal network environment.  The g_ether0_mac_address is the default
        mac address found inside the RA configuration inside the wolfssl project.
        The client wolfssl_thread_entry.h has defines (SERVER_IP and DEFAULT_PORT) that
        will need to be changed based on the server you're trying to connect to over
        the ethernet connection.

## Build and Run

### Build Each Project
Right-Click each Project and select Build.

### Run wolfCrypt Test and Benchmark

1.) Right-Click the Project name.  
2.) Select `Debug As` -> `Renesas GDB Hardware Debugging`  
3.) Select J-Link ARM. Click OK.  
4.) Select R7Fa6M3AH. Click OK.

### Run the wolfSSL TLS Server Example.

1.) Right-Click the Project name.  
2.) Select `Debug As` -> `Renesas GDB Hardware Debugging`  
3.) Select J-Link ARM. Click OK.  
4.) Select R7Fa6M3AH. Click OK.  
5.) Run the following wolfSSL example client command inside the base of the wolfssl directory.

```
./examples/client/client -v 4 -h "ucIPAddress" -p 11111 -A ./certs/1024/ca-cert.pem
```

**NOTE:** "ucIPAddress" is "192.168.1.241" by default. (See wolfssl_thread_entry.h)

### Run the wolfSSL TLS Client Example.

 1.) Run the following wolfSSL example server command inside the base of the wolfssl directory.

```
./examples/server/server -v 4 -b -d -p 11111 -c ./certs/1024/server-cert.pem -k ./certs/1024/server-key.pem
```

   **NOTE:** The port 11111 is the DEFAULT_PORT inside wolfssl_thread_entry.h.
   If DEFAULT_PORT was changed then the above command will need to match it.

 2.) Right-Click the Project name.\
 3.) Select `Debug As` -> `Renesas GDB Hardware Debugging`\
 4.) Select J-Link ARM. Click OK.\
 5.) Select R7Fa6M3AH. Click OK.

## Troubleshooting

+ The commands for the example client/server assumes it is being run from the
  base directory of wolfssl.

+ Enter "#define DEBUG_WOLFSSL" inside user_settings.h or wolfssl_thread_entry.c
   to enable wolfssl debug messages to the Renesas Virtual Debug Console.

+ Some linking errors can be caused by the e2studio project files needing to be rebuilt and freshened.
Right-Click a project, select Index, click Rebuild and then click Freshen Files. Repeat for each project.

[Support Forum](https://www.wolfssl.com/forums/)

Support Email: support@wolfssl.com


## Resources

[wolfSSL Website](https://www.wolfssl.com/)

[wolfSSL Wiki](https://github.com/wolfSSL/wolfssl/wiki)

[wolfSSL Manual](https://wolfssl.com/wolfSSL/Docs-wolfssl-manual-toc.html)

[wolfSSL API Reference](https://wolfssl.com/wolfSSL/Docs-wolfssl-manual-17-wolfssl-api-reference.html)

[wolfCrypt API Reference](https://wolfssl.com/wolfSSL/Docs-wolfssl-manual-18-wolfcrypt-api-reference.html)

[TLS 1.3](https://www.wolfssl.com/docs/tls13/)
