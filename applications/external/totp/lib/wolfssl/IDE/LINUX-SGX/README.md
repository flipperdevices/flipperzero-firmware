# Static Library: Building libwolfssl.sgx.static.lib.a for use with SGX Enclaves

### Requirements:
This code was created to use Intel's SGX hardware. It is expected that the user has gone through the steps of both turning on the hardware in bios if needed and has installed the necessary software from Intel to make use of the hardware. (https://software.intel.com/en-us/sgx) If these steps have not been done then it is expected that the user is familiar with simulation software being used in place of hardware.

### Security:
If not already in use, it is recommended that SP (single precision) RSA and ECC code is used. This will help mitigate potential side channel attacks. To use SP code check that wolfcrypt/src/sp_c32.c and wolfcrypt/src/sp_c64.c are compiled and add HAVE_WOLFSSL_SP=1 to the build command to define the necessary macros.

### Overview and Build:
This project creates a static library to then link with Enclaves. A simple example of an Enclave linking to the created wolfSSL library can be found in wolfssl-examples on github. This project has been tested with gcc 5.4.0 on Ubuntu 16.04.
When building with tests the file wolfssl/options.h is expected, in downloaded bundles from wolfssl.com this file exists but when building from a cloned version of wolfSSL from GitHub then the file needs created. This is done either through cd wolfssl && ./autogen.sh && ./configure && ./config.status or by cd wolfssl && touch wolfssl/options.h.

To create the static library, simply call make:

`make -f sgx_t_static.mk all`

To clean the static library and compiled objects use the provided clean script:

`clean.sh`

This will create a local static library, libwolfssl.sgx.static.lib.a, that can be linked with SGX enclaves to access wolfSSL APIs using SGX hardware.

### Customization:
    To enable wolfssl debug, add CFLAGS=-DDEBUG_WOLFSSL.
    To enable wolfssl benchmark tests with enclave, specify: HAVE_WOLFSSL_BENCHMARK at build
    To enable wolfcrypt testsuite with enclave, specify: HAVE_WOLFSSL_TEST at build
    To enable SP code, specify: HAVE_WOLFSSL_SP at build

For example:
`make -f sgx_t_static.mk CFLAGS=-DDEBUG_WOLFSSL HAVE_WOLFSSL_BENCHMARK=1 HAVE_WOLFSSL_TEST=1 HAVE_WOLFSSL_SP=1`

NOTE: This more customized step has been provided for easier execution in the
      script `build.sh`

### Limitations:
    Single Threaded (multiple threaded applications have not been tested)
    AES-NI use with SGX has not been added in yet
