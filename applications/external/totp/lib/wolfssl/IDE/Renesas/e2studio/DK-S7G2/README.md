## Project Summary
|Item|Name/Version|
|:--|:--|
|Board|DK-S7G2|
|Toolchain|GCC ARM Embedded|
|SSP Version|1.7.0|


## Building wolfSSL For DK-S7G2

- First physically toggle the ENET1 and JTAG switch to on with the DK-S7G2 board.
- Open e2studio and set the workspace to be wolfssl-X.X.X/IDE/Renesas/e2studio/DK-S7G2/
- Create a Synergy library project named wolfssl "File->New->Synergy C/C++ Project", "Renesas Synergy C Library Project" then "Next", set wolfssl as the "Project Name" then "Next", set Board to "S7G2 DK" then "Next", finally select the BSP radius and click "Finish"
- Copy configuration.xml and .project from wolfssl-X.X.X/IDE/Renesas/e2studio/DK-S7G2/wolfssl-template-project/ into the wolfssl-X.X.X/IDE/Renesas/e2studio/DK-S7G2/wolfssl directory
- (optional but necessary for production) Add TRNG support by clicking on Threads tab and highlight HAL/Common click "New Stack > Driver > Crypto > TRNG Driver on r_sce_trng". Then comment out WOLFSSL_SCE_NO_TRNG define in wolfssl project src/user_settings.h
- (optional SHA acceleration) Add HASH support by clicking on Threads tab and highlight HAL/Common click "New Stack > Driver > Crypto > HASH Driver on r_sce_hash". Then uncomment WOLFSSL_SCE_NO_HASH define in wolfssl project src/user_settings.h
- (optional AES acceleration) Add the stacks for AES128, AES192, and AES256. Click on Threads tab and highlight HAL/Common click "New Stack > Driver > Crypto > AES Driver on r_sce_aes". Add three one for each key size and rename them to g_sce_aes_256, g_sce_aes_192, and g_sce_aes_128. Changing each to ECB chaining mode and the key length that matches the name.
- Generate the changes by clicking on "Generate Project Content"
- Exclude src/wolfcrypt/port and all src/wolfcrypt/*.S and src/wolfcrypt/*.asm files from the build
- Exclude src/wolfcrypt/evp.c, src/wolfcrypt/misc.c and src/wolfssl/bio.c
- Set the Preprocessor define in wolfssl project to have WOLFSSL_USER_SETTINGS. Right click on wolfssl project "Properties -> C/C++ Build -> GNU ARM Cross C Compiler -> Preprocessor" add WOLFSSL_USER_SETTINGS under "Defined symbols"
- Set include to wolfssl directory. Right click on project "Properties -> C/C++Build -> GNU ARM Cross Compiler -> Includes". Add "${ProjDirPath}/../../../../.." and "${ProjDirPath}/../"
- Build wolfssl by right clicking on wolfssl project and selecting "Build Project"

## Example Projects and Building

- Create a new Synergy project "Renesas Synergy C Project Using Synergy Library"
- Set it to use the wolfssl library
- Copy in the .cproject, .project and source file from the template desired i.e. wolfssl-X.X.X/IDE/Renesas/e2studio/DK-S7G2/wolfcrypttest-template/
- Right click on the created project and select "Build Project"

The example_server loops looking to accept connections and closes immediately after a successful connection was made.

The benchmark example tries to do a TCP connection to SERVER_IP on port 11112 and a TLS connection to SERVER_IP on port 11111 then does wolfCrypt benchmark collection.

The wolfcryptest runs through all of the unit tests from wolfcrypt/test/test.c

## Advanced Overriding Driver Name
Defaults are set for when accessing the driver but the default names may not always work for an existing project. These are the macros and their defaults that could be mapped to other driver names:

```
/* For main SCE open and close */
WOLFSSL_SCE_GSCE_HANDLE g_sce

/* For AES operations */
WOLFSSL_SCE_AES256_HANDLE g_sce_aes_256
WOLFSSL_SCE_AES192_HANDLE g_sce_aes_192
WOLFSSL_SCE_AES128_HANDLE g_sce_aes_128

/* HASH operations */
WOLFSSL_SCE_SHA256_HANDLE g_sce_hash_0

/* TRNG access */
WOLFSSL_SCE_TRNG_HANDLE g_sce_trng
```


An example of remapping a driver name would be the following added to a wolfSSL user_settings.h file:
#define WOFSSL_SCE_SHA256_HANDLE my_sce_hash_driver
