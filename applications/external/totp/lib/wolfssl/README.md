<a href="https://repology.org/project/wolfssl/versions">
    <img src="https://repology.org/badge/vertical-allrepos/wolfssl.svg" alt="Packaging status" align="right">
</a>

# wolfSSL Embedded SSL/TLS Library

The [wolfSSL embedded SSL library](https://www.wolfssl.com/products/wolfssl/) 
(formerly CyaSSL) is a lightweight SSL/TLS library written in ANSI C and
targeted for embedded, RTOS, and resource-constrained environments - primarily
because of its small size, speed, and feature set.  It is commonly used in
standard operating environments as well because of its royalty-free pricing
and excellent cross platform support. wolfSSL supports industry standards up
to the current [TLS 1.3](https://www.wolfssl.com/tls13) and DTLS 1.2, is up to
20 times smaller than OpenSSL, and offers progressive ciphers such as ChaCha20,
Curve25519, Blake2b and Post-Quantum TLS 1.3 groups. User benchmarking and
feedback reports dramatically better performance when using wolfSSL over
OpenSSL.

wolfSSL is powered by the wolfCrypt cryptography library. Two versions of
wolfCrypt have been FIPS 140-2 validated (Certificate #2425 and
certificate #3389). FIPS 140-3 validation is in progress. For additional
information, visit the [wolfCrypt FIPS FAQ](https://www.wolfssl.com/license/fips/)
or contact fips@wolfssl.com.

## Why Choose wolfSSL?

There are many reasons to choose wolfSSL as your embedded, desktop, mobile, or
enterprise SSL/TLS solution. Some of the top reasons include size (typical
footprint sizes range from 20-100 kB), support for the newest standards
(SSL 3.0, TLS 1.0, TLS 1.1, TLS 1.2, TLS 1.3, DTLS 1.0, and DTLS 1.2), current
and progressive cipher support (including stream ciphers), multi-platform,
royalty free, and an OpenSSL compatibility API to ease porting into existing
applications which have previously used the OpenSSL package. For a complete
feature list, see [Chapter 4](https://www.wolfssl.com/docs/wolfssl-manual/ch4/)
of the wolfSSL manual.

## Notes, Please Read

### Note 1
wolfSSL as of 3.6.6 no longer enables SSLv3 by default.  wolfSSL also no longer
supports static key cipher suites with PSK, RSA, or ECDH. This means if you
plan to use TLS cipher suites you must enable DH (DH is on by default), or
enable ECC (ECC is on by default), or you must enable static key cipher suites
with one or more of the following defines:

```
WOLFSSL_STATIC_DH
WOLFSSL_STATIC_RSA
WOLFSSL_STATIC_PSK
```
Though static key cipher suites are deprecated and will be removed from future
versions of TLS.  They also lower your security by removing PFS.

When compiling `ssl.c`, wolfSSL will now issue a compiler error if no cipher
suites are available. You can remove this error by defining
`WOLFSSL_ALLOW_NO_SUITES` in the event that you desire that, i.e., you're
not using TLS cipher suites.

### Note 2
wolfSSL takes a different approach to certificate verification than OpenSSL
does. The default policy for the client is to verify the server, this means
that if you don't load CAs to verify the server you'll get a connect error,
no signer error to confirm failure (-188).

If you want to mimic OpenSSL behavior of having `SSL_connect` succeed even if
verifying the server fails and reducing security you can do this by calling:

```c
wolfSSL_CTX_set_verify(ctx, WOLFSSL_VERIFY_NONE, NULL);
```

before calling `wolfSSL_new();`. Though it's not recommended.

### Note 3
The enum values SHA, SHA256, SHA384, SHA512 are no longer available when
wolfSSL is built with `--enable-opensslextra` (`OPENSSL_EXTRA`) or with the
macro `NO_OLD_SHA_NAMES`. These names get mapped to the OpenSSL API for a
single call hash function. Instead the name `WC_SHA`, `WC_SHA256`, `WC_SHA384` and
`WC_SHA512` should be used for the enum name.


# wolfSSL Release 5.6.3 (Jun 20, 2023)

Release 5.6.3 has been developed according to wolfSSL's development and QA process (see link below) and successfully passed the quality criteria.

Release 5.6.3 of wolfSSL embedded TLS has 4 bug fixes:

* Fix for setting the atomic macro options introduced in release 5.6.2. This issue affects GNU gcc autoconf builds. The fix resolves a potential mismatch of the generated macros defined in options.h file and the macros used when the wolfSSL library is compiled. In version 5.6.2 this mismatch could result in unstable runtime behavior.
* Fix for invalid suffix error with Windows build using the macro GCM_TABLE_4BIT.
* Improvements to Encrypted Memory support (WC_PROTECT_ENCRYPTED_MEM) implementations for modular exponentiation in SP math-all (sp_int.c) and TFM (tfm.c).
* Improvements to SendAlert for getting output buffer.


# wolfSSL Release 5.6.2 (Jun 09, 2023)

Release 5.6.2 has been developed according to wolfSSL's development and QA process (see link below) and successfully passed the quality criteria.
https://www.wolfssl.com/about/wolfssl-software-development-process-quality-assurance

NOTE: * --enable-heapmath is being deprecated and will be removed by 2024

Release 5.6.2 of wolfSSL embedded TLS has bug fixes and new features including:

## Vulnerabilities
* [Low] In cases where a malicious agent could analyze cache timing at a very detailed level, information about the AES key used could be leaked during T/S Box lookups. One such case was shown on RISC-V hardware using the MicroWalk tool (https://github.com/microwalk-project/Microwalk). A hardened version of T/S Box lookups was added in wolfSSL to help mitigate this potential attack and is now on by default with RISC-V builds and can be enabled on other builds if desired by compiling wolfSSL with the macro WOLFSSL_AES_TOUCH_LINES. Thanks to Jan Wichelmann, Christopher Peredy, Florian Sieck, Anna Pätschke, Thomas Eisenbarth (University of Lübeck): MAMBO-V: Dynamic Side-Channel Leakage Analysis on RISC-V. Fixed in the following GitHub pull request https://github.com/wolfSSL/wolfssl/pull/6309
* [High] In previous versions of wolfSSL if a TLS 1.3 client gets neither a PSK (pre shared key) extension nor a KSE (key share extension) when connecting to a malicious server, a default predictable buffer gets used for the IKM value when generating the session master secret. Using a potentially known IKM value when generating the session master secret key compromises the key generated, allowing an eavesdropper to reconstruct it and potentially allowing surreptitious access to or meddling with message contents in the session. This issue does not affect client validation of connected servers, nor expose private key information, but could result in an insecure TLS 1.3 session when not controlling both sides of the connection. We recommend that TLS 1.3 client side users update the version of wolfSSL used. Thanks to Johannes from Sectra Communications and Linköping University for the report. Fixed in the following GitHub pull request https://github.com/wolfSSL/wolfssl/pull/6412

## New Feature Additions

### New Ports and Expansions
* Add support for STM32H5
* Add support for Renesas TSIP v1.17
* Add Renesas SCE RSA crypto-only support
* STARCORE DSP port and example builds added
* Add the function wc_PKCS7_SetDefaultSignedAttribs for setting PKCS7 signed attributes to use with PKCS7 bundle creation
* NXP IMX6Q CAAM port with QNX and performance optimizations for AES-CTR

### New Build Options
* ASN.1 print utility to decode ASN.1 syntax and print out human readable text --enable-asn-print. Utility app is located in the directory ./examples/asn1/
* Add introspection for math build, wc_GetMathInfo() to get information about the math library compiled into the linked wolfSSL library
* Implement TLS recommendations from RFC 9325 for hardening TLS/DTLS security. Enabled with the autoconf flag --enable-harden-tls.
* Add option to support disabling thread local storage, --disable-threadlocal
* Added wc_DsaSign_ex() and wc_DsaVerify_ex() for handling alternative digest algorithms with DSA Sign/Verify
* Implement atomic operations interface. Macros auto-detect if atomic operations are expected to be available, can be turned off with the macro WOLFSSL_NO_ATOMICS
* Added support for DTLS 1.3 Authentication and Integrity-Only Cipher Suites
* Expand crypto callback to have a device ID find callback function with wc_CryptoCb_SetDeviceFindCb. Enabled with the macro WOLF_CRYPTO_CB_FIND

## Enhancements and Optimizations

### Optimizations
* Increased performance with ChaCha20 C implementation and general XOR operations
* Added integer type to the ASN.1 sequencing with ASN.1 Integer sequence
* With wolfSSL_get_x509_next_altname reset alt name list to head once cycled through if compiling with the macro WOLFSSL_MULTICIRCULATE_ALTNAMELIST
* Additional key validity sanity checks on input to wolfSSL_EC_KEY_set_private_key
* adds support for TLSv1.3 stateful session tickets when using SSL_OP_NO_TICKET

### Memory Optimizations
* Improvements to stack usage and management with SP int math library
* Optimization to TLS 1.3 server to remove caching messages for Ed25519/Ed448
* Added a HAVE_CURL macro build for building a subset of the wolfSSL library when linking with cURL
* Memory usage improvement with reducing the size of alignment needed with AES
* Reduce run time memory used with ECC operations and ALT_ECC_SIZE
* Fixes and improvements for building edge cases such as crypto callback without hash-drbg with low footprint options
* Support HAVE_SESSION_TICKET build option without depending on realloc

### Documentation
* Instructions for GPDMA on STM32 configuration added
* Add in instructions for compiling with zephyr on STM32
* Documentation fixup for wolfSSL_get_chain_cert()
* Fix the file pointed to in the TI RTOS documentation that we maintain
* Documentation for wolfSSL_CertManagerFreeCRL
* Updates made to AES and Chacha documentation
* Update Japanese comments for Ed25519, AES, and other miscellaneous items

### Tests
* Add in an option for easily testing malloc failures when building with WOLFSSL_MEM_FAIL_COUNT macro
* Updated in process for using Expect vs Assert to facilitate more malloc failure tests
* Enhance wolfCrypt test for builds that do not have ECC SECP curves enabled
* ESP32 platform-specific VisualGDB test & benchmark projects
* Update to dependencies in docker container file used for tests
* Fix up for base 10 output with bundled benchmark application

### Port Updates
* Zephyr port update, compile time warning fixes, misc. fixes when used with TLS and update of includes
* Update RIOT-OS to not compile out use of writev by default
* Update Micrium port to enable use of STM32_RNG
* Micrium updates for XMEMOVE and XSTRTOK use
* Various Espressif HW crypto, SHA2, AES, MP updates
* Added in ASIO build option with CMake builds

### General Enhancements
* Global codebase cleanup for C89 compliance and wolfCrypt -Wconversion hygiene
* PKCS#11 enhancement adding a callback for RSA key size when using a hardware key, by default 2048 bit key is used
* Allow for unknown OIDs in extensions in wolfSSL_X509_set_ext()
* Allow user to override XSTAT by defining the macro XSTAT when compiling
* Support UPN and SID with x509 certificate extensions and custom OID build
* Write next IV in wolfSSL_DES_ede3_cbc_encrypt for better handling of inline encryption
* Adding NO_ASN_TIME_CHECK build option for compiling out certificate before/after checks
* Improve different peer recvfrom handling and error reporting with ipv4 vs ipv6

## Fixes
* Fix for STM32 ECC sign and verify out of bounds buffer write when the hash length passed in is larger than the key size. Thanks to Maximilian for the report.
* Fix to skip Async_DevCtxInit when using init rsa/ecc label/id api's
* Revert WOLFSSL_NO_ASN_STRICT macro guard around alternate names directory list
* In async mode, don't retry decrypting if a valid error is encountered on a packet parse attempt
* Add additional sanity check on PKCS7 index value in wc_PKCS7_DecryptKekri
* Fix for padding when using an AuthEnvelope PKCS7 type with GCM/CCM stream ciphers
* Fix siphash assembly so that no register is left behind
* Fix to not send a TLS 1.3 session ID resume response when resuming and downgrading to a protocol less than TLS 1.3
* Fix overwriting serialNumber by favouriteDrink when generating a certificate using Cert struct
* Fix for the default realloc used with EspressIf builds
* Track SetDigest usage to avoid invalid free under error conditions
* DTLS v1.3 fix for epoch 0 check on plaintext message
* Fix for session ticket memory leak in wolfSSL_Cleanup
* Fixes for propagating SendAlert errors when the peer disconnects
* Replace XMEMCPY with XMEMMOVE to fix valgrind-3.15.0 reports "Source and destination overlap in memcpy" when using --enable-aesgcm-stream
* Fix for potential out-of-bounds write edge case in fp_mod_2d with --enable-fastmath math library
* Fix getting ECC key size in stm32_ecc_sign_hash_ex
* Fix for case where wc_PeekErrorNodeLineData was not unlocking error queue on error
* Fix for async ECC shared secret state
* Fix for better error checking with sp_gcd with SP int math library
* Fix memory leak in TLSX_KeyShare_Setup when handling an error case
* Fix for double free edge case in InitOCSPRequest when handling a memory allocation failure
* X509 NAME Entry fix for leaking memory on error case
* Fix wolfssl_asn1_time_to_tm setting unexpected fields in tm struct
* Fix for FIPS ECC integrity check with crypto callback set
* BN_to_ASN1_INTEGER fix for handling leading zero byte padding when needed
* Fix a typo in PP macro and add a ceiling to guard against implementation bugs
* DTLS 1.3 fix for using the correct label when deriving the resumption key
* OCSP fix for GetDateInfo edge case with non ASN template builds
* Allow a user set certificate callback function to override the skipAddCA flag when parsing a certificate
* SP int: sp_radix_size when radix 10 fix temp size for handling edge case
* Fixes and improvements for handling failures with memory allocations
* Fix for DecodeECC_DSA_Sig to handle r and s being initialized
* Fix for wc_ecc_is_point to ensure that the x and y are in range [0, p-1] and z is one (affine ordinates)

### Build Fixes
* Fix for building on Windows with CMake and using USER_SETTINGS and fix for options.h creation with CMake when using USER_SETTINGS
* CMake fixes and improvements for use with mingw32
* Fix for building with wpas and x509 small options
* Check if colrm is available for options.h creation when using autoconf
* Clean up NO_BIG_INT build, removing WOLFSSL_SP_MATH macro and heapmath compile
* Fix PKCS#7 build with NO_PKCS7_STREAM
* Fix compilation error in CC-RX and remove unnecessary public key import
* SP Build fixes for ARM assembly with ARMv6 clz and ARM thumb debug build
* For to not advertise support for RSA in TLS extensions when compiled with NO_RSA

For additional vulnerability information visit the vulnerability page at:
https://www.wolfssl.com/docs/security-vulnerabilities/

See INSTALL file for build instructions.
More info can be found on-line at: https://wolfssl.com/wolfSSL/Docs.html


# Resources

[wolfSSL Website](https://www.wolfssl.com/)

[wolfSSL Wiki](https://github.com/wolfSSL/wolfssl/wiki)

[FIPS 140-2/140-3 FAQ](https://wolfssl.com/license/fips)

[wolfSSL Documentation](https://wolfssl.com/wolfSSL/Docs.html)

[wolfSSL Manual](https://wolfssl.com/wolfSSL/Docs-wolfssl-manual-toc.html)

[wolfSSL API Reference](https://wolfssl.com/wolfSSL/Docs-wolfssl-manual-17-wolfssl-api-reference.html)

[wolfCrypt API Reference](https://wolfssl.com/wolfSSL/Docs-wolfssl-manual-18-wolfcrypt-api-reference.html)

[TLS 1.3](https://www.wolfssl.com/docs/tls13/)

[wolfSSL Vulnerabilities](https://www.wolfssl.com/docs/security-vulnerabilities/)

[Additional wolfSSL Examples](https://github.com/wolfssl/wolfssl-examples)
