# ESP32 Port

Support for the ESP32-WROOM-32 on-board crypto hardware acceleration for symmetric AES, SHA1/SHA256/SHA384/SHA512 and RSA primitive including mul, mulmod and exptmod.

## ESP32 Acceleration

For detail about ESP32 HW Acceleration, you can find in [Technical Reference Manual](https://espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf)

### Building

To enable hw acceleration :

* Uncomment out `#define WOLFSSL_ESPIDF` in `/path/to/wolfssl/wolfssl/wolfcrypt/settings.h`
* Uncomment out `#define WOLFSSL_ESPWROOM32` in `/path/to/wolfssl/wolfssl/wolfcrypt/settings.h`

To disable portions of the hardware acceleration you can optionally define:

```c
/* Disabled SHA, AES and RSA acceleration */
#define NO_ESP32WROOM32_CRYPT
/* Disabled AES acceleration */
#define NO_WOLFSSL_ESP32WROOM32_CRYPT_AES
/* Disabled SHA acceleration */
#define NO_WOLFSSL_ESP32WROOM32_CRYPT_HASH
/* Disabled RSA Primitive acceleration */
#define NO_WOLFSSL_ESP32WROOM32_CRYPT_RSA_PRI
```

### Coding

In your application you must include `<wolfssl/wolfcrypt/settings.h>` before any other wolfSSL headers. If building the sources directly we recommend defining `WOLFSSL_USER_SETTINGS` and adding your own `user_settings.h` file. You can find a good reference for this in `IDE/GCC-ARM/Header/user_settings.h`.


### Benchmarks

w/ `USE_FAST_MATH` and `WOLFSSL_SMALL_STACK` options

Software only implementation :

```
AES-128-CBC-enc      1 MB took 1.001 seconds,    1.146 MB/s
AES-128-CBC-dec      1 MB took 1.017 seconds,    1.104 MB/s
AES-192-CBC-enc      1 MB took 1.018 seconds,    1.055 MB/s
AES-192-CBC-dec      1 MB took 1.006 seconds,    1.019 MB/s
AES-256-CBC-enc   1000 KB took 1.000 seconds, 1000.000 KB/s
AES-256-CBC-dec    975 KB took 1.007 seconds,  968.222 KB/s
AES-128-GCM-enc    350 KB took 1.055 seconds,  331.754 KB/s
AES-128-GCM-dec    350 KB took 1.054 seconds,  332.068 KB/s
AES-192-GCM-enc    325 KB took 1.013 seconds,  320.829 KB/s
AES-192-GCM-dec    325 KB took 1.013 seconds,  320.829 KB/s
AES-256-GCM-enc    325 KB took 1.041 seconds,  312.200 KB/s
AES-256-GCM-dec    325 KB took 1.041 seconds,  312.200 KB/s
SHA                  6 MB took 1.004 seconds,    5.714 MB/s
SHA-256              2 MB took 1.006 seconds,    1.747 MB/s
SHA-384              1 MB took 1.011 seconds,    1.159 MB/s
SHA-512              1 MB took 1.009 seconds,    1.161 MB/s
HMAC-SHA             6 MB took 1.001 seconds,    5.634 MB/s
HMAC-SHA256          2 MB took 1.000 seconds,    1.733 MB/s
HMAC-SHA384          1 MB took 1.004 seconds,    1.046 MB/s
HMAC-SHA512          1 MB took 1.002 seconds,    1.048 MB/s
RSA     2048 public         16 ops took 1.056 sec, avg 66.000 ms, 15.152 ops/sec
RSA     2048 private         2 ops took 2.488 sec, avg 1244.000 ms, 0.804 ops/sec
ECC      256 key gen         4 ops took 1.101 sec, avg 275.250 ms, 3.633 ops/sec
ECDHE    256 agree           4 ops took 1.098 sec, avg 274.500 ms, 3.643 ops/sec
ECDSA    256 sign            4 ops took 1.111 sec, avg 277.750 ms, 3.600 ops/sec
ECDSA    256 verify          2 ops took 1.099 sec, avg 549.500 ms, 1.820 ops/sec
```

Hardware Acceleration :


```
AES-128-CBC-enc      6 MB took 1.004 seconds,    5.958 MB/s
AES-128-CBC-dec      5 MB took 1.002 seconds,    5.287 MB/s
AES-192-CBC-enc      6 MB took 1.004 seconds,    5.958 MB/s
AES-192-CBC-dec      5 MB took 1.002 seconds,    5.287 MB/s
AES-256-CBC-enc      6 MB took 1.001 seconds,    5.951 MB/s
AES-256-CBC-dec      5 MB took 1.004 seconds,    5.277 MB/s
AES-128-GCM-enc    375 KB took 1.067 seconds,  351.453 KB/s
AES-128-GCM-dec    375 KB took 1.067 seconds,  351.453 KB/s
AES-192-GCM-enc    350 KB took 1.010 seconds,  346.535 KB/s
AES-192-GCM-dec    350 KB took 1.009 seconds,  346.878 KB/s
AES-256-GCM-enc    350 KB took 1.016 seconds,  344.488 KB/s
AES-256-GCM-dec    350 KB took 1.016 seconds,  344.488 KB/s
SHA                 14 MB took 1.000 seconds,   14.062 MB/s
SHA-256             15 MB took 1.000 seconds,   15.234 MB/s
SHA-384             17 MB took 1.000 seconds,   17.383 MB/s
SHA-512             18 MB took 1.001 seconds,   17.512 MB/s
HMAC-SHA            14 MB took 1.000 seconds,   13.818 MB/s
HMAC-SHA256         15 MB took 1.001 seconds,   14.951 MB/s
HMAC-SHA384         17 MB took 1.001 seconds,   16.683 MB/s
HMAC-SHA512         17 MB took 1.000 seconds,   16.943 MB/s
RSA     2048 public         20 ops took 1.017 sec, avg 50.850 ms, 19.666 ops/sec
RSA     2048 private         4 ops took 1.059 sec, avg 264.750 ms, 3.777 ops/sec
ECC      256 key gen         4 ops took 1.092 sec, avg 273.000 ms, 3.663 ops/sec
ECDHE    256 agree           4 ops took 1.089 sec, avg 272.250 ms, 3.673 ops/sec
ECDSA    256 sign            4 ops took 1.101 sec, avg 275.250 ms, 3.633 ops/sec
ECDSA    256 verify          2 ops took 1.092 sec, avg 546.000 ms, 1.832 ops/sec
```

Condition  :  
- Model    : ESP32-WROOM-32  
- CPU Speed: 240Mhz  
- ESP-IDF  : v3.3-beta1-39-g6cb37ecc5(commit hash : 6cb37ecc5)  
- OS       : Ubuntu 18.04.1 LTS (Bionic Beaver)

## Support

Email us at [support@wolfssl.com](mailto:support@wolfssl.com).
