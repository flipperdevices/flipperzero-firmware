# ST Ports

Support for the STM32 L4, F1, F2, F4 and F7 on-board crypto hardware acceleration:
 - symmetric AES (ECB/CBC/CTR/GCM)
 - MD5/SHA1/SHA224/SHA256

Support for the STM32 PKA on WB55, H7 and other devices with on-board public-key acceleration:
 - ECC192/ECC224/ECC256/ECC384

Support for the STSAFE-A100 crypto hardware accelerator co-processor via I2C for ECC supporting NIST or Brainpool 256-bit and 384-bit curves. It requires the ST-Safe SDK including wolf stsafe_interface.c/.h files. Please contact ST for these.


For details see our [wolfSSL ST](https://www.wolfssl.com/docs/stm32/) page.


## STM32 Symmetric Acceleration

We support using the STM32 CubeMX and Standard Peripheral Library.

### Building

To enable support define one of the following:

```
#define WOLFSSL_STM32L4
#define WOLFSSL_STM32F1
#define WOLFSSL_STM32F2
#define WOLFSSL_STM32F4
#define WOLFSSL_STM32F7
```

To use CubeMX define `WOLFSSL_STM32_CUBEMX` otherwise StdPeriLib is used.

To disable portions of the hardware acceleration you can optionally define:

```
#define NO_STM32_RNG
#define NO_STM32_CRYPTO
#define NO_STM32_HASH
```

### Coding

In your application you must include <wolfssl/wolfcrypt/settings.h> before any other wolfSSL headers. If building the sources directly we recommend defining `WOLFSSL_USER_SETTINGS` and adding your own `user_settings.h` file. You can find a good reference for this in `IDE/GCC-ARM/Header/user_settings.h`.


### Benchmarks

See our [benchmarks](https://www.wolfssl.com/docs/benchmarks/) on the wolfSSL website.


## STM32 PKA (Public Key Acceleration)

STM32 PKA is present in STM32WB55 as well as STM32H7 series.

### Building

To enable support define the following

`WOLFSSL_STM32_PKA`

### Using

When the support is enabled, the ECC operations will be accelerated using the PKA crypto co-processor.

## STSAFE-A100 ECC Acceleration

Using the wolfSSL PK callbacks and the reference ST Safe reference API's we support an ECC only cipher suite such as ECDHE-ECDSA-AES128-SHA256 for TLS client or server.

At the wolfCrypt level we also support ECC native API's for `wc_ecc_*` using the ST-Safe.

### Building

`./configure --enable-pkcallbacks CFLAGS="-DWOLFSSL_STSAFEA100"`

or 

`#define HAVE_PK_CALLBACKS`
`#define WOLFSSL_STSAFEA100`


### Coding

Setup the PK callbacks for TLS using:

```
/* Setup PK Callbacks for STSAFE-A100 */
WOLFSSL_CTX* ctx;
wolfSSL_CTX_SetEccKeyGenCb(ctx, SSL_STSAFE_CreateKeyCb);
wolfSSL_CTX_SetEccSignCb(ctx, SSL_STSAFE_SignCertificateCb);
wolfSSL_CTX_SetEccVerifyCb(ctx, SSL_STSAFE_VerifyPeerCertCb);
wolfSSL_CTX_SetEccSharedSecretCb(ctx, SSL_STSAFE_SharedSecretCb);
wolfSSL_CTX_SetDevId(ctx, 0); /* enables wolfCrypt `wc_ecc_*` ST-Safe use */
```

The reference STSAFE-A100 PK callback functions are located in the `wolfcrypt/src/port/st/stsafe.c` file.

Adding a custom context to the callbacks:

```
/* Setup PK Callbacks context */
WOLFSSL* ssl;
void* myOwnCtx;
wolfSSL_SetEccKeyGenCtx(ssl, myOwnCtx);
wolfSSL_SetEccVerifyCtx(ssl, myOwnCtx);
wolfSSL_SetEccSignCtx(ssl, myOwnCtx);
wolfSSL_SetEccSharedSecretCtx(ssl, myOwnCtx);
```

### Benchmarks and Memory Use

Software only implementation (STM32L4 120Mhz, Cortex-M4, Fast Math):

```
ECDHE    256 key gen       SW    4 ops took 1.278 sec, avg 319.500 ms,  3.130 ops/sec
ECDHE    256 agree         SW    4 ops took 1.306 sec, avg 326.500 ms,  3.063 ops/sec
ECDSA    256 sign          SW    4 ops took 1.298 sec, avg 324.500 ms,  3.082 ops/sec
ECDSA    256 verify        SW    2 ops took 1.283 sec, avg 641.500 ms,  1.559 ops/sec
```

Memory Use:

```
Peak Stack: 18456
Peak Heap: 2640
Total: 21096
```


STSAFE-A100 acceleration:

```
ECDHE    256 key gen       HW    8 ops took 1.008 sec, avg 126.000 ms,  7.937 ops/sec
ECDHE    256 agree         HW    6 ops took 1.051 sec, avg 175.167 ms,  5.709 ops/sec
ECDSA    256 sign          HW   14 ops took 1.161 sec, avg  82.929 ms, 12.059 ops/sec
ECDSA    256 verify        HW    8 ops took 1.184 sec, avg 148.000 ms,  6.757 ops/sec
```

Memory Use:

```
Peak Stack: 9592
Peak Heap: 170
Total: 9762
```


## Support

Email us at [support@wolfssl.com](mailto:support@wolfssl.com).
