# CLANG_VERSION is empty if the compiler is not clang-based
CLANG_VERSION = $(shell $(CC) --version | sed -nr 's/^.*clang version ([0-9.]+).*$$/\1/p')
CLANG_VERSION_MAJOR = $(shell echo $(CLANG_VERSION) | cut -f1 -d.)

# determine specific version ranges
ifneq ($(CLANG_VERSION),)
$(if $(shell [ $(CLANG_VERSION_MAJOR) -ge 13 ] && echo "OK"), \
    $(eval CLANG_AT_LEAST_13 := true), \
    $(eval CLANG_AT_LEAST_13 := false))
endif

ifeq ($(FUZZER),1)
CC       ?= clang
LD       ?= $(CC)
SANFLAGS += -fsanitize=fuzzer

# only clang versions >= 13 support this feature
ifeq ($(CLANG_AT_LEAST_13),true)
$(info "info: using -fsanitize-ignorelist")
SANFLAGS += -fsanitize-ignorelist=fuzzer/sanitizer_ignorelist.txt
else
$(info "info: not using -fsanitize-ignorelist")
endif

# TODO is there a better solution, for example by disabling a specific optimization technique?
# there is a clang optimization issue in relation with the blake2 code at -fsanitize=undefined
$(warning "warning: disabling optimization on blake2 code as workaround")
blake2b.o: OPTFLAGS += -O0
blake2s.o: OPTFLAGS += -O0

else ifeq ($(ADDRESS_SANITIZER),1)
SANFLAGS += -fsanitize=address,undefined
endif

CC       ?= cc

OPTFLAGS ?= -O3 -g

CFLAGS   += $(OPTFLAGS) \
            $(SANFLAGS) \
            -std=gnu99 \
            -W \
            -Wall \
            -Wextra \
            -Wimplicit-function-declaration \
            -Wredundant-decls \
            -Wstrict-prototypes \
            -Wundef \
            -Wshadow \
            -Wpointer-arith \
            -Wformat \
            -Wreturn-type \
            -Wsign-compare \
            -Wmultichar \
            -Wformat-nonliteral \
            -Winit-self \
            -Wuninitialized \
            -Wformat-security \
            -Wno-missing-braces \
            -Werror

ZKP_CFLAGS = \
	-DECMULT_GEN_PREC_BITS=4 \
	-DECMULT_WINDOW_SIZE=8 \
	-DENABLE_MODULE_GENERATOR \
	-DENABLE_MODULE_RECOVERY \
	-DENABLE_MODULE_SCHNORRSIG \
	-DENABLE_MODULE_EXTRAKEYS
ZKP_PATH = ../vendor/secp256k1-zkp
# this is specific for 64-bit builds
CFLAGS += -DSECP256K1_CONTEXT_SIZE=208

VALGRIND ?= 1
ifeq ($(VALGRIND),1)
CFLAGS += -DVALGRIND
endif

CFLAGS += -I.
CFLAGS += -I..
CFLAGS += -DUSE_ETHEREUM=1
CFLAGS += -DUSE_KECCAK=1
CFLAGS += -DUSE_MONERO=1
CFLAGS += -DUSE_NEM=1
CFLAGS += -DUSE_CARDANO=1
CFLAGS += $(shell pkg-config --cflags openssl)

# disable certain optimizations and features when small footprint is required
ifdef SMALL
CFLAGS += -DUSE_PRECOMPUTED_CP=0
endif

SRCS   = bignum.c ecdsa.c curves.c secp256k1.c nist256p1.c rand.c hmac.c bip32.c bip39.c bip39_english.c pbkdf2.c base58.c base32.c
SRCS  += address.c
SRCS  += script.c
SRCS  += ripemd160.c
SRCS  += sha2.c
SRCS  += sha3.c
SRCS  += hasher.c
SRCS  += aes/aescrypt.c aes/aeskey.c aes/aestab.c aes/aes_modes.c
SRCS  += ed25519_donna/curve25519_donna_32bit.c ed25519_donna/curve25519_donna_helpers.c ed25519_donna/modm_donna_32bit.c
SRCS  += ed25519_donna/ed25519_donna_basepoint_table.c ed25519_donna/ed25519_donna_32bit_tables.c ed25519_donna/ed25519_donna_impl_base.c
SRCS  += ed25519_donna/ed25519.c ed25519_donna/curve25519_donna_scalarmult_base.c ed25519_donna/ed25519_sha3.c ed25519_donna/ed25519_keccak.c
SRCS  += monero/base58.c
SRCS  += monero/serialize.c
SRCS  += monero/xmr.c
SRCS  += blake256.c
SRCS  += blake2b.c blake2s.c
SRCS  += chacha_drbg.c
SRCS  += groestl.c
SRCS  += chacha20poly1305/chacha20poly1305.c chacha20poly1305/chacha_merged.c chacha20poly1305/poly1305_donna.c chacha20poly1305/rfc7539.c
SRCS  += rc4.c
SRCS  += nem.c
SRCS  += segwit_addr.c cash_addr.c
SRCS  += memzero.c
SRCS  += shamir.c
SRCS  += hmac_drbg.c
SRCS  += rfc6979.c
SRCS  += slip39.c
SRCS  += zkp_context.c
SRCS  += zkp_ecdsa.c
SRCS  += zkp_bip340.c
SRCS  += cardano.c

OBJS   = $(SRCS:.c=.o)
OBJS  += secp256k1-zkp.o
OBJS  += precomputed_ecmult.o
OBJS  += precomputed_ecmult_gen.o

TESTLIBS = $(shell pkg-config --libs check) -lpthread -lm
TESTSSLLIBS = $(shell pkg-config --libs openssl)

all: tools tests

%.o: %.c %.h options.h
	$(CC) $(CFLAGS) -o $@ -c $<

tests: tests/test_check tests/test_openssl tests/test_speed tests/libtrezor-crypto.so tests/aestst

tests/aestst: aes/aestst.o aes/aescrypt.o aes/aeskey.o aes/aestab.o
	$(CC) $(CFLAGS) $^ -o $@

tests/test_check.o: tests/test_check_cardano.h tests/test_check_monero.h tests/test_check_cashaddr.h tests/test_check_segwit.h

tests/test_check: tests/test_check.o $(OBJS)
	$(CC) $(CFLAGS) tests/test_check.o $(OBJS) $(TESTLIBS) -o tests/test_check

tests/test_speed: tests/test_speed.o $(OBJS)
	$(CC) $(CFLAGS) tests/test_speed.o $(OBJS) -o tests/test_speed

tests/test_openssl: tests/test_openssl.o $(OBJS)
	$(CC) $(CFLAGS) tests/test_openssl.o $(OBJS) $(TESTSSLLIBS) -o tests/test_openssl

tests/libtrezor-crypto.so: $(SRCS) secp256k1-zkp.o precomputed_ecmult.o precomputed_ecmult_gen.o
	$(CC) $(CFLAGS) -DAES_128 -DAES_192 -fPIC -shared $(SRCS) secp256k1-zkp.o precomputed_ecmult.o precomputed_ecmult_gen.o -o tests/libtrezor-crypto.so

tools: tools/xpubaddrgen tools/mktable tools/bip39bruteforce

tools/xpubaddrgen: tools/xpubaddrgen.o $(OBJS)
	$(CC) $(CFLAGS) tools/xpubaddrgen.o $(OBJS) -o tools/xpubaddrgen

tools/mktable: tools/mktable.o $(OBJS)
	$(CC) $(CFLAGS) tools/mktable.o $(OBJS) -o tools/mktable

tools/bip39bruteforce: tools/bip39bruteforce.o $(OBJS)
	$(CC) $(CFLAGS) tools/bip39bruteforce.o $(OBJS) -o tools/bip39bruteforce

fuzzer: fuzzer/fuzzer.o $(OBJS)
	$(CC) $(CFLAGS) fuzzer/fuzzer.o $(OBJS) -o fuzzer/fuzzer

precomputed_ecmult.o:
	$(CC) $(CFLAGS) -Wno-unused-function $(ZKP_CFLAGS) -fPIC -c $(ZKP_PATH)/src/precomputed_ecmult.c -o precomputed_ecmult.o

precomputed_ecmult_gen.o:
	$(CC) $(CFLAGS) -Wno-unused-function $(ZKP_CFLAGS) -fPIC -c $(ZKP_PATH)/src/precomputed_ecmult_gen.c -o precomputed_ecmult_gen.o

secp256k1-zkp.o:
	$(CC) $(CFLAGS) -Wno-unused-function $(ZKP_CFLAGS) -fPIC -I$(ZKP_PATH) -I$(ZKP_PATH)/src -c $(ZKP_PATH)/src/secp256k1.c -o secp256k1-zkp.o

clean:
	rm -f *.o aes/*.o chacha20poly1305/*.o ed25519_donna/*.o monero/*.o
	rm -f tests/*.o tests/test_check tests/test_speed tests/test_openssl tests/libtrezor-crypto.so tests/aestst
	rm -f tools/*.o tools/xpubaddrgen tools/mktable tools/bip39bruteforce
	rm -f fuzzer/*.o fuzzer/fuzzer
	rm -f secp256k1-zkp.o precomputed_ecmult.o precomputed_ecmult_gen.o

clean-fuzzer: clean
	rm -f crash-* fuzz-*.log slow-unit-* timeout-*
