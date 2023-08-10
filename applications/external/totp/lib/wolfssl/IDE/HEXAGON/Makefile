# Makefile
#
# Copyright (C) 2006-2023 wolfSSL Inc.
#
# This file is part of wolfSSL.
#
# wolfSSL is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# wolfSSL is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA

ENVI=UbuntuARM

# default to Ubuntu Release build
ifndef V
V=UbuntuARM_Release_aarch64
endif

SUPPORTED_VS = $(default_VS)
include $(HEXAGON_SDK_ROOT)/build/make.d/$(ENVI)_vs.min
include $(HEXAGON_SDK_ROOT)/build/defines.min

ifeq ($(CDSP_FLAG), 1) 
	LIB_DSPRPC = libcdsprpc
else ifeq ($(MDSP_FLAG), 1) 
	LIB_DSPRPC = libmdsprpc
else ifeq ($(SLPI_FLAG), 1) 
	LIB_DSPRPC = libsdsprpc
else
	LIB_DSPRPC = libadsprpc
endif
$(info ************  LIB=$(LIB_DSPRPC) ************)
# include files
CC_FLAGS += -I../../
CC_FLAGS += -I./
CC_FLAGS += -DWOLFSSL_USER_SETTINGS
CC_FLAGS += -mcpu=generic+crypto
#CC_FLAGS += -O3

DEPENDENCIES = \
  ATOMIC \
  RPCMEM 
ATOMIC_DIR = $(HEXAGON_SDK_ROOT)/libs/common/atomic
RPCMEM_DIR = $(HEXAGON_SDK_ROOT)/libs/common/rpcmem

# stub library
BUILD_DLLS += libwolfssl
libwolfssl_QAICIDLS += DSP/wolfssl_dsp
libwolfssl_C_SRCS += $V/wolfSSL_stub
libwolfssl_DLLS += $(LIB_DSPRPC)
libwolfssl_C_SRCS += ../../wolfcrypt/src/wc_dsp
libwolfssl_LIBS += rpcmem
libwolfssl_LD_FLAGS += -ldl

# wolfSSL crypto source files
libwolfssl_C_SRCS += \
	../../wolfcrypt/src/aes \
	../../wolfcrypt/src/md2 \
	../../wolfcrypt/src/arc4 \
	../../wolfcrypt/src/md4 \
	../../wolfcrypt/src/asm \
	../../wolfcrypt/src/md5 \
	../../wolfcrypt/src/asn \
	../../wolfcrypt/src/memory \
	../../wolfcrypt/src/async \
	../../wolfcrypt/src/blake2b \
	../../wolfcrypt/src/pkcs12 \
	../../wolfcrypt/src/blake2s \
	../../wolfcrypt/src/pkcs7 \
	../../wolfcrypt/src/camellia \
	../../wolfcrypt/src/poly1305 \
	../../wolfcrypt/src/chacha20_poly1305 \
	../../wolfcrypt/src/pwdbased \
	../../wolfcrypt/src/chacha \
	../../wolfcrypt/src/cmac \
	../../wolfcrypt/src/random \
	../../wolfcrypt/src/coding \
	../../wolfcrypt/src/ripemd \
	../../wolfcrypt/src/compress \
	../../wolfcrypt/src/rsa \
	../../wolfcrypt/src/cpuid \
	../../wolfcrypt/src/selftest \
	../../wolfcrypt/src/cryptocb \
	../../wolfcrypt/src/sha256 \
	../../wolfcrypt/src/curve25519 \
	../../wolfcrypt/src/sha3 \
	../../wolfcrypt/src/des3 \
	../../wolfcrypt/src/sha512 \
	../../wolfcrypt/src/dh \
	../../wolfcrypt/src/sha \
	../../wolfcrypt/src/signature \
	../../wolfcrypt/src/ecc \
	../../wolfcrypt/src/ecc_fp \
	../../wolfcrypt/src/ed25519 \
	../../wolfcrypt/src/sp_armthumb \
	../../wolfcrypt/src/error \
	../../wolfcrypt/src/sp_int \
	../../wolfcrypt/src/fe_low_mem \
	../../wolfcrypt/src/sp_cortexm \
	../../wolfcrypt/src/fe_operations \
	../../wolfcrypt/src/fips \
	../../wolfcrypt/src/sp_x86_64 \
	../../wolfcrypt/src/fips_test \
	../../wolfcrypt/src/srp \
	../../wolfcrypt/src/ge_low_mem \
	../../wolfcrypt/src/ge_operations \
	../../wolfcrypt/src/wc_encrypt \
	../../wolfcrypt/src/hash \
	../../wolfcrypt/src/wc_pkcs11 \
	../../wolfcrypt/src/wc_port \
	../../wolfcrypt/src/hmac \
	../../wolfcrypt/src/wolfcrypt_first \
	../../wolfcrypt/src/wolfcrypt_last \
	../../wolfcrypt/src/wolfevent \
	../../wolfcrypt/src/logging \
	../../wolfcrypt/src/sp_c32 \
	../../wolfcrypt/src/sp_c64 \
	../../wolfcrypt/src/sp_arm32 \
	../../wolfcrypt/src/sp_arm64 \
	../../wolfcrypt/src/wolfmath


# wolfSSL TLS source files
libwolfssl_C_SRCS += \
	../../src/ocsp \
	../../src/tls \
	../../src/crl \
	../../src/sniffer \
	../../src/wolfio \
	../../src/internal \
	../../src/ssl \
	../../src/keys \
	../../src/tls13

# build benchmark app
BUILD_EXES += benchmark
benchmark_C_SRCS += ../../wolfcrypt/benchmark/benchmark
benchmark_LD_FLAGS += -ldl
benchmark_LD_FLAGS += -lpthread
benchmark_DLLS += libwolfssl
benchmark_LIBS += rpcmem
benchmark_DLLS += $(LIB_DSPRPC) 

# build test app
BUILD_EXES += testwolfcrypt 
testwolfcrypt_C_SRCS += ../../wolfcrypt/test/test
testwolfcrypt_DLLS += libwolfssl
testwolfcrypt_LD_FLAGS += -ldl
testwolfcrypt_LIBS += rpcmem
testwolfcrypt_DLLS += $(LIB_DSPRPC) 

# build ecc verify test app
BUILD_EXES += eccverify 
eccverify_C_SRCS += ecc-verify
eccverify_DLLS += libwolfssl
eccverify_LD_FLAGS += -ldl
eccverify_LIBS += rpcmem
eccverify_DLLS += $(LIB_DSPRPC) 

# build ecc verify test app
BUILD_EXES += eccbenchmark
eccbenchmark_C_SRCS += ecc-verify-benchmark
eccbenchmark_DLLS += libwolfssl
eccbenchmark_LD_FLAGS += -lpthread
eccbenchmark_LD_FLAGS += -ldl
eccbenchmark_LIBS += rpcmem
eccbenchmark_DLLS += $(LIB_DSPRPC) 

BUILD_COPIES = \
   $(DLLS) \
   $(EXES) \
   $(LIBS) \
   $(SHIP_DIR)/ ;


include $(RULES_MIN)

