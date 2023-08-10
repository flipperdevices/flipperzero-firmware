# Example build configurations

Example wolfSSL configuration file templates for use when autoconf is not available, such as building with a custom IDE.

## Files

* `user_settings_template.h`: Template that allows modular algorithm and feature selection using `#if 0` logic.
* `user_settings_all.h`: This is wolfSSL with all features enabled. Equivalent to `./configure --enable-all`.
* `user_settings_min_ecc.h`: This is ECC and SHA-256 only. For ECC verify only add `BUILD_VERIFY_ONLY`.
* `user_settings_wolfboot_keytools.h`: This from wolfBoot tools/keytools and is ECC, RSA, ED25519 and ChaCha20.
* `user_settings_fipsv2.h`: The FIPS v2 (3389) 140-2 certificate build options.
* `user_settings_fipsv5.h`: The FIPS v5 (ready) 140-3 build options. Equivalent to `./configure --enable-fips=v5-dev`.
* `user_settings_stm32.h`: Example configuration file generated from the wolfSSL STM32 Cube pack.
* `user_settings_wolftpm.h`: Minimum options for building wolfTPM. See comment at top for ./configure used to generate.
*.`user_settings_EBSnet.h`: Example configuration file for use with EBSnet ports.

## Usage

1. Copy to your local project and rename to `user_settings.h`.
2. Add pre-processor macro `WOLFSSL_USER_SETTINGS` to your project.
3. Make sure and include `#include <wolfssl/wolfcrypt/settings.h>` prior to any other wolfSSL headers in your application.

## Testing with Autoconf

To use these with autoconf:

1. Copy file to root as `user_settings.h`.
2. Run `./configure --enable-usersettings --disable-examples && make`
