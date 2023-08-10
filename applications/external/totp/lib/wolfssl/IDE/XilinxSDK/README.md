# Common Gotcha's

- If compiling all code together (ie no separate wolfssl library) then the -fPIC compiler flag should be used. Without using -fPIC in this build setup there could be unexpected failures.
- If building with ARMv8 crypto extensions then the compiler flags "-mstrict-align -mcpu=generic+crypto" must be used.
- Check that enough stack and heap memory is set for the operations if a crash or stall happens.

# Xilinx SDK wolfCrypt Vitis 2018.2 Project

To use this example project:
1. Start a new workspace
2. Create a new BSP called `standalone_bsp_0`.
3. Copy `.cproject` and `.project` from IDE/XilinxSDK/2018_2 into the wolfSSL root.
4. From the Xilinx SDK Import wolfBoot using "Import" -> "Existing Projects into Workspace".


# Detailed Instructions For Example Use With Vitis 2019.2

1. Create a new workspace located in the directory wolfssl/IDE/XilinxSDK/2019_2
2. Create a new BSP, by selecting;
   - File->New->Platform Project
   - Setting "Project name" to standalone_bsp_0, then click "Next"
   - Select the "Create from hardware specification" radius and click "Next"
   - "Browse..." to the desired XSA file for the hardware
   - (optional) change Processor to R5 now
   - click "Finish"
3. (optional) If building for TLS support than expand the standalone_bsp_0 project, double click on platform_spr, Expand the cpu (i.e psu_cortexa53_0), click on Board Support Package, select the "Modify BSP Settings..." box and click on lwip211. Note that the api_mode should be changed from RAW API to SOCKET API.
4. Right click on the standalone_bsp_0 project and click on "Build Project"
5. Import the wolfcrypt example project "File->Import->Eclipse workspace or zip file"
6. Uncheck "Copy projects into workspace"
7. Select the root directory of `wolfssl/IDE/XilinxSDK/2019_2`, and select `wolfCrypt_example` and `wolfCrypt_example_system`. Then click "Finish"


# Detailed Instructions For Example Use With Vitis 2022.1

This shows the necessary steps on the basis of using the VMK180 development board.

1. Create a new workspace located in the directory `wolfssl/IDE/XilinxSDK/2022_1`.
2. Create a new platform project.
   - On the welcome screen select "Create platform project".
   - Give it a good name, e.g. `vmk180` and click 'Next'.
   - In the Hardware Specification drop-down menu select "vmk180" and click 'Finish'.
3. Enable the necessary libraries in the bsp.
   - Open `platform.spr` in the 'vmk180' platform project.
   - In the tree-view select "Board Support Package" and click on "Modify BSP Settings...".
   - Enable the `xilmailbox` and `xilsecure` libraries.
   - In this tree-view on the left side under "Overview->standalone" appeared now a "xilsecure" entry.
   - Select "xilsecure" and change the configuration of `cache_disable` to `false`. Press 'OK' to confirm the change.
   - Build the platform project, either via "Project->Build All" or by the keyboard shortcut 'CTRL+b'.
4. Import the wolfcrypt example projects "File->Import->Eclipse workspace or zip file".
   - Behind 'Select Root directory' click on 'Browse...'. You should already be in the correct directory, but still make sure that you're in `wolfssl/IDE/XilinxSDK/2022_1/`. Click 'Open'.
   - Uncheck "Copy projects into workspace".
   - Select the two example projects `wolfCrypt_example` and `wolfCrypt_example_system`, then click 'Finish'.
5. Fix the missing link from platform project and system project.
   - In the 'Explorer' Window open the `wolfCrypt_example_system` and double-click `wolfCrypt_example_system.prj`.
   - You should normally see a pop-up with the title "Platform invalid". Click on "Change referred platform", select your platform and click 'OK'.
   - In case you didn't see the pop-up, you should see the 'System Project Settings'. Click on the `...` behind 'Platform', select your platform and click 'OK'.
   - Vitis will now remind you that all build configurations will be cleaned, click 'Yes'.
6. Create a new linker script
   - In the 'Explorer' Window open the `wolfCrypt_example_system` and right-click on the `wolfCrypt_example` project. Click 'Generate Linker Script'.
   - Configure the memory sections you want to use, the default should be OK for this example.
   - 'Heap Size' must be increased according to your use case. For this example to run it is sufficient to increase to `8 MB`. For benchmarks with big chunk sizes increase to `512 MB`.
   - Increase 'Stack Size' to `32 KB`.
   - Click 'Generate'

## FreeRTOS based example projects

1. In the previously created `vmk180` platform, one can see a tree where you can also find the "Board support package" settings.
   - Click on the bold green `+` on the top to 'Add Domain'.
   - Give the new domain the name "freertos" and change the "OS" in the drop-down menu to "freertos".
2. Repeat the same steps of the previous step 3 for the newly created domain.
   - In the tree-view select "freertos10_xilinx" and then open the "kernel_behavior" sub-entry.
   - Change `minimal_stack_size` to `8000`, `tick_rate` to `1000` and `total_heap_size` to `8388608`. "Big chunk sizes" have not been tested under FreeRTOS.
3. Repeat the same steps of the previous steps 4 to 6, but with the `wolfCrypt_FreeRTOS_example` resp. `wolfCrypt_FreeRTOS_example_system`.


## Troubleshooting

* `make: *** No rule to make target '../src/lscript.ld', needed by 'wolfCrypt_example.elf'.  Stop.`: You forgot to create the linker script
* `/path/to/wolfssl/wolfcrypt/port/xilinx/xil-versal-glue.h:30:10: fatal error: xsecure_mailbox.h: No such file or directory`: You forgot to add the `xilmailbox` and/or `xilsecure` libraries.
* `/path/to/wolfssl/wolfcrypt/src/port/xilinx/xil-aesgcm.c:90: undefined reference to 'Secure_AesInitialize'` (and others): You forgot to build the platform project after adding the libraries.

## Code formatter

In case one wants to write code in "wolfSSL style" in Vitis:
   - go to "Window->Preferences->Additional->C/C++->Code Style->Formatter".
   - Click on 'Import', navigate to `wolfssl/IDE/XilinxSDK/` and select the file `eclipse_formatter_profile.xml`, click on 'Apply and Close'.

## Configuration

All configuration is done through the header file in `wolfssl/IDE/XilinxSDK/user_settings.h`

### Build flavors

Define `WOLFSSL_XILINX_CRYPT` to enable the **crypto engines**. (This is the default)

Undefine `WOLFSSL_XILINX_CRYPT` and define `WOLFSSL_ARMASM` to enable the **ARMv8 Cryptographic Extensions**.

Undefine both `WOLFSSL_XILINX_CRYPT` and `WOLFSSL_ARMASM` to enable a **software-only build**.

### Further features

Define `WC_XIL_AESGCM_DPA_CM` to enable Counter Measures for Differential Power Analysis of the AES-GCM core. (Only available when using the crypto engines).

Undefine `HAVE_HASHDRBG` to enable usage of the DRBG in the crypto engine.

Define `WOLFSSL_ECDSA_DETERMINISTIC_K` to use "deterministic K" as of RFC6979. (Supported for all three build flavors).


# Steps For Creating Project From Scratch

1. Create a new workspace
2. Create a new BSP, by selecting;
   - File->New->Platform Project
   - Setting "Project name" to standalone_bsp_0, then click "Next"
   - Select the "Create from hardware specification" radius and click "Next"
   - "Browse..." to the desired XSA file for the hardware
   - (optional) change Processor to R5 now
   - click "Finish"
3. (optional) If building for TLS support than expand the standalone_bsp_0 project, double click on platform_spr, Expand the cpu (i.e psu_cortexa53_0), click on Board Support Package, select the "Modify BSP Settings..." box and click on lwip211. Note that the api_mode should be changed from RAW API to SOCKET API.
4. Right click on the standalone_bsp_0 project and click on "Build Project"
5. Create wolfssl project File->New->Application Project
6. Name the project wolfCrypt_example, select "Next"
7. For the platform select standalone_bsp_0 and click next, then next once more on Domain.
8. Select "Empty Application" and click "Finish"
9. Expand the wolfCrypt_example project and right click on the folder "src".
10. Select "Import Sources" and set the "From directory" to be the wolfssl root directory.
11. Select the folders to import as ./src, ./IDE/XilinxSDK, ./wolfcrypt/benchmark, ./wolfcrypt/test, ./wolfcrypt/src
12. (optional) Expand the Advanced table and select "Create links in workspace"
13. Click on "Finish"
14. Expand the wolfcrypt/src directory and exclude all .S files from the build
15. Right click on the wolfCrypt_example project and got to Properties. Set the macro `WOLFSSL_USER_SETTINGS` in C/C++ Build->Settings->ARM v8 gcc compiler->Symbols
16. Set the include path for finding user_settings.h by going to the Properties and setting it in C/C++ Build->Settings->ARM v8 gcc compiler->Directories. This is to the directory wolfssl/IDE/XilinxSDK
17. Set the include path for finding wolfSSL headers. To the root directory wolfssl
18. Add compiler flags "-fPIC -mstrict-align -mcpu=generic+crypto" to the project properties. C/C++ Build->Settings->ARM v8 gcc compiler->Miscellaneous
19. Right click on wolfCrypt_example and "Build Project"


## Platform

Tested on the Zynq UltraScale+ MPSoC (ZUC102).

This is a bare-metal example for wolfCrypt only with algorithm support for:
* RNG
* RSA
* ECC
* AES-GCM
* ChaCha20
* Poly1305
* SHA2
* SHA3
* PBKDF2

## Benchmark Results

```
------------------------------------------------------------------------------
 wolfSSL version 4.3.0
------------------------------------------------------------------------------
wolfCrypt Benchmark (block bytes 1024, min  sec each)
RNG                 72 MB took 1.000 seconds,   72.388 MB/s
AES-128-GCM-enc    370 MB took 1.000 seconds,  370.312 MB/s
AES-128-GCM-dec    187 MB took 1.000 seconds,  187.451 MB/s
AES-192-GCM-enc    341 MB took 1.000 seconds,  341.382 MB/s
AES-192-GCM-dec    180 MB took 1.000 seconds,  179.663 MB/s
AES-256-GCM-enc    316 MB took 1.000 seconds,  316.382 MB/s
AES-256-GCM-dec    172 MB took 1.000 seconds,  172.485 MB/s
CHACHA             256 MB took 1.000 seconds,  255.859 MB/s
CHA-POLY            98 MB took 1.000 seconds,   97.559 MB/s
POLY1305           517 MB took 1.000 seconds,  516.895 MB/s
SHA-256            535 MB took 1.000 seconds,  534.595 MB/s
SHA-384            123 MB took 1.000 seconds,  123.291 MB/s
SHA-512            124 MB took 1.000 seconds,  123.657 MB/s
SHA3-224            70 MB took 1.000 seconds,   70.337 MB/s
SHA3-256            67 MB took 1.000 seconds,   66.528 MB/s
SHA3-384            53 MB took 1.000 seconds,   52.710 MB/s
SHA3-512            38 MB took 1.000 seconds,   37.598 MB/s
HMAC-SHA256        520 MB took 1.000 seconds,  520.093 MB/s
HMAC-SHA384        121 MB took 1.000 seconds,  121.265 MB/s
HMAC-SHA512        121 MB took 1.000 seconds,  121.289 MB/s
PBKDF2              28 KB took 1.000 seconds,   28.375 KB/s
ECC      256 key gen      8518 ops took 1.000 sec, avg 0.117 ms, 8518.000 ops/sec
ECDHE    256 agree        1818 ops took 1.000 sec, avg 0.550 ms, 1818.000 ops/sec
ECDSA    256 sign         4448 ops took 1.000 sec, avg 0.225 ms, 4448.000 ops/sec
ECDSA    256 verify       1430 ops took 1.000 sec, avg 0.699 ms, 1430.000 ops/sec
Benchmark complete
Benchmark Test: Return code 0
```
