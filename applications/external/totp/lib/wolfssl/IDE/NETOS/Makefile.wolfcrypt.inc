WOLFSSL_ROOT=wolfCrypt_v4_5_2
APP_WOLFCRYPTOBJS = $(NETOS_DIR)/src/$(WOLFSSL_ROOT)/objs/ns9210/32b/gnu/wolfcrypt_first.o\
            $(NETOS_DIR)/src/$(WOLFSSL_ROOT)/objs/ns9210/32b/gnu/aes.o\
            $(NETOS_DIR)/src/$(WOLFSSL_ROOT)/objs/ns9210/32b/gnu/cmac.o\
            $(NETOS_DIR)/src/$(WOLFSSL_ROOT)/objs/ns9210/32b/gnu/des3.o\
            $(NETOS_DIR)/src/$(WOLFSSL_ROOT)/objs/ns9210/32b/gnu/dh.o\
            $(NETOS_DIR)/src/$(WOLFSSL_ROOT)/objs/ns9210/32b/gnu/ecc.o\
            $(NETOS_DIR)/src/$(WOLFSSL_ROOT)/objs/ns9210/32b/gnu/hmac.o\
            $(NETOS_DIR)/src/$(WOLFSSL_ROOT)/objs/ns9210/32b/gnu/random.o\
            $(NETOS_DIR)/src/$(WOLFSSL_ROOT)/objs/ns9210/32b/gnu/rsa.o\
            $(NETOS_DIR)/src/$(WOLFSSL_ROOT)/objs/ns9210/32b/gnu/sha.o\
            $(NETOS_DIR)/src/$(WOLFSSL_ROOT)/objs/ns9210/32b/gnu/sha256.o\
            $(NETOS_DIR)/src/$(WOLFSSL_ROOT)/objs/ns9210/32b/gnu/sha3.o\
            $(NETOS_DIR)/src/$(WOLFSSL_ROOT)/objs/ns9210/32b/gnu/sha512.o\
            $(NETOS_DIR)/src/$(WOLFSSL_ROOT)/objs/ns9210/32b/gnu/fips.o\
            $(NETOS_DIR)/src/$(WOLFSSL_ROOT)/objs/ns9210/32b/gnu/fips_test.o\
            $(NETOS_DIR)/src/$(WOLFSSL_ROOT)/objs/ns9210/32b/gnu/wolfcrypt_last.o

