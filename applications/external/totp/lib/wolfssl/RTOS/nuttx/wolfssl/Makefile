############################################################################
# apps/crypto/wolfssl/Makefile
#
# Copyright (C) 2006-2023 wolfSSL Inc.
#
# This file is part of wolfSSL. (formerly known as CyaSSL)
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
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
#
############################################################################

include $(APPDIR)/Make.defs

# wolfSSL SSL/TLS crypto library

# Set up build configuration and environment
https://github.com/wolfSSL/wolfssl/archive/refs/tags/v5.5.4-stable.zip
WOLFSSL_URL ?= "https://github.com/wolfSSL/wolfssl/archive/refs/tags"

WOLFSSL_VERSION = $(patsubst "%",%,$(strip $(CONFIG_WOLFSSL_VERSION)))
WOLFSSL_ZIP = v$(WOLFSSL_VERSION)-stable.zip

WOLFSSL_UNPACKNAME = wolfssl
WOLFSSL_EXAMPLESNAME = wolfssl-examples
UNPACK ?= unzip -q -o

# WOLFSSL TLS FILES
CSRCS += $(WOLFSSL_UNPACKNAME)/src/crl.c
CSRCS += $(WOLFSSL_UNPACKNAME)/src/internal.c
CSRCS += $(WOLFSSL_UNPACKNAME)/src/keys.c
CSRCS += $(WOLFSSL_UNPACKNAME)/src/ocsp.c
CSRCS += $(WOLFSSL_UNPACKNAME)/src/sniffer.c
CSRCS += $(WOLFSSL_UNPACKNAME)/src/ssl.c
CSRCS += $(WOLFSSL_UNPACKNAME)/src/tls.c
CSRCS += $(WOLFSSL_UNPACKNAME)/src/tls13.c
CSRCS += $(WOLFSSL_UNPACKNAME)/src/wolfio.c

# wolfCrypt Core (FIPS validated crypto available contact fips@wolfssl.com)
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/aes.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/cmac.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/des3.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/dh.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/ecc.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/hmac.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/random.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/rsa.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/sha.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/sha256.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/sha512.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/sha3.c

# wolfCrypt Additional
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/asm.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/asn.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/blake2s.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/chacha.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/chacha20_poly1305.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/coding.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/compress.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/cpuid.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/cryptocb.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/curve25519.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/curve448.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/ecc_fp.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/eccsi.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/ed25519.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/ed448.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/error.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/fe_448.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/fe_low_mem.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/fe_operations.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/ge_448.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/ge_low_mem.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/ge_operations.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/hash.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/kdf.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/integer.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/logging.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/md5.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/memory.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/pkcs12.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/pkcs7.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/poly1305.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/pwdbased.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/rc2.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/sakke.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/signature.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/srp.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/sp_arm32.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/sp_arm64.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/sp_armthumb.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/sp_c32.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/sp_c64.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/sp_cortexm.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/sp_dsp32.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/sp_int.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/sp_x86_64.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/tfm.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/wc_dsp.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/wc_encrypt.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/wc_pkcs11.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/wc_port.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/wolfevent.c
CSRCS += $(WOLFSSL_UNPACKNAME)/wolfcrypt/src/wolfmath.c


CFLAGS += -DWOLFSSL_USER_SETTINGS


ifneq ($(CONFIG_WOLFCRYPT_APPS),)

MODULE = $(CONFIG_WOLFSSL_APPS)

ifeq ($(CONFIG_WOLFCRYPT_APP_BENCHMARK),y)

PROGNAME  += $(CONFIG_WOLFCRYPT_APP_BENCHMARK_PROGNAME)
PRIORITY  += $(CONFIG_WOLFCRYPT_APP_BENCHMARK_PRIORITY)
STACKSIZE += $(CONFIG_WOLFCRYPT_APP_BENCHMARK_STACKSIZE)

CFLAGS += -I$(WOLFSSL_UNPACKNAME)
CFLAGS += -I$(WOLFSSL_UNPACKNAME)/../ # To reach user_settings.h
MAINSRC += $(WOLFSSL_UNPACKNAME)/wolfcrypt/benchmark/benchmark.c

endif

ifeq ($(CONFIG_WOLFCRYPT_APP_SELFTEST),y)

PROGNAME  += $(CONFIG_WOLFCRYPT_APP_SELFTEST_PROGNAME)
PRIORITY  += $(CONFIG_WOLFCRYPT_APP_SELFTEST_PRIORITY)
STACKSIZE += $(CONFIG_WOLFCRYPT_APP_SELFTEST_STACKSIZE)

CFLAGS += -I$(WOLFSSL_UNPACKNAME)
CFLAGS += -I$(WOLFSSL_UNPACKNAME)/../ # To reach user_settings.h
MAINSRC += $(WOLFSSL_UNPACKNAME)/wolfcrypt/test/test.c

endif

ifeq ($(CONFIG_WOLFSSL_APP_CLIENT_SERVER),y)

PROGNAME  += $(CONFIG_WOLFSSL_APP_CLIENT_SERVER_PROGNAME)
PRIORITY  += $(CONFIG_WOLFSSL_APP_CLIENT_SERVER_PRIORITY)
STACKSIZE += $(CONFIG_WOLFSSL_APP_CLIENT_SERVER_STACKSIZE)

CFLAGS += -I$(WOLFSSL_UNPACKNAME)
CFLAGS += -I$(WOLFSSL_UNPACKNAME)/../ # To reach user_settings.h
MAINSRC += $(WOLFSSL_EXAMPLESNAME)/embedded/tls-client-server.c

endif

endif

$(WOLFSSL_ZIP):
	@echo "Downloading: $(WOLFSSL_ZIP)"
	$(Q) curl -O -L $(CONFIG_WOLFSSL_URL)/$(WOLFSSL_ZIP)

$(WOLFSSL_UNPACKNAME): $(WOLFSSL_ZIP)
	@echo "Unpacking: $(WOLFSSL_ZIP) -> $(WOLFSSL_UNPACKNAME)"
	$(Q) $(UNPACK) $(WOLFSSL_ZIP)
	$(Q) mv	wolfssl-$(WOLFSSL_VERSION) $(WOLFSSL_UNPACKNAME)
	$(Q) touch $(WOLFSSL_UNPACKNAME)


include $(APPDIR)/Application.mk
