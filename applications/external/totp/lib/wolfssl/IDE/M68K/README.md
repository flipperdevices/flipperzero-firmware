This is makefile's for creating a wolfCrypt library using the m68k-elf-gcc
toolchain and example benchmark/testwolfcrypt application linking to it. The
examples and default builds where made to support a MCF5441X board.

Macros to define for use:
WOLFSSL_MCF5441X /* arch settings i.e. sizeof long and endianness */
WOLFSSL_NETBURNER /* for use of NetBurner headers and RNG seed */


To build the wolfssl.a library (settings for wolfCrypt only by default) run
"make" from the directory wolfssl-root/IDE/M68K/.
By default this outputs the wolfssl.a library to be at $(NBROOT)/lib. This can
be adjusted by adjusting the variable OUTPUT in Makefile.

If the macro WOLFSSL_MCF5441X is defined then
wolfssl-root/wolfssl/wolfcrypt/settings.h sets the sizeof long and long long
along with big endian macro.

The configuration for the build is located in wolfssl-root/IDE/M68K/user_settings.h
Along with the default build there is 2 others BUILD_B (smaller resource use),
and BUILD_C (faster runtime with more resource use).

RSA speeds of the builds

default:
RSA 2048 public 3.333 ops/sec
RSA 2048 private 0.190 ops/sec

BUILD_B
RSA 2048 public 3.333 ops/sec
RSA 2048 private 0.053 ops/sec

BUILD_C
RSA 2048 public 7.619 ops/sec
RSA 2048 private 0.276 ops/sec

###Building testwolfcryt/benchmark
To build either testwolfcrypt or benchmark first build wolfssl.a, place it in
$(NBROOT)/lib and then cd into the respective directory. Running "make" will
then create a .s19 application that can be ran on the board.

When running either testwolfcrypt or the benchmark app the first thing they do
is loop on calling RandomValid until getting a successful return. This is done
in order to wait for a source of entropy. It could take several moments until
completed.

