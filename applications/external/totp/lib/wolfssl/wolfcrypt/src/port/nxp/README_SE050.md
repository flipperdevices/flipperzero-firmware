# wolfSSL NXP SE050 Support

wolfSSL includes support for the NXP SE050 Plug & Trust Secure Element.

For details about the NXP SE050, see [NXP's SE050 page](https://www.nxp.com/products/security-and-authentication/authentication/edgelock-se050-plug-trust-secure-element-family-enhanced-iot-security-with-maximum-flexibility:SE050).

## SE050 Acceleration

wolfSSL supports the following hardware acceleration with SE050:

- TRNG
- AES (128, 192, 256) encrypt/decrypt
- SHA-1, SHA2-224, SHA2-256, SHA2-384, SHA2-512
- ECC support and key generation (NIST 192-521 bit, Brainpool, Koblitz)
    * ECDSA sign/verify and key generation
    * ECDH shared secret generation
- Ed25519 sign/verify and key generation (Twisted Edwards)
- Curve25519 shared secret and key generation
- RSA sign/verify/encrypt/decrypt and key generation (up to 4096-bit)

## Building SE05x Plug & Trust Middleware (simw-top)

wolfSSL uses the "EdgeLock SE05x Plug & Trust Middleware" to interface with
SE050. This can be downloaded from the NXP website [here](https://www.nxp.com/products/security-and-authentication/authentication/edgelock-se050-plug-trust-secure-element-family-enhanced-iot-security-with-high-flexibility:SE050#design-resources).
An free NXP account is required to download the middleware.

wolfSSL last tested with SE05x middleware version 04.02.00.

Instructions for building will vary on target platform and host operating
system. A Raspberry Pi with an NXP EdgeLock SE050 Development Kit can be used
to easily set up and test the SE050. For build instructions on this combination,
follow the AN12570 application note (EdgeLock SE05x Quick start guide with
Raspberry Pi, [here](https://www.nxp.com/docs/en/application-note/AN12570.pdf).

Summarizing the build steps for Raspberry Pi:

```sh
$ cd ~
$ mkdir se_mw
$ unzip SE-PLUG-TRUST-MW.zip -d se_mw
$ cd se_mw/simw-top/scripts
$ python create_cmake_projects.py rpi
$ cd ~/se_mw/simw-top_build/raspbian_native_se050_t1oi2c
$ ccmake .
# Make sure the following are set:
#    `Host OS` to `Raspbian`
#    `Host Crypto` to `None` (see HostCrypto section below)
#    `SMCOM` to `T1oI2C`
$ c # to configure
$ g # to generate
$ q
$ cmake --build .
$ sudo make install
```

This will also compile several demo apps which can be run if wanted, ie:

```sh
$ cd ~/se_mw/simw-top_build/raspbian_native_se050_t1oi2c/bin
$ ./ex_ecc  # (or, ./se05x_GetInfo, etc)
```

Running `sudo make install` will install SE050 library and header files to:

    /usr/local/lib
    /usr/local/include/se05x

### Customizing SE05x Middleware

The SE05x Middleware can be configured to use a custom `fsl_sss_ftr.h` header
file when **`SSS_USE_FTR_FILE`** is defined when compiling the SDK.

When wolfSSL tested SE050 integration on an embedded target, `fsl_sss_ftr.h`:

- Enabled SE050 variant C (`SSS_HAVE_APPLET_SE05X_C`)
- Enabled SE05X applet version 03 for SE050 (`SSS_HAVE_SE05X_VER_03_XX`)
- Enabled wolfSSL HostCrypto support (`SSS_HAVE_HOSTCRYPTO_WOLFSSL`)
- Disabled mbedTLS alt API (`SSS_HAVE_MBEDTLS_ALT_NONE`)
- Enabled SSS layer for SCP03 (`SSS_HAVE_SCP_SCP03_SSS`)
- Enabled Platform SCP03 (`SSS_HAVE_SE05X_AUTH_PLATFSCP03`)
- Set default SCP03 ENC/MAC/DEK keys to match variant in use
- Algorithm selection left same as default configuration

## Building wolfSSL

To compile wolfSSL with SE050 support using Autoconf/configure:

```sh
$ cd wolfssl-X.X.X
$ ./configure --with-se050
OR
$ ./configure --with-se050=PATH
$ make
```

If no installation path is provided to `--with-se05x`, wolfSSL will use the
default installation locations above.

Example: `--with-se050=/home/pi/se_mw/simw-top/`

If the SE05x middleware libraries have been linked against OpenSSL (on Linux),
and you run into compiler errors in wolfSSL due to conflicts with the wolfSSL
compatibility layer headers when compiling wolfSSL's examples and test apps,
you can compile and install only the wolfSSL library proper using:

```sh
$ cd wolfssl-X.X.X
$ ./configure --with-se050 <options>
$ make src/libwolfssl.la
$ sudo make install-binPROGRAMS
$ sudo make install-nobase_includeHEADERS
```

### wolfSSL Key Generation Inside SE050

wolfSSL can generate RSA and ECC keys inside the SE050. To include that support,
wolfSSL should be configured with `--enable-keygen` or `-DWOLFSSL_KEY_GEN`.

```sh
$ ./configure --with-se050 --enable-keygen
```

### wolfSSL HostCrypto support for SCP03 Authentication

wolfSSL can be used on the host side (HostCrypto) for secure SCP03
authentication, in place of OpenSSL or mbedTLS. See the HostCrypto section
below. To support SCP03, wolfSSL also needs to be compiled with CMAC support:

```
$ cd wolfssl-X.X.X
$ ./configure --with-se050 --enable-keygen --enable-cmac
```

To disable SCP03 and use a non-authenticated I2C connection, wolfSSL was using
the following define set in `fsl_sss_ftr.h`, with other defines left to the
defaults:

```c
#define SSS_HAVE_APPLET_SE05X_C 1
#define SSS_HAVE_SE05X_VER_03_XX 1
#define SSS_HAVE_HOSTCRYPTO_NONE 1
#define SSS_HAVE_MBEDTLS_ALT_NONE 1
#define SSS_HAVE_SCP_NONE 1
#define SSS_HAVE_SCP_SCP03_SSS 0
#define SSS_HAVE_SCP_SCP03_HOSTCRYPTO 0
#define SSS_HAVE_SE05X_AUTH_NONE 1
#define SSS_HAVE_SE05X_AUTH_PLATFSCP03 0
```

To enable SCP03 authentication, wolfSSL was using the following defines:

```c
#define SSS_HAVE_APPLET_SE05X_C 1
#define SSS_HAVE_SE05X_VER_03_XX 1
#define SSS_HAVE_HOSTCRYPTO_WOLFSSL 1
#define SSS_HAVE_HOSTCRYPTO_NONE 0
#define SSS_HAVE_MBEDTLS_ALT_NONE 1
#define SSS_HAVE_SCP_NONE 0
#define SSS_HAVE_SCP_SCP03_SSS 1
#define SSS_HAVE_SCP_SCP03_HOSTCRYPTO 0
#define SSS_HAVE_SE05X_AUTH_NONE 0
#define SSS_HAVE_SE05X_AUTH_PLATFSCP03 1
```

Default ENC, MAC, and DEK keys for SCP03 should be set by defining the
following values. These are the default keys wolfSSL used for the SE50C2
variant (OEF OID: A201). The variant can be seen by running the
`se05x_GetInfo` sample application.

```c
#define EX_SSS_AUTH_SE05X_KEY_ENC SSS_AUTH_SE050C2_KEY_ENC
#define EX_SSS_AUTH_SE05X_KEY_MAC SSS_AUTH_SE050C2_KEY_MAC
#define EX_SSS_AUTH_SE05X_KEY_DEK SSS_AUTH_SE050C2_KEY_DEK
```

Default SCP03 keys are located in the following middleware file:

```sh
<middleware>/simw-top/sss/ex/inc/ex_sss_tp_scp03_keys.h
```

### wolfSSL SE050 Build Customization

There are several preprocessor defines that can control wolfSSL's SE050
integration behavior, including:

**`WOLFSSL_SE050_INIT`**

wolfSSL will initialize the SE050 internally. When this is used, developers
also need to define `SE050_DEFAULT_PORT`. See wolfSSL library initialization
below.

**`SE050_DEFAULT_PORT`**

Mentinoed above, this should be defined to match the mount location of the
SE050 if on Linux. Or defined to **NULL** for embedded targets.

**`SE050_KEYID_START`**

When generating keys inside SE050, wolfSSL will automatically pick a key ID
value based on an incrementing counter past the value defined by this define.

If not defined, this value will default to **100**.

**`WOLFSSL_SE050_FACTORY_RESET`**

When defined, calls to `wolfSSL_Init()` or `wolfCrypt_Init()` will factory
reset the SE050 board by calling `ex_sss_boot_factory_reset()` internally.

This will erase all user-provisioned key and credential material, leaving only
the NXP pre-provisioned credentials in place.

**`WOLFSSL_SE050_HASH`**

wolfSSL supports offloading hash operations (SHA-1, SHA2-224, SHA2-256,
SHA2-384, SHA2-512) to the SE050. This is MUCH slower than using wolfCrypt
software cryptography due to the I2C communication channel. This support is
disabled by default unless this define explicitly enables it.

**`WOLFSSL_SE050_CRYPT`**

wolfSSL supports offloading symmetric crypto operations (AES-ECB/CBC) to the
SE050. Also due to the I2C communication channel, this is MUCH slower than using
wolfCrypt software crypto. This support is disabled by default unless this
define explicitly enables it.

**`WOLFSSL_SE050_NO_TRNG`**

By default when `WOLFSSL_SE050` is defined, wolfSSL will try to use the TRNG
on the SE050 device as a TRNG for seeding wolfCrypt's PRNG/DRBG. To disable
the use of the SE050 TRNG inside wolfCrypt and instead fall back to the system
default, this can be defined. This might be used for example when working on
a Raspberry Pi with SE05x EdgeLock dev kit. If `WOLFSSL_SE050_NO_TRNG` is
defined, wolfCrypt will instead fall back to using `/dev/random` and
`/dev/urandom` on the Raspberry Pi.

## wolfSSL HostCrypto Support

The NXP SE05x Plug & Trust Middleware by default can use either OpenSSL or
mbedTLS has a HostCrypto provider to support secure SCP03 authenticated
communication between the host and SE050. The HostCrypto provider is used
for AES CMAC operations on the host side to set up and authenticate the SE050.
If SCP03 is not used, a plaintext communication channel can be used.

wolfSSL has implemented a HostCrypto layer that can integrate into the
SE05x Middleware to provide an alternative crypto provider to OpenSSL or
mbedTLS. To learn more about access to this layer, please contact wolfSSL
at support@wolfssl.com.

Once a SE05x Middleware source tree has been updated with wolfSSL HostCrypto
support, wolfSSL support can be enabled by defining
`SSS_HAVE_HOSTCRYPTO_WOLFSSL` in `fsl_sss_ftr.h`. In this scenario, all of the
following must be defined to 0 in `fsl_sss_ftr.h`:

```c
#define SSS_HAVE_HOSTCRYPTO_WOLFSSL 1
#define SSS_HAVE_HOSTCRYPTO_MBEDTLS 0
#define SSS_HAVE_HOSTCRYPTO_OPENSSL 0
#define SSS_HAVE_HOSTCRYPTO_USER 0
#define SSS_HAVE_HOSTCRYPTO_NONE 0
```
## wolfSSL Usage with NXP SE050

### Library and SE050 Initialization and Cleanup

When looking at NXP's SE050 demo applications, developers will notice that the
connection to SE050 is handled by shared code in
`<middleware>/sss/ex/inc/ex_sss_main_inc.h`. This code calls
`ex_sss_boot_open()` to open the connection to SE050. NXP demo applications then
implement an `ex_sss_entry()` function, which is called by `main()` in
`ex_sss_main_inc.h`.

wolfSSL has the ability to open the connection to SE050 internally upon library
initialization. This can make it much easier and simpler for developers who will
be using the SE050 primarily underneath the wolfSSL APIs.

wolfSSL will initialize the SE050 when the wolfSSL library has been compiled
with `WOLFSSL_SE050_INIT` defined. When this is used, applications need to:

1. Also define `SE050_DEFAULT_PORT` when compiling wolfSSL
(ie: `user_settings.h`) to match the mount location of SE050 if on Linux. Or, if
on an embedded target, this should be defined to **NULL**.

2. Application code should initialize the wolfSSL library like normal with
`wolfSSL_Init()` or `wolfCrypt_Init()`. When the application is done using
wolfSSL, resources can be freed and the SE050 connection closed using
`wolfSSL_Cleanup()` or `wolfCrypt_Cleanup()`. For example:

```c
/* Initialize wolfCrypt, debugging, logging */
wolfCrypt_Init();
wolfSSL_SetLoggingCb(my_logging_cb);
wolfSSL_Debugging_ON();
...
wolfCrypt_Cleanup();
```

If `WOLFSSL_SE050_INIT` has not been defined when compiling wolfSSL, the
following API can be called after wolfSSL library initialization to pass the
correct pre-initialized `sss_session_t` and `sss_key_store_t` structure
pointers to wolfSSL for internal use. These structures would need to be set up
by the application using NXP's SSS API from the middleware SDK.

```c
#include <wolfssl/wolfcrypt/port/nxp/se050_port.h>
int wc_se050_set_config(
        sss_session_t *pSession,
        sss_key_store_t *pHostKeyStore,
        sss_key_store_t *pKeyStore);
```

### wolfSSL SE050 Key Generation

wolfSSL includes APIs for key generation when `WOLFSSL_KEY_GEN` has been
defined while compiling wolfSSL. When wolfSSL has been compiled with SE050
support (`WOLFSSL_SE050`), it will delegate these key generation operations to
the SE050 and the private keys will remain in the SE050 for added security.

wolfSSL APIs that will generate keys internal to SE050 are:

```c
int wc_ecc_make_key(WC_RNG* rng, int keysize, ecc_key* key);
int wc_MakeRsaKey(RsaKey* key, int size, long e, WC_RNG* rng);
int wc_ed25519_make_key(WC_RNG* rng, int keysize, ed25519_key* key);
int wc_curve25519_make_key(WC_RNG* rng, int keysize, curve25519_key* key);
```

wolfSSL will also use the SE050 for ECDH shared secret generation, but will
extract the shared secret to hand back to the application.

When generating keys in SE050 wolfSSL will automatically pick a key ID value
based on an incrementing counter past the value defined by `SE050_KEYID_START`.
`SE050_KEYID_START` should be defined when compiling wolfSSL
(`user_settings.h`), otherwise it will default to 100.

### wolfSSL SE050 Key Insertion

Applications can insert public or private RSA and ECC keys into the SE050 at a
specific key ID using the following wolfSSL helper functions:

```c
#include <wolfssl/wolfcrypt/port/nxp/se050_port.h>

int wc_se050_ecc_insert_public_key(word32 keyId,
        const byte* eccDer, word32 eccDerSize);
int wc_se050_ecc_insert_private_key(word32 keyId,
        const byte* eccDer, word32 eccDerSize);

int wc_se050_rsa_insert_public_key(word32 keyId,
        const byte* rsaDer, word32 rsaDerSize);
int wc_se050_rsa_insert_private_key(word32 keyId,
        const byte* rsaDer, word32 rsaDerSize);
```

These APIs will all return 0 on success or a negative error code on failure.
The input to all these functions is a DER-encoded key and the size of that DER
array in bytes.

### wolfSSL SE050 Certificate Insertion and Retrieval

Applications can insert or retrieve certificates or binary data into an SE050
key ID using the following wolfSSL helper functions:

```c
int wc_se050_insert_binary_object(word32 keyId,
        const byte* object, word32 objectSz);
int wc_se050_get_binary_object(word32 keyId,
        byte* out, word32* outSz);
```

These APIs will all return 0 on success or a negative error code on failure.
The input to `wc_se050_insert_binary_object()` is a byte array to be stored in
the SE050 along with the size of that array in bytes.

The arguments to `wc_se050_get_binary_object()` are the key ID to retrieve data
from, the output array for data to be placed, and an IN/OUT variable “outSz”
representing the size of the “out” buffer on input, and on output “outSz” gets
set to the number of bytes written into “out”.

### wolfSSL SE050 Credential Deletion

wolfSSL will not auto-delete generated keys associated with wolfCrypt
structures (ex: `RsaKey`, `ecc_key`, etc) when the respective key free function
is called (ex: `wc_ecc_free()`, `wc_FreeRsaKey()`). This is done by design in
case the application wants to reuse that key that has been generated and
stored in the SE050.

Credentials can be deleted from the SE050 storage by calling the wolfSSL helper
function `wc_se050_erase_object(int keyId)`. This function is available through
`<wolfssl/wolfcrypt/port/nxp/se050_port.h>`, and should be passed the key ID
to be deleted.

### wolfSSL SE050 Factory Reset

If wolfSSL is compiled with `WOLFSSL_SE050_FACTORY_RESET` defined, when
`wolfSSL_Init()` or `wolfCrypt_Init()` is called, wolfSSL will factory reset
the SE050 board by calling `ex_sss_boot_factory_reset()` internally.

This will erase all user-provisioned key and credential material, leaving only
the NXP pre-provisioned credentials in place.

## Building wolfSSL SE050 Examples

wolfSSL demos can be easily added to the SE05x middleware source tree such that
they are build with CMake when the middleware is compiled.

Assuming a Raspberry Pi host platform is being used, with an SE05x EdgeLock
dev kit:

1. Create a `wolfssl` directory under the demos directory for wolfSSL demos:

```sh
$ mkdir /home/pi/se_mw/simw-top/demos/wolfssl
```

2. Create a directory for a wolfSSL demo, for example to create one for the
wolfCrypt test application:

```sh
$ mkdir /home/pi/se_mw/simw-top/demos/wolfssl/wolfcrypt_test
```

3. Create a CMakeLists.txt to put inside `demos/wolfssl`, tying the
`wolfcrypt_test` app into CMake. This CMakeLists.txt would contain:

```cmake
ADD_SUBDIRECTORY(wolfcrypt_test)
```

4. Add the `demos/wolfssl` directory to the top `demos/CMakeLists.txt` file.
At the bottom of that file, place:

```cmake
ADD_SUBDIRECTORY(wolfssl)
```

5. Inside `demos/wolfssl/wolfcrypt_test`, copy the wolfCrypt `test.c` and
`test.h` files from a wolfSSL installation:

```sh
$ cd /home/pi/se_mw/simw-top/demos/wolfssl/wolfcrypt_test
$ cp wolfssl-X.X.X/wolfcrypt/test/test.c ./
$ cp wolfssl-X.X.X/wolfcrypt/test/test.h ./
```

6. Create a file called `wolfcrypt_test.c` which will act as the demo
application. That file would look similar to:

```c
#ifdef __cplusplus
extern "C" {
#endif

#include <wolfssl/options.h>
#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/port/nxp/se050_port.h>
#include <wolfssl/ssl.h>
#include "test.h"

#include <ex_sss_boot.h>
#include <fsl_sss_se05x_apis.h>
#include <nxLog_App.h>

#ifdef __cplusplus
}
#endif

#if defined(SIMW_DEMO_ENABLE__DEMO_WOLFCRYPTTEST)

static ex_sss_boot_ctx_t gex_sss_boot_ctx;

#define EX_SSS_BOOT_PCONTEXT (&gex_sss_boot_ctx)
#define EX_SSS_BOOT_DO_ERASE 1
#define EX_SSS_BOOT_EXPOSE_ARGC_ARGV 0

#include <ex_sss_main_inc.h>

sss_status_t ex_sss_entry(ex_sss_boot_ctx_t *pCtx)
{
    int ret = 0;
    sss_status_t status = kStatus_SSS_Success;
    sss_session_t *pSession = (sss_session_t*)&pCtx->session;
    sss_key_store_t *pKeyStore = (sss_key_store_t*)&pCtx->ks;

    LOG_I("running setconfig");
    ret = wc_se050_set_config(pSession, NULL, pKeyStore);
    if (ret != 0) {
        LOG_E("wc_se050_set_config failed");
        return kStatus_SSS_Fail;
    }
    LOG_I("Ran setconfig successfully");

    wolfSSL_Init();
    wolfcrypt_test(NULL);
    wolfSSL_Cleanup();

    LOG_I("Ran wolfCrypt test");
    return status;
}

#endif /* SIMW_DEMO_ENABLE__DEMO_WOLFCRYPTTEST */
```

7. Create a CMakeLists.txt inside `demos/wolfssl/wolfcrypt_test` so that it
can be compiled with CMake:

```cmake
PROJECT(wolfcrypt_test)
FILE(
    GLOB
    files
    *.c
)

ADD_EXECUTABLE(
    ${PROJECT_NAME}
    ${KSDK_STARTUP_FILE} ${files}
)

TARGET_COMPILE_DEFINITIONS(
    ${PROJECT_NAME}
    PRIVATE SIMW_DEMO_ENABLE__DEMO_WOLFCRYPTTEST NO_MAIN_DRIVER BENCH_EMBEDDED USE_CERT_BUFFERS_2048 USE_CERT_BUFFERS_256
)

TARGET_INCLUDE_DIRECTORIES(
    ${PROJECT_NAME}
    PRIVATE ${SIMW_TOP_DIR}/sss/ex/inc /home/pi/se_mw/wolfssl
)

TARGET_LINK_LIBRARIES(
    ${PROJECT_NAME}
    SSS_APIs
    ex_common
    wolfssl
)

CREATE_BINARY(${PROJECT_NAME})

IF(SSS_HAVE_HOST_LINUX_LIKE)
    INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)
ENDIF()
```

8. Build the demo app with CMake

The wolfCrypt test demo app should now compile along with the SE05x middleware.
This assumes that the NXP instructions for setting up the build have been
completed.

```sh
$ cd /home/pi/se_mw/simw-top_build/raspbian_native_se050_t1oi2c
$ cmake --build .
```

Once the build has finished, the `wolfcrypt_test` executable can be run with:

```sh
$ cd /home/pi/se_mw/simw-top_build/raspbian_native_se050_t1oi2c/bin
$ ./wolfcrypt_test
```

