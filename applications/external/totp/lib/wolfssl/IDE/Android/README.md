# Android wolfSSL Support

Tested on Android v8.1 with WPA Supplicant and KeyStore to replace BoringSSL.

## Files

* `Android.bp`: Template build system file for wolfSSL.
* `user_settings.h`: Template build settings for wolfSSL

## Installation

1) Place the wolfSSL library into `./external/wolfssl`
2) Copy `Android.bp` into `./external/wolfssl`
3) Copy `user_settings.h` into `./external/wolfssl`
4) Add `PRODUCT_PACKAGES += libwolfssl` to your device .mk.

## Typical Android build instruction

```sh
source build/envsetup.sh
lunch [num]
mm -j8
```

## Using wolfSSL in your Application

In your `Android.mk` build file for your application add the following:

```makefile
# Crypto Provider - wolfSSL
LOCAL_CFLAGS += -DWOLFSSL_USER_SETTINGS -Iexternal/wolfssl -Iexternal/wolfssl/wolfssl
LOCAL_SHARED_LIBRARIES += libwolfssl
```

## Support

For questions please email support@wolfssl.com
