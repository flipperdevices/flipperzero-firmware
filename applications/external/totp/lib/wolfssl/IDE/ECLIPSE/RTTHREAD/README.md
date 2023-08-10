# RT-Thread Port
## Overview
You can enable the wolfSSL support for RT-Thread available [here](https://www.rt-thread.io) using the define `RTTHREAD`.

## Usage

wolfSSL supports a compile-time user configurable options in the `IDE/ECLIPSE/RTTHREAD/user_settings.h` file.

The `wolfssl_test.c` example application provides a simple function to run the test and benchmark.

1. Open your IDE-based example project for RT-Thread.

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
   |example
```
The folder hierarchy is the same as the wolfSSL folders with an exception of the example folder.

3. Add or link all of the header and source files in `IDE/ECLIPSE/RTTHREAD/` folder into the example folder.

4. Add or link all the source code in the corresponding folder in wolfSSL.

5. Remove non-C platform dependent files from your build.

6. In your C/C++ compiler preprocessor settings, add the wolfSSL directories to your include paths.
Here's an example of the paths that must be added.

```
$PROJ_DIR$\...
$PROJ_DIR$\...\wolfcrypt
$PROJ_DIR$\...\wolfssl
$PROJ_DIR$\...\IDE\ECLIPSE\RTTHREAD
```

7. In your C/C++ compiler preprocessor settings, define the WOLFSSL_USER_SETTINGS symbol to add user_settings.h file in your project.

8. Add a call to `wolfssl_test()` from your startup task. Here's an example:

```c
static void test_task (void *p_arg)
{
    ...
    while (1) {
           wolfssl_test();
           rt_thread_mdelay(500);
        }
}
```
9. Rebuild all your project.

10. Now you are ready to download and debug your image on the board.


The test results below were collected from the RT-Thread ART-Pi with the following software and tool chains:

- STM32H750XBH6

- RT-Thread Studio (Version: 2.0.0)

- GNU ARM Cross C Compiler (Optimization level: -O0)

- The starting project is based on [RT-Thread ART-Pi SDK](https://github.com/RT-Thread-Studio/sdk-bsp-stm32h750-realthread-artpi) (./projects/art_pi_wifi)

- wolfssl [latest version](https://github.com/wolfSSL/wolfssl)


### `WOLFSSL_WOLFCRYPT_TEST` output of wolfcrypt_test()
```
error    test passed!
MEMORY   test passed!
base64   test passed!
asn      test passed!
RANDOM   test passed!
MD5      test passed!
MD4      test passed!
SHA      test passed!
SHA-256  test passed!
SHA-512  test passed!
Hash     test passed!
HMAC-MD5 test passed!
HMAC-SHA test passed!
HMAC-SHA256 test passed!
HMAC-SHA512 test passed!
X963-KDF    test passed!
GMAC     test passed!
ARC4     test passed!
DES      test passed!
DES3     test passed!
AES      test passed!
AES192   test passed!
AES256   test passed!
AES-GCM  test passed!
AES Key Wrap test passed!
RSA      test passed!
DH       test passed!
DSA      test passed!
PWDBASED test passed!
ECC      test passed!
ECC buffer test passed!
CURVE25519 test passed!
ED25519  test passed!
PKCS7encrypted  test passed!
PKCS7signed     test passed!
PKCS7enveloped  test passed!
PKCS7authenveloped  test passed!
logging  test passed!
mutex    test passed!
memcb    test passed!
```
### `WOLFSSL_BENCHMARK_TEST` output of benchmark_test()
```
------------------------------------------------------------------------------
 wolfSSL version 4.5.0
------------------------------------------------------------------------------
wolfCrypt Benchmark (block bytes 1024, min 1.0 sec each)
RNG                 50 KB took 1.000 seconds,   50.000 KB/s
AES-128-CBC-enc      2 MB took 1.000 seconds,    2.075 MB/s
AES-128-CBC-dec      2 MB took 1.000 seconds,    1.611 MB/s
AES-192-CBC-enc      2 MB took 1.000 seconds,    2.002 MB/s
AES-192-CBC-dec      2 MB took 1.000 seconds,    1.514 MB/s
AES-256-CBC-enc      2 MB took 1.000 seconds,    1.855 MB/s
AES-256-CBC-dec      1 MB took 1.000 seconds,    1.465 MB/s
AES-128-GCM-enc    700 KB took 1.000 seconds,  700.000 KB/s
AES-128-GCM-dec    675 KB took 1.000 seconds,  675.000 KB/s
AES-192-GCM-enc    675 KB took 1.000 seconds,  675.000 KB/s
AES-192-GCM-dec    675 KB took 1.000 seconds,  675.000 KB/s
AES-256-GCM-enc    650 KB took 1.000 seconds,  650.000 KB/s
AES-256-GCM-dec    650 KB took 1.000 seconds,  650.000 KB/s
AES-128-ECB-enc      2 MB took 1.000 seconds,    1.902 MB/s
AES-128-ECB-dec      2 MB took 1.000 seconds,    1.521 MB/s
AES-192-ECB-enc      2 MB took 1.000 seconds,    1.780 MB/s
AES-192-ECB-dec      1 MB took 1.000 seconds,    1.433 MB/s
AES-256-ECB-enc      2 MB took 1.000 seconds,    1.638 MB/s
AES-256-ECB-dec      1 MB took 1.000 seconds,    1.405 MB/s
ARC4                 5 MB took 1.000 seconds,    4.956 MB/s
RABBIT               6 MB took 1.000 seconds,    6.470 MB/s
3DES               750 KB took 1.000 seconds,  750.000 KB/s
MD5                 12 MB took 1.000 seconds,   12.061 MB/s
SHA                  4 MB took 1.000 seconds,    3.979 MB/s
SHA-256              2 MB took 1.000 seconds,    1.782 MB/s
SHA-512              1 MB took 1.000 seconds,    1.001 MB/s
HMAC-MD5            12 MB took 1.000 seconds,   12.329 MB/s
HMAC-SHA             4 MB took 1.000 seconds,    3.662 MB/s
HMAC-SHA256          2 MB took 1.000 seconds,    1.758 MB/s
HMAC-SHA512          1 MB took 1.000 seconds,    1.001 MB/s
PBKDF2             224 bytes took 1.000 seconds,  224.000 bytes/s
RSA     2048 public         20 ops took 1.000 sec, avg 50.000 ms, 20.000 ops/sec
RSA     2048 private         2 ops took 1.000 sec, avg 500.000 ms, 2.000 ops/sec
DH      2048 key gen         4 ops took 1.000 sec, avg 250.000 ms, 4.000 ops/sec
DH      2048 agree           4 ops took 1.000 sec, avg 250.000 ms, 4.000 ops/sec
ECC      256 key gen         6 ops took 1.000 sec, avg 166.667 ms, 6.000 ops/sec
ECDHE    256 agree           6 ops took 1.000 sec, avg 166.667 ms, 6.000 ops/sec
ECDSA    256 sign            6 ops took 1.000 sec, avg 166.667 ms, 6.000 ops/sec
ECDSA    256 verify          4 ops took 1.000 sec, avg 250.000 ms, 4.000 ops/sec
CURVE  25519 key gen         4 ops took 1.000 sec, avg 250.000 ms, 4.000 ops/sec
CURVE  25519 agree           2 ops took 1.000 sec, avg 500.000 ms, 2.000 ops/sec
ED     25519 key gen         4 ops took 1.000 sec, avg 250.000 ms, 4.000 ops/sec
ED     25519 sign            2 ops took 1.000 sec, avg 500.000 ms, 2.000 ops/sec
ED     25519 verify          2 ops took 1.000 sec, avg 500.000 ms, 2.000 ops/sec
```

## References

For more information please contact info@wolfssl.com.
