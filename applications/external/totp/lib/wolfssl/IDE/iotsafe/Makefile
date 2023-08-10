# Makefile
#
# Copyright (C) 2006-2021 wolfSSL Inc.
#
# This file is part of wolfSSL. (formerly known as CyaSSL)
#
#
CROSS_COMPILE:=arm-none-eabi-
CC:=$(CROSS_COMPILE)gcc
LD:=$(CROSS_COMPILE)gcc
LSCRIPT:=target.ld

WOLFSSL_ROOT=$(PWD)/../..
WOLFSSL_BUILD=build/wolfssl


OBJCOPY:=$(CROSS_COMPILE)objcopy

CFLAGS:=-mthumb -g -ggdb -Wall -Wextra -Wno-main -fsigned-char -Wstack-usage=65535 -Wno-unused -I$(WOLFSSL_ROOT) -I.
CFLAGS+=-lc -lg -lm -Wno-pointer-sign 
#CFLAGS+=-mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -lm
CFLAGS+=-mcpu=cortex-m3
CFLAGS+=-DWOLFSSL_USER_SETTINGS
ASFLAGS+=-mthumb -mlittle-endian -mthumb-interwork -ggdb -ffreestanding -mcpu=cortex-m3
LDFLAGS:=-T $(LSCRIPT) -Wl,-gc-sections -Wl,-Map=image.map -mthumb  -mthumb-interwork -mlittle-endian
#LDFLAGS+=-mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -lm
LDFLAGS+=-mcpu=cortex-m3
LDFLAGS+=-lc -lg -lm
LDFLAGS+=--specs=nosys.specs

OBJS:=main.o startup.o devices.o memory-tls.o

WOLFSSL_OBJS += 	\
    $(WOLFSSL_BUILD)/internal.o \
	$(WOLFSSL_BUILD)/wolfio.o \
    $(WOLFSSL_BUILD)/keys.o \
    $(WOLFSSL_BUILD)/crl.o \
    $(WOLFSSL_BUILD)/ssl.o \
    $(WOLFSSL_BUILD)/tls.o \
	$(WOLFSSL_BUILD)/wolfcrypt/aes.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/asn.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/chacha.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/chacha20_poly1305.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/coding.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/curve25519.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/dh.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/dsa.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/error.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/ecc.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/ed25519.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/rsa.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/fe_low_mem.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/fe_operations.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/ge_low_mem.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/ge_operations.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/hash.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/hmac.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/integer.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/logging.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/md5.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/memory.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/poly1305.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/pwdbased.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/random.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/kdf.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/sha.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/sha256.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/sha512.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/wc_encrypt.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/wc_port.o  \
	$(WOLFSSL_BUILD)/wolfcrypt/wolfmath.o \
    $(WOLFSSL_BUILD)/wolfcrypt/iotsafe.o \
    $(WOLFSSL_BUILD)/tls13.o
	
OBJS_SPMATH:= $(WOLFSSL_BUILD)/wolfcrypt/sp_c32.o  \
 		 		$(WOLFSSL_BUILD)/wolfcrypt/sp_int.o

OBJS+=$(WOLFSSL_OBJS) $(OBJS_SPMATH)

vpath %.c $(dir $(WOLFSSL_ROOT)/src)
vpath %.c $(dir $(WOLFSSL_ROOT)/wolfcrypt/src)

#CFLAGS+=-g -ggdb3
CFLAGS+=-Os

#all: image.bin

image.bin: image.elf
	$(OBJCOPY) -O binary $^ $@

image.elf: $(WOLFSSL_BUILD)/wolfcrypt $(OBJS) $(LSCRIPT)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

$(WOLFSSL_BUILD)/wolfcrypt:
	mkdir -p $(@)

%.o:%.S
	$(CC) -c -o $(@) $(CFLAGS) $^

%.o:%.c
	$(CC) -c -o $(@) $(CFLAGS) $^

$(WOLFSSL_BUILD)/%.o: $(WOLFSSL_ROOT)/src/%.c
	$(CC) -c -o $(@) $(CFLAGS) $^

$(WOLFSSL_BUILD)/wolfcrypt/%.o: $(WOLFSSL_ROOT)/wolfcrypt/src/%.c
	$(CC) -c -o $(@) $(CFLAGS) $^

$(WOLFSSL_BUILD)/wolfcrypt/iotsafe.o: $(WOLFSSL_ROOT)/wolfcrypt/src/port/iotsafe/iotsafe.c
	$(CC) -c -o $(@) $(CFLAGS) $^

clean:
	rm -f image.bin image.elf *.o image.map
	rm -rf build
