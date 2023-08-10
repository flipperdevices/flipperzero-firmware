# DEMO program with ATECC608A on ESP-WROOM-32SE
## Overview
 Running demo programs with ATECC608A on 32SE by setting `WOLFSSL_ESPWROOM32SE` definition

Including the following examples:

* simple `TLS client`/`server`
* crypt benchmark

 The `user_settings.h` file enables some of the hardened settings.

## Requirements
1. ESP-IDF development framework: https://docs.espressif.com/projects/esp-idf/en/latest/get-started/

2. Microchip CryptoAuthentication Library: https://github.com/MicrochipTech/cryptoauthlib

## Setup
1. Comment out `#define WOLFSSL_ESPWROOM32` in `/path/to/wolfssl/IDE/Espressif/ESP-IDF/user_settings.h`\
   Uncomment out `#define WOLFSSL_ESPWROOM32SE` in `/path/to/wolfssl/IDE/Espressif/ESP-IDF/user_settings.h`
    * **Note:** crypt test will fail if enabled `WOLFSSL_ESPWROOM32SE`
3. wolfSSL under ESP-IDF. Please see [README.md](https://github.com/wolfSSL/wolfssl/blob/master/IDE/Espressif/ESP-IDF/README.md)
4. CryptoAuthentication Library under ESP-IDF. Please see [README.md](https://github.com/miyazakh/cryptoauthlib_esp_idf/blob/master/README.md)

## Configuration
1. The `user_settings.h` can be found in `/path/to/esp-idf/components/wolfssl/include/user_settings.h`

## Build examples
1. See `README` in each example folder

## Benchmark
w/o atecc608a
```
ECC      256 key gen         4 ops took 1.092 sec, avg 273.000 ms, 3.663 ops/sec
ECDHE    256 agree           4 ops took 1.091 sec, avg 272.750 ms, 3.666 ops/sec
ECDSA    256 sign            4 ops took 1.102 sec, avg 275.500 ms, 3.630 ops/sec
ECDSA    256 verify          2 ops took 1.091 sec, avg 545.500 ms, 1.833 ops/sec
```
w/ atecc608a
```
ECC      256 key gen        11 ops took 1.074 sec, avg 97.636 ms, 10.242 ops/sec
ECDHE    256 agree           6 ops took 1.068 sec, avg 178.000 ms, 5.618 ops/sec
ECDSA    256 sign            8 ops took 1.009 sec, avg 126.125 ms, 7.929 ops/sec
ECDSA    256 verify         14 ops took 1.079 sec, avg 77.071 ms, 12.975 ops/sec
```

## Support
For question please email [support@wolfssl.com](mailto:support@wolfssl.com)

Note: This is tested with the following condition:

- Model    : ESP32-WROOM-32SE
- ESP-IDF  : v3.3-beta1-39-g6cb37ecc5(commit hash : 6cb37ecc5)
- CryptAuthLib: commit hash : c6b176e
- OS       : Ubuntu 18.04.1 LTS (Bionic Beaver)
