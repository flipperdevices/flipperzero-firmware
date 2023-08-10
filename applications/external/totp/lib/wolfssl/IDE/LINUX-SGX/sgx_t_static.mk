######## Intel(R) SGX SDK Settings ########
SGX_SDK ?= /opt/intel/sgxsdk
SGX_MODE ?= SIM
SGX_ARCH ?= x64
WOLFSSL_ROOT ?= $(shell readlink -f ../..)

ifeq ($(shell getconf LONG_BIT), 32)
	SGX_ARCH := x86
else ifeq ($(findstring -m32, $(CXXFLAGS)), -m32)
	SGX_ARCH := x86
endif

ifeq ($(SGX_ARCH), x86)
	SGX_COMMON_CFLAGS := -m32
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x86/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x86/sgx_edger8r
else
	SGX_COMMON_CFLAGS := -m64
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib64
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x64/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x64/sgx_edger8r
endif

ifeq ($(SGX_DEBUG), 1)
ifeq ($(SGX_PRERELEASE), 1)
$(error Cannot set SGX_DEBUG and SGX_PRERELEASE at the same time!!)
endif
endif

ifeq ($(SGX_DEBUG), 1)
        SGX_COMMON_CFLAGS += -O0 -g
else
        SGX_COMMON_CFLAGS += -O2
endif

ifneq ($(SGX_MODE), HW)
	Trts_Library_Name := sgx_trts_sim
	Service_Library_Name := sgx_tservice_sim
else
	Trts_Library_Name := sgx_trts
	Service_Library_Name := sgx_tservice
endif

Crypto_Library_Name := sgx_tcrypto

Wolfssl_C_Extra_Flags := -DWOLFSSL_SGX

Wolfssl_C_Files :=$(WOLFSSL_ROOT)/wolfcrypt/src/aes.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/arc4.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/asn.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/blake2b.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/camellia.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/coding.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/chacha.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/chacha20_poly1305.c\
					$(WOLFSSL_ROOT)/src/crl.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/des3.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/dh.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/tfm.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/ecc.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/error.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/hash.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/kdf.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/hmac.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/integer.c\
					$(WOLFSSL_ROOT)/src/internal.c\
					$(WOLFSSL_ROOT)/src/wolfio.c\
					$(WOLFSSL_ROOT)/src/keys.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/logging.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/md4.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/md5.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/memory.c\
					$(WOLFSSL_ROOT)/src/ocsp.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/pkcs7.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/pkcs12.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/poly1305.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/wc_port.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/wolfmath.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/pwdbased.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/random.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/ripemd.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/rsa.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/dsa.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/sha.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/sha256.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/sha512.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/signature.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/sp_c32.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/sp_c64.c\
					$(WOLFSSL_ROOT)/src/ssl.c\
					$(WOLFSSL_ROOT)/src/tls.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/wc_encrypt.c\
					$(WOLFSSL_ROOT)/wolfcrypt/src/wolfevent.c\

Wolfssl_Include_Paths := -I$(WOLFSSL_ROOT)/ \
						 -I$(WOLFSSL_ROOT)/wolfcrypt/ \
						 -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/stlport

ifeq ($(HAVE_WOLFSSL_TEST), 1)
	Wolfssl_Include_Paths += -I$(WOLFSSL_ROOT)/wolfcrypt/test
	Wolfssl_C_Files += $(WOLFSSL_ROOT)/wolfcrypt/test/test.c
endif

ifeq ($(HAVE_WOLFSSL_BENCHMARK), 1)
	Wolfssl_C_Files += $(WOLFSSL_ROOT)/wolfcrypt/benchmark/benchmark.c
	Wolfssl_Include_Paths += -I$(WOLFSSL_ROOT)/wolfcrypt/benchmark/
endif

ifeq ($(HAVE_WOLFSSL_SP), 1)
    Wolfssl_C_Extra_Flags += -DWOLFSSL_HAVE_SP_RSA \
                             -DWOLFSSL_HAVE_SP_DH  \
                             -DWOLFSSL_HAVE_SP_ECC
endif


Flags_Just_For_C := -Wno-implicit-function-declaration -std=c11
Common_C_Cpp_Flags := $(SGX_COMMON_CFLAGS) -nostdinc -fvisibility=hidden -fpie -fstack-protector $(Wolfssl_Include_Paths) -fno-builtin-printf -I.
Wolfssl_C_Flags := $(Flags_Just_For_C) $(Common_C_Cpp_Flags) $(Wolfssl_C_Extra_Flags)

Wolfssl_Link_Flags := $(SGX_COMMON_CFLAGS) -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_LIBRARY_PATH) \
	-Wl,--whole-archive -l$(Trts_Library_Name) -Wl,--no-whole-archive \
	-Wl,--start-group -lsgx_tstdc -lsgx_tstdcxx -l$(Crypto_Library_Name) -l$(Service_Library_Name) -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0 \
	-Wl,--version-script=trusted/wolfcrypt.lds

Wolfssl_C_Objects := $(Wolfssl_C_Files:.c=.o)

ifeq ($(SGX_MODE), HW)
ifneq ($(SGX_DEBUG), 1)
ifneq ($(SGX_PRERELEASE), 1)
Build_Mode = HW_RELEASE
endif
endif
endif

override CFLAGS += $(Wolfssl_C_Flags)

.PHONY: all run

all: libwolfssl.sgx.static.lib.a

######## WolfSSL Objects ########

libwolfssl.sgx.static.lib.a: $(Wolfssl_C_Objects)
	ar rcs libwolfssl.sgx.static.lib.a $(Wolfssl_C_Objects)
	@echo "LINK =>  $@"

clean:
	@rm -f $(WOLFSSL_ROOT)/wolfcrypt/benchmark/*.o $(WOLFSSL_ROOT)/wolfcrypt/test/*.o static_trusted/wolfssl_t.* libwolfssl.sgx.static.lib.a  $(Wolfssl_C_Objects)
