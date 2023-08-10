# Example Project for GCC ARM

This example is for Cortex M series, but can be adopted for other architectures.

## Design

* All library options are defined in `Header/user_settings.h`.
* The memory map is located in the linker file in `linker.ld`.
* Entry point function is `reset_handler` in `armtarget.c`.
* The RTC and RNG hardware interface needs implemented for real production applications in `armtarget.c`

## Building

1. Make sure you have `gcc-arm-none-eabi` installed.
2. Modify the `Makefile.common`:
  * Use correct toolchain path `TOOLCHAIN`.
  * Use correct architecture 'ARCHFLAGS'. See [GCC ARM Options](https://gcc.gnu.org/onlinedocs/gcc-4.7.3/gcc/ARM-Options.html) `-mcpu=name`.
  * Confirm memory map in linker.ld matches your flash/ram or comment out `SRC_LD = -T./linker.ld` in Makefile.common.
3. Use `make` to build the static library (libwolfssl.a), wolfCrypt test/benchmark and wolfSSL TLS client targets as `.elf` and `.hex` in `/Build`.


## Building for Raspberry Pi

Example `Makefile.common` changes for Raspberry Pi with Cortex-A53:

1. In Makefile.common change `ARCHFLAGS` to `-mcpu=cortex-a53 -mthumb`.
2. Comment out `SRC_LD`, since custom memory map is not applicable.
3. Clear `TOOLCHAIN`, so it will use default `gcc`. Set `TOOLCHAIN = `
4. Comment out `LDFLAGS += --specs=nano.specs` and `LDFLAGS += --specs=nosys.specs` to nosys and nano.

Note: To comment out a line in a Makefile use place `#` in front of line.

## Building for FIPS

1. Request evaluation from wolfSSL by emailing fips@wolfss.com.
2. Modify user_settings.h so section for `HAVE_FIPS` is enabled.
3. Use `make`.
4. Run the wolfCrypt test `./Build/WolfCryptTest.elf` to generate the FIPS boundary HASH

Example:

```
$ Crypt Test
error    test passed!
base64   test passed!
base16   test passed!
asn      test passed!
in my Fips callback, ok = 0, err = -203
message = In Core Integrity check FIPS error
hash = F607C7B983D1D283590448A56381DE460F1E83CB02584F4D77B7F2C583A8F5CD
In core integrity hash check failure, copy above hash
into verifyCore[] in fips_test.c and rebuild
SHA      test failed!
 error = -1802
Crypt Test: Return code -1
```

5. Update the `../../wolfcrypt/src/fips_test.c` array `static const char verifyCore[] = {}` with the correct core hash check.
6. Build again using `make`.
7. Run the wolfCrypt test.

## Building with configure

The configure script in the main project directory can perform a cross-compile
build with the the gcc-arm-none-eabi tools. Assuming the tools are installed in
your executable path:

```
$ ./configure \
    --host=arm-non-eabi \
    CC=arm-none-eabi-gcc \
    AR=arm-none-eabi-ar \
    STRIP=arm-none-eabi-strip \
    RANLIB=arm-none-eabi-ranlib \
    --prefix=/path/to/build/wolfssl-arm \
    CFLAGS="-march=armv8-a --specs=nosys.specs \
        -DHAVE_PK_CALLBACKS -DWOLFSSL_USER_IO -DNO_WRITEV" \
    --disable-filesystem --enable-fastmath \
    --disable-shared
$ make
$ make install
```

If you are building for a 32-bit architecture, add `-DTIME_T_NOT_64BIT` to the
list of CFLAGS.

## Example Build Output

```
make clean && make

   text    data     bss     dec     hex   filename
  50076    2508      44   52628    cd94   ./Build/WolfCryptTest.elf

   text    data     bss     dec     hex   filename
  39155    2508      60   41723    a2fb   ./Build/WolfCryptBench.elf

   text    data     bss     dec     hex filename
  70368     464      36   70868   114d4 ./Build/WolfSSLClient.elf
```

## Performance Tuning Options

These settings are located in `Header/user_settings.h`.

* `DEBUG_WOLFSSL`: Undefine this to disable debug logging.
* `NO_ERROR_STRINGS`: Disables error strings to save code space.
* `NO_INLINE`: Disabling inline function saves about 1KB, but is slower.
* `WOLFSSL_SMALL_STACK`: Enables stack reduction techniques to allocate stack sections over 100 bytes from heap.
* `USE_FAST_MATH`: Uses stack based math, which is faster than the heap based math.
* `ALT_ECC_SIZE`: If using fast math and RSA/DH you can define this to reduce your ECC memory consumption.
* `FP_MAX_BITS`: Is the maximum math size (key size * 2). Used only with `USE_FAST_MATH`.
* `ECC_TIMING_RESISTANT`: Enables timing resistance for ECC and uses slightly less memory.
* `ECC_SHAMIR`: Doubles heap usage, but slightly faster
* `RSA_LOW_MEM`: Half as much memory but twice as slow. Uses Non-CRT method for private key.
* AES GCM: `GCM_SMALL`, `GCM_WORD32` or `GCM_TABLE`: Tunes performance and flash/memory usage.
* `CURVED25519_SMALL`: Enables small versions of Ed/Curve (FE/GE math).
* `USE_SLOW_SHA`: Enables smaller/slower version of SHA.
* `USE_SLOW_SHA256`: About 2k smaller and about 25% slower
* `USE_SLOW_SHA512`: Over twice as small, but 50% slower
* `USE_CERT_BUFFERS_1024` or `USE_CERT_BUFFERS_2048`: Size of RSA certs / keys to test with. 
* `BENCH_EMBEDDED`: Define this if using the wolfCrypt test/benchmark and using a low memory target.
* `ECC_USER_CURVES`: Allows user to define curve sizes to enable. Default is 256-bit on. To enable others use `HAVE_ECC192`, `HAVE_ECC224`, etc....
* `TFM_ARM`, `TFM_SSE2`, `TFM_AVR32`, `TFM_PPC32`, `TFM_MIPS`, `TFM_X86` or `TFM_X86_64`: These are assembly optimizations available with USE_FAST_MATH.
* Single Precision Math for ARM: See `WOLFSSL_SP`. Optimized math for ARM performance of specific RSA, DH and ECC algorithms.
