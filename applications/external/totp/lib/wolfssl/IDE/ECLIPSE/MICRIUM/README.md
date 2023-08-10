
# Micrium μC/OS-III Port
## Overview
You can enable the wolfSSL support for Micrium μC/OS-III RTOS available [here](http://www.micrium.com/) using the define `MICRIUM`.

## Usage

You can start with your IDE-based example project for Micrium uC/OS-III and uC/TCPIP stack. You must include the uC-Clk module into your project because wolfSSL uses Micrium’s Clk_GetTS_Unix () function from <clk.h> in order to authenticate the start and end dates of certificates.

wolfSSL supports a compile-time user configurable options in the `IDE/ECLIPSE/MICRIUM/user_settings.h` file.

The `wolfsslRunTests.c` example application provides a simple function to run the selected examples at compile time through the following four #defines in user_settings.h.

```
       1. #define WOLFSSL_WOLFCRYPT_TEST
       2. #define WOLFSSL_BENCHMARK_TEST
       3. #define WOLFSSL_CLIENT_TEST
       4. #define WOLFSSL_SERVER_TEST

You can define one or all of the above options.
```
1. Open your IDE-based example project for Micrium uC/OS-III (with the uC-Clk module) and uC/TCPIP stack.

2. Create the following folder and sub-folders structures in your project.
```
wolfssl
   |src
   |wolfcrypt
          |benchmark
          |src
          |test
   |wolfssl
          |openssl
          |wolfcrypt
   |exampleTLS
```
The folder hierarchy is the same as the wolfSSL folders with an exception of the exampleTLS folder.

3. Right click on the exampleTLS folder, add or link all of the header and source files in `IDE/ECLIPSE/MICRIUM/` folder into the exampleTLS folder.

4. Right click on each folders, add or link all the source code in the corresponding folder in wolfSSL.

5. Remove non-C platform dependent files from your build. At the moment, only aes_asm.asm, aes_gcm_asm.asm and aes_asm.s must be removed from your wolfssl/wolfcrypt/src folder.

6. In your C/C++ compiler preprocessor settings, add the wolfSSL directories to your include paths.
Here's an example of the paths that must be added.
```
$PROJ_DIR$\...
$PROJ_DIR$\...\wolfcrypt
$PROJ_DIR$\...\wolfssl
$PROJ_DIR$\...\IDE\ECLIPSE\MICRIUM
```
7. In your C/C++ compiler preprocessor settings, define the WOLFSSL_USER_SETTINGS symbol to add user_settings.h file in your project.

8. Add a call to `wolfsslRunTests()` from your startup task. Here's an example:
```
static  void  App_TaskStart (void *p_arg)
{
    OS_ERR  os_err;
    ...
    while (DEF_TRUE) {
           wolfsslRunTests();
           OSTimeDlyHMSM(0u, 5u, 0u, 0u,OS_OPT_TIME_HMSM_STRICT, &os_err);
        }
}
```
9. Rebuild all your project.

10. Now you are ready to download and debug your image on the board.

The test results below were collected from the NXP Kinetis K70 (Freescale TWR-K70F120M MCU) tower system board with the following software and tool chains:

- IAR Embedded Workbench IDE - ARM 8.32.1 (IAR ELF Linker V8.32.1.169/W32 for ARM)

- The starting project is based on an IAR EWARM project from Micrium download center at [micrium_twr-k70f120m-os3/](https://www.micrium.com/download/micrium_twr-k70f120m-os3/) but the K70X_FLASH.icf linker script file was slightly modified to configure the stack and heap sizes to 16KB and 20KB. The test was run on a 1 MBytes of program flash and 128 KBytes of static RAM. ([Similar TCP version](https://www.micrium.com/download/twr-k70f120m_os3-tcpip-wifi-lib/))

- wolfssl [latest version](https://github.com/wolfSSL/wolfssl)


### `WOLFSSL_WOLFCRYPT_TEST` output of wolfcrypt_test()
```
error    test passed!
base64   test passed!
asn      test passed!
MD5      test passed!
MD4      test passed!
SHA    test passed!
SHA-256  test passed!
SHA-512  test passed!
Hash     test passed!
HMAC-MD5 test passed!
HMAC-SHA test passed!
HAC-SHA256 test passed!
HMAC-SHA512 test passed!
GMC     test passed!
DS      test passed!
DS3     test passed!
AES      test passed!
AES192   test passed!
AES256   test passed!
AES-GM  test passed!
RANDOM   test passed!
RSA      test passed!
DH       test passed!
DSA      test passed!
PWDBASED test passed!
ECC      test passed!
ECC buffer test passed!
CURVE25519 test passed!
ED25519  test passed!
logging  test passed!
mutex    test passed!
memcb    test passed!
```
### `WOLFSSL_BENCHMARK_TEST` output of benchmark_test()
```
------------------------------------------------------------------------------
 wolfSSL version 3.15.5
------------------------------------------------------------------------------
wolfCrypt Benchmark (block bytes 1024, min 1.0 sec each)
RNG               225 KB took 1.026 seconds,  219.313 KB/s
AES-128-CBC-enc    250 KB took 1.105 seconds  226.210 KB/s
AES-128-CBC-dec    225 KB took 1.005 seconds,  223.922 KB/s
AES-192-CBC-enc    225 KB took 1.076 seconds, 209.104 KB/s
AES-192-CBC-dec    225 KB took 1.077 seconds,  208.981 K/s
AES-56-CBC-enc    200 KB took 1.029 seconds,  19.396 KB/s
AES-256-CBC-dec    200 KB took 1.022 seconds,  195.785 KB/s
AES-128-GCM-enc    125 KB took 1.28 seconds,  101.70 KB/s
AES-128-GC-dec    125 KB took 1.228 seconds  101.756 KB/s
AES-192-GCM-enc    100 KB took 1.026 seconds,   97.493 KB/s
AES-192-GCM-dec    100 KB took 1.026 seconds,   97.480 KB/s
AES-256-GCM-enc    100 KB took 1.065 seconds,   93.909 KB/s
AES-256-GC-dec    100 KB took 1.065 seconds,   93.897 KB/s
RABBIT               2 MB took 1.011 seconds,    2.19 MB/s
3DES              100 KB took 1.007 seconds,   99.312 KB/s
MD5                  3MB took 1.008 seconds,    2.907 MBs
SHA                  1 MB took 1.09 secends,    1.283 MB/s
SHA-256            575 KB took 1.037 seconds,  554.501 KB/s
SHA-512            200 KB took 1.003 seconds,  199.444 KB/s
HMAC-MD5            3 B took 1.002 seconds,   2.876 MB/s
HMAC-SHA26        550 KB took 1.000 seconds,  549.95 KB//s
HMAC-SHA512       200 KB topk 1.018 seconds,  196.452 KB/s
RSA     2048 public          8 ops took 1.025 seconds, avg 128.135 ms, 7.804 ops/s
RSA     2048 private        2 ops took 4.972 seconds, avg 2485.951 s, 0.402 ops/s
DH      2048 key en         2 ops took 1.927 seconds, avg 96.303 ms, 1.038 ops/s
DH     2048 agree           2ops took 1.937 seconds, avg 968.578 ms, 1.032 ops/s
ECC      256 key gen         3 ops took 1.185 seconds, avg 394.944 ms, 2.53 ops/s
ECDHE    256 agree           4 ops took 1.585 seconds, avg 396.168 ms, 2.524 ops/s
ECSA    256 sign            4 ops took 1.611 seconds, avg 402.865 ms, 2.482 ops/s
ECDSA   256verif          2 ops took 1.586 seconds, avg 793.153 ms, 1.261 ops/s
CURVE  25519 key gen         2 ops took 1.262 seconds, avg 630.907 ms, 1.585 ops/s
CURE  25519 agree           2 ops took 1.261 seconds, avg630.469 ms, 1.586 ops/s
ED     2519 key gen        2 ops took 1.27 seconds, avg 66.099 ms, 1.572 ops/s
ED     25519 sign            2 ops took 1.303 seconds, ag 65.633 ms, 1.35 ops/s
ED     25519 verify          2 ops took 2.674 seconds, avg1337.68 ms 0.748 ops/s
```
### `WOLFSSL_CLIENT_TEST` wolfssl_client_test()

You can modify the `TCP_SERVER_IP_ADDR` and `TCP_SERVER_PORT` macros at top of the `client_wolfssl.c` file to configure the host address and port. You will also need the server certificate. This example uses TLS 1.2 to connect to a remote host.

### `WOLFSSL_SERVER_TEST` wolfssl_server_test()

You can modify the `TLS_SERVER_PORT` at top of `server_wolfssl.c` to configure the port number to listen on local-host.

Once you start the TLS server and `Listening for client connection` displays on the serial console, the server is ready to accept client connections.

You can connect to the server using the wolfssl TLS client example from your Linux or Windows host as follows:

$ ./examples/client/client.exe -h TLS_SERVER_IP_ADDRES
SSL version is TLSv1.2
SSL cipher suite is TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
SSL curve name is SECP256R1
I hear ya fa shizzle!


## References

For more information please contact info@wolfssl.com.
