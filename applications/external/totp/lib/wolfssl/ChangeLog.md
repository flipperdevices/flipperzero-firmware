# wolfSSL Release 5.6.3 (Jun 16, 2023)

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

# wolfSSL Release 5.6.0 (Mar 24, 2023)

Release 5.6.0 has been developed according to wolfSSL's development and QA process (see link below) and successfully passed the quality criteria.
https://www.wolfssl.com/about/wolfssl-software-development-process-quality-assurance

NOTE: * --enable-heapmath is being deprecated and will be removed by 2024
      * This release makes ASN Template the default with ./configure, the previous ASN parsing can be built with --enable-asn=original

Release 5.6.0 of wolfSSL embedded TLS has bug fixes and new features including:

## New Feature Additions

* ASN template is now the default ASN parsing implementation when compiling with configure
* Added in support for TLS v1.3 Encrypted Client Hello (ECH) and HPKE (Hybrid Public Key Encryption)
* DTLS 1.3 stateless server ClientHello parsing support added

### Ports
* Add RX64/RX71 SHA hardware support
* Port to RT1170 and expand NXP CAAM driver support
* Add NuttX integration files for ease of use
* Updated Stunnel support for version 5.67
Compatibility Layer
* Add in support for AES-CCM with EVP
* BN compatibility API refactoring and separate API created
* Expanding public key type cipher suite list strings support

### Misc.
* Support pthread_rwlock and add enable option
* Add wolfSSL_CertManagerLoadCABuffer_ex() that takes a user certificate chain flag and additional verify flag options
* Docker build additions for wolfSSL library and wolfCLU application
* Add favorite drink pilot attribute type to get it from the encoding
* Added in support for indefinite length BER parsing with PKCS12
* Add dynamic session cache which allocates sessions from the heap with macro SESSION_CACHE_DYNAMIC_MEM


## Improvements / Optimizations

### Tests
* Additional CI (continuous integration) testing and leveraging of GitHub workflows
* Add CI testing for wpa_supplicant, OpenWrt and OpenVPN using GitHub workflows
* Add compilation of Espressif to GitHub workflows tests
* Refactoring and improving error results with wolfCrypt unit test application
* Minor warning fixes from Coverity static analysis scan
* Add new SHA-512/224 and SHA-512/256 tests
* Used codespell and fixed some minor typos

### Ports
* Improve TLS1.2 client authentication to use TSIP
* Updated Kyber macro to be WOLFSSL_HAVE_KYBER and made changes that make Kyber work on STM32
* AES-GCM Windows assembly additions
* CRLF line endings, trailing spaces for C# Wrapper Projects
Compatibility Layer
* Update `PubKey` and `Key` PEM-to-DER APIs to support return of needed DER size
* Allow reading ENC EC PRIVATE KEY as well via wolfSSL_PEM_read_bio_ECPrivateKey
* Improve wolfSSL_EC_POINT_cmp to handle Jacobian ordinates
* Fix issue with BIO_reset() and add BIO_FLAGS_MEM_RDONLY flag support for read only BIOs

### SP
* In SP math library rework mod 3 and use count leading zero instruction
* Fix with SP ECC sign to reject the random k generated when r is 0
* With SP math add better detection of when add won't work and double is needed with point_add_qz1 internal function
* With SP int fail when buffer writing to is too small for number rather than discarding the extra values

### Builds
* Define WOLFSSL_SP_SMALL_STACK if wolfSSL is build with --enable-smallstack
* Fix CMake to exclude libm when DH is not enabled
* Allow building of SAKKE as external non-FIPS algorithm with wolfmikey product
* Add option to add library suffix, --with-libsuffix
* ASN template compile option WOLFSSL_ASN_INT_LEAD_0_ANY to allow leading zeros
* Add user_settings.h template for wolfTPM to examples/configs/user_settings_wolftpm.h
* Purge the AES variant of Dilithium
* Expand WOLFSSL_NO_ASN_STRICT to allow parsing of explicit ECC public key
* Remove relocatable text in ARMv7a AES assembly for use with FIPS builds
* Expand checking for hardware that supports ARMv7a neon with autotools configure
* Sanity check on allocation fails with DSA and FP_ECC build when zeroizing internal buffer
* Additional TLS alerts sent when compiling with WOLFSSL_EXTRA_ALERTS macro defined

### Benchmarking
* Update wolfCrypt benchmark Windows build files to support x64 Platform
* Add SHA512/224 and SHA512/256 benchmarks, fixed CVS macro and display sizes
* Separate AES-GCM streaming runs when benchmarked
* No longer call external implementation of Kyber from benchmark
* Fix for benchmarking shake with custom block size
* Fixes for benchmark help `-alg` list and block format
Documentation/Examples
* Document use of wc_AesFree() and update documentation of Ed25519 with Doxygen
* Move the wolfSSL Configuration section higher in QUIC.md
* Add Japanese Doxygen documentation for cmac.h, quic.h and remove incomplete Japanese doxygen in asn_public.h
* Espressif examples run with local wolfSSL now with no additional setup needed
* Added a fix for StartTLS use In the example client
* Add a base-line user_settings.h for use with FIPS 140-3 in XCode example app

### Optimizations
* AES-NI usage added for AES modes ECB/CTR/XTS

### Misc
* Update AES-GCM stream decryption to allow long IVs
* Internal refactor to use wolfSSL_Ref functions when incrementing or decrementing the structures reference count and fixes for static analysis reports
* Cleanup function logging making adjustments to the debug log print outs
* Remove realloc dependency in DtlsMsgCombineFragBuckets function
* Refactor to use WOLFSSL_CTX’s cipher suite list when possible
* Update internal padding of 0’s with DSA sign and additional tests with mp_to_unsigned_bin_len function
* With DTLS SRTP use wolfSSL_export_keying_material instead of wc_PRF_TLS
* Updated macro naming from HAVE_KYBER to be WOLFSSL_HAVE_KYBER
* Update AES XTS encrypt to handle in-place encryption properly
* With TLS 1.3 add option to require only PSK with DHE

## Fixes

### Ports
* Fix for AES use with CAAM on imx8qxp with SECO builds
* Fix for PIC32 crypto HW and unused `TLSX_SetResponse`
* Fix warning if ltime is unsigned seen with QNX build
* Updates and fix for Zephyr project support
* Include sys/time.h for WOLFSSL_RIOT_OS
* Move X509_V errors from enums to defines for use with HAProxy CLI
* Fix IAR compiler warnings resolved
* Fix for STM32 Hash peripherals (like on F437) with FIFO depth = 1
* ESP32 fix for SHA384 init with hardware acceleration

### Builds
* Add WOLFSSL_IP_ALT_NAME macro define to --enable-curl
* Fixes for building with C++17 and avoiding clashing with byte naming
* Fixes SP math all build issue with small-stack and no hardening
* Fix for building with ASN template with `NO_ASN_TIME` defined
* Fix building FIPSv2 with WOLFSSL_ECDSA_SET_K defined
* Don't allow aesgcm-stream option with kcapi
* Fix DTLS test case for when able to read peers close notify alert on FreeBSD systems
* Fix for "expression must have a constant value" in tls13.c with Green Hills compiler
* Fixes for building KCAPI with opensslextra enabled
* Fix warnings of shadows min and subscript with i486-netbsd-gcc compiler
* Fix issue with async and `WOLFSSL_CHECK_ALERT_ON_ERR`
* Fix for PKCS7 with asynchronous crypto enabled

### Math Library
* SP Aarch64 fix for conditional changed in asm needing "cc" and fix for ECC P256 mont reduce
* In SP builds add sanity check with DH exp. to check the output length for minimum size
* In SP math fix scalar length check with EC scalar multiply
* With SP int fix handling negative character properly with read radix
* Add error checks before setting variable err in SP int with the function sp_invmod_mont_ct
* Fix to add sanity check for malloc of zero size in fastmath builds
* In fastmath  fix a possible overflow in fp_to_unsigned_bin_len length check
* Heapmath fast mod. reduce fix

### Compatibility Layer
* Fixes for encoding/decoding ecc public keys and ensure i2d public key functions do not include any private key information
* Fix for EVP_EncryptUpdate to update outl on empty input
* Fix SE050 RSA public key loading and RSA/ECC SE050 TLS Compatibility
* Rework EC API and validate point after setting it
* Fix for X509 RSA PSS with compatibility layer functions
* Fix size of structures used with SHA operations when built with opensslextra for Espressif hardware accelerated hashing
* Added sanity check on key length with wolfSSL_CMAC_Init function
* Fix for return value type conversion of bad mutex error in logging function
* Fix NID conflict NID_givenName and NID_md5WithRSAEncryption
* Fix unguarded XFPRINTF calls with opensslextra build
* Fix wolfSSL_ASN1_INTEGER_to_BN for negative values
* Fix for potential ASN1_STRING leak in wolfSSL_X509_NAME_ENTRY_create_by_txt  and wolfSSL_X509_NAME_ENTRY_create_by_NID when memory allocation fails

### Misc.
* Add sanity check to prevent an out of bounds read with OCSP response decoding
* Sanity check to not allow 0 length with bit string and integer when parsing ASN1 syntax
* Adjust RNG sanity checks and remove error prone first byte comparison
* With PKCS7 add a fix for GetAsnTimeString() to correctly increment internal data pointer
* PKCS7 addition of sequence around algo parameters with authenvelop
* DSA fixes for clearing mp_int before re-reading data and avoid mp_clear without first calling mp_init
* Fix for SRTP setting bitfield when it is encoded for the TLS extension
* Fix for handling small http headers when doing CRL verification
* Fix for ECCSI hash function to validate the output size and curve size
* Fix for value of givenName and name being reversed with CSR generation
* Fix for error type returned (OCSP_CERT_UNKNOWN) with OCSP verification
* Fix for a potential memory leak with ProcessCSR when handling OCSP responses
* Fix for VERIFY_SKIP_DATE flag not ignoring date errors when set
* Fix for zlib decompression buffer issue with PKCS7
* Fix for DTLS message pool send size used and DTLS server saving of the handshake sequence
* Fix to propagate WOLFSSL_TICKET_RET_CREATE error return value from DoDecryptTicket()
* Fix for handling long session IDs with TLS 1.3 session tickets
* Fix for AES-GCM streaming when caching an IV
* Fix for test case with older selftest that returns bad padding instead of salt len error
* Add fix for siphash cache and added in additional tests
* Fix potential out of bounds memset to 0 in error case with session export function used with --enable-sessionexport builds
* Fix possible NULL dereference in TLSX_CSR_Parse with TLS 1.3
* Fix for sanity check on RSA pad length with no padding using the build macro WC_RSA_NO_PADDING

# wolfSSL Release 5.5.4 (Dec 21, 2022)

Release 5.5.4 of wolfSSL embedded TLS has bug fixes and new features including:

## New Feature Additions

* QUIC related changes for HAProxy integration and config option
* Support for Analog Devices MAXQ1080 and MAXQ1065
* Testing and build of wolfSSL with NuttX
* New software based entropy gatherer with configure option --enable-entropy-memuse
* NXP SE050 feature expansion and fixes, adding in RSA support and conditional compile of AES and CMAC
* Support for multi-threaded sniffer

## Improvements / Optimizations

### Benchmark and Tests
* Add alternate test case for unsupported static memory API when testing mutex allocations
* Additional unit test cases added for AES CCM 256-bit
* Initialize and free AES object with benchmarking AES-OFB
* Kyber with DTLS 1.3 tests added
* Tidy up Espressif ESP32 test and benchmark examples
* Rework to be able to run API tests individually and add display of time taken per test

### Build and Port Improvements
* Add check for 64-bit ABI on MIPS64 before declaring a 64-bit CPU
* Add support to detect SIZEOF_LONG in armclang and diab
* Added in a simple example working on Rx72n
* Update azsphere support to prevent compilation of file included inline
* --enable-brainpool configure option added and default to on when custom curves are also on
* Add RSA PSS salt defines to engine builds if not FIPS v2

### Post Quantum
* Remove kyber-90s and route all Kyber through wolfcrypt
* Purge older version of NTRU and SABER from wolfSSL

### SP Math
* Support static memory build with sp-math
* SP C, SP int: improve performance
* SP int: support mingw64 again
* SP int: enhancements to guess 64-bit type and check on NO_64BIT macro set before using long long
* SP int: check size required when using sp_int on stack
* SP: --enable-sp-asm now enables SP by default if not set
* SP: support aarch64 big endian

### DTLS
* Allow DTLS 1.3 to compile when FIPS is enabled
* Allow for stateless DTLS client hello parsing

### Misc.
* Easier detection of DRBG health when using Intel’s RDRAND by updating the structures status value
* Detection of duplicate known extensions with TLS
* PKCS#11 handle a user PIN that is a NULL_PTR, compile time check in finding keys, add initialization API
* Update max Cert Policy size based on RFC 5280
* Add Android CA certs path for wolfSSL_CTX_load_system_CA_certs()
* Improve logic for enabling system CA certs on Apple devices
* Stub functions to allow for cpuid public functions with non-intel builds
* Increase RNG_SECURITY_STRENGTH for FIPS
* Improvements in OpenSSL Compat ERR Queue handling
* Support ASN1/DER CRLs in LoadCertByIssuer
* Expose more ECC math functions and improve async shared secret
* Improvement for sniffer error messages
* Warning added that renegotiation in TLS 1.3 requires session ticket
* Adjustment for TLS 1.3 post auth support
* Rework DH API and improve PEM read/write

## Fixes

### Build Fixes
* Fix --enable-devcrypto build error for sys without u_int8_t type
* Fix casts in evp.c and build issue in ParseCRL
* Fixes for compatibility layer building with heap hint and OSSL callbacks
* fix compile error due to Werro=undef on gcc-4.8
* Fix mingw-w64 build issues on windows
* Xcode project fixes for different build settings
* Initialize variable causing failures with gcc-11 and gcc-12 with a unique wolfSSL build configuration
* Prevent WOLFSSL_NO_MALLOC from breaking RSA certificate verification
* Fixes for various tests that do not properly handle `WC_PENDING_E` with async. builds
* Fix for misc `HashObject` to be excluded for `WOLFCRYPT_ONLY`

### OCSP Fixes
* Correctly save next status with OCSP response verify
* When the OCSP responder returns an unknown exception, continue through to checking the CRL

### Math Fixes
* Fix for implicit conversion with 32-bit in SP math
* Fix for error checks when modulus is even with SP int build
* Fix for checking of err in _sp_exptmod_nct with SP int build
* ECC cofactor fix when checking scalar bits
* ARM32 ASM: don't use ldrd on user data
* SP int, fix when ECC specific size code included

### Port Fixes
* Fixes for STM32 PKA ECC (not 256-bit) and improvements for AES-GCM
* Fix for cryptocell signature verification with ECC
* Benchmark devid changes, CCM with SECO fix, set IV on AES import into SECO

### Compat. Layer Fixes
* Fix for handling DEFAULT:... cipher suite list
* Fix memory leak in wolfSSL_X509_NAME_ENTRY_get_object
* Set alt name type to V_ASN1_IA5STRING
* Update name hash functions wolfSSL_X509_subject_name_hash and wolfSSL_X509_issuer_name_hash to hash the canonical form of subject
* Fix wolfSSL_set_SSL_CTX() to be usable during handshake
* Fix X509_get1_ocsp to set num of elements in stack
* X509v3 EXT d2i: fix freeing of aia
* Fix to remove recreation of certificate with wolfSSL_PEM_write_bio_X509()
* Link newly created x509 store's certificate manager to self by default to assist with CRL verification
* Fix for compatibility `EC_KEY_new_by_curve_name` to not create a key if the curve is not found

### Misc.
* Free potential signer malloc in a fail case
* fix other name san parsing and add RID cert to test parsing
* WOLFSSL_OP_NO_TICKET fix for TLSv1.2
* fix ASN template parsing of X509 subject directory attribute
* Fix the wrong IV size with the cipher suite TLS_ECDHE_PSK_WITH_AES_128_GCM_SHA256
* Fix incorrect self signed error return when compiled with certreq and certgen.
* Fix wrong function name in debug comment with wolfSSL_X509_get_name_oneline()
* Fix for decryption after second handshake with async sniffer
* Allow session tickets to properly resume when using PQ KEMs
* Add sanity overflow check to DecodeAltNames input buffer access

# wolfSSL Release 5.5.3 (Nov 2, 2022)

Release 5.5.3 of wolfSSL embedded TLS has the following bug fix:

## Fixes

* Fix for possible buffer zeroization overrun introduced at the end of v5.5.2 release cycle in GitHub pull request 5743 (https://github.com/wolfSSL/wolfssl/pull/5743) and fixed in pull request 5757 (https://github.com/wolfSSL/wolfssl/pull/5757). In the case where a specific memory allocation failed or a hardware fault happened there was the potential for an overrun of 0’s when masking the buffer used for (D)TLS 1.2 and lower operations. (D)TLS 1.3 only and crypto only users are not affected by the issue. This is not related in any way to recent issues reported in OpenSSL.


# wolfSSL Release 5.5.2 (Oct 28, 2022)
Release 5.5.2 of wolfSSL embedded TLS has bug fixes and new features including:

## Vulnerabilities
* [Med] In the case that the WOLFSSL_CALLBACKS macro is set when building wolfSSL, there is a potential heap over read of 5 bytes when handling TLS 1.3 client connections. This heap over read is limited to wolfSSL builds explicitly setting the macro WOLFSSL_CALLBACKS, the feature does not get turned on by any other build options. The macro WOLFSSL_CALLBACKS is intended for debug use only, but if having it enabled in production, users are recommended to disable WOLFSSL_CALLBACKS. Users enabling WOLFSSL_CALLBACKS are recommended to update their version of wolfSSL. Thanks to Lucca Hirschi and Steve Kremer from LORIA, Inria and Max Ammann from Trail of Bits for finding and reporting the bug with the tlspuffin tool developed partly at LORIA and Trail of Bits. CVE 2022-42905

Release 5.5.2 of wolfSSL embedded TLS has bug fixes and new features including:

## New Feature Additions
* Add function wolfSSL_CTX_load_system_CA_certs to load system CA certs into a WOLFSSL_CTX and  --sys-ca-certs option to example client
* Add wolfSSL_set1_host to OpenSSL compatible API
* Added the function sk_X509_shift
* AES x86 ASM for AES-CBC and GCM performance enhancements
* Add assembly for AES for ARM32 without using crypto hardware instructions
* Xilinx Versal port and hardware acceleration tie in
* SP Cortex-M support for ICCARM

## Enhancements
* Add snifftest vcxproj file and documentation
* Nucleus Thread Types supported
* Handle certificates with RSA-PSS signature that have RSAk public keys
* Small stack build improvements
* DTLS 1.3 improvements for Alerts and unit tests
* Add a binary search for CRL
* Improvement of SSL/CTX_set_max_early_data() for client side
* Remove unused ASN1_GENERALIZEDTIME enum value from wolfssl/ssl.h
* Add user_settings.h for Intel/M1 FIPSv2 macOS C++ projects
* Add dtlscid.test to ‘make check’ unit testing
* Generate an assembler-safe user_settings.h in configure.ac and CMakeLists.txt
* ForceZero enabled with USE_FAST_MATH
* Add TLS 1.3 support of ticketNonce sizes bigger than MAX_TICKET_NONCE_SZ
* FIPSv2 builds on win10 adjust for new fastmath default in settings.h
* Add IRQ install for Aruix example

## Fixes
* When looking up the session by ID on the server, check that the protocol version of the SSL and session match on TLS 1.3 or not
* Fix for potential EVP_PKEY_DH memory leak with OPENSSL_EXTRA
* Curve448 32-bit C code: handle corner case
* Fixup builds using WOLFSSL_LOG_PRINTF
* Correct DIST_POINT_NAME type value
* Do not perform IV Wrap test when using cert3389 inlined armasm
* Fix for Linux kernel module and stdio.h
* (D)TLS: send alert on version mismatch
* Fix PKCS#7 SignedData verification when signer cert is not first in SET
* Fix bug with wolfIO_TcpConnect not working with timeout on Windows
* Fix output length bug in SP non-blocking ECC shared secret gen
* Fix build with enable-fastmath and disable-rsa
* Correct wolfSSL_sk_X509_new in OpenSSL compatible API
* Fixes for SP and x86_64 with MSVC
* Fix wrong size using DTLSv1.3 in RestartHandshakeHashWithCookie
* Fix redundant file include with TI RTOS build
* Fix wolfCrypt only build with wincrypt.h
* DTLS 1.2: Reset state when sending HelloVerifyRequest

# wolfSSL Release 5.5.1 (Sep 28, 2022)
Release 5.5.1 of wolfSSL embedded TLS has bug fixes and new features including:

## Vulnerabilities
* [Med] Denial of service attack and buffer overflow against TLS 1.3 servers using session ticket resumption. When built with --enable-session-ticket and making use of TLS 1.3 server code in wolfSSL, there is the possibility of a malicious client to craft a malformed second ClientHello packet that causes the server to crash. This issue is limited to when using both --enable-session-ticket and TLS 1.3 on the server side. Users with TLS 1.3 servers, and having --enable-session-ticket, should update to the latest version of wolfSSL. Thanks to Max at Trail of Bits for the report, found by Lucca Hirschi from LORIA, Inria, France with the tlspuffin tool developed partly at LORIA and Trail of Bits. CVE-2022-39173

## New Feature Additions
* Add support for non-blocking ECC key gen and shared secret gen for P-256/384/521
* Add support for non-blocking ECDHE/ECDSA in TLS/DTLS layer.
* Port to NXP RT685 with FreeRTOS
* Add option to build post quantum Kyber API (--enable-kyber)
* Add post quantum algorithm sphincs to wolfCrypt
* Config. option to force no asm with SP build (--enable-sp=noasm)
* Allow post quantum keyshare for DTLS 1.3

## Enhancements
* DTLSv1.3: Do HRR Cookie exchange by default
* Add wolfSSL_EVP_PKEY_new_CMAC_key to OpenSSL compatible API 
* Update ide win10 build files to add missing sp source files 
* Improve Workbench docs 
* Improve EVP support for CHACHA20_POLY1305
* Improve `wc_SetCustomExtension` documentation
* RSA-PSS with OCSP and add simple OCSP response DER verify test case
* Clean up some FIPS versioning logic in configure.ac and WIN10 user_settings.h
* Don't over-allocate memory for DTLS fragments
* Add WOLFSSL_ATECC_TFLXTLS for Atmel port
* SHA-3 performance improvements with x86_64 assembly
* Add code to fallback to S/W if TSIP cannot handle 
* Improves entropy with VxWorks
* Make time in milliseconds 64-bits for longer session ticket lives
* Support for setting cipher list with bytes
* wolfSSL_set1_curves_list(), wolfSSL_CTX_set1_curves_list() improvements
* Add to RSAES-OAEP key parsing for pkcs7
* Add missing DN nid to work with PrintName()
* SP int: default to 16 bit word size when NO_64BIT defined 
* Limit the amount of fragments we store per a DTLS connection and error out when max limit is reached
* Detect when certificate's RSA public key size is too big and fail on loading of certificate

## Fixes
* Fix for async with OCSP non-blocking in `ProcessPeerCerts`
* Fixes for building with 32-bit and socket size sign/unsigned mismatch
* Fix Windows CMakeList compiler options 
* TLS 1.3 Middle-Box compat: fix missing brace 
* Configuration consistency fixes for RSA keys and way to force disable of private keys 
* Fix for Aarch64 Mac M1 SP use
* Fix build errors and warnings for MSVC with DTLS 1.3
* Fix HMAC compat layer function for SHA-1
* Fix DTLS 1.3 do not negotiate ConnectionID in HelloRetryRequest
* Check return from call to wc_Time
* SP math: fix build configuration with opensslall
* Fix for async session tickets
* SP int mp_init_size fixes when SP_WORD_SIZE == 8 
* Ed. function to make public key now checks for if the private key flag is set
* Fix HashRaw WC_SHA256_DIGEST_SIZE for wc_Sha256GetHash 
* Fix for building with PSK only
* Set correct types in wolfSSL_sk_*_new functions
* Sanity check that size passed to mp_init_size() is no more than SP_INT_DIGITS


# wolfSSL Release 5.5.0 (Aug 30, 2022)

Note:
** If not free’ing FP_ECC caches per thread by calling wc_ecc_fp_free there is a possible memory leak during TLS 1.3 handshakes which use ECC. Users are urged to confirm they are free’ing FP_ECC caches per thread if enabled to avoid this issue.

Release 5.5.0 of wolfSSL embedded TLS has bug fixes and new features including:

## Vulnerabilities
* [Low] Fault injection attack on RAM via Rowhammer leads to ECDSA key disclosure. Users doing operations with private ECC keys such as server side TLS connections and creating ECC signatures, who also have hardware that could be targeted with a sophisticated Rowhammer attack should update the version of wolfSSL and compile using the macro WOLFSSL_CHECK_SIG_FAULTS. Thanks to Yarkin Doroz, Berk Sunar, Koksal Must, Caner Tol, and Kristi Rahman all affiliated with the Vernam Applied Cryptography and Cybersecurity Lab at Worcester Polytechnic Institute for the report.
* [Low] In wolfSSL version 5.3.0 if compiled with --enable-session-ticket and the client has non-empty session cache, with TLS 1.2 there is the possibility of a man in the middle passing a large session ticket to the client and causing a crash due to an invalid free. There is also the potential for a malicious TLS 1.3 server to crash a client in a similar manner except in TLS 1.3 it is not susceptible to a man in the middle attack. Users on the client side with –enable-session-ticket compiled in and using wolfSSL version 5.3.0 should update their version of wolfSSL. Thanks to Max at Trail of Bits for the report and "LORIA, INRIA, France" for research on tlspuffin.
* [Low] If using wolfSSL_clear to reset a WOLFSSL object (vs the normal wolfSSL_free/wolfSSL_new) it can result in runtime issues. This exists with builds using the wolfSSL compatibility layer (--enable-opnesslextra) and only when the application is making use of wolfSSL_clear instead of SSL_free/SSL_new. In the case of a TLS 1.3 resumption, after continuing to use the WOLFSSH object after having called wolfSSL_clear, an application could crash. It is suggested that users calling wolfSSL_clear update the version of wolfSSL used. Thanks to Max at Trail of Bits for the report and "LORIA, INRIA, France" for research on tlspuffin.
* Potential DoS attack on DTLS 1.2. In the case of receiving a malicious plaintext handshake message at epoch 0 the connection will enter an error state reporting a duplicate message. This affects both server and client side. Users that have DTLS enabled and in use should update their version of wolfSSL to mitigate the potential for a DoS attack.

## New Feature Additions
* QUIC support added, for using wolfSSL with QUIC implementations like ngtcp2
* SE050 port additions and fixes
* Added support for Dilithium post quantum algorithm use with TLS
* Support for RSA-PSS signed certificates
* Support for Infineon AURIX IDE
* Add Zephyr support for nRF5340 with CryptoCell-312

## Enhancements
* Expanded ABI support by 50 APIs to include wolfCrypt and Certificates making a total of 113 ABIs controlled and maintained
* DTLS 1.3 partial support for ConnectionID as described by RFC9146 and RFC9147
* Added support for X509_CRL_print function
* Remove deprecated algorithms in Renesas cs+ project
* Support more build options disable/enable with i.MX CAAM build
* wolfSSL_CTX_set_options and wolfSSL_CTX_get_options functions added to non compatibility layer builds
* TFM: change inline x86 asm code to compile with clang
* Improvements to error queue and fix for behavior of wolfSSL_ERR_get_error
* scripts/makedistsmall.sh script added for creating a small source/header only package
* TLS 1.3: restrict extension validity by message, Extensions ServerName, SupportedGroups and ALPN must not appear in server_hello
* Add liboqs integration to CMake build system
* Adds wolfSSL_PEM_read_RSAPrivateKey() to the OpenSSL compatible API
* Added support for P384 pre-share in bundled example server
* Replace clz assembly instruction in ARM 32 builds when not supported
* Integrate chacha20-poly1305 into the EVP interface
* Additional validation that extensions appear in correct messages
* Allow SAN to be critical with ASN template build
* Support wolfSSL_CTX_set1_curves_list being available when X25519 and/or X448 only defined
* Adds wolfSSL_PEM_read_RSA_PUBKEY() to the OpenSSL compatible API
* Match OpenSSL self signed error return with compatibility layer build
* Added wolfSSL_dtls_create_peer and wolfSSL_dtls_free_peer to help with Python and Go wrappers for DTLS

## Fixes
* DTLS 1.3 asynchronous use case fixes
* Fix handling of counter to support incrementing across all bytes in ARM crypto asm
* Fixes for ED25519/ED448 private key with public key export (RFC8410)
* Fix for build with NO_TLS macro
* Fix for write dup function to copy over TLS version
* Fix to handle path lengths of 0 when checking certificate CA path lengths
* Fix for CMake not installing sp_int.h for SP math all
* When WOLFSSL_VALIDATE_ECC_IMPORT is defined ECC import validates private key value is less than order
* PSA crypto fixes
* Fix for not having default pkcs7 signed attributes
* DTLS socket and timeout fixes
* SP int: exptmod ensure base is less than modulus
* Fix for AddPacketInfo with WOLFSSL_CALLBACKS to not pass encrypted TLS 1.3 handshake messages to callbacks
* Fix for sniffer to ensure the session was polled before trying to reprocess it

# wolfSSL Release 5.4.0 (July 11, 2022)

Note:
** Future releases of wolfSSL will turn off TLS 1.1 by default
** Release 5.4.0 made SP math the default math implementation. To make an equivalent build as –disable-fastmath from previous versions of wolfSSL, now requires using the configure option –enable-heapmath instead.

Release 5.4.0 of wolfSSL embedded TLS has bug fixes and new features including:

## Vulnerabilities
* [High]  Potential for DTLS DoS attack. In wolfSSL versions before 5.4.0 the return-routability check is wrongly skipped in a specific edge case. The check on the return-routability is there for stopping attacks that either consume excessive resources on the server, or try to use the server as an amplifier sending an excessive amount of messages to a victim IP. If using DTLS 1.0/1.2 on the server side users should update to avoid the potential DoS attack. CVE-2022-34293
* [Medium] Ciphertext side channel attack on ECC and DH operations. Users on systems where rogue agents can monitor memory use should update the version of wolfSSL and change private ECC keys. Thanks to Sen Deng from Southern University of Science and Technology (SUSTech) for the report.
* [Medium] Public disclosure of a side channel vulnerability that has been fixed since wolfSSL version 5.1.0. When running on AMD there is the potential to leak private key information with ECDSA operations due to a ciphertext side channel attack. Users on AMD doing ECDSA operations with wolfSSL versions less than 5.1.0 should update their wolfSSL version used. Thanks to professor Yinqian Zhang from Southern University of Science and Technology (SUSTech), his Ph.D. student Mengyuan Li from The Ohio State University, and his M.S students Sen Deng and Yining Tang from SUStech along with other collaborators; Luca Wilke, Jan Wichelmann and Professor Thomas Eisenbarth from the University of Lubeck, Professor Shuai Wang from Hong Kong University of Science and Technology, Professor Radu Teodorescu from The Ohio State University, Huibo Wang, Kang Li and Yueqiang Cheng from Baidu Security and Shoumeng Yang from Ant Financial Services Group.
CVE-2020-12966 https://www.amd.com/en/corporate/product-security/bulletin/amd-sb-1013 CVE-2021-46744 https://www.amd.com/en/corporate/product-security/bulletin/amd-sb-1033


## New Feature Additions

### DTLS 1.3
* Support for using the new DTLSv1.3 protocol was added
* Enhancements to bundled examples for an event driven server with DTLS 1.3 was added
### Ports
* Update for the version of VxWorks supported, adding in support for version 6.x
* Support for new DPP and EAP-TEAP/EAP-FAST in wpa_supplicant
* Update for TSIP version support, adding support for version 1.15 for RX65N and RX72N
* Improved TSIP build to handle having the options WOLFSSL_AEAD_ONLY defined or NO_AES_CBC defined
* Added support for offloading TLS1.3 operations to Renesas RX boards with TSIP
### Misc.
* Constant time improvements due to development of new constant time tests
* Initial translation of API headers to Japanese and expansion of Japanese help message support in example applications
* Add support for some FPKI (Federal PKI) certificate cases, UUID, FASC-N, PIV extension for use with smart cards
* Add support for parsing additional CSR attributes such as unstructured name and content type
* Add support for Linux getrandom() when defining the macro WOLFSSL_GETRANDOM
* Add TLS 1.2 ciphersuite ECDHE_PSK_WITH_AES_128_GCM_SHA256 from RFC 8442
* Expand CAAM support with QNX to include i.MX8 boards and add AES-CTR support
* Enhanced glitching protection by hardening the TLS encrypt operations

## Math and Performance

### SP Math Additions
* Support for ARMv3, ARMv6 and ARMv7a
    - Changes and improvements to get SP building for armv7-a
    - Updated assembly for moving large immediate values on ARMv6
    - Support for architectures with no ldrd/strd and clz
* Reworked generation using common asm ruby code for 32bit ARM
* Enable wolfSSL SP math all by default (sp_int.c)
* Update SP math all to not use sp_int_word when SQR_MUL_ASM is available
### SP Math Fixes
* Fixes for constant time with div function
* Fix casting warnings for Windows builds and assembly changes to support XMM6-15 being non-volatile 
* Fix for div_word when not using div function
* Fixes for user settings with SP ASM and ED/Curve25519 small
* Additional Wycheproof tests ran and fixes
* Fix for SP math ECC non-blocking to always check `hashLen`
* Fix for SP math handling edge case with submod

## Improvements and Optimizations

### Compatibility Layer
* Provide access to "Finished" messages outside of compatibility layer builds
* Remove unneeded FIPS guard on wolfSSL_EVP_PKEY_derive
* Fix control command issues with AES-GCM, control command EVP_CTRL_GCM_IV_GEN
* Add support for importing private only EC key to a WOLFSSL_EVP_PKEY struct
* Add support for more extensions to wolfSSL_X509_print_ex
* Update for internal to DER (i2d) AIPs to move the buffer pointer when passed in and the operation is successful
* Return subject and issuer X509_NAME object even when not set
### Ports
* Renesas RA6M4 example update and fixes
* Support multi-threaded use cases with Renesas SCE protected mode and TSIP
* Add a global variable for heap-hint for use with TSIP
* Changes to support v5.3.0 cube pack for STM32
* Use the correct mutex type for embOS
* ESP-IDF build cleanup and enhancements, adding in note regarding ESP-IDF Version
* Support for SEGGER embOS and emNET
* Fix to handle WOLFSSL_DTLS macro in Micrium build
### Build Options
* Support for verify only and no-PSS builds updated
* Add the enable options wolfssh (mapped to the existing –enable-ssh)
* Remove WOLFSSL_ALT_NAMES restriction on notBefore/notAfter use in Cert struct
* Move several more definitions outside the BUILDING_WOLFSSL gate with linux kernel module build
* Modify --enable-openssh to not enable non-FIPS algos for FIPS builds
* Remove the Python wrappers from wolfSSL source (use pip install instead of using wolfSSL with Python and our separate Python repository)
* Add --enable-openldap option to configure.ac for building the OpenLDAP port
* Resolve DTLS build to handle not having –enable-hrrcookie when not needed
* Add an --enable-strongswan option to configure.ac for building the Strongswan port
* Improve defaults for 64-bit BSDs in configure
* Crypto only build can now be used openssl extra
* Update ASN template build to properly handle WOLFSSL_CERT_EXT and HAVE_OID_ENCODING
* Allow using 3DES and MD5 with FIPS 140-3, as they fall outside of the FIPS boundary
* Add the build option --enable-dh=const which replaces setting the macro WOLFSSL_DH_CONST and now conditionally link to -lm as needed
* Add the macro WOLFSSL_HOSTNAME_VERIFY_ALT_NAME_ONLY which is used to verify hostname/ip address using alternate name (SAN) only and does not use the common name
* WOLFSSL_DTLS_NO_HVR_ON_RESUME macro added (off by default to favor more security). If defined, a DTLS server will not do a cookie exchange on successful client resumption: the resumption will be faster (one RTT less) and will consume less bandwidth (one ClientHello and one HelloVerifyRequest less). On the other hand, if a valid SessionID is collected, forged clientHello messages will consume resources on the server.
* Misc.
* Refactoring of some internal TLS functions to reduce the memory usage
* Make old less secure TimingPadVerify implementation available
* Add support for aligned data with clang LLVM
* Remove subject/issuer email from the list of alt. Email names in the DecodedCerts struct
* Zeroizing of pre-master secret buffer in TLS 1.3
* Update to allow TLS 1.3 application server to send session ticket
* Improve the sniffer asynchronous test case to support multiple concurrent streams
* Clean up wolfSSL_clear() and add more logging
* Update to not error out on bad CRL next date if using NO_VERIFY when parsing
* Add an example C# PSK client
* Add ESP-IDF WOLFSSL_ESP8266 setting for ESP8266 devices
* Support longer sigalg list for post quantum use cases and inter-op with OQS's OpenSSL fork
* Improve AES-GCM word implementation of GMULT to be constant time
* Additional sanity check with Ed25519/Ed448, now defaults to assume public key is not trusted
* Support PSK ciphersuites in benchmark apps
* FIPS in core hash using SHA2-256 and SHA2-384
* Add ability to store issuer name components when parsing a certificate
* Make the critical extension flags in DecodedCert always available
* Updates to the default values for basic constraint with X509’s
* Support using RSA OAEP with no malloc and add additional sanity checks
* Leverage async code paths to support WANT_WRITE while sending packet fragments
* New azsphere example for continuous integration testing
* Update RSA key generation function to handle pairwise consistency tests with static memory pools used
* Resolve build time warning by passing in and checking output length with internal SetCurve function
* Support DTLS bidirectional shutdown in the examples
* Improve DTLS version negotiation and downgrade capability

### General Fixes
* Fixes for STM32 Hash/PKA, add some missing mutex frees, and add an additional benchmark
* Fix missing return checks in KSDK ED25519 code
* Fix compilation warnings from IAR
* Fixes for STM32U5/H7 hash/crypto support
* Fix for using track memory feature with FreeRTOS
* Fixup XSTR processing for MICRIUM
* Update Zephyr fs.h path
* DTLS fixes with WANT_WRITE simulations
* Fixes for BER use with PKCS7 to have additional sanity checks and guards on edge cases
* Fix to handle exceptional edge case with TFM mp_exptmod_ex
* Fix for stack and heap measurements of a 32-bit build
* Fix to allow enabling AES key wrap (direct) with KCAPI
* Fix --enable-openssh FIPS detection syntax in configure.ac
* Fix to move wolfSSL_ERR_clear_error outside gate for OPENSSL_EXTRA
* Remove MCAPI project's dependency on zlib version
* Only use __builtin_offset on supported GCC versions (4+)
* Fix for c89 builds with using WOLF_C89
* Fix 64bit postfix for constants building with powerpc
* Fixed async Sniffer with TLS v1.3, async removal of `WC_HW_WAIT_E` and sanitize leak
* Fix for QAT ECC to gate use of HW based on marker
* Fix the supported version extension to always check minDowngrade
* Fix for TLS v1.1 length sanity check for large messages
* Fixes for loading a long DER/ASN.1 certificate chain
* Fix to expose the RSA public DER export functions with certgen
* Fixes for building with small version of SHA3
* Fix configure with WOLFSSL_WPAS_SMALL
* Fix to free PKCS7 recipient list in error cases
* Sanity check to confirm ssl->hsHashes is not NULL before attempting to dereference it
* Clear the leftover byte count in Aes struct when setting IV

# wolfSSL Release 5.3.0 (May 3rd, 2022)

Release 5.3.0 of wolfSSL embedded TLS has bug fixes and new features including:

## New Feature Additions

### Ports
* Updated support for Stunnel to version 5.61
* Add i.MX8 NXP SECO use for secure private ECC keys and expand cryptodev-linux for use with the RSA/Curve25519 with the Linux CAAM driver
* Allow encrypt then mac with Apache port
* Update Renesas TSIP version to 1.15 on GR-ROSE and certificate signature data for TSIP / SCE example
* Add IAR MSP430 example, located in IDE/IAR-MSP430 directory
* Add support for FFMPEG with the enable option `--enable-ffmpeg`, FFMPEG is used for recording and converting video and audio (https://ffmpeg.org/)
* Update the bind port to version 9.18.0

### Post Quantum
* Add Post-quantum KEM benchmark for STM32
* Enable support for using post quantum algorithms with embedded STM32 boards and port to STM32U585

### Compatibility Layer Additions
* Add port to support libspdm (https://github.com/DMTF/libspdm/blob/main/README.md), compatibility functions added for the port were:
    - ASN1_TIME_compare
    - DH_new_by_nid
    - OBJ_length, OBJ_get0_data,
    - EVP layer ChaCha20-Poly1305, HKDF
    - EC_POINT_get_affine_coordinates
    - EC_POINT_set_affine_coordinates
* Additional functions added were:
    - EC_KEY_print_fp
    - EVP_PKEY_paramgen
    - EVP_PKEY_sign/verify functionality
    - PEM_write_RSAPublicKey
    - PEM_write_EC_PUBKEY
    - PKCS7_sign
    - PKCS7_final
    - SMIME_write_PKCS7
    - EC_KEY/DH_up_ref
    - EVP_DecodeBlock
    - EVP_EncodeBlock
    - EC_KEY_get_conv_form
    - BIO_eof
    - Add support for BIO_CTRL_SET and BIO_CTRL_GET
* Add compile time support for the type SSL_R_NULL_SSL_METHOD_PASSED
* Enhanced X509_NAME_print_ex() to support RFC5523 basic escape
* More checks on OPENSSL_VERSION_NUMBER for API prototype differences
* Add extended key usage support to wolfSSL_X509_set_ext
* SSL_VERIFY_FAIL_IF_NO_PEER_CERT now can also connect with compatibility layer enabled and a TLS 1.3 PSK connection is used
* Improve wolfSSL_BN_rand to handle non byte boundaries and top/bottom parameters
* Changed X509_V_ERR codes to better match OpenSSL values used
* Improve wolfSSL_i2d_X509_name to allow for a NULL input in order to get the expected resulting size
* Enhance the smallstack build to reduce stack size farther when built with compatibility layer enabled

### Misc.
* Sniffer asynchronous support addition, handling of DH shared secret and tested with Intel QuickAssist
* Added in support for OCSP with IPv6
* Enhance SP (single precision) optimizations for use with the ECC P521
* Add new public API wc_CheckCertSigPubKey() for use to easily check the signature of a certificate given a public key buffer
* Add CSR (Certificate Signing Request) userId support in subject name
* Injection and parsing of custom extensions in X.509 certificates
* Add WOLF_CRYPTO_CB_ONLY_RSA and WOLF_CRYPTO_CB_ONLY_ECC to reduce code size if using only crypto callback functions with RSA and ECC
* Created new --enable-engine configure flag used to build wolfSSL for use with wolfEngine
* With TLS 1.3 PSK, when WOLFSSL_PSK_MULTI_ID_PER_CS is defined multiple IDs for a cipher suite can be handled
* Added private key id/label support with improving the PK (Public Key) callbacks
* Support for Intel QuickAssist ECC KeyGen acceleration
* Add the function wolfSSL_CTX_SetCertCbCtx to set user context for certificate call back
* Add the functions wolfSSL_CTX_SetEccSignCtx(WOLFSSL_CTX* ctx, void *userCtx) and wolfSSL_CTX_GetEccSignCtx(WOLFSSL_CTX* ctx) for setting and getting a user context
* wolfRand for AMD --enable-amdrand

## Fixes
### PORT Fixes
* KCAPI memory optimizations and page alignment fixes for ECC, AES mode fixes and reduction to memory usage
* Add the new kdf.c file to the TI-RTOS build
* Fix wait-until-done in RSA hardware primitive acceleration of ESP-IDF port
* IOTSafe workarounds when reading files with ending 0’s and for ECC signatures

### Math Library Fixes
* Sanity check with SP math that ECC points ordinates are not greater than modulus length
* Additional sanity checks that _sp_add_d does not error due to overflow
* Wycheproof fixes, testing integration, and fixes for AVX / AArch64 ASM edge case tests 
* TFM fp_div_2_ct rework to avoid potential overflow

### Misc.
* Fix for PKCS#7 with Crypto Callbacks
* Fix for larger curve sizes with deterministic ECC sign
* Fixes for building wolfSSL alongside openssl using --enable-opensslcoexist
* Fix for compatibility layer handling of certificates with SHA256 SKID (Subject Key ID)
* Fix for wolfSSL_ASN1_TIME_diff erroring out on a return value of 0 from mktime
* Remove extra padding when AES-CBC encrypted with PemToDer
* Fixes for TLS v1.3 early data with async.
* Fixes for async disables around the DevCopy calls
* Fixes for Windows AES-NI with clang compiler
* Fix for handling the detection of processing a plaintext TLS alert packet
* Fix for potential memory leak in an error case with TLSX supported groups
* Sanity check on `input` size in `DecodeNsCertType`
* AES-GCM stack alignment fixes with assembly code written for AVX/AVX2
* Fix for PK callbacks with server side and setting a public key

## Improvements/Optimizations
### Build Options and Warnings
* Added example user settings template for FIPS v5 ready
* Automake file touch cleanup for use with Yocto devtool
* Allow disabling forced 'make clean' at the end of ./configure by using --disable-makeclean
* Enable TLS 1.3 early data when specifying `--enable-all` option
* Disable PK Callbacks with JNI FIPS builds
* Add a FIPS cert 3389 ready option, this is the fips-ready build
* Support (no)inline with Wind River Diab compiler
* ECDH_compute_key allow setting of globalRNG with FIPS 140-3
* Add logic equivalent to configure.ac in settings.h for Poly1305
* Fixes to support building opensslextra with SP math
* CPP protection for extern references to x86_64 asm code
* Updates and enhancements for Espressif ESP-IDF wolfSSL setup_win.bat
* Documentation improvements with auto generation
* Fix reproducible-build for working an updated version of libtool, version 2.4.7
* Fixes for Diab C89 and armclang
* Fix `mcapi_test.c` to include the settings.h before crypto.h
* Update and handle builds with NO_WOLFSSL_SERVER and NO_WOLFSSL_CLIENT
* Fix for some macro defines with FIPS 140-3 build so that RSA_PKCS1_PSS_PADDING can be used with RSA sign/verify functions

### Math Libraries
* Add RSA/DH check for even modulus
* Enhance TFM math to handle more alloc failure cases gracefully
* SP ASM performance improvements mostly around AArch64
* SP ASM improvements for additional cache attack resistance
* Add RSA check for small difference between p and q
* 6-8% performance increase with ECC operations using SP int by improving the Montgomery Reduction

### Testing and Validation
* All shell scripts in source tree now tested for correctness using shellcheck and bash -n
* Added build testing under gcc-12 and -std=c++17 and fixed warnings
* TLS 1.3 script test improvement to wait for server to write file
* Unit tests for ECC r/s zeroness handling
* CI server was expanded with a very “quiet” machine that can support multiple ContantTime tests ensuring ongoing mitigation against side-channel timing based attacks. Algorithms being assessed on this machine are: AES-CBC, AES-GCM, CHACHA20, ECC, POLY1305, RSA, SHA256, SHA512, CURVE25519.
* Added new multi configuration windows builds to CI testing for greater testing coverage of windows use-cases

### Misc.
* Support for ECC import to check validity of key on import even if one of the coordinates (x or y) is 0
* Modify example app to work with FreeRTOS+IoT
* Ease of access for cert used for verifying a PKCS#7 bundle
* Clean up Visual Studio output and intermediate directories
* With TLS 1.3 fail immediately if a server sends empty certificate message
* Enhance the benchmark application to support multi-threaded testing
* Improvement for `wc_EccPublicKeyToDer` to not overestimate the buffer size required
* Fix to check if `wc_EccPublicKeyToDer` has enough output buffer space
* Fix year 2038 problem in wolfSSL_ASN1_TIME_diff
* Various portability improvements (Time, DTLS epoch size, IV alloc)
* Prefer status_request_v2 over status_request when both are present
* Add separate "struct stat" definition XSTATSTRUCT to make overriding XSTAT easier for portability
* With SipHash replace gcc specific ASM instruction with generic
* Don't force a ECC CA when a custom CA is passed with `-A`
* Add peer authentication failsafe for TLS 1.2 and below
* Improve parsing of UID from subject and issuer name with the compatibility layer by
* Fallback to full TLS handshake if session ticket fails
* Internal refactoring of code to reduce ssl.c file size

# wolfSSL Release 5.2.0 (Feb 21, 2022)

## Vulnerabilities

* \[High\] A TLS v1.3 server who requires mutual authentication can be
  bypassed. If a malicious client does not send the certificate_verify
  message a client can connect without presenting a certificate even
  if the server requires one. Thank you to Aina Toky Rasoamanana and
  Olivier Levillain of Télécom SudParis.
* \[High\] A TLS v1.3 client attempting to authenticate a TLS v1.3
  server can have its certificate check bypassed. If the sig_algo in
  the certificate_verify message is different than the certificate
  message checking may be bypassed. Thank you to Aina Toky Rasoamanana and
  Olivier Levillain of Télécom SudParis.

## New Feature Additions

* Example applications for Renesas RX72N with FreeRTOS+IoT
* Renesas FSP 3.5.0 support for RA6M3
* For TLS 1.3, improved checks on order of received messages.
* Support for use of SHA-3 cryptography instructions available in
  ARMv8.2-A architecture extensions. (For Apple M1)
* Support for use of SHA-512 cryptography instructions available in
  ARMv8.2-A architecture extensions.  (For Apple M1)
* Fixes for clang -Os on clang >= 12.0.0
* Expose Sequence Numbers so that Linux TLS (kTLS) can be configured
* Fix bug in TLSX_ALPN_ParseAndSet when using ALPN select callback.
* Allow DES3 with FIPS v5-dev.
* Include HMAC for deterministic ECC sign build
* Add --enable-chrony configure option. This sets build options needed
  to build the Chrony NTP (Network Time Protocol) service.
* Add support for STM32U575xx boards.
* Fixes for NXP’s SE050 Ed25519/Curve25519.
* TLS: Secure renegotiation info on by default for compatibility.
* Inline C code version of ARM32 assembly for cryptographic algorithms
  available and compiling for improved performance on ARM platforms
* Configure HMAC: define NO_HMAC to disable HMAC (default: enabled)
* ISO-TP transport layer support added to wolfio for TLS over CAN Bus
* Fix initialization bug in SiLabs AES support
* Domain and IP check is only performed on leaf certificates

## ARM PSA Support (Platform Security Architecture) API

* Initial support added for ARM’s Platform Security Architecture (PSA)
  API in wolfCrypt which allows support of ARM PSA enabled devices by
  wolfSSL, wolfSSH, and wolfBoot and wolfCrypt FIPS.
* Included algorithms: ECDSA, ECDH, HKDF, AES, SHA1, SHA256, SHA224, RNG

## ECICE Updates

* Support for more encryption algorithms: AES-256-CBC, AES-128-CTR,
  AES-256-CTR
* Support for compressed public keys in messages.

## Math Improvements

* Improved performance of X448 and Ed448 through inlining Karatsuba in
  square and multiplication operations for 128-bit implementation
  (64-bit platforms with 128-bit type support).
* SP Math C implementation: fix for corner case in curve specific
  implementations of Montgomery Reduction (P-256, P-384).
* SP math all: assembly snippets added for ARM Thumb. Performance
  improvement on platform.
* SP math all: ARM64/32 sp_div_word assembly snippets added to remove
  dependency on __udiv3.
* SP C implementation: multiplication of two signed types with overflow
  is undefined in C. Now cast to unsigned type before multiplication is
  performed.
* SP C implementation correctly builds when using CFLAG: -m32

## OpenSSL Compatibility Layer

* Added DH_get_2048_256 to compatibility layer.
* wolfSSLeay_version now returns the version of wolfSSL
* Added C++ exports for API’s in wolfssl/openssl/crypto.h. This allows
  better compatibility when building with a C++ compiler.
* Fix for OpenSSL x509_NAME_hash mismatch
* Implement FIPS_mode and FIPS_mode_set in the compat layer.
* Fix for certreq and certgen options with openssl compatibility
* wolfSSL_BIO_dump() and wolfSSL_OBJ_obj2txt() rework
* Fix IV length bug in EVP AES-GCM code.
* Add new ASN1_INTEGER compatibility functions.
* Fix wolfSSL_PEM_X509_INFO_read with NO_FILESYSTEM

## CMake Updates

* Check for valid override values.
* Add `KEYGEN` option.
* Cleanup help messages.
* Add options to support wolfTPM.

## VisualStudio Updates

* Remove deprecated VS solution
* Fix VS unreachable code warning

## New Algorithms and Protocols

* AES-SIV (RFC 5297)
* DTLS SRTP (RFC 5764), used with WebRTC to agree on profile for new
  real-time session keys
* SipHash MAC/PRF for hash tables. Includes inline assembly for
  x86_64 and Aarch64.

## Remove Obsolete Algorithms

* IDEA
* Rabbit
* HC-128


# wolfSSL Release 5.1.1 (Jan 3rd, 2022)
Release 5.1.1 of wolfSSL embedded TLS has a high vulnerability fix:

### Vulnerabilities
* \[High\]  In connections using AES-CBC or DES3 with TLS/DTLS 1.2 or 1.1 the IV being used is not random. Users using wolfSSL version 5.0.0 or 5.1.0 doing TLS/DTLS 1.2 or 1.1 connections, without AEAD only, should update the version of wolfSSL used. (CVE-2022-23408)

# wolfSSL Release 5.1.0 (Dec 27, 2021)
Release 5.1.0 of wolfSSL embedded TLS has bug fixes and new features including:

### Vulnerabilities
* \[Low\]  Potential for DoS attack on a wolfSSL client due to processing hello packets of the incorrect side. This affects only connections using TLS v1.2 or less that have also been compromised by a man in the middle attack. Thanks to James Henderson, Mathy Vanhoef, Chris M. Stone, Sam L. Thomas, Nicolas Bailleut, and Tom Chothia (University of Birmingham, KU Leuven, ENS Rennes for the report.
* \[Low\] Client side session resumption issue once the session resumption cache has been filled up. The hijacking of a session resumption has been demonstrated so far with only non verified peer connections. That is where the client is not verifying the server’s CA that it is connecting to. There is the potential though for other cases involving proxies that are verifying the server to be at risk, if using wolfSSL in a case involving proxies use wolfSSL_get1_session and then wolfSSL_SESSION_free when done where possible. If not adding in the session get/free function calls we recommend that users of wolfSSL that are resuming sessions update to the latest version (wolfSSL version 5.1.0 or later). Thanks to the UK's National Cyber Security Centre (NCSC) for the report.

### New Feature Additions
###### Ports
* Curve25519 support with NXP SE050 added
* Renesas RA6M4 support with SCE Protected Mode and FSP 3.5.0
* Renesas TSIP 1.14 support for RX65N/RX72N

###### Post Quantum
* Post quantum resistant algorithms used with Apache port
* NIST round 3 FALCON Signature Scheme support added to TLS 1.3 connections
* FALCON added to the benchmarking application
* Testing of cURL with wolfSSL post quantum resistant build

###### Compatibility Layer Additions
* Updated NGINX port to NGINX version 1.21.4
* Updated Apache port to Apache version 2.4.51
* Add support for SSL_OP_NO_TLSv1_2 flag with wolfSSL_CTX_set_options function
* Support added for the functions
    - SSL_CTX_get_max_early_data
    - SSL_CTX_set_max_early_data
    - SSL_set_max_early_data
    - SSL_get_max_early_data
    - SSL_CTX_clear_mode
    - SSL_CONF_cmd_value_type
    - SSL_read_early_data
    - SSL_write_early_data

###### Misc.
* Crypto callback support for AES-CCM added. A callback function can be registered and used instead of the default AES-CCM implementation in wolfSSL.
* Added AES-OFB to the FIPS boundary for future FIPS validations.
* Add support for custom OIDs used with CSR (certificate signing request) generation using the macro WOLFSSL_CUSTOM_OID
* Added HKDF extract callback function for use with TLS 1.3
* Add variant from RFC6979 of deterministic ECC signing that can be enabled using the macro WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT
* Added the function wc_GetPubKeyDerFromCert to get the public key from a DecodedCert structure
* Added the functions wc_InitDecodedCert, wc_ParseCert and wc_FreeDecodedCert for access to decoding a certificate into a DecodedCert structure
* Added the macro WOLFSSL_ECC_NO_SMALL_STACK for hybrid builds where the numerous malloc/free with ECC is undesired but small stack use is desired throughout the rest of the library
* Added the function wc_d2i_PKCS12_fp for reading a PKCS12 file and parsing it

### Fixes
###### PORT Fixes
* Building with Android wpa_supplicant and KeyStore
* Setting initial value of CA certificate with TSIP enabled
* Cryptocell ECC build fix and fix with RSA disabled 
* IoT-SAFE improvement for Key/File slot ID size, fix for C++ compile, and fixes for retrieving the public key after key generation

###### Math Library Fixes
* Check return values on TFM library montgomery function in case the system runs out of memory. This resolves an edge case of invalid ECC signatures being created.
* SP math library sanity check on size of values passed to sp_gcd.
* SP math library sanity check on exponentiation by 0 with mod_exp
* Update base ECC mp_sqrtmod_prime function to handle an edge case of zero
* TFM math library with Intel MULX multiply fix for carry in assembly code

###### Misc.
* Fix for potential heap buffer overflow with compatibility layer PEM parsing
* Fix for edge memory leak case with an error encountered during TLS resumption
* Fix for length on inner sequence created with wc_DhKeyToDer when handling small DH keys
* Fix for sanity check on input argument to DSA sign and verify
* Fix for setting of the return value with ASN1 integer get on an i386 device
* Fix for BER to DER size checks with PKCS7 decryption
* Fix for memory leak with PrintPubKeyEC function in compatibility layer
* Edge case with deterministic ECC key generation when the private key has leading 0’s
* Fix for build with OPENSSL_EXTRA and NO_WOLFSSL_STUB both defined
* Use page aligned memory with ECDSA signing and KCAPI
* Skip expired sessions for TLS 1.3 rather than turning off the resume behavior
* Fix for DTLS handling dropped or retransmitted messages

### Improvements/Optimizations
###### Build Options and Warnings
* Bugfix: could not build with liboqs and without DH enabled
* Build with macro NO_ECC_KEY_EXPORT fixed
* Fix for building with the macro HAVE_ENCRYPT_THEN_MAC when session export is enabled
* Building with wolfSentry and HAVE_EX_DATA macro set

###### Math Libraries
* Improvement for performance with SP C implementation of montgomery reduction for ECC (P256 and P384) and SP ARM64 implementation for ECC (P384)
* With SP math handle case of dividing by length of dividend
* SP math improvement for lo/hi register names to be used with older GCC compilers

###### Misc.
* ASN name constraints checking code refactor for better efficiency and readability
* Refactor of compatibility layer stack free’ing calls to simplify and reduce code
* Scrubbed code for trailing spaces, hard tabs, and any control characters
* Explicit check that leaf certificate's public key type match cipher suite signature algorithm
* Additional NULL sanity checks on WOLFSSL struct internally and improve switch statement fallthrough
* Retain OCSP error value when CRL is enabled with certificate parsing
* Update to NATIVE LwIP support for TCP use
* Sanity check on PEM size when parsing a PEM with OpenSSL compatibility layer API.
* SWIG wrapper was removed from the codebase in favor of dedicated Java and Python wrappers.
* Updates to bundled example client for when to load the CA, handling print out of IP alt names, and printing out the peers certificate in PEM format
* Handling BER encoded inner content type with PKCS7 verify
* Checking for SOCKET_EPIPE errors from low level socket
* Improvements to cleanup in the case that wolfSSL_Init fails
* Update test and example certificates expiration dates


# wolfSSL Release 5.0.0 (Nov 01, 2021)
Release 5.0.0 of wolfSSL embedded TLS has bug fixes and new features including:

### Vulnerabilities
* [\Low\] Hang with DSA signature creation when a specific q value is used in a maliciously crafted key. If a DSA key with an invalid q value of either 1 or 0 was decoded and used for creating a signature, it would result in a hang in wolfSSL. Users that are creating signatures with DSA and are using keys supplied from an outside source are affected.
* [\Low\] Issue with incorrectly validating a certificate that has multiple subject alternative names when given a name constraint. In the case where more than one subject alternative name is used in the certificate, previous versions of wolfSSL could incorrectly validate the certificate. Users verifying certificates with multiple alternative names and name constraints, are recommended to either use the certificate verify callback to check for this case or update the version of wolfSSL used. Thanks to Luiz Angelo Daros de Luca for the report.

### New Feature Additions
###### New Product
* FIPS 140-3 -- currently undergoing laboratory testing, code review and ultimately CMVP validation. Targeting the latest FIPS standard.

###### Ports
* IoT-Safe with TLS demo
* SE050 port with support for RNG, SHA, AES, ECC (sign/verify/shared secret) and ED25519
* Support for Renesas TSIP v1.13 on RX72N

###### Post Quantum
* Support for OQS's (liboqs version 0.7.0) implementation of NIST Round 3 KEMs as TLS 1.3 groups --with-liboqs
* Hybridizing NIST ECC groups with the OQS groups
* Remove legacy NTRU and QSH
* Make quantum-safe groups available to the compatibility layer

###### Linux Kernel Module
* Full support for FIPS 140-3, with in-kernel power on self test (POST) and conditional algorithm self test(s) (CAST)
* --enable-linuxkm-pie -- position-independent in-kernel wolfCrypt container, for FIPS
* Vectorized x86 acceleration in PK algs (RSA, ECC, DH, DSA) and AES/AES-GCM
* Vectorized x86 acceleration in interrupt handlers
* Support for Linux-native module signatures
* Complete SSL/TLS and Crypto API callable from other kernel module(s)
* Support for LTS kernel lines: 3.16, 4.4, 4.9, 5.4, 5.10

###### Compatibility Layer Additions
* Ports
    - Add support for libssh2
    - Add support for pyOpenSSL
    - Add support for libimobiledevice
    - Add support for rsyslog
    - Add support for OpenSSH 8.5p1
    - Add support for Python 3.8.5
* API/Structs Added
    - ERR_lib_error_string
    - EVP_blake2
    - wolfSSL_set_client_CA_list
    - wolfSSL_EVP_sha512_224
    - wolfSSL_EVP_sha512_256
    - wc_Sha512_224/2256Hash
    - wc_Sha512_224/256Hash
    - wc_InitSha512_224/256
    - wc_InitSha512_224/256_ex
    - wc_Sha512_224/256Update
    - wc_Sha512_224/256FinalRaw
    - wc_Sha512_224/256Final
    - wc_Sha512_224/256Free
    - wc_Sha512_224/256GetHash
    - wc_Sha512_224/256Copy
    - wc_Sha512_224/256SetFlags
    - wc_Sha512_224/256GetFlags
    - wc_Sha512_224/256Transform
    - EVP_MD_do_all and OBJ_NAME_do_all
    - EVP_shake128
    - EVP_shake256
    - SSL_CTX_set_num_tickets
    - SSL_CTX_get_num_tickets
    - SSL_CIPHER_get_auth_nid
    - SSL_CIPHER_get_cipher_nid
    - SSL_CIPHER_get_digest_nid
    - SSL_CIPHER_get_kx_nid
    - SSL_CIPHER_is_aead
    - SSL_CTX_set_msg_callback
    - a2i_IPADDRESS
    - GENERAL_NAME_print
    - X509_VERIFY_PARAM_set1_ip
    - EVP_CIPHER_CTX_set_iv_length
    - PEM_read_bio_RSA_PUBKEY
    - i2t_ASN1_OBJECT
    - DH_set_length
    - Set_tlsext_max_fragment_length
    - AUTHORITY_iNFO_ACCESS_free
    - EVP_PBE_scrypt
    - ASN1_R_HEADER_TOO_LONG
    - ERR_LIB
    - X509_get_default_cert_file/file_env/dir/dir_env() stubs
    - SSL_get_read_ahead/SSL_set_read_ahead()
    - SSL_SESSION_has_ticket()
    - SSL_SESSION_get_ticket_lifetime_hint()
    - DIST_POINT_new
    - DIST_POINT_free 
    - DIST_POINTS_free
    - CRL_DIST_POINTS_free
    - sk_DIST_POINT_push
    - sk_DIST_POINT_value
    - sk_DIST_POINT_num
    - sk_DIST_POINT_pop_free
    - sk_DIST_POINT_free
    - X509_get_extension_flags
    - X509_get_key_usage
    - X509_get_extended_key_usage
    - ASN1_TIME_to_tm
    - ASN1_TIME_diff
    - PEM_read_X509_REQ
    - ERR_load_ERR_strings
    - BIO_ssl_shutdown
    - BIO_get_ssl
    - BIO_new_ssl_connect
    - BIO_set_conn_hostname
    - NID_pkcs9_contentType

###### Misc.
* KCAPI: add support for using libkcapi for crypto (Linux Kernel)
* Configure option for --with-max-rsa-bits= and --with-max-ecc-bits=
* SP ARM Thumb support for Keil and performance improvements
* Add support for WOLFSSL_VERIFY_POST_HANDSHAKE verify mode
* PKCS #11: support static linking with PKCS #11 library --enable-pkcs11=static LIBS=-l
* Add build option --enable-wolfclu for use with wolfCLU product
* Add support for X9.42 header i.e “BEGIN X9.42 DH PARAMETERS”
* Add --enable-altcertchains for configuring wolfSSL with alternate certificate chains feature enabled
* Add public API wc_RsaKeyToPublicDer_ex to allow getting RSA public key without ASN.1 header (can return only seq + n + e)
* Add SNI and TLSx options to CMake build

### Fixes
###### PORT Fixes
* Add return value checking for FREESCALE_RNGA
* Fix MMCAU_SHA256 type warnings
* Fixes for building with Microchip XC32 and ATECC

###### Math Library Fixes
* TFM check that the modulus length is valid for fixed data array size
* TFM fp_submod_ct fix check for greater
* Check return value of mp_grow in mp_mod_2d
* Fix for ECC point multiply to error out on large multipliers
* SP ECC error on multiplier larger than curve order

###### TLS 1.3
* TLS1.3 sanity check for cases where a private key is larger than the configured maximum
* Fix early data max size handling in TLS v1.3
* Fixes for PK callbacks with TLS v1.3
* Check min downgrade when no extensions are sent with the ServerHello

###### Misc.
* Previously wolfSSL enum values were used as NID’s. Now only the compatibility layer NID enums are the NID values:
    - CTC_SHAwDSA -> NID_dsaWithSHA1
    - CTC_SHA256wDSA -> NID_dsa_with_SHA256
    - CTC_MD2wRSA -> NID_md2WithRSAEncryption
    - CTC_MD5wRSA -> NID_md5WithRSAEncryption
    - CTC_SHAwRSA -> NID_sha1WithRSAEncryption
    - CTC_SHA224wRSA -> NID_sha224WithRSAEncryption
    - CTC_SHA256wRSA -> NID_sha256WithRSAEncryption
    - CTC_SHA384wRSA -> NID_sha384WithRSAEncryption
    - CTC_SHA512wRSA -> NID_sha512WithRSAEncryption
    - CTC_SHA3_224wRSA -> NID_RSA_SHA3_224
    - CTC_SHA3_256wRSA -> NID_RSA_SHA3_256
    - CTC_SHA3_384wRSA -> NID_RSA_SHA3_384
    - CTC_SHA3_512wRSA -> NID_RSA_SHA3_512
    - CTC_SHAwECDSA -> NID_ecdsa_with_SHA1
    - CTC_SHA224wECDSA -> NID_ecdsa_with_SHA224
    - CTC_SHA256wECDSA -> NID_ecdsa_with_SHA256
    - CTC_SHA384wECDSA -> NID_ecdsa_with_SHA384
    - CTC_SHA512wECDSA -> NID_ecdsa_with_SHA512
    - CTC_SHA3_224wECDSA -> NID_ecdsa_with_SHA3_224
    - CTC_SHA3_256wECDSA -> NID_ecdsa_with_SHA3_256
    - CTC_SHA3_384wECDSA -> NID_ecdsa_with_SHA3_384
    - CTC_SHA3_512wECDSA -> NID_ecdsa_with_SHA3_512
    - DSAk -> NID_dsa
    - RSAk -> NID_rsaEncryption
    - ECDSAk -> NID_X9_62_id_ecPublicKey
    - BASIC_CA_OID -> NID_basic_constraints
    - ALT_NAMES_OID -> NID_subject_alt_name
    - CRL_DIST_OID -> NID_crl_distribution_points
    - AUTH_INFO_OID -> NID_info_access
    - AUTH_KEY_OID -> NID_authority_key_identifier
    - SUBJ_KEY_OID -> NID_subject_key_identifier
    - INHIBIT_ANY_OID -> NID_inhibit_any_policy
* Fix for DES IV size used with FIPSv2
* Fix signed comparison issue with serialSz
* Fix missing CBIOSend and properly guard hmac in DupSSL()
* Fix calculation of length of encoding in ssl.c
* Fix encoding to check proper length in asn.c
* Fix for wc_ecc_ctx_free and heap hint
* Fix for debug messages with AF_ALG build
* Fix for static memory with bucket size matching.
* Fixes for SRP with heap hint.
* Fixes for CAAM build macros and spelling for Keil build
* Sniffer fix for possible math issue around 64-bit pointer and 32-bit unsigned int
* Fix for sniffer TCP sequence rollover
* wolfSSL_PEM_write_bio_PUBKEY to write only the public part
* Fix for sending only supported groups in TLS extension
* Fix for sniffer to better handle spurious retransmission edge case
* SSL_set_alpn_protos and SSL_CTX_set_alpn_protos now returns 0 on successFixes issue with SSL_CTX_set1_curves_list and SSL_set1_curves_list not checking the last character of the names variable provided, non-0 on failure to better match expected return values
* Fixes and improvements for crypto callbacks with TLS (mutual auth)
* Fix for bad memory_mutex lock on static memory cleanup
* Zero terminate name constraints strings when parsing certificates
* Fix for verifying a certificate when multiple permitted name constraints are used
* Fix typo in ifdef for HAVE_ED448
* Fix typos in comments in SHA512
* Add sanity check on buffer size with ED25519 key decode
* Sanity check on PKCS7 stream amount read
* PKCS7 fix for double free on error case and sanity check on set serial number
* Sanity check on PKCS7 input size wc_PKCS7_ParseSignerInfo
* Forgive a DTLS session trying to send too much at once

### Improvements/Optimizations
###### Build Options and Warnings
* Rework of RC4 disable by default and depreciation
* wolfSSL as a Zephyr module (without setup.sh)
* Add include config.h to bio.c
* Support for PKCS7 without AES CBC.
* Fixes for building without AES CBC
* Added WOLFSSL_DH_EXTRA to --enable-all and --enable-sniffer
* Add a CMake option to build wolfcrypt test and bench code as libraries
* GCC makefile: allow overriding and provide more flexibility

###### Math Libraries
* Improve performance of fp_submod_ct() and fp_addmod_ct()
* Improve performance of sp_submod_ct() and sp_addmod_ct()
* SP int, handle even modulus with exponentiation

###### Misc.
* Cleanups for Arduino examples and memory documentation
* Refactor hex char to byte conversions
* Added GCC-ARM TLS server example
* Improvements to session locking to allow per-row
* Improved sniffer statistics and documentation
* EVP key support for heap hint and crypto callbacks
* Reduced stack size for dh_generation_test and Curve ASN functions
* Espressif README Syntax / keyword highlighting / clarifications
* AARCH64 SHA512: implementation using crypto instructions added
* wc_RsaPSS_CheckPadding_ex2 added for use with HEAP hint
* wc_AesKeyWrap_ex and wc_AesKeyUnWrap_ex bound checks on input and output sizes
* Add additional error handling to wolfSSL_BIO_get_len
* Add code to use popen and the command 'host', useful with qemu
* Adjustment to subject alt names order with compatibility layer to better match expected order
* Reduce BIO compatibility layer verbosity
* Set a default upper bound on error queue size with compatibility layer
* WOLFSSL_CRL_ALLOW_MISSING_CDP macro for Skip CRL verification in case no CDP in peer cert
* Fixes for scan-build LLVM-13 and expanded coverage
* Increase the default DTLS_MTU_ADDITIONAL_READ_BUFFER and make it adjustable

# wolfSSL Release 4.8.1 (July 16, 2021)
Release 4.8.1 of wolfSSL embedded TLS has an OCSP vulnerability fix:

### Vulnerabilities
* [High] OCSP verification issue when response is for a certificate with no relation to the chain in question BUT that response contains the NoCheck extension which effectively disables ALL verification of that one cert. Users who should upgrade to 4.8.1 are TLS client users doing OCSP, TLS server users doing mutual auth with OCSP, and CertManager users doing OCSP independent of TLS. Thanks to Jan Nauber, Marco Smeets, Werner Rueschenbaum and Alissa Kim of Volkswagen Infotainment for the report.


# wolfSSL Release 4.8.0 (July 09, 2021)
Release 4.8.0 of wolfSSL embedded TLS has bug fixes and new features including:

### Vulnerabilities
* [Low] CVE-2021-37155: OCSP request/response verification issue. In the case that the serial number in the OCSP request differs from the serial number in the OCSP response the error from the comparison was not resulting in a failed verification. We recommend users that have wolfSSL version 4.6.0 and 4.7.0 with OCSP enabled update their version of wolfSSL. Version 4.5.0 and earlier are not affected by this report. Thanks to Rainer Mueller-Amersdorffer, Roee Yankelevsky, Barak Gutman, Hila Cohen and Shoshi Berko (from CYMOTIVE Technologies and CARIAD) for the report.
* [Low] CVE-2021-24116: Side-Channel cache look up vulnerability in base64 PEM decoding for versions of wolfSSL 4.5.0 and earlier. Versions 4.6.0 and up contain a fix and do not need to be updated for this report. If decoding a PEM format private key using version 4.5.0 and older of wolfSSL then we recommend updating the version of wolfSSL used. Thanks to Florian Sieck, Jan Wichelmann, Sebastian Berndt and Thomas Eisenbarth for the report. 

### New Feature Additions
###### New Product
* Added wolfSentry build with --enable-wolfsentry and tie-ins to wolfSSL code for use with wolfSentry

###### Ports
* QNX CAAM driver added, supporting ECC black keys, CMAC, BLOBs, and TRNG use
*  _WIN32_WCE wolfCrypt port added
* INTIME_RTOS directory support added
* Added support for STM32G0
* Renesas RX: Added intrinsics for rot[rl], revl (thanks @rliebscher)
* Added support for running wolfcrypt/test/testwolfcrypt on Dolphin emulator to test DEVKITPRO port
* Zephyr project port updated to latest version 2.6.X

###### ASN1 and PKCS
* Storing policy constraint extension from certificate added
* Added support for NID_favouriteDrink pilot
* Added the API function wc_EncryptPKCS8Key to handle encrypting a DER, PKCS#8-formatted key

###### Compatibility Layer Additions
* Open Source PORTS Added/Updated
    - OpenVPN
    - OpenLDAP
    - socat-1.7.4.1
    - Updated QT port for 5.15.2
* Changes to extend set_cipher_list() compatibility layer API to have set_ciphersuites compatibility layer API capability
* Added more support for SHA3 in the EVP layer
* API Added
    - MD5/MD5_Transform
    - SHA/SHA_Transform/SHA1_Transform
    - SHA224/SHA256_Transform/SHA512_Transform
    - SSL_CTX_get0_param/SSL_CTX_set1_param
    - X509_load_crl_file
    - SSL_CTX_get_min_proto_version
    - EVP_ENCODE_CTX_new
    - EVP_ENCODE_CTX_free
    - EVP_EncodeInit
    - EVP_EncodeUpdate
    - EVP_EncodeFinal
    - EVP_DecodeInit
    - EVP_DecodeUpdate
    - EVP_DecodeFinal
    - EVP_PKEY_print_public
    - BIO_tell
    - THREADID_current
    - THREADID_hash
    - SSL_CTX_set_ecdh_auto
    - RAND_set_rand_method()
    - X509_LOOKUP_ctrl()
    - RSA_bits
    - EC_curve_nist2nid
    - EC_KEY_set_group
    - SSL_SESSION_set_cipher
    - SSL_set_psk_use_session_callback
    - EVP_PKEY_param_check
    - DH_get0_pqg
    - CRYPTO_get_ex_new_index
    - SSL_SESSION_is_resumable
    - SSL_CONF_cmd
    - SSL_CONF_CTX_finish
    - SSL_CTX_keylog_cb_func
    - SSL_CTX_set_keylog_callback
    - SSL_CTX_get_keylog_callback

###### Misc.
* Added wolfSSL_CTX_get_TicketEncCtx getter function to return the ticket encryption ctx value
* Added wc_AesKeyWrap_ex and wc_AesKeyUnWrap_ex APIs to accept an Aes object to use for the AES operations
* Added implementation of AES-GCM streaming (--enable-aesgcm-stream)
* Added deterministic generation of k with ECC following RFC6979 when the macro WOLFSSL_ECDSA_DETERMINISTIC_K is defined and wc_ecc_set_deterministic function is called
* Implemented wc_DsaParamsDecode and wc_DsaKeyToParamsDer
* Asynchronous support for TLS v1.3 TLSX ECC/DH key generation and key agreement
* Added crypto callback support for Ed/Curve25519 and SHA2-512/384
* TLS 1.3 wolfSSL_key_update_response function added to see if a update response is needed

### Fixes
* Fix for detecting extra unused bytes that are in an ASN1 sequence appended to the end of a valid ECC signature
* Fix for keyid with ktri CMS (breaks compatibility with previous keyid ASN1 syntax)
* Fix for failed handshake if a client offers more than 150 cipher suites. Thanks to Marcel Maehren, Philipp Nieting, Robert Merget from Ruhr University Bochum Sven Hebrok, Juraj Somorovsky from Paderborn University
* Fix for default order of deprecated elliptic curves SECP224R1, SECP192R1, SECP160R1. Thanks to Marcel Maehren, Philipp Nieting, Robert Merget from Ruhr University Bochum Sven Hebrok, Juraj Somorovsky from Paderborn University
* Fix for corner TLS downgrade case where a TLS 1.3 setup that allows for downgrades but has TLS 1.3 set as the minimum version would still downgrade to TLS 1.2

###### PKCS7 (Multiple fixes throughout regarding memory leaks with SMIME and heap buffer overflows due to streaming functionality)
* Fix PKCS7 dynamic content save/restore in PKCS7_VerifySignedData
* Fix for heap buffer overflow on compare with wc_PKCS7_DecryptKtri
* Fix for heap buffer overflow with wc_PKCS7_VerifySignedData
* Fix for heap buffer overflow with wc_PKCS7_DecodeEnvelopedData
* Check size of public key used with certificate passed into wc_PKCS7_InitWithCert before XMEMCPY to avoid overflow
* Fix for heap buffer overflow fix for wolfSSL_SMIME_read_PKCS7
* Fix to cleanly free memory in error state with wolfSSL_SMIME_read_PKCS7
* SMIME error checking improvements and canonicalize multi-part messages before hashing

###### DTLS Fixes
* DTLS fix to correctly move the Tx sequence number forward
* DTLS fix for sequence and epoch number with secure renegotiation cookie exchange
* Fix for Chacha-Poly AEAD for DTLS 1.2 with secure renegotiation

###### PORT Fixes
* Fix AES, aligned key for the HW module with DCP port
* Fix ATECC608A TNGTLS certificate size issue (thanks @vppillai)
* Fixes for mingw compile warnings
* Fixes for NXP LTC ECC/RSA
* Fix ESP32 RSA hw accelerator initialization issue
* Fixes for STM32 PKA with ECC
* Fixes for STM32 AES GCM for HAL's that support byte sized headers
* Espressif ESP32 SHA_CTX macro conflict resolved

###### Math Library Fixes
* For platforms that support limits.h or windows make sure both SIZEOF_LONG_LONG and SIZEOF_LONG are set to avoid issues with CTC_SETTINGS
* SP C 32/64: fix corner cases around subtraction affecting RSA PSS use
* Fix to return the error code from sp_cond_swap_ct when malloc fails
* Fix potential memory leak with small stack in the function fp_gcd
* Static Analysis Fixes
* Fixes made from Coverity analysis including:
* Cleanups for some return values,
* Fix for leak with wolfSSL_a2i_ASN1_INTEGER
* Sanity check on length in wolfSSL_BN_rand
* Sanity check size in TLSX_Parse catching a possible integer overflow
* Fixes found with -fsanitize=undefined testing
* Fix null dereferences or undefined memcpy calls
* Fix alignment in myCryptoDevCb
* Fix default DTLS context assignment
* Added align configure option to force data alignment

###### Misc.
* Fix for wolfSSL_ASN1_TIME_adj set length
* Fix for freeing structure on error case in the function AddTrustedPeer
* Return value of SSL_read when called after bidirectional shutdown
* Fix for build options ./configure --enable-dtls --disable-asn
* FIx for detection of a salt length from an RSA PSS signature
* Fix to free up globalRNGMutex mutex when cleaning up global RNG
* Fix leak when multiple hardware names are in SAN
* Fix nonblocking ret value from CRL I/O callbacks
* Fix wolfSSL_BIO_free_all return type to better match for compatibility layer
* Fix for make distcheck, maintainer-clean, to allow distribution builds
* Fix for async with fragmented packets
* Fix for the build or RSA verify or public only
* Fix for return value of wolfSSL_BIO_set_ssl to better match expected compatibility layer return value
* Fix for sanity checks on size of issuer hash and key along with better freeing on error cases with DecodeBasicOcspResponse
* Fix for potential memory leak with wolfSSL_OCSP_cert_to_id

### Improvements/Optimizations
###### DTLS/TLS Code Base
* Improved TLS v1.3 time rollover support
* TLS 1.3 PSK: use the hash algorithm to choose cipher suite
* TLS Extended Master Secret ext: TLS13 - send in second Client Hello if in first
* TLS Encrypt then MAC: check all padding bytes are the same value
* wolfSSL_GetMaxRecordSize updated to now take additional cipher data into account
* Updated session export/import with DTLS to handle a new internal options flag
* Refactored dtls_expected_peer_handshake_number handling
* Added wolfSSL_CTX_get_ephemeral_key and wolfSSL_get_ephemeral_key for loading a constant key in place of an ephemeral one
* Improved checking of XSNPRINTF return value in DecodePolicyOID

###### Build Options and Warnings
* Added wolfSSL_CTX_set_verify to the ABI list
* Adjusted FP_ECC build to not allow SECP160R1, SECP160R2, SECP160K1 and SECP224K1. FP_ECC does not work with scalars that are the length of the order when the order is longer than the prime.
* Added CMake support for CURVE25519, ED25519, CURVE448, and ED448
* cmake addition to test paths when building
* Added support for session tickets in CMake
* Added support for reproducible builds with CMake
* Turn on reproducible-build by default when enable-distro
* Windows Project: Include the X448 and Ed448 files
* GCC-11 compile time warning fixes
* Fix for compiling build of ./configure '--disable-tlsv12' '-enable-pkcallbacks'
* Added build error for insecure build combination of secure renegotiation enabled with extended master secret disabled when session resumption is enabled
* Updated building and running with Apple M1
* Apache httpd build without TLS 1.3 macro guard added
* Enable SHA3 and SHAKE256 requirements automatically when ED448 is enabled
* Added option for AES CBC cipher routines to return BAD_LENGTH_E when called with an input buffer length not a multiple of AES_BLOCK_SIZE
* Macro WOLFSSL_SP_INT_DIGIT_ALIGN added for alignment on buffers with SP build. This was needed for compiler building on a Renesas board.
* Build support with no hashes enabled an no RNG compiled in
* Allow use of FREESCALE hardware RNG without a specific port
* Resolved some warnings with Windows builds and PBKDF disabled
* Updated the version of autoconf and automake along with fixes for some new GCC-10 warnings

###### Math Libraries
* SP: Thumb implementation that works with clang
* SP math all: sp_cmp handling of negative values
* SP C ECC: mont sub - always normalize after sub before check for add
* TFM math library prime checking, added more error checks with small stack build
* Sanity checks on 0 value with GCD math function
* fp_exptmod_ct error checking and small stack variable free on error
* Sanity check on supported digit size when calling mp_add_d in non fastmath builds
* Support for mp_dump with SP Math ALL
* WOLFSSL_SP_NO_MALLOC for both the normal SP build and small SP build now
* WOLFSSL_SP_NO_DYN_STACK added for SP small code that is not small stack build to avoid dynamic stack

###### PKCS 7/8
* wc_PKCS7_DecodeCompressedData to optionally handle a packet without content wrapping
* Added setting of content type parsed with PKCS7  wc_PKCS7_DecodeAuthEnvelopedData and wc_PKCS7_DecodeEnvelopedData
* PKCS8 code improvements and refactoring

###### Misc.
* Sanity checks on null inputs to the functions wolfSSL_X509_get_serialNumber and wolfSSL_X509_NAME_print_ex
* Added ARM CryptoCell support for importing public key with wc_ecc_import_x963_ex()
* Improved checking for possible use of key->dp == NULL cases with ECC functions
* Updated SHAKE256 to compile with NIST FIPS 202 standard and added support for OID values (thanks to strongX509)
* Improved ECC operations when using WOLFSSL_NO_MALLOC
* Added WOLFSSL_SNIFFER_FATAL_ERROR for an return value when sniffer is in a fatal state
* Allow parsing spaces in Base64_SkipNewline
* Issue callback when exceeding depth limit rather than error out with OPENSSL_EXTRA build
* Added NXP LTC RSA key generation acceleration

# wolfSSL Release 4.7.0 (February 16, 2021)
Release 4.7.0 of wolfSSL embedded TLS has bug fixes and new features including:

### New Feature Additions
* Compatibility Layer expansion SSL_get_verify_mode, X509_VERIFY_PARAM API, X509_STORE_CTX API added
* WOLFSSL_PSK_IDENTITY_ALERT macro added for enabling a subset of TLS alerts
* Function wolfSSL_CTX_NoTicketTLSv12 added to enable turning off session tickets with TLS 1.2 while keeping TLS 1.3 session tickets available
* Implement RFC 5705: Keying Material Exporters for TLS
* Added --enable-reproducible-build flag for making more deterministic library outputs to assist debugging
* Added support for S/MIME (Secure/Multipurpose Internet Mail Extensions) bundles

### Fixes
* Fix to free mutex when cert manager is free’d
* Compatibility layer EVP function to return the correct block size and type
* DTLS secure renegotiation fixes including resetting timeout and retransmit on duplicate HelloRequest
* Fix for edge case with shrink buffer and secure renegotiation
* Compile fix for type used with curve448 and PPC64
* Fixes for SP math all with PPC64 and other embedded compilers
* SP math all fix when performing montgomery reduction on one word modulus
* Fixes to SP math all to better support digit size of 8-bit
* Fix for results of edge case with SP integer square operation
* Stop non-ct mod inv from using register x29 with SP ARM64 build
* Fix edge case when generating z value of ECC with SP code
* Fixes for PKCS7 with crypto callback (devId) with RSA and RNG
* Fix for compiling builds with RSA verify and public only
* Fix for PKCS11 not properly exporting the public key due to a missing key type field
* Call certificate callback with certificate depth issues
* Fix for out-of-bounds read in TLSX_CSR_Parse()
* Fix incorrect AES-GCM tag generation in the EVP layer
* Fix for out of bounds write with SP math all enabled and an edge case of calling sp_tohex on the result of sp_mont_norm
* Fix for parameter check in sp_rand_prime to handle 0 length values
* Fix for edge case of failing malloc resulting in an out of bounds write with SHA256/SHA512 when small stack is enabled


### Improvements/Optimizations
* Added --enable-wolftpm option for easily building wolfSSL to be used with wolfTPM
* DTLS macro WOLFSSL_DTLS_RESEND_ONLY_TIMEOUT added for resending flight only after a timeout
* Update linux kernel module to use kvmalloc and kvfree
* Add user settings option to cmake build
* Added support for AES GCM session ticket encryption
* Thread protection for global RNG used by wolfSSL_RAND_bytes function calls
* Sanity check on FIPs configure flag used against the version of FIPs bundle
* --enable-aesgcm=table now is compatible with --enable-linuxkm
* Increase output buffer size that wolfSSL_RAND_bytes can handle
* Out of directory builds resolved, wolfSSL can now be built in a separate directory than the root wolfssl directory

### Vulnerabilities
* [HIGH] CVE-2021-3336: In earlier versions of wolfSSL there exists a potential man in the middle attack on TLS 1.3 clients. Malicious attackers with a privileged network position can impersonate TLS 1.3 servers and bypass authentication. Users that have applications with client side code and have TLS 1.3 turned on, should update to the latest version of wolfSSL. Users that do not have TLS 1.3 turned on, or that are server side only, are NOT affected by this report. For the code change see https://github.com/wolfSSL/wolfssl/pull/3676. Thanks to Aina Toky Rasoamanana and Olivier Levillain from Télécom SudParis for the report.
* [LOW] In the case of using custom ECC curves there is the potential for a crafted compressed ECC key that has a custom prime value to cause a hang when imported. This only affects applications that are loading in ECC keys with wolfSSL builds that have compressed ECC keys and custom ECC curves enabled.
* [LOW] With TLS 1.3 authenticated-only ciphers a section of the server hello could contain 16 bytes of uninitialized data when sent to the connected peer. This affects only a specific build of wolfSSL with TLS 1.3 early data enabled and using authenticated-only ciphers with TLS 1.3.

# wolfSSL Release 4.6.0 (December 22, 2020)
Release 4.6.0 of wolfSSL embedded TLS has bug fixes and new features including:

### New Feature Additions
###### New Build Options
* wolfSSL now enables linux kernel module support. Big news for Linux kernel module developers with crypto requirements! wolfCrypt and wolfSSL are now loadable as modules in the Linux kernel, providing the entire libwolfssl API natively to other kernel modules. For the first time on Linux, the entire TLS protocol stack can be loaded as a module, allowing fully kernel-resident TLS/DTLS endpoints with in-kernel handshaking.  (--enable-linuxkm, --enable-linuxkm-defaults, --with-linux-source) (https://www.wolfssl.com/loading-wolfssl-into-the-linux-kernel/)
* Build tests and updated instructions for use with Apple’s A12Z chipset  (https://www.wolfssl.com/preliminary-cryptographic-benchmarks-on-new-apple-a12z-bionic-platform/)
* Expansion of wolfSSL SP math implementation and addition of --enable-sp-math-all build option
* Apache httpd w/TLS 1.3 support added
* Sniffer support for TLS 1.3 and AES CCM
* Support small memory footprint build with only TLS 1.3 and PSK without code for (EC)DHE and certificates

###### New Hardware Acceleration
* Added support for NXP DCP (i.MX RT1060/1062) crypto co-processor
* Add Silicon Labs hardware acceleration using [SL SE Manager](https://docs.silabs.com/gecko-platform/latest/service/api/group-sl-se-manager)

###### New Algorithms
* RC2 ECB/CBC added for use with PKCS#12 bundles
* XChaCha and the XChaCha20-Poly1305 AEAD algorithm support added

###### Misc
* Added support for 802.11Q VLAN frames to sniffer
* Added OCSP function wolfSSL_get_ocsp_producedDate
* Added API to set CPU ID flags cpuid_select_flags, cpuid_set_flag, cpuid_clear_flag
* New DTLS/TLS non-blocking Secure Renegotiation example added to server.c and client.c

### Fixes
###### Math Library
* Fix mp_to_unsigned_bin_len out of bounds read with buffers longer than maximum MP
* Fix for fp_read_radix_16 out of bounds read
* Fix to add wrapper for new timing resistant wc_ecc_mulmod_ex2 function version in HW ECC acceleration
* Handle an edge case with RSA-PSS encoding message to hash

###### Compatibility Layer Fixes
* Fix for setting serial number wolfSSL_X509_set_serialNumber
* Fix for setting ASN1 time not before / not after with WOLFSSL_X509
* Fix for order of components in issuer name when using X509_sign
* Fix for compatibility layer API DH_compute_key
* EVP fix incorrect block size for GCM and buffer up AAD for encryption/decryption
* EVP fix for AES-XTS key length return value and fix for string compare calls
* Fix for mutex freeing during RNG failure case with EVP_KEY creation
* Non blocking use with compatibility layer BIOs in TLS connections

###### Build Configuration
* Fix for custom build with WOLFSSL_USER_MALLOC defined
* ED448 compiler warning on Intel 32bit systems
* CURVE448_SMALL build fix for 32bit systems with Curve448
* Fix to build SP math with IAR
* CMake fix to only set ranlib arguments for Mac, and for stray typo of , -> ;
* Build with --enable-wpas=small fix
* Fix for building fips ready using openssl extra
* Fixes for building with Microchip (min/max and undef SHA_BLOCK_SIZE)
* FIx for NO_FILESYSTEM build on Windows
* Fixed SHA256 support for IMX-RT1060
* Fix for ECC key gen with NO_TFM_64BIT

###### Sniffer
* Fixes for sniffer when using static ECC keys. Adds back TLS v1.2 static ECC key fallback detection and fixes new ECC RNG requirement for timing resistance
* Fix for sniffer with SNI enabled to properly handle WOLFSSL_SUCCESS error code in ProcessClientHello
* Fix for sniffer using HAVE_MAX_FRAGMENT in "certificate" type message
* Fix build error with unused "ret" when building with WOLFSSL_SNIFFER_WATCH.
* Fix to not treat cert/key not found as error in myWatchCb and WOLFSSL_SNIFFER_WATCH.
* Sniffer fixes for handling TCP `out-of-range sequence number`
* Fixes SSLv3 use of ECDH in sniffer

###### PKCS
* PKCS#11 fix to generate ECC key for decrypt/sign or derive
* Fix for resetting internal variables when parsing a malformed PKCS#7 bundle with PKCS7_VerifySignedData()
* Verify the extracted public key in wc_PKCS7_InitWithCert
* Fix for internal buffer size when using decompression with PKCS#7

###### Misc
* Pin the C# verify callback function to keep from garbage collection
* DH fixes for when public key is owned and free’d after a handshake
* Fix for TLS 1.3 early data packets
* Fix for STM32 issue with some Cube HAL versions and STM32 example timeout
* Fix mmCAU and LTC hardware mutex locking to prevent double lock
* Fix potential race condition with CRL monitor
* Fix for possible malformed encrypted key with 3DES causing negative length
* AES-CTR performance fixed with AES-NI

### Improvements/Optimizations
##### SP and Math
* mp_radix_size adjustment for leading 0
* Resolve implicit cast warnings with SP build
* Change mp_sqr to return an error if the result won't fit into the fixed length dp
* ARM64 assembly with clang improvements, clang doesn't always handle use of x29 (FP or Frame Pointer) in inline assembly code correctly - reworked sp_2048_sqr_8 to not use x29
* SP mod exp changed to support exponents of different lengths
* TFM div: fix initial value of size in q so clamping doesn't OOB read
* Numerous stack depth improvements with --enable-smallstack
* Improve cache resistance with Base64 operations

###### TLS 1.3
* TLS 1.3 wolfSSL_peek want read return addition
* TLS 1.3: Fix P-521 algorithm matching

###### PKCS
* Improvements and refactoring to PKCS#11 key look up
* PKCS #11 changes for signing and loading RSA public key from private
* check PKCS#7 SignedData private key is valid before using it
* check PKCS#7 VerifySignedData content length against total bundle size to avoid large malloc

###### Compatibility Layer
* EVP add block size for more ciphers in wolfSSL_EVP_CIPHER_block_size()
* Return long names instead of short names in wolfSSL_OBJ_obj2txt()
* Add additional OpenSSL compatibility functions to update the version of Apache httpd supported
* add "CCM8" variants to cipher_names "CCM-8" ciphers, for OpenSSL compat

###### Builds
* Cortex-M SP ASM support for IAR 6.70
* STM Cube pack support (IDE/STM32Cube)
* Build option --enable-aesgcm=4bit added for AES-GCM GMULT using 4 bit table
* Xilinx IDE updates to allow XTIME override for Xilinx, spelling fixes in Xilinx README.md, and add Xilinx SDK printf support
* Added ED448 to the "all" options and ED448 check key null argument sanity check
* Added ARC4, 3DES, nullcipher, BLAKE2, BLAKE2s, XChaCha, MD2, and MD4 to the “all” options
* Added an --enable-all-crypto option, to enable only the wolfCrypt features of --enable-all, combinable with --enable-cryptonly
* Added the ability to selectively remove features from --enable-all and --enable-all-crypto using specific --disable-<feature> options
* Use Intel intrinsics with Windows for RDSEED and RDRAND (thanks to dr-m from MariaDB)
* Add option to build with WOLFSSL_NO_CLIENT_AUTH
* Updated build requirements for wolfSSH use to be less restrictive
* lighttpd support update for v1.4.56
* Added batch file to copy files to ESP-IDF folders and resolved warnings when using v4.0 ESP-IDF
* Added --enable-stacksize=verbose, showing at a glance the stack high water mark for each subtest in testwolfcrypt

###### ECC
* Performance increase for ECC verify only, using non constant time SP modinv
* During ECC verify add validation of r and s before any use
* Always use safe add and dbl with ECC
* Timing resistant scalar multiplication updated with use of Joye double-add ladder
* Update mp_jacobi function to reduce stack and increase performance for base ECC build
* Reduce heap memory use with wc_EccPrivateKeyDecode, Improvement to ECC wc_ecc_sig_to_rs and wc_ecc_rs_raw_to_sig to reduce memory use (avoid the mp_int)
* Improve StoreECC_DSA_Sig bounds checking

###### OCSP
* OCSP improvement to handle extensions in singleResponse
* support for OCSP request/response for multiple certificates
* OCSP Must Staple option added to require OCSP stapling response
* Add support for id-pkix-ocsp-nocheck extension

###### Misc
* Additional code coverage added for ECC and RSA, PKCS#7, 3DES, EVP and Blake2b operations
* DTLS MTU: check MTU on write
* Refactor hash sig selection and add the macros WOLFSSL_STRONGEST_HASH_SIG (picks the strongest hash) and WOLFSSL_ECDSA_MATCH_HASH (will pick the hash to match the ECC curve)
* Strict certificate version allowed from client, TLS 1.2 / 1.3 can not accept client certificates lower than version 3
* wolfSSL_get_ciphers_compat(), skip the fake indicator ciphers like the renegotiation indication and the quantum-safe hybrid
* When parsing session ticket, check TLS version to see whether they are version compatible
* Additional sanity check for invalid ASN1 padding on integer type
* Adding in ChaCha20 streaming feature with Mac and Intel assembly build
* Sniffer build with --enable-oldtls option on

# wolfSSL Release 4.5.0 (August 19, 2020)

If you have questions about this release, feel free to contact us on our
info@ address.

Release 4.5.0 of wolfSSL embedded TLS has bug fixes and new features including:

## New Feature Additions

* Added Xilinx Vitis 2019.2 example and README updates
* TLS v1.3 is now enabled by default
* Building FIPS 140-2 code and test on Solaris
* Secure renegotiation with DTLS 1.2
* Update RSA calls for hardware acceleration with Xilsecure
* Additional OpenSSL compatibility layer functions added
* Cypress PSoC6 wolfCrypt driver added
* Added STM32CubeIDE support
* Added certificate parsing and inspection to C# wrapper layer
* TLS v1.3 sniffer support added
* TSIP v1.09 for target board GR-ROSE support added
* Added support for the "X72N Envision Kit" evaluation board
* Support for ECC nonblocking using the configure options
  "--enable-ecc=nonblock --enable-sp=yes,nonblock CFLAGS=-DWOLFSSL_PUBLIC_MP"
* Added wc_curve25519_make_pub function to generate a public key given the
  private one

## Fixes

* PIC32MZ hardware cache and large hashes fix
* AES-GCM use with EVP layer in compatibility layer code
* Fix for RSA_LOW_MEM with ARM build of SP code
* Sanity check on tag length with AES-CCM to conform with RFC 3610
* Fixes for 32 and 64 bit software implementations of SP code when
  WOLFSSL_SP_CACHE_RESISTANT is defined
* GCC warning fixes for GCC 9 and later
* Sanity check on HKDF expand length to conform with RFC 5869
* Fixes for STM32 CubeMX HAL with AES-GCM
* Fixed point cache look up table (LUT) implementation fixes
* Fix for ARM 32bit SP code when calling div word
* Fix for potential out of bounds read when parsing CRLs
* Fix for potential out of bounds read with RSA unpadding
* AES-CCM optimized counter fix
* Updates to Xcode projects for new files and features
* Fix for adding CRL’s to a WOLFSSL_X509_STORE structure
* FIPSv2 build with opensslall build fixes
* Fixes for CryptoCell use with ECC and signature wrappers
* Fix for mod calculation with SP code dealing with 3072 bit keys
* Fix for handling certificates with multiple OU’s in name
* Fix for SP math implementation of sp_add_d and add a sanity check on
  rshb range
* Fix for sanity check on padding with DES3 conversion of PEM to DER
* Sanity check for potential out of bounds read with fp_read_radix_16
* Additional checking of ECC scalars.
* Fixing the FIPS Ready build w.r.t. ecc.c.
* When processing certificate names with OpenSSL compatibility layer
  enabled, unknown name item types were getting handled as having NID 0,
  and failing. Added a couple more items to what is handled correctly,
  and ignoring anything that is an unknown type.

## Improvements/Optimizations

* TLS 1.3 certificate verify update to handle 8192 bit RSA keys
* wpa_supplicant support with reduced code size option
* TLS 1.3 alerts encrypted when possible
* Many minor coverity fixes added
* Error checking when parsing PKCS12 DER
* IAR warning in test.c resolved
* ATECC608A improvements for use with Harmony 3 and PIC32 MZ
* Support for AES-GCM and wc_SignatureVerifyHash with static memory and no
  malloc’s
* Enable SNI by default with JNI/JSSE builds
* NetBSD GCC compiler warnings resolved
* Additional test cases and code coverage added including curve25519 and
  curve448 tests
* Option for user defined mutexes with WOLFSSL_USER_MUTEX
* Sniffer API’s for loading buffer directly
* Fixes and improvements from going through the DO-178 process were added
* Doxygen updates and fixes for auto documentation generation
* Changed the configure option for FIPS Ready builds to be
  `--enable-fips=ready`.

## This release of wolfSSL includes fixes for 6 security vulnerabilities.

wolfSSL version 4.5.0 contains 6 vulnerability fixes: 2 fixes for TLS 1.3,
2 side channel attack mitigations, 1 fix for a potential private key leak
in a specific use case, 1 fix for DTLS.

* In earlier versions of wolfSSL there exists a potential man in the middle
  attack on TLS 1.3 clients. Malicious attackers with a privileged network
  position can impersonate TLS 1.3 servers and bypass authentication. Users
  that have applications with client side code and have TLS 1.3 turned on,
  should update to the latest version of wolfSSL. Users that do not have
  TLS 1.3 turned on, or that are server side only, are NOT affected by this
  report. Thanks to Gerald Doussot from NCC group for the report.
* Denial of service attack on TLS 1.3 servers from repetitively sending
  ChangeCipherSpecs messages. This denial of service results from the
  relatively low effort of sending a ChangeCipherSpecs message versus the
  effort of the server to process that message. Users with TLS 1.3 servers are
  recommended to update to the most recent version of wolfSSL which limits the
  number of TLS 1.3 ChangeCipherSpecs that can be received in order to avoid
  this DoS attack. CVE-2020-12457 was reserved for the report. Thanks to
  Lenny Wang of Tencent Security Xuanwu LAB.
* Potential cache timing attacks on public key operations in builds that are
  not using SP (single precision). Users that have a system where malicious
  agents could execute code on the system, are not using the SP build with
  wolfSSL, and are doing private key operations on the system (such as signing
  with a private key) are recommended to regenerate private keys and update to
  the most recent version of wolfSSL. CVE-2020-15309 is reserved for this
  issue. Thanks to Ida Bruhns from Universität zu Lübeck and Samira Briongos
  from NEC Laboratories Europe for the report.
* When using SGX with EC scalar multiplication the possibility of side-channel
  attacks are present. To mitigate the risk of side channel attacks wolfSSL’s
  single precision EC operations should be used instead. Release 4.5.0 turns
  this on be default now with SGX builds and in previous versions of wolfSSL
  this can be turned on by using the WOLFSSL_SP macros. Thank you to
  Alejandro Cabrera Aldaya, Cesar Pereida García and Billy Bob Brumley from
  the Network and Information Security Group (NISEC) at Tampere University for
  the report.
* Leak of private key in the case that PEM format private keys are bundled in
  with PEM certificates into a single file. This is due to the
  misclassification of certificate type versus private key type when parsing
  through the PEM file. To be affected, wolfSSL would need to have been built
  with OPENSSL_EXTRA (--enable-opensslextra). Some build variants such as
  --enable-all and --enable-opensslall also turn on this code path, checking
  wolfssl/options.h for OPENSSL_EXTRA will show if the macro was used with the
  build. If having built with the opensslextra enable option and having placed
  PEM certificates with PEM private keys in the same file when loading up the
  certificate file, then we recommend updating wolfSSL for this use case and
  also recommend regenerating any private keys in the file.
* During the handshake, clear application_data messages in epoch 0 are
  processed and returned to the application. Fixed by dropping received
  application_data messages in epoch 0. Thank you to Paul Fiterau of Uppsala
  University and Robert Merget of Ruhr-University Bochum for the report.

For additional vulnerability information visit the vulnerability page at
https://www.wolfssl.com/docs/security-vulnerabilities/

See INSTALL file for build instructions.
More info can be found on-line at https://wolfssl.com/wolfSSL/Docs.html


# wolfSSL Release 4.4.0 (04/22/2020)

If you have questions about this release, then feel free to contact us on our
info@ address.

Release 4.4.0 of wolfSSL embedded TLS has bug fixes and new features including:

## New Feature Additions

* Hexagon support.
* DSP builds to offload ECC verify operations.
* Certificate Manager callback support.
* New APIs for running updates to ChaCha20/Poly1305 AEAD.
* Support for use with Apache.
* Add support for IBM s390x.
* PKCS8 support for ED25519.
* OpenVPN support.
* Add P384 curve support to SP.
* Add BIO and EVP API.
* Add AES-OFB mode.
* Add AES-CFB mode.
* Add Curve448, X448, and Ed448.
* Add Renesas Synergy S7G2 build and hardware acceleration.

## Fixes

* Fix for RSA public encrypt / private sign with RSA key sizes over 2048-bit.
* Correct misspellings.
* Secure renegotiation fix.
* Fix memory leak when using ATECC and non-SECP256R1 curves for sign, verify,
  or shared secret.
* Fix for K64 MMCAU with `WOLFSSL_SMALL_STACK_CACHE`.
* Fix the RSA verify only build.
* Fix in SP C implementation for small stack.
* Fix using the auth key id extension is set, hash might not be present.
* Fix when flattening certificate structure to include the subject alt names.
* Fixes for building with ECC sign/verify only.
* Fix for ECC and no cache resistance.
* Fix memory leak in DSA.
* Fix build on minGW.
* Fix `PemToDer()` call in `ProcessBuffer()` to set more than ECC.
* Fix for using RSA without SHA-512.
* Add some close tags to the echoserver HTTP example output.
* Miscellaneous fixes and updates for static analysis reports.
* Fixes for time structure support.
* Fixes for VxWorks support.
* Fixes for Async crypto support.
* Fix cache resist compile to work with SP C code.
* Fixes for Curve25519 x64 asm.
* Fix for SP x64 div.
* Fix for DTLS edge case where CCS and Finished come out of order and the
  retransmit pool gets flushed.
* Fix for infinite loop in SHA-1 with small inputs. Thanks to Peter W.
* Fix for FIPS Hmac where `wc_HmacInit()` isn't used. `wc_HmacSetKey()` needs
  to initialize the Hmac structure. Type is set to NONE, and checked against
  NONE, not 0.
* Fixes for SP RSA private operations.
* Fixes for Xilinx SDK and Zynq UltraScale+ MPSoC
* Fix leak when building with HAVE_AESGCM and NO_AES_DECRYPT. Thanks G.G.
* Fixes for building ECC without ASN.
* Fix for async TLSv1.3 issues.
* Fix `wc_KeyPemToDer()` with PKCS1 and empty key.
* Omit `-fomit-frame-pointer` from CFLAGS in configure.ac.

## Improvements/Optimizations

* Qt 5.12 and 5.13 support.
* Added more digest types to Cryptocell RSA sign/verify.
* Some memory usage improvements.
* Speed improvements for mp_rand.
* Improvements to CRL and OCSP support.
* Refactor Poly1305 AEAD/MAC to reduce duplicate code.
* Add blinding to RSA key gen.
* Improvements to blinding.
* Improvement and expansion of OpenSSL Compatibility Layer.
* Improvements to ChaCha20.
* Improvements to X.509 processing.
* Improvements to ECC support.
* Improvement in detecting 64-bit support.
* Refactor to combine duplicate ECC parameter parsing code.
* Improve keyFormat to be set by algId and let later key parsing produce fail.
* Add test cases for 3072-bit and 4096-bit RSA keys.
* Improve signature wrapper and DH test cases.
* Improvements to the configure.ac script.
* Added constant time RSA q modinv p.
* Improve performance of SP Intel 64-bit asm.
* Added a few more functions to the ABI list.
* Improve TLS bidirectional shutdown behavior.
* OpenSSH 8.1 support.
* Improve performance of RSA/DH operations on x64.
* Add support for PKCS7/CMS Enveloped data with fragmented encrypted content.
* Example linker description for FIPS builds to enforce object ordering.
* C# wrapper improvements. Added TLS client example and TLSv1.3 methods.
* Allow setting MTU in DTLS.
* Improve PKCS12 create for outputting encrypted bundles.
* Constant time EC map to affine for private operations.
* Improve performance of RSA public key ops with TFM.
* Smaller table version of AES encrypt/decrypt.
* Support IAR with position independent code (ROPI).
* Improve speed of AArch64 assembly.
* Support AES-CTR on esp32.
* Add a no malloc option for small SP math.

## This release of wolfSSL includes fixes for 2 security vulnerabilities.

* For fast math, use a constant time modular inverse when mapping to affine
  when operation involves a private key - keygen, calc shared secret, sign.
  Thank you to Alejandro Cabrera Aldaya, Cesar Pereida García and
  Billy Bob Brumley from the Network and Information Security Group (NISEC)
  at Tampere University for the report.

* Change constant time and cache resistant ECC mulmod. Ensure points being
  operated on change to make constant time. Thank you to Pietro Borrello at
  Sapienza University of Rome.

For additional vulnerability information visit the vulnerability page at
https://www.wolfssl.com/docs/security-vulnerabilities/

See INSTALL file for build instructions.
More info can be found on-line at https://wolfssl.com/wolfSSL/Docs.html



# wolfSSL Release 4.3.0 (12/20/2019)

If you have questions about this release, then feel free to contact us on our info@ address.

Release 4.3.0 of wolfSSL embedded TLS has bug fixes and new features including:

##### New Feature Additions
* Add --enable-libwebsockets option for support of libwebsockets build
* Updated support for NGINX 1.15.0 and added support for 1.16.1
* Add wc_scrypt_ex API which can take in iterations rather than cost
* Updates to RSA-PSS salt lengths. Macro WOLFSSL_PSS_SALT_LEN_DISCOVER allows for discovering the salt length. Passing RSA_PSS_SALT_LEN_DISCOVER value into wc_RsaPSS_Verify_ex attempts to discover salt length and can use larger salt lengths
* Additional OpenSSL compatibility API wolfSSL_CertManagerGetCerts and wolfSSL_X509_STORE_GetCerts for retrieving certificates
* Add support for 4096-bit RSA/DH operations to SP (single precision) build
* Update support for Google WebRTC to branch m79
* Adds new FREESCALE_MQX_5_0 macro for MQX 5.0 support
* Adds a CMS/PKCS#7 callback for signing SignedData raw digests enabled with macro HAVE_PKCS7_RSA_RAW_SIGN_CALLBACK and call to function wc_PKCS7_SetRsaSignRawDigestCb
* Add --disable-errorqueue feature to disable adding debug nodes to queue with --enable-opensslextra build
* After defining WOLFSSL_SHUTDOWNONCE macro the function wolfSSL_shutdown will return a specific error code of SSL_SHUTDOWN_ALREADY_DONE_E, to indicate to the application that the shutdown has already occurred
* Add AES-CCM decryption to benchmarking app bundled with wolfSSL


##### Fixes
* Fixes IAR warnings with IAR-EWARM 7.50.2
* Alignment fixes for mmCAU with AES and hashing algorithms
* Fix check for plaintext length when using Encrypt-Then-MAC
* Fix for unit tests with NGINX and debug mode
* Fix for macro names in test cases (WOLFSSL_PUBLIC_MP) and pkcs7.c (HAVE_AESCCM)
* Fix for Apache want read case with BIO retry flag
* Fix for PKCS7 streaming mode that would error rather than verify bundle
* Fix for freeing mutex for X509 and wolfSSL_EVP_PKEY_free, applies to OPENSSL_EXTRA / --enable-opensslextra builds
* Fix for encrypt then MAC when re-handshaking, encrypted handshakes change over to ETM now
* Fix for curve25519 assembly optimizations with GCC + AVX2
* Fix to hang onto certificate for retrieval if using secure renegotiation and session resumption
* Fixes case where the heap hint is created before WOLFSSL_CTX, when calling wc_LoadStaticMemory instead of wolfSSL_CTX_load_static_memory
* Fix for setting correct return value in PKCS12 parse error case
* Reset certificate extension policy count
* Fix for memcpy with TLS I/O buffers when using staticmemory pools and loading memory as WOLFMEM_IO_POOL_FIXED
* Fixes and updates for STM32 port, including additional mutex protection, AES-GCM decrypt auth tag, AES-CTR mode with CubeMX, update to OpenSTM32 project
* Fix for EVP CipherUpdate decrypt and add a test case
* DTLS fixes including; some DTLS sequence number issues in general where the sequence was incremented twice for each record and some offset values in the DTLS window checking
* Fix sp_add to handle carries properly (--enable-sp-math build)
* Additional sanity check on OCSP response decoder
* Fix for vasprintf with Solaris and AIX builds
* Fix for missing variable declaration with --enable-scep --with-libz build
* Fix for certificate date check with async build
* Sanity check on “out” length with Base64_Decode added
* Decode X.509 name - check input length for jurisdiction
* Additional sanity check on variable out index with DecodePolicyOID
* Fix for PKCS#12 PBKDF buffer size for buffer overflow
* TLS supported curve extension check curve name is in range before checking for disabled
* Sanity check for non TLS 1.3 cipher suite with TLS 1.3 connection
* Poly1305 AVX2 assembly optimization fix for carry with large input values
* Fixes for coverity report including null termination of test case strings and initialization of PKCS7 variables
* Fix for API visibility of wc_ed25519_check_key which resolves a wolfcrypt-py install issue
* Sanity check on max ALPN length accepted
* Additional sanity check when parsing CRL’s for copying the structure, fix for bounds checking
* Additional checks on error string length for debug mode and check for null termination
* ProcessPeerCerts allocating memory for exts with OPENSSL_EXTRA properly
* Clear the top bit when generating a serial number
* Sanity check that ASN date characters are valid ASCII characters
* Fix to add deterministic ECDSA and fix corner cases for add point.
* When getting the DH public key, initialize the P, G, and Pub pointers to NULL, then set that we own the DH parameters flag. This allows FreeSSL to correctly clean up the DH key.

##### Improvements/Optimizations
* Added configure error report with using invalid build of --enable-opensslextra and --enable-opensslcoexist together
* Update PKCS11 for determining key type given the private key type
* Update DoVerifyCallback to check verify param hostName and ipasc (--enable-opensslextra builds)
* additional null sanity checks on input arguments with QSH and Cryptocell builds
* Additional checks on RSA key added to the function wc_CheckRsaKey
* Updates for EBSNET support, including fseek, revised macros in settings.h, and realloc support
* MISRA-C updates for SP math code
* Update to allow compiling for pwdbased/PBKDF2 with having NO_ASN defined
* Modify KeyShare and PreSharedKey TLS 1.3 extension linked list advancement to be easier for compilers to handle
* Optimization to parsing certificate extension name strings
* Adjustment to example server -x runtime behavior when encountering an unrecoverable error case
* Remove Blake2b support from HMAC
* Adds new hash wrapper init wc_HashInit_ex and Adds new PBKDF2 API wc_PBKDF2_ex for using heap hints for custom memory pools
* Adding script to cleanup generated test files,  scripts/cleanup_testfiles.sh
* Support 20-byte serial numbers and disallow 0
* sp_div improved to handle when a has less digits than d (--enable-sp-math build)
* When decoding a policy OID and turning it into a human readable string use snprintf()
* set the IV length of EVP AES GCM to 96-bits by default
* Allow adding CAs for root CA's over the wire that do not have the extended key usage cert_sign set
* Added logging messages for SendAlert call and update to send alert after verify certificate callback
* updates for synchronous OCTEON support in the Sniffer
* Rework BER to DER functions to not be recursive
* Updates to find CRL by AuthKeyId
* Add a check for subject name hash after matching AKID
* Enhancement to mp_invmod/fp_exptmod/sp_exptmod to handle more inputs
* Remove requirement for macro NO_SKID when CRL use is enabled
* Improvements on XFTELL return code and MAX_WOLFSSL_FILE_SIZE checking
* When checking if value is prime return NO in the case of the value 1
* Improve Cortex-M RSA/DH assembly code performance
* Additional sanity checks on arrays and buffers with OCSP


##### This release of wolfSSL includes a fix for 6 security vulnerabilities.


A fix for having an additional sanity check when parsing certificate domain names was added. This fix checks that the domain name location index is not past the maximum value before setting it. The reported issue affects users that are parsing certificates and have --enable-opensslextra (macro OPENSSL_EXTRA), or build options that turn this on such as --enable-all, when building wolfSSL. The CVE associated with the fix is CVE-2019-18840.

Fix to set a limit on the maximum size of DTLS handshake messages. By default the RFC allows for handshake message sizes of up to 2^24-1 bytes long but in typical field use cases the handshake messages are not this large. Setting a maximum size limit on the handshake message helps avoid a potential DoS attack due to memory being malloc’d. The new default max size is set to handle a certificate chain length of approximately 9, 2048 bit RSA certificates. This only effects builds that have DTLS turned on and have applications that are using DTLS.

Fix for a potential hang when ECC caching is enabled (off by default) and --enable-fastmath is used. ECC caching is off by default and is turned on in builds that are using --enable-all or --enable-fpecc. This issue does not affect builds that are using the macro WOLFSSL_VALIDATE_ECC_IMPORT which turns on validating all ECC keys that are imported. To fix this potential hang case a sanity check on the input values to the internal invmod function was added.


To fix a potential fault injection attack on a wrapper function for wolfCrypt RSA signature generations an additional sanity check verifying the signature after it’s creation was added. This check is already done automatically in current versions of wolfSSL with TLS connections (internal function call of VerifyRsaSign during TLS state machine). The report only affects users making calls to the wolfCrypt function wc_SignatureGenerateHash and does not affect current TLS use cases. Thanks to Daniel Moghimi (@danielmgmi) from Worcester Polytechnic Institute for the report.


Blinding was added for DSA signing operations. The DSA signing uses the BEEA algorithm during modular inversion of the nonce which can potentially leak the nonce through side channels such as cache and power fluctuations. The fix of adding in blinding makes the DSA signing operation more resistant to side channel attacks. Users who have turned on DSA (disabled by default) and are performing signing operations should update. Note that DSA is not used in any TLS connections. Thanks to Daniel Moghimi (@danielmgmi) from Worcester Polytechnic Institute for the report.


Fix to add additional side channel cache attack resistance to the internal ECC function wc_ecc_mulmod_ex. This function by default is used with ECDSA signing operations. Users should update if performing ECDSA singing operations (server side ECC TLS connections, mutual authentication on client side) or calling wolfCrypt ECC sign functions and have the potential for outside users to perform sophisticated monitoring of the cache.Thanks to Daniel Moghimi (@danielmgmi) from Worcester Polytechnic Institute for the report.


For additional vulnerability information visit the vulnerability page at https://www.wolfssl.com/docs/security-vulnerabilities/

See INSTALL file for build instructions.
More info can be found on-line at http://wolfssl.com/wolfSSL/Docs.html


# wolfSSL Release 4.2.0 (10/22/2019)

If you have questions about this release, then feel free to contact us on our info@ address.

Release 4.2.0 of wolfSSL embedded TLS has bug fixes and new features including:


##### New Feature Additions
* Over 198 OpenSSL compatibility API’s added
* Apache port added for compiling with wolfSSL using --enable-apachehttpd
* Port for using wolfSSL with OpenVSwitch
* Port for Renesas TSIP added
* Visual Studio Solution for Azure Sphere Devices (MT3620 and MT3620-mini) added to the directory IDE/VS-AZURE-SPHERE
* Addition of Coldfire MCF5441X NetBurner example to the directory IDE/M68K/
* Added support for prime checking to SP math build
* Addition of DYNAMIC_TYPE_BIGINT type for tracking mp_int allocations
* Addition of wc_ecc_get_curve_params API for getting ecc_set_type params for a curve
* Adding in TLS_SHA256_SHA256 and TLS_SHA384_SHA384 TLS1.3 cipher suites (null ciphers)
* Added in PKCS7 decryption callbacks for CMS operations
* Added handling for optional ECC parameters with PKCS7 KARI
* Addition to configure.ac for FIPS wolfRand builds
* Adding the flag WOLFSSL_LOAD_FLAG_DATE_ERR_OKAY for ignoring certificate date checks with the functions wolfSSL_CTX_load_verify_buffer_ex and wolfSSL_CTX_load_verify_locations_ex
* Support for PKCS8 keys added to the function wolfSSL_CTX_use_PrivateKey_buffer
* Support for KECCAK hashing. Build with macro WOLFSSL_HASH_FLAGS and call wc_Sha3_SetFlags(&sha, WC_HASH_SHA3_KECCAK256) before the first SHA3 update
* Addition of setting secure renegotiation at CTX level
* Addition of KDS (NXP Kinetis Design Studio) example project to directory IDE/KDS/
* Support for Encrypt-Then-MAC to TLS 1.2 and below
* Added a new build option for a TITAN session cache that can hold just over 2 million session entries (--enable-titancache)
* Synchronous Quick Assist Support for Sniffer
* Added Support for SiFive HiFive Unleashed board
* Support for Google WebRTC added in to compatibility layer build
* Additional Sniffer features; IPv6 sniffer support, Fragment chain input, Data store callback, Various statistics tweaks and other Sniffer fixes


##### Fixes
* Addition of internal content digest checks for PKCS7 SignedData message types that also have signed attributes present. Users could previously check the content digest against the messageDigest attribute, but this adds a default internal check. It’s advised that if users are not doing their own checks that they update wolfSSL version.
* A fix for BIO and base64 encoding/decoding
* A fix for memory management of CTX / WOLFSSL_METHOD pointer with staticmemory build
* A fix for detection of AES-NI support to read bit 25 from ECX
* A fix a DTLS handshake message retransmit bug corner case
* Additional fixes to coding style and uninitialized values using cppcheck tool
* Fixes for failing IAR builds, thanks to Joseph C. for the report
* Fixes for ARMv8 Poly1305 inline assembly code to compile with clang 3.5
* Fixes for esp-idf build warnings
* A fix for XSNPRINTF with mingw32 builds
* Fixes for strncpy warnings when compiling with newer versions of GCC
* A fix for using IV of all 0’s as default case with AES-NI when no IV passed in
* Fixes for types with 16 bit systems, thanks to Ralf Schlatterbeck
* Fixes for build with devcrypto/afalg and aesccm, thanks to GitHub user cotequeiroz for the report
* Fixes for addressing handling of pathLen constraint when parsing certificate chains
* A DTLS fix for alert packet injection at end of handshake
* Fixes for Poly1305 AArch64 assembly code
* A fix for memory management in error cases when adding a CA, this resolves a coverity report
* A fix for SP math for precomputation tables to not include infinity field
* Fixes for checks on defines with AVX2 and Poly1305 build
* Fixes for CubeMX HAL v1.15 with STM32F7
* A fix for TLS 1.3 to always send Key Share extension
* A fix for a potential buffer over read in TLS 1.3 DoTls13SupportedVersions, thanks to Artem for the report


##### Improvements/Optimizations
* Optimization to SP math, changing variables to const where possible. Thanks to Yair Poleg (yair.poleg@ayyeka.com) of Ayyeka for proposing static declaration of global constant variables in SP code
* Additional fuzz testing and fixes for TLS 1.3 use, including additional TLS 1.3 alert messages (PR#2440 for more information)
* Additional sanity check that ciphersuite from client hello is used in server hello response (check can be removed with the macro WOLFSSL_NO_STRICT_CIPHER_SUITE)
* Improved MMCAU performance: SHA-1 by 35%, SHA-256 by 20% and MD5 by 78%
* By default, disallow SHA-2 cipher suites from being used in TLS 1.0 and 1.1 handshakes (can be ignored with macro WOLFSSL_OLDTLS_SHA2_CIPHERSUITES)
* Optimization of export session buffer size with enable option --enable-sessionexport=nopeer
* Spelling fixes in comments and some cast warnings resolved
* Updates to abstract atoi to XATOI when used, this allows for better portability when making calls to the system function atoi for converting strings to integers
* Improvements to the STSAFE-A100 error code handling, providing better debug information
* Adding a sanity check on TLS 1.3 ticket encrypt callback
* Specialized implementations of mod exp when base is 2



##### This release of wolfSSL includes a fix for 5 security vulnerabilities.

Fix for sanity check on reading TLS 1.3 pre-shared key extension. This fixes a potential for an invalid read when TLS 1.3 and pre-shared keys is enabled. Users without TLS 1.3 enabled are unaffected. Users with TLS 1.3 enabled and HAVE_SESSION_TICKET defined or NO_PSK not defined should update wolfSSL versions. Thanks to Robert Hoerr for the report.

Fix for potential program hang when ocspstapling2 is enabled. This is a moderate level fix that affects users who have ocspstapling2 enabled(off by default) and are on the server side. In parsing a CSR2 (Certificate Status Request v2 ) on the server side, there was the potential for a malformed extension to cause a program hang. Thanks to Robert Hoerr for the report.

Two moderate level fixes involving an ASN.1 over read by one byte. CVE-2019-15651 is for a fix that is due to a potential one byte over read when decoding certificate extensions. CVE-2019-16748 is for a fix on a potential one byte overread with checking certificate signatures. This affects builds that do certificate parsing and do not have the macro NO_SKID defined.Thanks to Yan Jia and the researcher team  from Institute of Software, Chinese Academy of Sciences for the report.

High level fix for DSA operations involving an attack on recovering DSA private keys. This fix affects users that have DSA enabled and are performing DSA operations (off by default). All users that have DSA enabled and are using DSA keys are advised to regenerate DSA keys and update wolfSSL version. ECDSA is NOT affected by this and TLS code is NOT affected by this issue. Thanks to Ján Jančár for the report.


For additional vulnerability information visit the vulnerability page at https://www.wolfssl.com/docs/security-vulnerabilities/

See INSTALL file for build instructions.
More info can be found on-line at http://wolfssl.com/wolfSSL/Docs.html



# wolfSSL Release 4.1.0 (07/22/2019)

Release 4.1.0 of wolfSSL embedded TLS has bug fixes and new features including:

* A fix for the check on return value when verifying PKCS7 bundle signatures, all users with applications using the function wc_PKCS7_VerifySignedData should update
* Adding the function wc_PKCS7_GetSignerSID for PKCS7 firmware bundles as a getter function for the signers SID
* PKCS7 callback functions for unwrapping of CEK and for decryption
* Adding the error value PKCS7_SIGNEEDS_CHECK when no certificates are available in a PKCS7 bundle to verify the signature
* TLS 1.3 fixes including if major version is TLS Draft then it is now ignored and if version negotiation occurs but none were matched then an alert is now sent
* Addition of the WOLFSSL_PSK_ONE_ID macro for indicating that only one identity in TLS 1.3 PSK is available and will be cached
* Adding sanity checks on length of PSK identity from a TLS 1.3 pre-shared key extension
* Additional sanity checks and alert messages added for TLS 1.3
* Adding XTIME_MS macro to simplify the tls13.c time requirement
* Improvements and refactoring of code related to parsing and creating TLS 1.3 client hello packets
* TLS 1.3 version renegotiation now happens before interpreting ClientHello message
* Chacha20 algorithm optimizations on the ARM architecture for performance increase
* Poly1305 algorithm performance enhancements for the ARM architecture using the SIMD NEON extension
* Curve25519 and Ed25519 optimized for ARM architecture for performance increase
* SHA-512/384 optimizations for performance with ARM architecture using the SIMD NEON extension
* Sniffer updates including adding support for the null cipher and static ECDH key exchange and new SSLWatchCb callback
* Cipher suite TLS_RSA_WITH_NULL_MD5 for use with the sniffer (off by default)
* Sniffer statistic print outs with the macro WOLFSSL_SNIFFER_STATS defined
* A fix for wolfSSL_DH_generate_key when WOLFSSL_SMALL_STACK is defined
* wolfSSL_BN_Init implementation for opensslextra builds
* Updates to the function wolfSSL_i2d_RSAPrivateKey and additional automated tests
* Fixes for EVP_CipherFinal edge cases to match behavior desired
* Check for appropriate private vs public flag with ECC key decode in wolfSSL_EC_KEY_LoadDer_ex, thanks to Eric Miller for the report
* Implementation of the function wolfSSL_PEM_write_DHparams
* wolfSSL_RAND_seed is called in wolfSSL_Init now when opensslextra is enabled
* CryptoCell-310 support on nRF52840 added
* Fixes for atmel_ecc_create_pms to free the used slot.
* Fixes for building ATECC with ATCAPRINTF or WOLFSSL_PUBLIC_MP
* Cortex-M code changes to support IAR compiler
* Improvements to STM32 AES-GCM performance
* Fixes for 16-bit systems including PK callbacks, ATECC and LowResTimer function ptoto.
* IAR-EWARM compiler warning fix
* Clean up of user_settings for CS+ port
* Updating Renesas example projects to the latest version
* Micrium updates adjusting STATIC macro name and added inline flag
* Fixes for building with WOLFSSL_CUSTOM_CURVES on Windows
* Updates and refactor to the default build settings with Arduino
* Fixes for visibility tags with Cygwin build
* STSAFE Improvements to support wolfSSL Crypto Callbacks
* Improvements to NetBSD builds and mutex use in test case
* Updating TI hardware offload with WOLFSSL_TI_CRYPT build
* Maintaining Xilinx FreeRTOS port by adjusting time.h include in wolfSSL
* SiFive HiFive E31 RISC‐V core family port
* Port for Telit IoT AppZone SDK
* OCSP Response signed by issuer with identical SKID fix
* Fix for sending revoked certificate with OCSP
* Honor the status sent over connection with peers and do not perform an internal OCSP lookup
* Adding the build flag `--enable-ecccustcurves=all` to enable all curve types
* Support add for Ed25519ctx and Ed25519ph sign/verify algorithms as per RFC 8032
* Addition of the macro WOLFSSL_NO_SIGALG to disable signature algorithms extension
* wc_AesCtrEncrypt in place addition, where input and output buffer can be the same buffer
* Single shot API added for SHA3; wc_Sha3_224Hash, wc_Sha3_256Hash, wc_Sha3_384Hash, wc_Sha3_512Hash
* Function additions for JSSE support some of which are wolfSSL_get_ciphers_iana and wolfSSL_X509_verify along with expansion of the --enable-jni option
* Macro guards for more modular SHA3 build (i.e. support for 384 size only)
* Benchmarking -thread <num> argument support for asynchronous crypto
* Blake2s support (--enable-blake2s), which provides 32-bit Blake2 support
* Macro SHA256_MANY_REGISTERS addition to potentially speed up SHA256 depending on architecture
* Additional TLS alert messages sent with the macro WOLFSSL_EXTRA_ALERTS defined
* Feature to fail resumption of a session if the session’s cipher suite is not in the client’s list, this can be overridden by defining the macro NO_RESUME_SUITE_CHECK
* Fallback SCSV (Signaling Cipher Suite Value) support on Server only (--enable-fallback-scsv)
* DTLS export state only (wolfSSL_dtls_export_state_only) which is a subset of the information exported from previous DTLS export function
* Function wc_DhCheckPubValue added to perform simple validity checks on DH keys
* Support for RSA SHA-224 signatures with TLS added
* Additional option “-print” to the benchmark app for printing out a brief summary after benchmarks are complete
*  Adding (--disable-pkcs12) option and improvements for disabled sections in pwdbased.c, asn.c, rsa.c, pkcs12.c and wc_encrypt
* Added DES3 support to the wolfSSL crypto callbacks
* Compile time fixes for build case with SP math and RSA only
* Fixes for Coverity static analysis report including explicit initialization of reported stack variables some additional Coverity fixes added thanks to Martin
* Fixes for scan build warnings (i.e possible null dereference in ecc.c)
* Resetting verify send value with a call to wolfSSL_clear function
* Fix for extern with sp_ModExp_2048 when building with --cpp option
* Fix for typo issue with --enable-sp=cortexm
* Adding #pragma warning disable 4127 for tfm.c when building with Visual Studio
* Improvements to the maximum ECC signature calculations
* Improvements to TLS write handling in error cases which helps user application not go through with a wolfSSL_write attempt after a wolfSSL_read failure
* Fix for read directory functions with Windows (wc_ReadDirFirst and wc_ReadDirNext)
* Sanity check on index before accessing domain component buffer in call to wolfSSL_X509_NAME_get_entry
* Sending fatal alert from client side on version error
* Fix for static RSA cipher suite with PK callback and no loaded private key
* Fix for potential memory leak in error case with the function wc_DsaKeyToDer, thanks to Chris H. for the report
* Adjusting STRING_USER macro to remove includes of standard lib <string.h> or <stdio.h>
* Bug fix for checking wrong allocation assignment in the function wc_PBKDF2 and handling potential leak on allocation failure. This case is only hit when the specific call to malloc fails in the function wc_PBKDF2. Thanks to Robert Altnoeder (Linbit) for the report
* Improved length checks when parsing ASN.1 certificates
* extern "C" additions to header files that were missing them
* Improved checking of return values with TLS extension functions and error codes
* Removing redundant calls to the generate function when instantiating and reseeding DRBG
* Refactoring and improvements to autoconf code with consolidating AM_CONDITIONAL statements
* Improvements for handling error return codes when reading input from transport layer
* Improvements to efficiency of SNI extension parsing and error checking with ALPN parsing
* Macro WOLFSSL_DEBUG_TLS addition for printing out extension data being parsed during a TLS connection
* Adjustment of prime testing with --disable-fastmath builds


This release of wolfSSL includes a fix for 2 security vulnerabilities.

There is a fix for a potential buffer overflow case with the TLSv1.3 PSK extension parsing. This affects users that are enabling TLSv1.3 (--enable-tls13). Thanks to Robert Hoerr for the report. The CVE associated with the report is CVE-2019-11873.

There is a fix for the potential leak of nonce sizes when performing ECDSA signing operations. The leak is considered to be difficult to exploit but it could potentially be used maliciously to perform a lattice based timing attack against previous wolfSSL versions. ECC operations with --enable-sp and --enable-sp-asm are not affected, users with private ECC keys in other builds that are performing ECDSA signing operations should update versions of wolfSSL along with private ECC keys. Thanks to Ján Jančár from Masaryk University for the report.


# wolfSSL Release 4.0.0 (03/20/2019)

Release 4.0.0 of wolfSSL embedded TLS has bug fixes and new features including:

* Support for wolfCrypt FIPS v4.0.0, certificate #3389
* FIPS Ready Initiative
* Compatibility fixes for secure renegotiation with Chrome
* Better size check for TLS record fragment reassembly
* Improvements to non-blocking and handshake message retry support for DTLS
* Improvements to OCSP with ECDSA signers
* Added TLS server side secure renegotiation
* Added TLS Trusted CA extension
* Add support for the Deos Safety Critical RTOS
* OCSP fixes for memory management and initializations
* Fixes for EVP Cipher decryption padding checks
* Removal of null terminators on `wolfSSL_X509_print` substrings
* `wolfSSL_sk_ASN1_OBJCET_pop` function renamed to `wolfSSL_sk_ASN1_OBJECT_pop`
* Adjustment to include path in compatibility layer for evp.h and objects.h
* Fixes for decoding BER encoded PKCS7 contents
* TLS handshake now supports using PKCS #11 for private keys
* PKCS #11 support of HMAC, AES-CBC and random seeding/generation
* Support for named FFDHE parameters in TLS 1.2 (RFC 7919)
* Port to Zephyr Project
* Move the TLS PRF to wolfCrypt.
* Update to CMS KARI support
* Added ESP32 WROOM support
* Fixes and additions to the OpenSSL compatibility layer
* Added WICED Studio Support
* MDK CMSIS RTOS v2
* Xcode project file update
* Fixes for ATECC508A/ATECC608A
* Fixes issue with CA path length for self signed root CA's
* Fixes for Single Precision (SP) ASM when building sources directly
* Fixes for STM32 AES GCM
* Fixes for ECC sign with hardware to ensure the input is truncated
* Fixes for proper detection of PKCS7 buffer overflow case
* Fixes to handle degenerate PKCS 7 with BER encoding
* Fixes for TLS v1.3 handling of 6144 and 8192 bit keys
* Fixes for possible build issues with SafeRTOS
* Added `ECC_PUBLICKEY_TYPE` to the support PEM header types
* Added strict checking of the ECDSA signature DER encoding length
* Added ECDSA option to limit sig/algos in client_hello to key size with
  `USE_ECDSA_KEYSZ_HASH_ALGO`
* Added Cortex-M support for Single Precision (SP) math
* Added wolfCrypt RSA non-blocking time support
* Added 16-bit compiler support using --enable-16bit option
* Improved Arduino sketch example
* Improved crypto callback features
* Improved TLS benchmark tool
* Added new wrapper for snprintf for use with certain Visual Studio builds,
  thanks to David Parnell (Cambridge Consultants)

This release of wolfSSL includes a fix for 1 security vulnerability.

* Fixed a bug in tls_bench.c example test application unrelated to the crypto
  or TLS portions of the library. (CVE-2019-6439)


# wolfSSL Release 3.15.7 (12/26/2018)

Release 3.15.7 of wolfSSL embedded TLS has bug fixes and new features including:

* Support for Espressif ESP-IDF development framework
* Fix for XCode build with iPhone simulator on i386
* PKCS7 support for generating and verify bundles using a detached signature
* Fix for build disabling AES-CBC and enabling opensslextra compatibility layer
* Updates to sniffer for showing session information and handling split messages across records
* Port update for Micrium uC/OS-III
* Feature to adjust max fragment size post handshake when compiled with the macro WOLFSSL_ALLOW_MAX_FRAGMENT_ADJUST
* Adding the macro NO_MULTIBYTE_PRINT for compiling out special characters that embedded devices may have problems with
* Updates for Doxygen documentation, including PKCS #11 API and more
* Adding Intel QuickAssist v1.7 driver support for asynchronous crypto
* Adding Intel QuickAssist RSA key generation and SHA-3 support
* RSA verify only (--enable-rsavfy) and RSA public only (--enable-rsapub) builds added
* Enhancements to test cases for increased code coverage
* Updates to VxWorks port for use with Mongoose, including updates to the OpenSSL compatibility layer
* Yocto Project ease of use improvements along with many updates and build instructions added to the INSTALL file
* Maximum ticket nonce size was increased to 8
* Updating --enable-armasm build for ease of use with autotools
* Updates to internal code checking TLS 1.3 version with a connection
* Removing unnecessary extended master secret from ServerHello if using TLS 1.3
* Fix for TLS v1.3 HelloRetryRequest to be sent immediately and not grouped



This release of wolfSSL includes a fix for 1 security vulnerability.

Medium level fix for potential cache attack with a variant of Bleichenbacher’s attack. Earlier versions of wolfSSL leaked PKCS #1 v1.5 padding information during private key decryption that could lead to a potential padding oracle attack. It is recommended that users update to the latest version of wolfSSL if they have RSA cipher suites enabled and have the potential for malicious software to be ran on the same system that is performing RSA operations. Users that have only ECC cipher suites enabled and are not performing RSA PKCS #1 v1.5 Decryption operations are not vulnerable. Also users with TLS 1.3 only connections are not vulnerable to this attack. Thanks to Eyal Ronen (Weizmann Institute), Robert Gillham (University of Adelaide), Daniel Genkin (University of Michigan), Adi Shamir (Weizmann Institute), David Wong (NCC Group), and Yuval Yarom (University of Adelaide and Data61) for the report.

The paper for further reading on the attack details can be found at http://cat.eyalro.net/cat.pdf.


See INSTALL file for build instructions.
More info can be found on-line at http://wolfssl.com/wolfSSL/Docs.html


# wolfSSL Release 3.15.5 (11/07/2018)

Release 3.15.5 of wolfSSL embedded TLS has bug fixes and new features including:

* Fixes for GCC-8 warnings with strings
* Additional compatibility API’s added, including functions like wolfSSL_X509_CA_num and wolfSSL_PEM_read_X509_CRL
* Fixes for OCSP use with NGINX port
* Renamed the macro INLINE to WC_INLINE for inline functions
* Doxygen updates and formatting for documentation generation
* Added support for the STM32L4 with AES/SHA hardware acceleration
* Adds checking for critical extension with certificate Auth ID and the macro WOLFSSL_ALLOW_CRIT_SKID to override the check
* Added public key callbacks to ConfirmSignature function to expand public key callback support
* Added ECC and Curve25519 key generation callback support
* Fix for memory management with wolfSSL_BN_hex2bn function
* Added support for dynamic allocation of PKCS7 structure using wc_PKCS7_New and wc_PKCS7_Free
* Port to apache mynewt added in the directory wolfssl-3.15.5/IDE/mynewt/*
* OCSP stapling in TLS 1.3 additions
* Port for ASIO added with --enable-asio configure flag
* Contiki port added with macro WOLFSSL_CONTIKI
* Memory free optimizations with adding in earlier free’s where possible
* Made modifications to the primality testing so that the Miller-Rabin tests check against up to 40 random numbers rather than a fixed list of small primes
* Certificate validation time generation updated
* Fixes for MQX classic 4.0 with IAR-EWARM
* Fix for assembly optimized version of Curve25519
* Make SOCKET_PEER_CLOSED_E consistent between read and write cases
* Relocate compatibility layer functions for OpenSSH port update
* Update to Intel® SGX port, files included by Windows version and macros defined when using WOLFSSL_SGX
* Updates to Nucleus version supported
* Stack size reduction with smallstack build
* Updates to Rowley-Crossworks settings for CMSIS 4
* Added reference STSAFE-A100 public key callbacks for TLS support
* Added reference ATECC508A/ATECC608A public key callbacks for TLS support
* Updated support for latest CryptoAuthLib (10/25/2018)
* Added a wolfSSL static library project for Atollic TrueSTUDIO
* Flag to disable AES-CBC and have only AEAD cipher suites with TLS
* AF_ALG and cryptodev-linux crypto support added
* Update to IO callbacks with use of WOLFSSL_BIO
* Additional support for parsing certificate subject OIDs (businessCategory, jurisdiction of incorporation country, and jurisdiction of incorporation state)
* Added  wc_ecc_ecport_ex and wc_export_inti API's for ECC hex string exporting
* Updates to XCODE build with wolfSSL
* Fix for guard on when to include sys/time.h header
* Updates and enhancements to the GCC-ARM example
* Fix for PKCS8 padding with encryption
* Updates for wolfcrypt JNI wrapper
* ALT_ECC_SIZE use with SP math
* PIC32MZ hardware acceleration buffer alignment fixes
* Renesas e2studio project files added
* Renesas RX example project added
* Fix for DH algorithm when using SP math with ARM assembly
* Fixes and enhancements for NXP K82 support
* Benchmark enhancements to print in CSV format and in Japanese
* Support for PKCS#11 added with --enable-pkcs11
* Fixes for asynchronous crypto use with TLS 1.3
* TLS 1.3 only build, allows for disabling TLS 1.2 and earlier protocols
* Fix for GCC warnings in function wolfSSL_ASN1_TIME_adj
* Added --enable-asn=nocrypt for certificate only parsing support
* Added support for parsing PIV format certificates with the function wc_ParseCertPIV and macro WOLFSSL_CERT_PIV
* Added APIs to support GZIP
* Updates to support Lighttpd
* Version resource added for Windows DLL builds
* Increased code coverage with additional testing
* Added support for constructed OCTET_STRING with PKCS#7 signed data
* Added DTLS either (server/client) side initialization setting
* Minor fixes for building with MINGW32 compiler
* Added support for generic ECC PEM header/footer with PKCS8 parsing
* Added Japanese output to example server and client with “-1 1” flag
* Added USE_ECDSA_KEYSZ_HASH_ALGO macro for building to use digest sizes that match ephemeral key size
* Expand PKCS#7 CMS support with KEKRI, PWRI and ORI
* Streaming capability for PKCS#7 decoding and sign verify added


See INSTALL file for build instructions.
More info can be found on-line at http://wolfssl.com/wolfSSL/Docs.html


# wolfSSL Release 3.15.3 (6/20/2018)

Release 3.15.3 of wolfSSL embedded TLS has bug fixes and new features including:

* ECDSA blinding added for hardening against side channel attacks
* Fix for compatibility layer build with no server and no client defined
* Use of optimized Intel assembly instructions on compatible AMD processor
* wolfCrypt Nucleus port additions
* Fix added for MatchDomainName and additional tests added
* Fixes for building with ‘WOLFSSL_ATECC508A’ defined
* Fix for verifying a PKCS7 file in BER format with indefinite size


This release of wolfSSL fixes 2 security vulnerability fixes.

Medium level fix for PRIME + PROBE attack combined with a variant of Lucky 13. Constant time hardening was done to avoid potential cache-based side channel attacks when verifying the MAC on a TLS packet. CBC cipher suites are susceptible on systems where an attacker could gain access and run a parallel program for inspecting caching. Only wolfSSL users that are using TLS/DTLS CBC cipher suites need to update. Users that have only AEAD and stream cipher suites set, or have built with WOLFSSL_MAX_STRENGTH (--enable-maxstrength), are not vulnerable. Thanks to Eyal Ronen, Kenny Paterson, and Adi Shamir for the report.

Medium level fix for a ECDSA side channel attack. wolfSSL is one of over a dozen vendors mentioned in the recent Technical Advisory “ROHNP” by author Ryan Keegan. Only wolfSSL users with long term ECDSA private keys using our fastmath or normal math libraries on systems where attackers can get access to the machine using the ECDSA key need to update.  An attacker gaining access to the system could mount a memory cache side channel attack that could recover the key within a few thousand signatures. wolfSSL users that are not using ECDSA private keys, that are using the single precision math library, or that are using ECDSA offloading do not need to update. (blog with more information https://www.wolfssl.com/wolfssh-and-rohnp/)


See INSTALL file for build instructions.
More info can be found on-line at http://wolfssl.com/wolfSSL/Docs.html

# wolfSSL Release 3.15.0 (06/05/2018)

Release 3.15.0 of wolfSSL embedded TLS has bug fixes and new features including:

* Support for TLS 1.3 Draft versions 23, 26 and 28.
* Add FIPS SGX support!
* Single Precision assembly code added for ARM and 64-bit ARM to enhance performance.
* Improved performance for Single Precision maths on 32-bit.
* Improved downgrade support for the TLS 1.3 handshake.
* Improved TLS 1.3 support from interoperability testing.
* Added option to allow TLS 1.2 to be compiled out to reduce size and enhance security.
* Added option to support Ed25519 in TLS 1.2 and 1.3.
* Update wolfSSL_HMAC_Final() so the length parameter is optional.
* Various fixes for Coverity static analysis reports.
* Add define to use internal struct timeval (USE_WOLF_TIMEVAL_T).
* Switch LowResTimer() to call XTIME instead of time(0) for better portability.
* Expanded OpenSSL compatibility layer with a bevy of new functions.
* Added Renesas CS+ project files.
* Align DH support with NIST SP 800-56A, add wc_DhSetKey_ex() for q parameter.
* Add build option for CAVP self test build (--enable-selftest).
* Expose mp_toradix() when WOLFSSL_PUBLIC_MP is defined.
* Example certificate expiration dates and generation script updated.
* Additional optimizations to trim out unused strings depending on build options.
* Fix for DN tag strings to have “=” when returning the string value to users.
* Fix for wolfSSL_ERR_get_error_line_data return value if no more errors are in the queue.
* Fix for AES-CBC IV value with PIC32 hardware acceleration.
* Fix for wolfSSL_X509_print with ECC certificates.
* Fix for strict checking on URI absolute vs relative path.
* Added crypto device framework to handle PK RSA/ECC operations using callbacks, which adds new build option `./configure --enable-cryptodev` or `WOLF_CRYPTO_DEV`.
* Added devId support to ECC and PKCS7 for hardware based private key.
* Fixes in PKCS7 for handling possible memory leak in some error cases.
* Added test for invalid cert common name when set with `wolfSSL_check_domain_name`.
* Refactor of the cipher suite names to use single array, which contains internal name, IANA name and cipher suite bytes.
* Added new function `wolfSSL_get_cipher_name_from_suite` for getting IANA cipher suite name using bytes.
* Fixes for fsanitize reports.
* Fix for openssl compatibility function `wolfSSL_RSA_verify` to check returned size.
* Fixes and improvements for FreeRTOS AWS.
* Fixes for building openssl compatibility with FreeRTOS.
* Fix and new test for handling match on domain name that may have a null terminator inside.
* Cleanup of the socket close code used for examples, CRL/OCSP and BIO to use single macro `CloseSocket`.
* Refactor of the TLSX code to support returning error codes.
* Added new signature wrapper functions `wc_SignatureVerifyHash` and `wc_SignatureGenerateHash` to allow direct use of hash.
* Improvement to GCC-ARM IDE example.
* Enhancements and cleanups for the ASN date/time code including new API's `wc_GetDateInfo`, `wc_GetCertDates` and `wc_GetDateAsCalendarTime`.
* Fixes to resolve issues with C99 compliance. Added build option `WOLF_C99` to force C99.
* Added a new `--enable-opensslall` option to enable all openssl compatibility features.
* Added new `--enable-webclient` option for enabling a few HTTP API's.
* Added new `wc_OidGetHash` API for getting the hash type from a hash OID.
* Moved `wolfSSL_CertPemToDer`, `wolfSSL_KeyPemToDer`, `wolfSSL_PubKeyPemToDer` to asn.c and renamed to `wc_`. Added backwards compatibility macro for old function names.
* Added new `WC_MAX_SYM_KEY_SIZE` macro for helping determine max key size.
* Added `--enable-enckeys` or (`WOLFSSL_ENCRYPTED_KEYS`) to enable support for encrypted PEM private keys using password callback without having to use opensslextra.
* Added ForceZero on the password buffer after done using it.
* Refactor unique hash types to use same internal values (ex WC_MD5 == WC_HASH_TYPE_MD5).
* Refactor the Sha3 types to use `wc_` naming, while retaining old names for compatibility.
* Improvements to `wc_PBKDF1` to support more hash types and the non-standard extra data option.
* Fix TLS 1.3 with ECC disabled and CURVE25519 enabled.
* Added new define `NO_DEV_URANDOM` to disable the use of `/dev/urandom`.
* Added `WC_RNG_BLOCKING` to indicate block w/sleep(0) is okay.
* Fix for `HAVE_EXT_CACHE` callbacks not being available without `OPENSSL_EXTRA` defined.
* Fix for ECC max bits `MAX_ECC_BITS` not always calculating correctly due to macro order.
* Added support for building and using PKCS7 without RSA (assuming ECC is enabled).
* Fixes and additions for Cavium Nitrox V to support ECC, AES-GCM and HMAC (SHA-224 and SHA3).
* Enabled ECC, AES-GCM and SHA-512/384 by default in (Linux and Windows)
* Added `./configure --enable-base16` and `WOLFSSL_BASE16` configuration option to enable Base16 API's.
* Improvements to ATECC508A support for building without `WOLFSSL_ATMEL` defined.
* Refactor IO callback function names to use `_CTX_` to eliminate confusion about the first parameter.
* Added support for not loading a private key for server or client when `HAVE_PK_CALLBACK` is defined and the private PK callback is set.
* Added new ECC API `wc_ecc_sig_size_calc` to return max signature size for a key size.
* Cleanup ECC point import/export code and added new API `wc_ecc_import_unsigned`.
* Fixes for handling OCSP with non-blocking.
* Added new PK (Primary Key) callbacks for the VerifyRsaSign. The new callbacks API's are `wolfSSL_CTX_SetRsaVerifySignCb` and `wolfSSL_CTX_SetRsaPssVerifySignCb`.
* Added new ECC API `wc_ecc_rs_raw_to_sig` to take raw unsigned R and S and encodes them into ECDSA signature format.
* Added support for `WOLFSSL_STM32F1`.
* Cleanup of the ASN X509 header/footer and XSTRNCPY logic.
* Add copyright notice to autoconf files. (Thanks Brian Aker!)
* Updated the M4 files for autotools. (Thanks Brian Aker!)
* Add support for the cipher suite TLS_DH_anon_WITH_AES256_GCM_SHA384 with test cases. (Thanks Thivya Ashok!)
* Add the TLS alert message unknown_psk_identity (115) from RFC 4279, section 2. (Thanks Thivya Ashok!)
* Fix the case when using TCP with timeouts with TLS. wolfSSL shall be agnostic to network socket behavior for TLS. (DTLS is another matter.) The functions `wolfSSL_set_using_nonblock()` and `wolfSSL_get_using_nonblock()` are deprecated.
* Hush the AR warning when building the static library with autotools.
* Hush the “-pthread” warning when building in some environments.
* Added a dist-hook target to the Makefile to reset the default options.h file.
* Removed the need for the darwin-clang.m4 file with the updates provided by Brian A.
* Renamed the AES assembly file so GCC on the Mac will build it using the preprocessor.
* Add a disable option (--disable-optflags) to turn off the default optimization flags so user may supply their own custom flags.
* Correctly touch the dummy fips.h header.

If you have questions on any of this, then email us at info@wolfssl.com.
See INSTALL file for build instructions.
More info can be found on-line at http://wolfssl.com/wolfSSL/Docs.html


# wolfSSL Release 3.14.0 (03/02/2018)

Release 3.14.0 of wolfSSL embedded TLS has bug fixes and new features including:

* TLS 1.3 draft 22 and 23 support added
* Additional unit tests for; SHA3, AES-CMAC, Ed25519, ECC, RSA-PSS, AES-GCM
* Many additions to the OpenSSL compatibility layer were made in this release. Some of these being enhancements to PKCS12, WOLFSSL_X509 use, WOLFSSL_EVP_PKEY, and WOLFSSL_BIO operations
* AVX1 and AVX2 performance improvements with ChaCha20 and Poly1305
* Added i.MX CAAM driver support with Integrity OS support
* Improvements to logging with debugging, including exposing more API calls and adding options to reduce debugging code size
* Fix for signature type detection with PKCS7 RSA SignedData
* Public key call back functions added for DH Agree
* RSA-PSS API added for operating on non inline buffers (separate input and output buffers)
* API added for importing and exporting raw DSA parameters
* Updated DSA key generation to be FIPS 186-4 compliant
* Fix for wolfSSL_check_private_key when comparing ECC keys
* Support for AES Cipher Feedback(CFB) mode added
* Updated RSA key generation to be FIPS 186-4 compliant
* Update added for the ARM CMSIS software pack
* WOLFSSL_IGNORE_FILE_WARN macro added for avoiding build warnings when not working with autotools
* Performance improvements for AES-GCM with AVX1 and AVX2
* Fix for possible memory leak on error case with wc_RsaKeyToDer function
* Make wc_PKCS7_PadData function available
* Updates made to building SGX on Linux
* STM32 hashing algorithm improvements including clock/power optimizations and auto detection of if SHA2 is supported
* Update static memory feature for FREERTOS use
* Reverse the order that certificates are compared during PKCS12 parse to account for case where multiple certificates have the same matching private key
* Update NGINX port to version 1.13.8
* Support for HMAC-SHA3 added
* Added stricter ASN checks to enforce RFC 5280 rules. Thanks to the report from Professor Zhenhua Duan, Professor Cong Tian, and Ph.D candidate Chu Chen from Institute of Computing Theory and Technology (ICTT) of Xidian University.
* Option to have ecc_mul2add function public facing
* Getter function wc_PKCS7_GetAttributeValue added for PKCS7 attributes
* Macros NO_AES_128, NO_AES_192, NO_AES_256 added for AES key size selection at compile time
* Support for writing multiple organizations units (OU) and domain components (DC) with CSR and certificate creation
* Support for indefinite length BER encodings in PKCS7
* Added API for additional validation of prime q in a public DH key
* Added support for RSA encrypt and decrypt without padding


See INSTALL file for build instructions.
More info can be found on-line at http://wolfssl.com/wolfSSL/Docs.html


# wolfSSL (Formerly CyaSSL) Release 3.13.0 (12/21/2017)

wolfSSL 3.13.0 includes bug fixes and new features, including support for
TLS 1.3 Draft 21, performance and footprint optimizations, build fixes,
updated examples and project files, and one vulnerability fix. The full list
of changes and additions in this release include:

* Fixes for TLS 1.3, support for Draft 21
* TLS 1.0 disabled by default, addition of “--enable-tlsv10” configure option
* New option to reduce SHA-256 code size at expense of performance
  (USE_SLOW_SHA256)
* New option for memory reduced build (--enable-lowresource)
* AES-GCM performance improvements on AVX1 (IvyBridge) and AVX2
* SHA-256 and SHA-512 performance improvements using AVX1/2 ASM
* SHA-3 size and performance optimizations
* Fixes for Intel AVX2 builds on Mac/OSX
* Intel assembly for Curve25519, and Ed25519 performance optimizations
* New option to force 32-bit mode with “--enable-32bit”
* New option to disable all inline assembly with “--disable-asm”
* Ability to override maximum signature algorithms using WOLFSSL_MAX_SIGALGO
* Fixes for handling of unsupported TLS extensions.
* Fixes for compiling AES-GCM code with GCC 4.8.*
* Allow adjusting static I/O buffer size with WOLFMEM_IO_SZ
* Fixes for building without a filesystem
* Removes 3DES and SHA1 dependencies from PKCS#7
* Adds ability to disable PKCS#7 EncryptedData type (NO_PKCS7_ENCRYPTED_DATA)
* Add ability to get client-side SNI
* Expanded OpenSSL compatibility layer
* Fix for logging file names with OpenSSL compatibility layer enabled, with
  WOLFSSL_MAX_ERROR_SZ user-overridable
* Adds static memory support to the wolfSSL example client
* Fixes for sniffer to use TLS 1.2 client method
* Adds option to wolfCrypt benchmark to benchmark individual algorithms
* Adds option to wolfCrypt benchmark to display benchmarks in powers
  of 10 (-base10)
* Updated Visual Studio for ARM builds (for ECC supported curves and SHA-384)
* Updated Texas Instruments TI-RTOS build
* Updated STM32 CubeMX build with fixes for SHA
* Updated IAR EWARM project files
* Updated Apple Xcode projects with the addition of a benchmark example project

This release of wolfSSL fixes 1 security vulnerability.

wolfSSL is cited in the recent ROBOT Attack by Böck, Somorovsky, and Young.
The paper notes that wolfSSL only gives a weak oracle without a practical
attack but this is still a flaw.  This release contains a fix for this report.
Please note that wolfSSL has static RSA cipher suites disabled by default as
of version 3.6.6 because of the lack of perfect forward secrecy.  Only users
who have explicitly enabled static RSA cipher suites with WOLFSSL_STATIC_RSA
and use those suites on a host are affected.  More information will be
available on our website at:

https://wolfssl.com/wolfSSL/security/vulnerabilities.php

See INSTALL file for build instructions.
More info can be found on-line at http://wolfssl.com/wolfSSL/Docs.html


# wolfSSL (Formerly CyaSSL) Release 3.12.2 (10/23/2017)

## Release 3.12.2 of wolfSSL has bug fixes and new features including:

This release includes many performance improvements with Intel ASM (AVX/AVX2) and AES-NI. New single precision math option to speedup RSA, DH and ECC. Embedded hardware support has been expanded for STM32, PIC32MZ and ATECC508A. AES now supports XTS mode for disk encryption. Certificate improvements for setting serial number, key usage and extended key usage. Refactor of SSL_ and hash types to allow openssl coexistence. Improvements for TLS 1.3. Fixes for OCSP stapling to allow disable and WOLFSSL specific user context for callbacks. Fixes for openssl and MySQL compatibility. Updated Micrium port. Fixes for asynchronous modes.

* Added TLS extension for Supported Point Formats (ec_point_formats)
* Fix to not send OCSP stapling extensions in client_hello when not enabled
* Added new API's for disabling OCSP stapling
* Add check for SIZEOF_LONG with sun and LP64
* Fixes for various TLS 1.3 disable options (RSA, ECC and ED/Curve 25519).
* Fix to disallow upgrading to TLS v1.3
* Fixes for wolfSSL_EVP_CipherFinal() when message size is a round multiple of a block size.
* Add HMAC benchmark and expanded AES key size benchmarks
* Added simple GCC ARM Makefile example
* Add tests for 3072-bit RSA and DH.
* Fixed DRAFT_18 define and fixed downgrading with TLS v1.3
* Fixes to allow custom serial number during certificate generation
* Add method to get WOLFSSL_CTX certificate manager
* Improvement to `wolfSSL_SetOCSP_Cb` to allow context per WOLFSSL object
* Alternate certificate chain support `WOLFSSL_ALT_CERT_CHAINS`. Enables checking cert against multiple CA's.
* Added new `--disable-oldnames` option to allow for using openssl along-side wolfssl headers (without OPENSSL_EXTRA).
* Refactor SSL_ and hashing types to use wolf specific prefix (WOLFSSL and WC_) to allow openssl coexistence.
* Fixes for HAVE_INTEL_MULX
* Cleanup include paths for MySQL cmake build
* Added configure option for building library for wolfSSH (--enable-wolfssh)
* Openssl compatibility layer improvements
* Expanded API unit tests
* Fixes for STM32 crypto hardware acceleration
* Added AES XTS mode (--enable-xts)
* Added ASN Extended Key Usage Support (see wc_SetExtKeyUsage).
* Math updates and added TFM_MIPS speedup.
* Fix for creation of the KeyUsage BitString
* Fix for 8k keys with MySQL compatibility
* Fixes for ATECC508A.
* Fixes for PIC32MZ hashing.
* Fixes and improvements to asynchronous modes for Intel QuickAssist and Cavium Nitrox V.
* Update HASH_DRBG Reseed mechanism and add test case
* Rename the file io.h/io.c to wolfio.h/wolfio.c
* Cleanup the wolfIO_Send function.
* OpenSSL Compatibility Additions and Fixes
* Improvements to Visual Studio DLL project/solution.
* Added function to generate public ECC key from private key
* Added async blocking support for sniffer tool.
* Added wolfCrypt hash tests for empty string and large data.
* Added ability to use of wolf implementation of `strtok` using `USE_WOLF_STRTOK`.
* Updated Micrium uC/OS-III Port
* Updated root certs for OCSP scripts
* New Single Precision math option for RSA, DH and ECC (off by default). See `--enable-sp`.
* Speedups for AES GCM with AESNI (--enable-aesni)
* Speedups for SHA2, ChaCha20/Poly1035 using AVX/AVX2


# wolfSSL (Formerly CyaSSL) Release 3.12.0 (8/04/2017)

## Release 3.12.0 of wolfSSL has bug fixes and new features including:

- TLS 1.3 with Nginx! TLS 1.3 with ARMv8! TLS 1.3 with Async Crypto! (--enable-tls13)
- TLS 1.3 0RTT feature added
- Added port for using Intel SGX with Linux
- Update and fix PIC32MZ port
- Additional unit testing for MD5, SHA, SHA224, SHA256, SHA384, SHA512, RipeMd, HMAC, 3DES, IDEA, ChaCha20, ChaCha20Poly1305 AEAD, Camellia, Rabbit, ARC4, AES, RSA, Hc128
- AVX and AVX2 assembly for improved ChaCha20 performance
- Intel QAT fixes for when using --disable-fastmath
- Update how DTLS handles decryption and MAC failures
- Update DTLS session export version number for --enable-sessionexport feature
- Add additional input argument sanity checks to ARMv8 assembly port
- Fix for making PKCS12 dynamic types match
- Fixes for potential memory leaks when using --enable-fast-rsa
- Fix for when using custom ECC curves and add BRAINPOOLP256R1 test
- Update TI-RTOS port for dependency on new wolfSSL source files
- DTLS multicast feature added, --enable-mcast
- Fix for Async crypto with GCC 7.1 and HMAC when not using Intel QuickAssist
- Improvements and enhancements to Intel QuickAssist support
- Added Xilinx port
- Added SHA3 Keccak feature, --enable-sha3
- Expand wolfSSL Python wrapper to now include a client side implementation
- Adjust example servers to not treat a peer closed error as a hard error
- Added more sanity checks to fp_read_unsigned_bin function
- Add SHA224 and AES key wrap to ARMv8 port
- Update MQX classics and mmCAU ports
- Fix for potential buffer over read with wolfSSL_CertPemToDer
- Add PKCS7/CMS decode support for KARI with IssuerAndSerialNumber
- Fix ThreadX/NetX warning
- Fixes for OCSP and CRL non blocking sockets and for incomplete cert chain with OCSP
- Added RSA PSS sign and verify
- Fix for STM32F4 AES-GCM
- Added enable all feature (--enable-all)
- Added trackmemory feature (--enable-trackmemory)
- Fixes for AES key wrap and PKCS7 on Windows VS
- Added benchmark block size argument
- Support use of staticmemory with PKCS7
- Fix for Blake2b build with GCC 5.4
- Fixes for compiling wolfSSL with GCC version 7, most dealing with switch statement fall through warnings.
- Added warning when compiling without hardened math operations


Note:
There is a known issue with using ChaCha20 AVX assembly on versions of GCC earlier than 5.2. This is encountered with using the wolfSSL enable options --enable-intelasm and --enable-chacha. To avoid this issue ChaCha20 can be enabled with --enable-chacha=noasm.
If using --enable-intelasm and also using --enable-sha224 or --enable-sha256 there is a known issue with trying to use -fsanitize=address.

This release of wolfSSL fixes 1 low level security vulnerability.

Low level fix for a potential DoS attack on a wolfSSL client. Previously a client would accept many warning alert messages without a limit. This fix puts a limit to the number of warning alert messages received and if this limit is reached a fatal error ALERT_COUNT_E is returned. The max number of warning alerts by default is set to 5 and can be adjusted with the macro WOLFSSL_ALERT_COUNT_MAX. Thanks for the report from Tarun Yadav and Koustav Sadhukhan from Defence Research and Development Organization, INDIA.


See INSTALL file for build instructions.
More info can be found on-line at http://wolfssl.com/wolfSSL/Docs.html


# wolfSSL (Formerly CyaSSL) Release 3.11.1 (5/11/2017)

## Release 3.11.1 of wolfSSL is a TLS 1.3 BETA release, which includes:

- TLS 1.3 client and server support for TLS 1.3 with Draft 18 support

This is strictly a BETA release, and designed for testing and user feedback.
Please send any comments, testing results, or feedback to wolfSSL at
support@wolfssl.com.

See INSTALL file for build instructions.
More info can be found on-line at http://wolfssl.com/wolfSSL/Docs.html


# wolfSSL (Formerly CyaSSL) Release 3.11.0 (5/04/2017)

## Release 3.11.0 of wolfSSL has bug fixes and new features including:

- Code updates for warnings reported by Coverity scans
- Testing and warning fixes for FreeBSD on PowerPC
- Updates and refactoring done to ASN1 parsing functions
- Change max PSK identity buffer to account for an identity length of 128 characters
- Update Arduino script to handle recent files and additions
- Added support for PKCS#7 Signed Data with ECDSA
- Fix for interoperability with ChaCha20-Poly1305 suites using older draft versions
- DTLS update to allow multiple handshake messages in one DTLS record. Thanks to Eric Samsel over at Welch Allyn for reporting this bug.
- Intel QuickAssist asynchronous support (PR #715 - https://www.wolfssl.com/wolfSSL/Blog/Entries/2017/1/18_wolfSSL_Asynchronous_Intel_QuickAssist_Support.html)
- Added support for HAproxy load balancer
- Added option to allow SHA1 with TLS 1.2 for IIS compatibility (WOLFSSL_ALLOW_TLS_SHA1)
- Added Curve25519 51-bit Implementation, increasing performance on systems that have 128 bit types
- Fix to not send session ID on server side if session cache is off unless we're echoing
session ID as part of session tickets
- Fixes for ensuring all default ciphers are setup correctly (see PR #830)
- Added NXP Hexiwear example in `IDE/HEXIWEAR`.
- Added wolfSSL_write_dup() to create write only WOLFSSL object for concurrent access
- Fixes for TLS elliptic curve selection on private key import.
- Fixes for RNG with Intel rdrand and rdseed speedups.
- Improved performance with Intel rdrand to use full 64-bit output
- Added new --enable-intelrand option to indicate use of RDRAND preference for RNG source
- Removed RNG ARC4 support
- Added ECC helpers to get size and id from curve name.
- Added ECC Cofactor DH (ECC-CDH) support
- Added ECC private key only import / export functions.
- Added PKCS8 create function
- Improvements to TLS layer CTX handling for switching keys / certs.
- Added check for duplicate certificate policy OID in certificates.
- Normal math speed-up to not allocate on mp_int and defer until mp_grow
- Reduce heap usage with fast math when not using ALT_ECC_SIZE
- Fixes for building CRL with Windows
- Added support for inline CRL lookup when HAVE_CRL_IO is defined
- Added port for tenAsys INtime RTOS
- Improvements to uTKernel port (WOLFSSL_uTKERNEL2)
- Updated WPA Supplicant support
- Added support for Nginx
- Update stunnel port for version 5.40
- Fixes for STM32 hardware crypto acceleration
- Extended test code coverage in bundled test.c
- Added a sanity check for minimum authentication tag size with AES-GCM. Thanks to Yueh-Hsun Lin and Peng Li at KNOX Security at Samsung Research America for suggesting this.
- Added a sanity check that subject key identifier is marked as non-critical and a check that no policy OIDS appear more than once in the cert policies extension. Thanks to the report from Professor Zhenhua Duan, Professor Cong Tian, and Ph.D candidate Chu Chen from Institute of Computing Theory and Technology (ICTT) of Xidian University, China. Profs. Zhenhua Duan and Cong Tian are supervisors of Ph.D candidate Chu Chen.

This release of wolfSSL fixes 5 low and 1 medium level security vulnerability.

3 Low level fixes reported by Yueh-Hsun Lin and Peng Li from KNOX Security, Samsung Research America.
- Fix for out of bounds memory access in wc_DhParamsLoad() when GetLength() returns a zero. Before this fix there is a case where wolfSSL would read out of bounds memory in the function wc_DhParamsLoad.
- Fix for DH key accepted by wc_DhAgree when the key was malformed.
- Fix for a double free case when adding CA cert into X509_store.

Low level fix for memory management with static memory feature enabled. By default static memory is disabled. Thanks to GitHub user hajjihraf for reporting this.


Low level fix for out of bounds write in the function wolfSSL_X509_NAME_get_text_by_NID. This function is not used by TLS or crypto operations but could result in a buffer out of bounds write by one if called explicitly in an application. Discovered by Aleksandar Nikolic of Cisco Talos. http://talosintelligence.com/vulnerability-reports/

Medium level fix for check on certificate signature. There is a case in release versions 3.9.10, 3.10.0 and 3.10.2 where a corrupted signature on a peer certificate would not be properly flagged. Thanks to Wens Lo, James Tsai, Kenny Chang, and Oscar Yang at Castles Technology.


See INSTALL file for build instructions.
More info can be found on-line at http://wolfssl.com/wolfSSL/Docs.html


# wolfSSL (Formerly CyaSSL) Release 3.10.2 (2/10/2017)

## Release 3.10.2 of wolfSSL has bug fixes and new features including:

- Poly1305 Windows macros fix. Thanks to GitHub user Jay Satiro
- Compatibility layer expanded with multiple functions added
- Improve fp_copy performance with ALT_ECC_SIZE
- OCSP updates and improvements
- Fixes for IAR EWARM 8 compiler warnings
- Reduce stack usage with ECC_CACHE_CURVE disabled
- Added ECC export raw for public and private key
- Fix for NO_ASN_TIME build
- Supported curves extensions now populated by default
- Add DTLS build without big integer math
- Fix for static memory feature with wc_ecc_verify_hash_ex and not SHAMIR
- Added PSK interoperability testing to script bundled with wolfSSL
- Fix for Python wrapper random number generation. Compiler optimizations with Python could place the random number in same buffer location each time. Thanks to GitHub user Erik Bray (embray)
- Fix for tests on unaligned memory with static memory feature
- Add macro WOLFSSL_NO_OCSP_OPTIONAL_CERTS to skip optional OCSP certificates
- Sanity checks on NULL arguments added to wolfSSL_set_fd and wolfSSL_DTLS_SetCookieSecret
- mp_jacobi stack use reduced, thanks to Szabi Tolnai for providing a solution to reduce stack usage


This release of wolfSSL fixes 2 low and 1 medium level security vulnerability.

Low level fix of buffer overflow for when loading in a malformed temporary DH file. Thanks to Yueh-Hsun Lin and Peng Li from KNOX Security, Samsung Research America for the report.

Medium level fix for processing of OCSP response. If using OCSP without hard faults enforced and no alternate revocation checks like OCSP stapling then it is recommended to update.

Low level fix for potential cache attack on RSA operations. If using wolfSSL RSA on a server that other users can have access to monitor the cache, then it is recommended to update wolfSSL. Thanks to Andreas Zankl, Johann Heyszl and Georg Sigl at Fraunhofer AISEC for the initial report.

See INSTALL file for build instructions.
More info can be found on-line at http://wolfssl.com/wolfSSL/Docs.html


# wolfSSL (Formerly CyaSSL) Release 3.10.0 (12/21/2016)

## Release 3.10.0 of wolfSSL has bug fixes and new features including:

- Added support for SHA224
- Added scrypt feature
- Build for Intel SGX use, added in directory IDE/WIN-SGX
- Fix for ChaCha20-Poly1305 ECDSA certificate type request
- Enhance PKCS#7 with ECC enveloped data and AES key wrap support
- Added support for RIOT OS
- Add support for parsing PKCS#12 files
- ECC performance increased with custom curves
- ARMv8 expanded to AArch32 and performance increased
- Added ANSI-X9.63-KDF support
- Port to STM32 F2/F4 CubeMX
- Port to Atmel ATECC508A board
- Removed fPIE by default when wolfSSL library is compiled
- Update to Python wrapper, dropping DES and adding wc_RSASetRNG
- Added support for NXP K82 hardware acceleration
- Added SCR client and server verify check
- Added a disable rng option with autoconf
- Added more tests vectors to test.c with AES-CTR
- Updated DTLS session export version number
- Updated DTLS for 64 bit sequence numbers
- Fix for memory management with TI and WOLFSSL_SMALL_STACK
- Hardening RSA CRT to be constant time
- Fix uninitialized warning with IAR compiler
- Fix for C# wrapper example IO hang on unexpected connection termination


This release of wolfSSL fixes a low level security vulnerability. The vulnerability reported was a potential cache attack on RSA operations. If using wolfSSL RSA on a server that other users can have access to monitor the cache, then it is recommended to update wolfSSL. Thanks to Andreas Zankl, Johann Heyszl and Georg Sigl at Fraunhofer AISEC for the report. More information will be available on our site:

https://wolfssl.com/wolfSSL/security/vulnerabilities.php

See INSTALL file for build instructions.
More info can be found on-line at http://wolfssl.com/wolfSSL/Docs.html


# wolfSSL (Formerly CyaSSL) Release 3.9.10 (9/23/2016)

## Release 3.9.10 of wolfSSL has bug fixes and new features including:

- Default configure option changes:
  1. DES3 disabled by default
  2. ECC Supported Curves Extension enabled by default
  3. New option Extended Master Secret enabled by default
- Added checking CA certificate path length, and new test certs
- Fix to DSA pre padding and sanity check on R/S values
- Added CTX level RNG for single-threaded builds
- Intel RDSEED enhancements
- ARMv8 hardware acceleration support for AES-CBC/CTR/GCM, SHA-256
- Arduino support updates
- Added the Extended Master Secret TLS extension
  1. Enabled by default in configure options, API to disable
  2. Added support for Extended Master Secret to sniffer
- OCSP fix with issuer key hash, lookup refactor
- Added support for Frosted OS
- Added support for DTLS over SCTP
- Added support for static memory with wolfCrypt
- Fix to ECC Custom Curve support
- Support for asynchronous wolfCrypt RSA and TLS client
- Added distribution build configure option
- Update the test certificates

This release of wolfSSL fixes medium level security vulnerabilities.  Fixes for
potential AES, RSA, and ECC side channel leaks is included that a local user
monitoring the same CPU core cache could exploit.  VM users, hyper-threading
users, and users where potential attackers have access to the CPU cache will
need to update if they utilize AES, RSA private keys, or ECC private keys.
Thanks to Gorka Irazoqui Apecechea and Xiaofei Guo from Intel Corporation for
the report.  More information will be available on our site:

https://wolfssl.com/wolfSSL/security/vulnerabilities.php

See INSTALL file for build instructions.
More info can be found on-line at https://wolfssl.com/wolfSSL/Docs.html


# wolfSSL (Formerly CyaSSL) Release 3.9.8 (7/29/2016)

##Release 3.9.8 of wolfSSL has bug fixes and new features including:

- Add support for custom ECC curves.
- Add cipher suite ECDHE-ECDSA-AES128-CCM.
- Add compkey enable option. This option is for compressed ECC keys.
- Add in the option to use test.h without gettimeofday function using the macro
  WOLFSSL_USER_CURRTIME.
- Add RSA blinding for private key operations. Enable option of harden which is
  on by default. This negates timing attacks.
- Add ECC and TLS support for all SECP, Koblitz and Brainpool curves.
- Add helper functions for static memory option to allow getting optimum buffer
  sizes.
- Update DTLS behavior on bad MAC. DTLS silently drops packets with bad MACs now.
- Update fp_isprime function from libtom enhancement/cleanup repository.
- Update sanity checks on inputs and return values for AES-CMAC.
- Update wolfSSL for use with MYSQL v5.6.30.
- Update LPCXpresso eclipse project to not include misc.c when not needed.
- Fix retransmit of last DTLS flight with timeout notification. The last flight
  is no longer retransmitted on timeout.
- Fixes to some code in math sections for compressed ECC keys. This includes
  edge cases for buffer size on allocation and adjustments for compressed curves
  build. The code and full list can be found on github with pull request #456.
- Fix function argument mismatch for build with secure renegotiation.
- X.509 bug fixes for reading in malformed certificates, reported by researchers
  at Columbia University
- Fix GCC version 6 warning about hard tabs in poly1305.c. This was a warning
  produced by GCC 6 trying to determine the intent of code.
- Fixes for static memory option. Including avoid potential race conditions with
  counters, decrement handshake counter correctly.
- Fix anonymous cipher with Diffie Hellman on the server side. Was an issue of a
  possible buffer corruption. For information and code see pull request #481.


- One high level security fix that requires an update for use with static RSA
  cipher suites was submitted. This fix was the addition of RSA blinding for
  private RSA operations. We recommend servers who allow static RSA cipher
  suites to also generate new private RSA keys. Static RSA cipher suites are
  turned off by default.

See INSTALL file for build instructions.
More info can be found on-line at //http://wolfssl.com/wolfSSL/Docs.html

# wolfSSL (Formerly CyaSSL) Release 3.9.6 (6/14/2016)

##Release 3.9.6 of wolfSSL has bug fixes and new features including:

- Add staticmemory feature
- Add public wc_GetTime API with base64encode feature
- Add AES CMAC algorithm
- Add DTLS sessionexport feature
- Add python wolfCrypt wrapper
- Add ECC encrypt/decrypt benchmarks
- Add dynamic session tickets
- Add eccshamir option
- Add Whitewood netRandom support --with-wnr
- Add embOS port
- Add minimum key size checks for RSA and ECC
- Add STARTTLS support to examples
- Add uTasker port
- Add asynchronous crypto and wolf event support
- Add compile check for misc.c with inline
- Add RNG benchmark
- Add reduction to stack usage with hash-based RNG
- Update STM32F2_CRYPTO port with additional algorithms supported
- Update MDK5 projects
- Update AES-NI
- Fix for STM32 with STM32F2_HASH defined
- Fix for building with MinGw
- Fix ECC math bugs with ALT_ECC_SIZE and key sizes over 256 bit (1)
- Fix certificate buffers github issue #422
- Fix decrypt max size with RSA OAEP
- Fix DTLS sanity check with DTLS timeout notification
- Fix free of WOLFSSL_METHOD on failure to create CTX
- Fix memory leak in failure case with wc_RsaFunction (2)

- No high level security fixes that requires an update though we always
recommend updating to the latest
- (1) Code changes for ECC fix can be found at pull requests #411, #416, and #428
- (2) Builds using RSA with using normal math and not RSA_LOW_MEM should update

See INSTALL file for build instructions.
More info can be found on-line at //http://wolfssl.com/wolfSSL/Docs.html

# wolfSSL (Formerly CyaSSL) Release 3.9.0 (03/18/2016)

##Release 3.9.0 of wolfSSL has bug fixes and new features including:

- Add new leantls configuration
- Add RSA OAEP padding at wolfCrypt level
- Add Arduino port and example client
- Add fixed point DH operation
- Add CUSTOM_RAND_GENRATE_SEED_OS and CUSTOM_RAND_GENERATE_BLOCK
- Add ECDHE-PSK cipher suites
- Add PSK ChaCha20-Poly1305 cipher suites
- Add option for fail on no peer cert except PSK suites
- Add port for Nordic nRF51
- Add additional ECC NIST test vectors for 256, 384 and 521
- Add more granular ECC, Ed25519/Curve25519 and AES configs
- Update to ChaCha20-Poly1305
- Update support for Freescale KSDK 1.3.0
- Update DER buffer handling code, refactoring and reducing memory
- Fix to AESNI 192 bit key expansion
- Fix to C# wrapper character encoding
- Fix sequence number issue with DTLS epoch 0 messages
- Fix RNGA with K64 build
- Fix ASN.1 X509 V3 certificate policy extension parsing
- Fix potential free of uninitialized RSA key in asn.c
- Fix potential underflow when using ECC build with FP_ECC
- Fixes for warnings in Visual Studio 2015 build

- No high level security fixes that requires an update though we always
recommend updating to the latest
- FP_ECC is off by default, users with it enabled should update for the zero
sized hash fix

See INSTALL file for build instructions.
More info can be found on-line at //http://wolfssl.com/yaSSL/Docs.html

# wolfSSL (Formerly CyaSSL) Release 3.8.0 (12/30/2015)

##Release 3.8.0 of wolfSSL has bug fixes and new features including:

- Example client/server with VxWorks
- AESNI use with AES-GCM
- Stunnel compatibility enhancements
- Single shot hash and signature/verify API added
- Update cavium nitrox port
- LPCXpresso IDE support added
- C# wrapper to support wolfSSL use by a C# program
- (BETA version)OCSP stapling added
- Update OpenSSH compatibility
- Improve DTLS handshake when retransmitting finished message
- fix idea_mult() for 16 and 32bit systems
- fix LowResTimer on Microchip ports

- No high level security fixes that requires an update though we always
recommend updating to the latest

See INSTALL file for build instructions.
More info can be found on-line at //http://wolfssl.com/yaSSL/Docs.html

# wolfSSL (Formerly CyaSSL) Release 3.7.0 (10/26/2015)

##Release 3.7.0 of wolfSSL has bug fixes and new features including:

- ALPN extension support added for HTTP2 connections with --enable-alpn
- Change of example/client/client max fragment flag -L -> -F
- Throughput benchmarking, added scripts/benchmark.test
- Sniffer API ssl_FreeDecodeBuffer added
- Addition of AES_GCM to Sniffer
- Sniffer change to handle unlimited decrypt buffer size
- New option for the sniffer where it will try to pick up decoding after a
  sequence number acknowldgement fault. Also includes some additional stats.
- JNI API setter and getter function for jobject added
- User RSA crypto plugin abstraction. An example placed in wolfcrypt/user-crypto
- fix to asn configuration bug
- AES-GCM/CCM fixes.
- Port for Rowley added
- Rowley Crossworks bare metal examples added
- MDK5-ARM project update
- FreeRTOS support updates.
- VXWorks support updates.
- Added the IDEA cipher and support in wolfSSL.
- Update wolfSSL website CA.
- CFLAGS is usable when configuring source.

- No high level security fixes that requires an update though we always
recommend updating to the latest

See INSTALL file for build instructions.
More info can be found on-line at //http://wolfssl.com/yaSSL/Docs.html

#wolfSSL (Formerly CyaSSL) Release 3.6.8 (09/17/2015)

##Release 3.6.8 of wolfSSL fixes two high severity vulnerabilities.
##It also includes bug fixes and new features including:

- Two High level security fixes, all users SHOULD update.
  a) If using wolfSSL for DTLS on the server side of a publicly accessible
     machine you MUST update.
  b) If using wolfSSL for TLS on the server side with private RSA keys allowing
     ephemeral key exchange without low memory optimizations you MUST update and
     regenerate the private RSA keys.

     Please see https://www.wolfssl.com/wolfSSL/Blog/Blog.html for more details

- No filesystem build fixes for various configurations
- Certificate generation now supports several extensions including KeyUsage,
    SKID, AKID, and Certificate Policies
- CRLs can be loaded from buffers as well as files now
- SHA-512 Certificate Signing generation
- Fixes for sniffer reassembly processing

See INSTALL file for build instructions.
More info can be found on-line at //http://wolfssl.com/yaSSL/Docs.html

#wolfSSL (Formerly CyaSSL) Release 3.6.6 (08/20/2015)

##Release 3.6.6 of wolfSSL has bug fixes and new features including:

- OpenSSH  compatibility with --enable-openssh
- stunnel  compatibility with --enable-stunnel
- lighttpd compatibility with --enable-lighty
- SSLv3 is now disabled by default, can be enabled with --enable-sslv3
- Ephemeral key cipher suites only are now supported by default
    To enable static ECDH cipher suites define WOLFSSL_STATIC_DH
    To enable static  RSA cipher suites define WOLFSSL_STATIC_RSA
    To enable static  PSK cipher suites define WOLFSSL_STATIC_PSK
- Added QSH (quantum-safe handshake) extension with --enable-ntru
- SRP is now part of wolfCrypt, enable with --enabe-srp
- Certificate handshake messages can now be sent fragmented if the record
  size is smaller than the total message size, no user action required.
- DTLS duplicate message fixes
- Visual Studio project files now support DLL and static builds for 32/64bit.
- Support for new Freesacle I/O
- FreeRTOS FIPS support

- No high level security fixes that requires an update though we always
  recommend updating to the latest

See INSTALL file for build instructions.
More info can be found on-line at //http://wolfssl.com/yaSSL/Docs.html


#wolfSSL (Formerly CyaSSL) Release 3.6.0 (06/19/2015)

##Release 3.6.0 of wolfSSL has bug fixes and new features including:

- Max Strength build that only allows TLSv1.2, AEAD ciphers, and PFS (Perfect
   Forward Secrecy).  With --enable-maxstrength
- Server side session ticket support, the example server and echosever use the
   example callback myTicketEncCb(), see wolfSSL_CTX_set_TicketEncCb()
- FIPS version submitted for iOS.
- TI Crypto Hardware Acceleration
- DTLS fragmentation fixes
- ECC key check validation with wc_ecc_check_key()
- 32bit code options to reduce memory for Curve25519 and Ed25519
- wolfSSL JNI build switch with --enable-jni
- PicoTCP support improvements
- DH min ephemeral key size enforcement with wolfSSL_CTX_SetMinDhKey_Sz()
- KEEP_PEER_CERT and AltNames can now be used together
- ChaCha20 big endian fix
- SHA-512 signature algorithm support for key exchange and verify messages
- ECC make key crash fix on RNG failure, ECC users must update.
- Improvements to usage of time code.
- Improvements to VS solution files.
- GNU Binutils 2.24 ld has problems with some debug builds, to fix an ld error
  add -fdebug-types-section to C_EXTRA_FLAGS

- No high level security fixes that requires an update though we always
  recommend updating to the latest (except note 14, ecc RNG failure)

See INSTALL file for build instructions.
More info can be found on-line at //http://wolfssl.com/yaSSL/Docs.html


#wolfSSL (Formerly CyaSSL) Release 3.4.8 (04/06/2015)

##Release 3.4.8 of wolfSSL has bug fixes and new features including:

- FIPS version submitted for iOS.
- Max Strength build that only allows TLSv1.2, AEAD ciphers, and PFS.
- Improvements to usage of time code.
- Improvements to VS solution files.

See INSTALL file for build instructions.
More info can be found on-line at //http://wolfssl.com/yaSSL/Docs.html


#wolfSSL (Formerly CyaSSL) Release 3.4.6 (03/30/2015)

##Release 3.4.6 of wolfSSL has bug fixes and new features including:

- Intel Assembly Speedups using instructions rdrand, rdseed, aesni, avx1/2,
  rorx, mulx, adox, adcx .  They can be enabled with --enable-intelasm.
  These speedup the use of RNG, SHA2, and public key algorithms.
- Ed25519 support at the crypto level. Turn on with --enable-ed25519.  Examples
  in wolcrypt/test/test.c ed25519_test().
- Post Handshake Memory reductions.  wolfSSL can now hold less than 1,000 bytes
  of memory per secure connection including cipher state.
- wolfSSL API and wolfCrypt API fixes, you can still include the cyassl and
  ctaocrypt headers which will enable the compatibility APIs for the
  foreseeable future
- INSTALL file to help direct users to build instructions for their environment
- For ECC users with the normal math library a fix that prevents a crash when
  verify signature fails.  Users of 3.4.0 with ECC and the normal math library
  must update
- RC4 is now disabled by default in autoconf mode
- AES-GCM and ChaCha20/Poly1305 are now enabled by default to make AEAD ciphers
  available without a switch
- External ChaCha-Poly AEAD API, thanks to Andrew Burks for the contribution
- DHE-PSK cipher suites can now be built without ASN or Cert support
- Fix some NO MD5 build issues with optional features
- Freescale CodeWarrior project updates
- ECC curves can be individually turned on/off at build time.
- Sniffer handles Cert Status message and other minor fixes
- SetMinVersion() at the wolfSSL Context level instead of just SSL session level
  to allow minimum protocol version allowed at runtime
- RNG failure resource cleanup fix

- No high level security fixes that requires an update though we always
  recommend updating to the latest (except note 6 use case of ecc/normal math)

See INSTALL file for build instructions.
More info can be found on-line at //http://wolfssl.com/yaSSL/Docs.html


#wolfSSL (Formerly CyaSSL) Release 3.4.0 (02/23/2015)

## Release 3.4.0 wolfSSL has bug fixes and new features including:

- wolfSSL API and wolfCrypt API, you can still include the cyassl and ctaocrypt
  headers which will enable the compatibility APIs for the foreseeable future
- Example use of the wolfCrypt API can be found in wolfcrypt/test/test.c
- Example use of the wolfSSL API can be found in examples/client/client.c
- Curve25519 now supported at the wolfCrypt level, wolfSSL layer coming soon
- Improvements in the build configuration under AIX
- Microchip Pic32 MZ updates
- TIRTOS updates
- PowerPC updates
- Xcode project update
- Bidirectional shutdown examples in client/server with -w (wait for full
  shutdown) option
- Cycle counts on benchmarks for x86_64, more coming soon
- ALT_ECC_SIZE for reducing ecc heap use with fastmath when also using large RSA
  keys
- Various compile warnings
- Scan-build warning fixes
- Changed a memcpy to memmove in the sniffer (if using sniffer please update)
- No high level security fixes that requires an update though we always
  recommend updating to the latest


# CyaSSL Release 3.3.0 (12/05/2014)

- Countermeasuers for Handshake message duplicates, CHANGE CIPHER without
  FINISHED, and fast forward attempts.  Thanks to Karthikeyan Bhargavan from
  the Prosecco team at INRIA Paris-Rocquencourt for the report.
- FIPS version submitted
- Removes SSLv2 Client Hello processing, can be enabled with OLD_HELLO_ALLOWED
- User can set minimum downgrade version with CyaSSL_SetMinVersion()
- Small stack improvements at TLS/SSL layer
- TLS Master Secret generation and Key Expansion are now exposed
- Adds client side Secure Renegotiation, * not recommended *
- Client side session ticket support, not fully tested with Secure Renegotiation
- Allows up to 4096bit DHE at TLS Key Exchange layer
- Handles non standard SessionID sizes in Hello Messages
- PicoTCP Support
- Sniffer now supports SNI Virtual Hosts
- Sniffer now handles non HTTPS protocols using STARTTLS
- Sniffer can now parse records with multiple messages
- TI-RTOS updates
- Fix for ColdFire optimized fp_digit read only in explicit 32bit case
- ADH Cipher Suite ADH-AES128-SHA for EAP-FAST

The CyaSSL manual is available at:
http://www.wolfssl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.


# CyaSSL Release 3.2.0 (09/10/2014)

#### Release 3.2.0 CyaSSL has bug fixes and new features including:

- ChaCha20 and Poly1305 crypto and suites
- Small stack improvements for OCSP, CRL, TLS, DTLS
- NTRU Encrypt and Decrypt benchmarks
- Updated Visual Studio project files
- Updated Keil MDK5 project files
- Fix for DTLS sequence numbers with GCM/CCM
- Updated HashDRBG with more secure struct declaration
- TI-RTOS support and example Code Composer Studio project files
- Ability to get enabled cipher suites, CyaSSL_get_ciphers()
- AES-GCM/CCM/Direct support for Freescale mmCAU and CAU
- Sniffer improvement checking for decrypt key setup
- Support for raw ECC key import
- Ability to convert ecc_key to DER, EccKeyToDer()
- Security fix for RSA Padding check vulnerability reported by Intel Security
  Advanced Threat Research team

The CyaSSL manual is available at:
http://www.wolfssl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.


# CyaSSL Release 3.1.0 (07/14/2014)

#### Release 3.1.0 CyaSSL has bug fixes and new features including:

- Fix for older versions of icc without 128-bit type
- Intel ASM syntax for AES-NI
- Updated NTRU support, keygen benchmark
- FIPS check for minimum required HMAC key length
- Small stack (--enable-smallstack) improvements for PKCS#7, ASN
- TLS extension support for DTLS
- Default I/O callbacks external to user
- Updated example client with bad clock test
- Ability to set optional ECC context info
- Ability to enable/disable DH separate from opensslextra
- Additional test key/cert buffers for CA and server
- Updated example certificates

The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.


# CyaSSL Release 3.0.2 (05/30/2014)

#### Release 3.0.2 CyaSSL has bug fixes and new features including:

- Added the following cipher suites:
  * TLS_PSK_WITH_AES_128_GCM_SHA256
  * TLS_PSK_WITH_AES_256_GCM_SHA384
  * TLS_PSK_WITH_AES_256_CBC_SHA384
  * TLS_PSK_WITH_NULL_SHA384
  * TLS_DHE_PSK_WITH_AES_128_GCM_SHA256
  * TLS_DHE_PSK_WITH_AES_256_GCM_SHA384
  * TLS_DHE_PSK_WITH_AES_128_CBC_SHA256
  * TLS_DHE_PSK_WITH_AES_256_CBC_SHA384
  * TLS_DHE_PSK_WITH_NULL_SHA256
  * TLS_DHE_PSK_WITH_NULL_SHA384
  * TLS_DHE_PSK_WITH_AES_128_CCM
  * TLS_DHE_PSK_WITH_AES_256_CCM
- Added AES-NI support for Microsoft Visual Studio builds.
- Changed small stack build to be disabled by default.
- Updated the Hash DRBG and provided a configure option to enable.

The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.


# CyaSSL Release 3.0.0 (04/29/2014)

#### Release 3.0.0 CyaSSL has bug fixes and new features including:

- FIPS release candidate
- X.509 improvements that address items reported by Suman Jana with security
  researchers at UT Austin and UC Davis
- Small stack size improvements, --enable-smallstack. Offloads large local
  variables to the heap. (Note this is not complete.)
- Updated AES-CCM-8 cipher suites to use approved suite numbers.

The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.


# CyaSSL Release 2.9.4 (04/09/2014)

#### Release 2.9.4 CyaSSL has bug fixes and new features including:

- Security fixes that address items reported by Ivan Fratric of the Google
  Security Team
- X.509 Unknown critical extensions treated as errors, report by Suman Jana with
  security researchers at UT Austin and UC Davis
- Sniffer fixes for corrupted packet length and Jumbo frames
- ARM thumb mode assembly fixes
- Xcode 5.1 support including new clang
- PIC32 MZ hardware support
- CyaSSL Object has enough room to read the Record Header now w/o allocs
- FIPS wrappers for AES, 3DES, SHA1, SHA256, SHA384, HMAC, and RSA.
- A sample I/O pool is demonstrated with --enable-iopool to overtake memory
  handling and reduce memory fragmentation on I/O large sizes

The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.


# CyaSSL Release 2.9.0 (02/07/2014)

#### Release 2.9.0 CyaSSL has bug fixes and new features including:
- Freescale Kinetis RNGB support
- Freescale Kinetis mmCAU support
- TLS Hello extensions
  - ECC
  - Secure Renegotiation (null)
  - Truncated HMAC
- SCEP support
  - PKCS #7 Enveloped data and signed data
  - PKCS #10 Certificate Signing Request generation
- DTLS sliding window
- OCSP Improvements
  - API change to integrate into Certificate Manager
  - IPv4/IPv6 agnostic
  - example client/server support for OCSP
  - OCSP nonces are optional
- GMAC hashing
- Windows build additions
- Windows CYGWIN build fixes
- Updated test certificates
- Microchip MPLAB Harmony support
- Update autoconf scripts
- Additional X.509 inspection functions
- ECC encrypt/decrypt primitives
- ECC Certificate generation

The Freescale Kinetis K53 RNGB documentation can be found in Chapter 33 of the
K53 Sub-Family Reference Manual:
http://cache.freescale.com/files/32bit/doc/ref_manual/K53P144M100SF2RM.pdf

Freescale Kinetis K60 mmCAU (AES, DES, 3DES, MD5, SHA, SHA256) documentation
can be found in the "ColdFire/ColdFire+ CAU and Kinetis mmCAU Software Library
User Guide":
http://cache.freescale.com/files/32bit/doc/user_guide/CAUAPIUG.pdf


# CyaSSL Release 2.8.0 (8/30/2013)

#### Release 2.8.0 CyaSSL has bug fixes and new features including:
- AES-GCM and AES-CCM use AES-NI
- NetX default IO callback handlers
- IPv6 fixes for DTLS Hello Cookies
- The ability to unload Certs/Keys after the handshake, CyaSSL_UnloadCertsKeys()
- SEP certificate extensions
- Callback getters for easier resource freeing
- External CYASSL_MAX_ERROR_SZ for correct error buffer sizing
- MacEncrypt and DecryptVerify Callbacks for User Atomic Record Layer Processing
- Public Key Callbacks for ECC and RSA
- Client now sends blank cert upon request if doesn't have one with TLS <= 1.2


The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.


# CyaSSL Release 2.7.0 (6/17/2013)

#### Release 2.7.0 CyaSSL has bug fixes and new features including:
- SNI support for client and server
- KEIL MDK-ARM projects
- Wildcard check to domain name match, and Subject altnames are checked too
- Better error messages for certificate verification errors
- Ability to discard session during handshake verify
- More consistent error returns across all APIs
- Ability to unload CAs at the CTX or CertManager level
- Authority subject id support for Certificate matching
- Persistent session cache functionality
- Persistent CA cache functionality
- Client session table lookups to push serverID table to library level
- Camellia support to sniffer
- User controllable settings for DTLS timeout values
- Sniffer fixes for caching long lived sessions
- DTLS reliability enhancements for the handshake
- Better ThreadX support

When compiling with Mingw, libtool may give the following warning due to
path conversion errors:

```
libtool: link: Could not determine host file name corresponding to **
libtool: link: Continuing, but uninstalled executables may not work.
```

If so, examples and testsuite will have problems when run, showing an
error while loading shared libraries. To resolve, please run "make install".

The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.


# CyaSSL Release 2.6.0 (04/15/2013)

#### Release 2.6.0 CyaSSL has bug fixes and new features including:
- DTLS 1.2 support including AEAD ciphers
- SHA-3 finalist Blake2 support, it's fast and uses little resources
- SHA-384 cipher suites including ECC ones
- HMAC now supports SHA-512
- Track memory use for example client/server with -t option
- Better IPv6 examples with --enable-ipv6, before if ipv6 examples/tests were
  turned on, localhost only was used.  Now link-local (with scope ids) and ipv6
  hosts can be used as well.
- Xcode v4.6 project for iOS v6.1 update
- settings.h is now checked in all *.c files for true one file setting detection
- Better alignment at SSL layer for hardware crypto alignment needs
    * Note, SSL itself isn't friendly to alignment with 5 byte TLS headers and
      13 bytes DTLS headers, but every effort is now made to align with the
      CYASSL_GENERAL_ALIGNMENT flag which sets desired alignment requirement
- NO_64BIT flag to turn off 64bit data type accumulators in public key code
    * Note, some systems are faster with 32bit accumulators
- --enable-stacksize for example client/server stack use
    * Note, modern desktop Operating Systems may add bytes to each stack frame
- Updated compression/decompression with direct crypto access
- All ./configure options are now lowercase only for consistency
- ./configure builds default to fastmath option
    * Note, if on ia32 and building in shared mode this may produce a problem
      with a missing register being available because of PIC, there are at least
      6 solutions to this:
      1) --disable-fastmath , don't use fastmath
      2) --disable-shared, don't build a shared library
      3) C_EXTRA_FLAGS=-DTFM_NO_ASM , turn off assembly use
      4) use clang, it just seems to work
      5) play around with no PIC options to force all registers being open,
         e.g., --without-pic
      6) if static lib is still a problem try removing fPIE
- Many new ./configure switches for option enable/disable for example
    * rsa
    * dh
    * dsa
    * md5
    * sha
    * arc4
    * null    (allow NULL ciphers)
    * oldtls  (only use TLS 1.2)
    * asn     (no certs or public keys allowed)
- ./configure generates cyassl/options.h which allows a header the user can
  include in their app to make sure the same options are set at the app and
  CyaSSL level.
- autoconf no longer needs serial-tests which lowers version requirements of
  automake to 1.11 and autoconf to 2.63

The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.



# CyaSSL Release 2.5.0 (02/04/2013)

#### Release 2.5.0 CyaSSL has bug fixes and new features including:
- Fix for TLS CBC padding timing attack identified by Nadhem Alfardan and
  Kenny Paterson: http://www.isg.rhul.ac.uk/tls/
- Microchip PIC32 (MIPS16, MIPS32) support
- Microchip MPLAB X example projects for PIC32 Ethernet Starter Kit
- Updated CTaoCrypt benchmark app for embedded systems
- 1024-bit test certs/keys and cert/key buffers
- AES-CCM-8 crypto and cipher suites
- Camellia crypto and cipher suites
- Bumped minimum autoconf version to 2.65, automake version to 1.12
- Addition of OCSP callbacks
- STM32F2 support with hardware crypto and RNG
- Cavium NITROX support

CTaoCrypt now has support for the Microchip PIC32 and has been tested with
the Microchip PIC32 Ethernet Starter Kit, the XC32 compiler and
MPLAB X IDE in both MIPS16 and MIPS32 instruction set modes. See the README
located under the <cyassl_root>/mplabx directory for more details.

To add Cavium NITROX support do:

./configure --with-cavium=/home/user/cavium/software

pointing to your licensed cavium/software directory.  Since Cavium doesn't
build a library we pull in the cavium_common.o file which gives a libtool
warning about the portability of this.  Also, if you're using the github source
tree you'll need to remove the -Wredundant-decls warning from the generated
Makefile because the cavium headers don't conform to this warning.  Currently
CyaSSL supports Cavium RNG, AES, 3DES, RC4, HMAC, and RSA directly at the crypto
layer.  Support at the SSL level is partial and currently just does AES, 3DES,
and RC4.  RSA and HMAC are slower until the Cavium calls can be utilized in non
blocking mode.  The example client turns on cavium support as does the crypto
test and benchmark.  Please see the HAVE_CAVIUM define.

CyaSSL is able to use the STM32F2 hardware-based cryptography and random number
generator through the STM32F2 Standard Peripheral Library. For necessary
defines, see the CYASSL_STM32F2 define in settings.h. Documentation for the
STM32F2 Standard Peripheral Library can be found in the following document:
http://www.st.com/internet/com/TECHNICAL_RESOURCES/TECHNICAL_LITERATURE/USER_MANUAL/DM00023896.pdf

The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.



# CyaSSL Release 2.4.6 (12/20/2012)

#### Release 2.4.6 CyaSSL has bug fixes and a few new features including:
- ECC into main version
- Lean PSK build (reduced code size, RAM usage, and stack usage)
- FreeBSD CRL monitor support
- CyaSSL_peek()
- CyaSSL_send() and CyaSSL_recv() for I/O flag setting
- CodeWarrior Support
- MQX Support
- Freescale Kinetis support including Hardware RNG
- autoconf builds use jobserver
- cyassl-config
- Sniffer memory reductions

Thanks to Brian Aker for the improved autoconf system, make rpm, cyassl-config,
warning system, and general good ideas for improving CyaSSL!

The Freescale Kinetis K70 RNGA documentation can be found in Chapter 37 of the
K70 Sub-Family Reference Manual:
http://cache.freescale.com/files/microcontrollers/doc/ref_manual/K70P256M150SF3RM.pdf

The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.


# CyaSSL Release 2.4.0 (10/10/2012)

#### Release 2.4.0 CyaSSL has bug fixes and a few new features including:
- DTLS reliability
- Reduced memory usage after handshake
- Updated build process

The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.



# CyaSSL Release 2.3.0 (8/10/2012)

#### Release 2.3.0 CyaSSL has bug fixes and a few new features including:
- AES-GCM crypto and cipher suites
- make test cipher suite checks
- Subject AltName processing
- Command line support for client/server examples
- Sniffer SessionTicket support
- SHA-384 cipher suites
- Verify cipher suite validity when user overrides
- CRL dir monitoring
- DTLS Cookie support, reliability coming soon

The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.



# CyaSSL Release 2.2.0 (5/18/2012)

#### Release 2.2.0 CyaSSL has bug fixes and a few new features including:
- Initial CRL support (--enable-crl)
- Initial OCSP support (--enable-ocsp)
- Add static ECDH suites
- SHA-384 support
- ECC client certificate support
- Add medium session cache size (1055 sessions)
- Updated unit tests
- Protection against mutex reinitialization


The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.



# CyaSSL Release 2.0.8 (2/24/2012)

#### Release 2.0.8 CyaSSL has bug fixes and a few new features including:
- A fix for malicious certificates pointed out by Remi Gacogne (thanks)
  resulting in NULL pointer use.
- Respond to renegotiation attempt with no_renegoatation alert
- Add basic path support for load_verify_locations()
- Add set Temp EC-DHE key size
- Extra checks on rsa test when porting into


The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.



# CyaSSL Release 2.0.6 (1/27/2012)

#### Release 2.0.6 CyaSSL has bug fixes and a few new features including:
- Fixes for CA basis constraint check
- CTX reference counting
- Initial unit test additions
- Lean and Mean Windows fix
- ECC benchmarking
- SSMTP build support
- Ability to group handshake messages with set_group_messages(ctx/ssl)
- CA cache addition callback
- Export Base64_Encode for general use

The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.



# CyaSSL Release 2.0.2 (12/05/2011)

#### Release 2.0.2 CyaSSL has bug fixes and a few new features including:
- CTaoCrypt Runtime library detection settings when directly using the crypto
  library
- Default certificate generation now uses SHAwRSA and adds SHA256wRSA generation
- All test certificates now use 2048bit and SHA-1 for better modern browser
  support
- Direct AES block access and AES-CTR (counter) mode
- Microchip pic32 support

The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.



# CyaSSL Release 2.0.0rc3 (9/28/2011)

#### Release 2.0.0rc3 for CyaSSL has bug fixes and a few new features including:
- updated autoconf support
- better make install and uninstall  (uses system directories)
- make test / make check
- CyaSSL headers now in <cyassl/*.h>
- CTaocrypt headers now in <cyassl/ctaocrypt/*.h>
- OpenSSL compatibility headers now in <cyassl/openssl/*.h>
- examples and tests all run from home directory so can use certs in ./certs
        (see note 1)

So previous applications that used the OpenSSL compatibility header
<openssl/ssl.h> now need to include <cyassl/openssl/ssl.h> instead, no other
changes are required.

Special Thanks to Brian Aker for his autoconf, install, and header patches.

The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.

# CyaSSL Release 2.0.0rc2 (6/6/2011)

#### Release 2.0.0rc2 for CyaSSL has bug fixes and a few new features including:
- bug fixes (Alerts, DTLS with DHE)
- FreeRTOS support
- lwIP support
- Wshadow warnings removed
- asn public header
- CTaoCrypt public headers now all have ctc_ prefix (the manual is still being
        updated to reflect this change)
- and more.

This is the 2nd and perhaps final release candidate for version 2.
Please send any comments or questions to support@yassl.com.

The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.

# CyaSSL Release 2.0.0rc1 (5/2/2011)

#### Release 2.0.0rc1 for CyaSSL has many new features including:
- bug fixes
- SHA-256 cipher suites
- Root Certificate Verification (instead of needing all certs in the chain)
- PKCS #8 private key encryption (supports PKCS #5 v1-v2 and PKCS #12)
- Serial number retrieval for x509
- PBKDF2 and PKCS #12 PBKDF
- UID parsing for x509
- SHA-256 certificate signatures
- Client and server can send chains (SSL_CTX_use_certificate_chain_file)
- CA loading can now parse multiple certificates per file
- Dynamic memory runtime hooks
- Runtime hooks for logging
- EDH on server side
- More informative error codes
- More informative logging messages
- Version downgrade more robust (use SSL_v23*)
- Shared build only by default through ./configure
- Compiler visibility is now used, internal functions not polluting namespace
- Single Makefile, no recursion, for faster and simpler building
- Turn on all warnings possible build option, warning fixes
- and more.

Because of all the new features and the multiple OS, compiler, feature-set
options that CyaSSL allows, there may be some configuration fixes needed.
Please send any comments or questions to support@yassl.com.

The CyaSSL manual is available at:
http://www.yassl.com/documentation/CyaSSL-Manual.pdf.  For build instructions
and comments about the new features please check the manual.

# CyaSSL Release 1.9.0 (3/2/2011)

Release 1.9.0 for CyaSSL adds bug fixes, improved TLSv1.2 through testing and
better hash/sig algo ids, --enable-webServer for the yaSSL embedded web server,
improper AES key setup detection, user cert verify callback improvements, and
more.

The CyaSSL manual offering is included in the doc/ directory.  For build
instructions and comments about the new features please check the manual.

Please send any comments or questions to support@yassl.com.

# CyaSSL Release 1.8.0 (12/23/2010)

Release 1.8.0 for CyaSSL adds bug fixes, x509 v3 CA signed certificate
generation, a C standard library abstraction layer, lower memory use, increased
portability through the os_settings.h file, and the ability to use NTRU cipher
suites when used in conjunction with an NTRU license and library.

The initial CyaSSL manual offering is included in the doc/ directory.  For
build instructions and comments about the new features please check the manual.

Please send any comments or questions to support@yassl.com.

Happy Holidays.


# CyaSSL Release 1.6.5 (9/9/2010)

Release 1.6.5 for CyaSSL adds bug fixes and x509 v3 self signed certificate
generation.

For general build instructions see doc/Building_CyaSSL.pdf.

To enable certificate generation support add this option to ./configure
./configure --enable-certgen

An example is included in ctaocrypt/test/test.c and documentation is provided
in doc/CyaSSL_Extensions_Reference.pdf item 11.

# CyaSSL Release 1.6.0 (8/27/2010)

Release 1.6.0 for CyaSSL adds bug fixes, RIPEMD-160, SHA-512, and RSA key
generation.

For general build instructions see doc/Building_CyaSSL.pdf.

To add RIPEMD-160 support add this option to ./configure
./configure --enable-ripemd

To add SHA-512 support add this option to ./configure
./configure --enable-sha512

To add RSA key generation support add this option to ./configure
./configure --enable-keygen

Please see ctaocrypt/test/test.c for examples and usage.

For Windows, RIPEMD-160 and SHA-512 are enabled by default but key generation is
off by default.  To turn key generation on add the define CYASSL_KEY_GEN to
CyaSSL.


# CyaSSL Release 1.5.6 (7/28/2010)

Release 1.5.6 for CyaSSL adds bug fixes, compatibility for our JSSE provider,
and a fix for GCC builds on some systems.

For general build instructions see doc/Building_CyaSSL.pdf.

To add AES-NI support add this option to ./configure
./configure --enable-aesni

You'll need GCC 4.4.3 or later to make use of the assembly.

# CyaSSL Release 1.5.4 (7/7/2010)

Release 1.5.4 for CyaSSL adds bug fixes, support for AES-NI, SHA1 speed
improvements from loop unrolling, and support for the Mongoose Web Server.

For general build instructions see doc/Building_CyaSSL.pdf.

To add AES-NI support add this option to ./configure
./configure --enable-aesni

You'll need GCC 4.4.3 or later to make use of the assembly.

# CyaSSL Release 1.5.0 (5/11/2010)

Release 1.5.0 for CyaSSL adds bug fixes, GoAhead WebServer support, sniffer
support, and initial swig interface support.

For general build instructions see doc/Building_CyaSSL.pdf.

To add support for GoAhead WebServer either --enable-opensslExtra or if you
don't want all the features of opensslExtra you can just define GOAHEAD_WS
instead.  GOAHEAD_WS can be added to ./configure with CFLAGS=-DGOAHEAD_WS or
you can define it yourself.

To look at the sniffer support please see the sniffertest app in
sslSniffer/sslSnifferTest.  Build with --enable-sniffer on *nix or use the
vcproj files on windows.  You'll need to have pcap installed on *nix and
WinPcap on windows.

A swig interface file is now located in the swig directory for using Python,
Java, Perl, and others with CyaSSL.  This is initial support and experimental,
please send questions or comments to support@yassl.com.

When doing load testing with CyaSSL, on the echoserver example say, the client
machine may run out of tcp ephemeral ports, they will end up in the TIME_WAIT
queue, and can't be reused by default.  There are generally two ways to fix
this.

1. Reduce the length sockets remain on the TIME_WAIT queue OR
2. Allow items on the TIME_WAIT queue to be reused.


To reduce the TIME_WAIT length in OS X to 3 seconds (3000 milliseconds)

`sudo sysctl -w net.inet.tcp.msl=3000`

In Linux

`sudo sysctl -w net.ipv4.tcp_tw_reuse=1`

allows reuse of sockets in TIME_WAIT

`sudo sysctl -w net.ipv4.tcp_tw_recycle=1`

works but seems to remove sockets from  TIME_WAIT entirely?

`sudo sysctl -w net.ipv4.tcp_fin_timeout=1`

doesn't control TIME_WAIT, it controls FIN_WAIT(2) contrary to some posts


# CyaSSL Release 1.4.0 (2/18/2010)

Release 1.3.0 for CyaSSL adds bug fixes, better multi TLS/SSL version support
through SSLv23_server_method(), and improved documentation in the doc/ folder.

For general build instructions doc/Building_CyaSSL.pdf.

# CyaSSL Release 1.3.0 (1/21/2010)

Release 1.3.0 for CyaSSL adds bug fixes, a potential security problem fix,
better porting support, removal of assert()s, and a complete THREADX port.

For general build instructions see rc1 below.

# CyaSSL Release 1.2.0 (11/2/2009)

Release 1.2.0 for CyaSSL adds bug fixes and session negotiation if first use is
read or write.

For general build instructions see rc1 below.

# CyaSSL Release 1.1.0 (9/2/2009)

Release 1.1.0 for CyaSSL adds bug fixes, a check against malicious session
cache use, support for lighttpd, and TLS 1.2.

To get TLS 1.2 support please use the client and server functions:

```c
SSL_METHOD *TLSv1_2_server_method(void);
SSL_METHOD *TLSv1_2_client_method(void);
```

CyaSSL was tested against lighttpd 1.4.23.  To build CyaSSL for use with
lighttpd use the following commands from the CyaSSL install dir <CyaSSLDir>:

```
./configure --disable-shared --enable-opensslExtra --enable-fastmath --without-zlib

make
make openssl-links
```

Then to build lighttpd with CyaSSL use the following commands from the
lighttpd install dir:

```
./configure --with-openssl --with-openssl-includes=<CyaSSLDir>/include --with-openssl-libs=<CyaSSLDir>/lib LDFLAGS=-lm

make
```

On some systems you may get a linker error about a duplicate symbol for
MD5_Init or other MD5 calls.  This seems to be caused by the lighttpd src file
md5.c, which defines MD5_Init(), and is included in liblightcomp_la-md5.o.
When liblightcomp is linked with the SSL_LIBs the linker may complain about
the duplicate symbol.  This can be fixed by editing the lighttpd src file md5.c
and adding this line to the beginning of the file:

\#if 0

and this line to the end of the file

\#endif

Then from the lighttpd src dir do a:

```
make clean
make
```

If you get link errors about undefined symbols more than likely the actual
OpenSSL libraries are found by the linker before the CyaSSL openssl-links that
point to the CyaSSL library, causing the linker confusion.  This can be fixed
by editing the Makefile in the lighttpd src directory and changing the line:

`SSL_LIB = -lssl -lcrypto`

to

`SSL_LIB = -lcyassl`

Then from the lighttpd src dir do a:

```
make clean
make
```

This should remove any confusion the linker may be having with missing symbols.

For any questions or concerns please contact support@yassl.com .

For general build instructions see rc1 below.

# CyaSSL Release 1.0.6 (8/03/2009)

Release 1.0.6 for CyaSSL adds bug fixes, an improved session cache, and faster
math with a huge code option.

The session cache now defaults to a client mode, also good for embedded servers.
For servers not under heavy load (less than 200 new sessions per minute), define
BIG_SESSION_CACHE.  If the server will be under heavy load, define
HUGE_SESSION_CACHE.

There is now a fasthugemath option for configure.  This enables fastmath plus
even faster math by greatly increasing the code size of the math library. Use
the benchmark utility to compare public key operations.


For general build instructions see rc1 below.

# CyaSSL Release 1.0.3 (5/10/2009)

Release 1.0.3 for CyaSSL adds bug fixes and add increased support for OpenSSL
compatibility when building other applications.

Release 1.0.3 includes an alpha release of DTLS for both client and servers.
This is only for testing purposes at this time.  Rebroadcast and reordering
aren't fully implemented at this time but will be for the next release.

For general build instructions see rc1 below.

# CyaSSL Release 1.0.2 (4/3/2009)

Release 1.0.2 for CyaSSL adds bug fixes for a couple I/O issues.  Some systems
will send a SIGPIPE on socket recv() at any time and this should be handled by
the application by turning off SIGPIPE through setsockopt() or returning from
the handler.

Release 1.0.2 includes an alpha release of DTLS for both client and servers.
This is only for testing purposes at this time.  Rebroadcast and reordering
aren't fully implemented at this time but will be for the next release.

For general build instructions see rc1 below.

## CyaSSL Release Candidate 3 rc3-1.0.0 (2/25/2009)


Release Candidate 3 for CyaSSL 1.0.0 adds bug fixes and adds a project file for
iPhone development with Xcode.  cyassl-iphone.xcodeproj is located in the root
directory.  This release also includes a fix for supporting other
implementations that bundle multiple messages at the record layer, this was
lost when cyassl i/o was re-implemented but is now fixed.

For general build instructions see rc1 below.

## CyaSSL Release Candidate 2 rc2-1.0.0 (1/21/2009)


Release Candidate 2 for CyaSSL 1.0.0 adds bug fixes and adds two new stream
ciphers along with their respective cipher suites.  CyaSSL adds support for
HC-128 and RABBIT stream ciphers.  The new suites are:

```
TLS_RSA_WITH_HC_128_SHA
TLS_RSA_WITH_RABBIT_SHA
```

And the corresponding cipher names are

```
HC128-SHA
RABBIT-SHA
```

CyaSSL also adds support for building with devkitPro for PPC by changing the
library proper to use libogc.  The examples haven't been changed yet but if
there's interest they can be.  Here's an example ./configure to build CyaSSL
for devkitPro:

```
./configure --disable-shared CC=/pathTo/devkitpro/devkitPPC/bin/powerpc-gekko-gcc --host=ppc --without-zlib --enable-singleThreaded RANLIB=/pathTo/devkitpro/devkitPPC/bin/powerpc-gekko-ranlib CFLAGS="-DDEVKITPRO -DGEKKO"
```

For linking purposes you'll need

`LDFLAGS="-g -mrvl -mcpu=750 -meabi -mhard-float -Wl,-Map,$(notdir $@).map"`

For general build instructions see rc1 below.


## CyaSSL Release Candidate 1 rc1-1.0.0 (12/17/2008)


Release Candidate 1 for CyaSSL 1.0.0 contains major internal changes.  Several
areas have optimization improvements, less dynamic memory use, and the I/O
strategy has been refactored to allow alternate I/O handling or Library use.
Many thanks to Thierry Fournier for providing these ideas and most of the work.

Because of these changes, this release is only a candidate since some problems
are probably inevitable on some platform with some I/O use.  Please report any
problems and we'll try to resolve them as soon as possible.  You can contact us
at support@yassl.com or todd@yassl.com.

Using TomsFastMath by passing --enable-fastmath to ./configure now uses assembly
on some platforms.  This is new so please report any problems as every compiler,
mode, OS combination hasn't been tested.  On ia32 all of the registers need to
be available so be sure to pass these options to CFLAGS:

`CFLAGS="-O3 -fomit-frame-pointer"`

OS X will also need -mdynamic-no-pic added to CFLAGS

Also if you're building in shared mode for ia32 you'll need to pass options to
LDFLAGS as well on OS X:

`LDFLAGS=-Wl,-read_only_relocs,warning`

This gives warnings for some symbols but seems to work.


#### To build on Linux, Solaris, *BSD, Mac OS X, or Cygwin:

    ./configure
    make

    from the ./testsuite/ directory run ./testsuite

#### To make a debug build:

    ./configure --enable-debug --disable-shared
    make



#### To build on Win32

Choose (Re)Build All from the project workspace

Run the testsuite program





# CyaSSL version 0.9.9 (7/25/2008)

This release of CyaSSL adds bug fixes, Pre-Shared Keys, over-rideable memory
handling, and optionally TomsFastMath.  Thanks to Moisés Guimarães for the
work on TomsFastMath.

To optionally use TomsFastMath pass --enable-fastmath to ./configure
Or define USE_FAST_MATH in each project from CyaSSL for MSVC.

Please use the benchmark routine before and after to see the performance
difference, on some platforms the gains will be little but RSA encryption
always seems to be faster.  On x86-64 machines with GCC the normal math library
may outperform the fast one when using CFLAGS=-m64 because TomsFastMath can't
yet use -m64 because of GCCs inability to do 128bit division.

     *** UPDATE GCC 4.2.1 can now do 128bit division ***

See notes below (0.2.0) for complete build instructions.


# CyaSSL version 0.9.8 (5/7/2008)

This release of CyaSSL adds bug fixes, client side Diffie-Hellman, and better
socket handling.

See notes below (0.2.0) for complete build instructions.


# CyaSSL version 0.9.6 (1/31/2008)

This release of CyaSSL adds bug fixes, increased session management, and a fix
for gnutls.

See notes below (0.2.0) for complete build instructions.


# CyaSSL version 0.9.0 (10/15/2007)

This release of CyaSSL adds bug fixes, MSVC 2005 support, GCC 4.2 support,
IPV6 support and test, and new test certificates.

See notes below (0.2.0) for complete build instructions.


# CyaSSL version 0.8.0 (1/10/2007)

This release of CyaSSL adds increased socket support, for non-blocking writes,
connects, and interrupted system calls.

See notes below (0.2.0) for complete build instructions.


# CyaSSL version 0.6.3 (10/30/2006)

This release of CyaSSL adds debug logging to stderr to aid in the debugging of
CyaSSL on systems that may not provide the best support.

If CyaSSL is built with debugging support then you need to call
CyaSSL_Debugging_ON() to turn logging on.

On Unix use ./configure --enable-debug

On Windows define DEBUG_CYASSL when building CyaSSL


To turn logging back off call CyaSSL_Debugging_OFF()

See notes below (0.2.0) for complete build instructions.


# CyaSSL version 0.6.2 (10/29/2006)

This release of CyaSSL adds TLS 1.1.

Note that CyaSSL has certificate verification on by default, unlike OpenSSL.
To emulate OpenSSL behavior, you must call SSL_CTX_set_verify() with
SSL_VERIFY_NONE.  In order to have full security you should never do this,
provide CyaSSL with the proper certificates to eliminate impostors and call
CyaSSL_check_domain_name() to prevent man in the middle attacks.

See notes below (0.2.0) for build instructions.

# CyaSSL version 0.6.0 (10/25/2006)

This release of CyaSSL adds more SSL functions, better autoconf, nonblocking
I/O for accept, connect, and read.  There is now an --enable-small configure
option that turns off TLS, AES, DES3, HMAC, and ERROR_STRINGS, see configure.in
for the defines.  Note that TLS requires HMAC and AES requires TLS.

See notes below (0.2.0) for build instructions.


# CyaSSL version 0.5.5 (09/27/2006)

This mini release of CyaSSL adds better input processing through buffered input
and big message support.  Added SSL_pending() and some sanity checks on user
settings.

See notes below (0.2.0) for build instructions.


# CyaSSL version 0.5.0 (03/27/2006)

This release of CyaSSL adds AES support and minor bug fixes.

See notes below (0.2.0) for build instructions.


# CyaSSL version 0.4.0 (03/15/2006)

This release of CyaSSL adds TLSv1 client/server support and libtool.

See notes below for build instructions.


# CyaSSL version 0.3.0 (02/26/2006)

This release of CyaSSL adds SSLv3 server support and session resumption.

See notes below for build instructions.


# CyaSSL version 0.2.0 (02/19/2006)


This is the first release of CyaSSL and its crypt brother, CTaoCrypt.  CyaSSL
is written in ANSI C with the idea of a small code size, footprint, and memory
usage in mind.  CTaoCrypt can be as small as 32K, and the current client
version of CyaSSL can be as small as 12K.


The first release of CTaoCrypt supports MD5, SHA-1, 3DES, ARC4, Big Integer
Support, RSA, ASN parsing, and basic x509 (en/de)coding.

The first release of CyaSSL supports normal client RSA mode SSLv3 connections
with support for SHA-1 and MD5 digests.  Ciphers include 3DES and RC4.


#### To build on Linux, Solaris, *BSD, Mac OS X, or Cygwin:

    ./configure
    make

    from the ./testsuite/ directory run ./testsuite

#### to make a debug build:

    ./configure --enable-debug --disable-shared
    make



#### To build on Win32

Choose (Re)Build All from the project workspace

Run the testsuite program



*** The next release of CyaSSL will support a server and more OpenSSL
compatibility functions.


Please send questions or comments to todd@wolfssl.com
