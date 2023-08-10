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

ENVI=hexagon

# default to hexagon v65 Release build
ifndef V
V=hexagon_Release_dynamic_toolv83_v65
endif

SUPPORTED_VS = $(default_VS)

DEPENDENCIES = \
  ATOMIC \
  RPCMEM \
  TEST_MAIN \
  TEST_UTIL
ATOMIC_DIR = $(HEXAGON_SDK_ROOT)/libs/common/atomic
RPCMEM_DIR = $(HEXAGON_SDK_ROOT)/libs/common/rpcmem
TEST_MAIN_DIR = $(HEXAGON_SDK_ROOT)/test/common/test_main
TEST_UTIL_DIR = $(HEXAGON_SDK_ROOT)/test/common/test_util

include $(HEXAGON_SDK_ROOT)/build/make.d/$(ENVI)_vs.min
include $(HEXAGON_SDK_ROOT)/build/defines.min

QURT = $(HEXAGON_SDK_ROOT)/libs/common/qurt
QAIC_FLAGS += -I../../../
CC_FLAGS += -I../../../
CC_FLAGS += -I $(QURT)/computev65/include/posix/
CC_FLAGS += -I $(QURT)/computev65/include/qurt/
CC_FLAGS += -I../
CC_FLAGS += -DWOLFSSL_USER_SETTINGS
CC_FLAGS += -DWOLFSSL_DSP_BUILD
CC_FLAGS += -DWC_NO_RNG
#CC_FLAGS += -O3

C_FLAGS += $(MHVX_DOUBLE_FLAG) -mllvm -hexagon-eif=0
CC_FLAGS += $(MHVX_DOUBLE_FLAG) -mllvm -hexagon-eif=0

# stub library
BUILD_LIBS += libwolfssl_dsp_skel
ifeq (1,$(V_dynamic))
BUILD_DLLS += libwolfssl_dsp_skel
endif
libwolfssl_dsp_skel_QAICIDLS += wolfssl_dsp
libwolfssl_dsp_skel_C_SRCS += $V/wolfSSL_skel
libwolfssl_dsp_skel.C_SRCS += ../../../wolfcrypt/src/sp_dsp32.c
libwolfssl_dsp_skel.C_SRCS += ../../../wolfcrypt/src/sp_int.c
libwolfssl_dsp_skel.C_SRCS += ../../../wolfcrypt/src/wc_port.c
libwolfssl_dsp_skel.C_SRCS += ../../../wolfcrypt/src/logging.c
libwolfssl_dsp_skel.C_SRCS += ../../../wolfcrypt/src/memory.c
libwolfssl_dsp_skel.C_SRCS += ../../../wolfcrypt/src/wolfmath.c
#libsp_dsp_skel_DLLS += libdspCV_skel

# quality test
#BUILD_QEXES += eccverify_q
#eccverify_q_QAICIDLS = wolfssl_dsp
#eccverify_q_C_SRCS = ../ecc-verify
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/wc_dsp.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/sp_dsp32.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/sp_int.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/wc_port.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/wc_encrypt.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/pwdbased.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/hash.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/arc4.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/hmac.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/md5.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/coding.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/aes.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/des3.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/random.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/logging.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/ecc.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/memory.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/sha256.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/sha.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/asn.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/wolfmath.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/sp_c32.c
#eccverify_q.C_SRCS += ../../../wolfcrypt/src/sp_c64.c
#eccverify_q_LIBS = rtld rpcmem test_util atomic test_main

BUILD_COPIES = \
   $(DLLS) \
   $(EXES) \
   $(LIBS) \
   $(SHIP_DIR)/ ;


include $(RULES_MIN)


