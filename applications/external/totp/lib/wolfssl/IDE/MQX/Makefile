WOLF_ROOT = ../..
MQX_ROOT = $(WOLF_ROOT)/../../../Freescale/Freescale_MQX_4_1/
CC       = arm-none-eabi-gcc
AR		 = arm-none-eabi-ar

WOLFLIB  = ./libwolfssl.a
MQXLIB   =

OPTIMIZE = -O2

WORNING  = \
 -Wno-int-to-pointer-cast\
 -Wno-incompatible-pointer-types\
 -Wno-discarded-qualifiers\
 -Wno-int-conversion\
 -Wno-implicit-function-declaration\
 -Wno-pointer-to-int-cast

USER_SETTINGS_H = -I./ -DWOLFSSL_USER_SETTINGS
CFLAGS   = $(OPTIMIZE) -I$(WOLF_ROOT)/. $(USER_SETTINGS_H) $(MQX_INCLUDE) $(WORNING)

MQX_INCLUDE = -I$(MQX_ROOT)/mqx/source/include\
-I$(MQX_ROOT)/mqx/source/psp/cortex_m\
-I$(MQX_ROOT)/mqx/source/psp/cortex_a/compiler/gcc_arm\
-I$(MQX_ROOT)/mqx/source/psp/cortex_m/cpu\
-I$(MQX_ROOT)/mfs/source/include\
-I$(MQX_ROOT)/mqx/source/bsp/twrk70f120m\
-I$(MQX_ROOT)/mqx/source/io/cm\
-I$(MQX_ROOT)/mqx/source/io/lpm\
-I$(MQX_ROOT)/mqx/source/io/serial\
-I$(MQX_ROOT)/mqx/source/io/dma\
-I$(MQX_ROOT)/mqx/source/io/spi\
-I$(MQX_ROOT)/mqx/source/io/i2c\
-I$(MQX_ROOT)/mqx/source/io/io_mem\
-I$(MQX_ROOT)/mqx/source/io/io_null\
-I$(MQX_ROOT)/mqx/source/io/enet\
-I$(MQX_ROOT)/mqx/source/io/enet/macnet\
-I$(MQX_ROOT)/mqx/source/io/lwgpio\
-I$(MQX_ROOT)/mqx/source/io/gpio\
-I$(MQX_ROOT)/mqx/source/io/gpio/kgpio\
-I$(MQX_ROOT)/mqx/source/io/esdhc\
-I$(MQX_ROOT)/mqx/source/io/sdcard\
-I$(MQX_ROOT)/mqx/source/io/sdcard/sdcard_spi\
-I$(MQX_ROOT)/mqx/source/io/sdcard/sdcard_esdhc\
-I$(MQX_ROOT)/mqx/source/io/pccard\
-I$(MQX_ROOT)/mqx/source/io/pcflash\
-I$(MQX_ROOT)/mqx/source/io/adc/kadc\
-I$(MQX_ROOT)/mqx/source/io/adc\
-I$(MQX_ROOT)/mqx/source/io/lwadc\
-I$(MQX_ROOT)/mqx/source/io/flashx\
-I$(MQX_ROOT)/mqx/source/io/flashx/freescale\
-I$(MQX_ROOT)/mqx/source/io/hwtimer\
-I$(MQX_ROOT)/mqx/source/io/rtc\
-I$(MQX_ROOT)/mqx/source/io/debug\
-I$(MQX_ROOT)/mqx/source/io/tchres\
-I$(MQX_ROOT)/mqx/source/io/nandflash\
-I$(MQX_ROOT)/mqx/source/io/timer\
-I$(MQX_ROOT)/mqx/source/io/usb\
-I$(MQX_ROOT)/mqx/source/io/hmi\
-I$(MQX_ROOT)/mqx/source/io/sai\
-I$(MQX_ROOT)/mqx/source/io/rtcs\
-I$(MQX_ROOT)/rtcs/source/include

OBJ =\
$(WOLF_ROOT)/src/crl.o\
$(WOLF_ROOT)/src/internal.o\
$(WOLF_ROOT)/src/keys.o\
$(WOLF_ROOT)/src/ocsp.o\
$(WOLF_ROOT)/src/sniffer.o\
$(WOLF_ROOT)/src/ssl.o\
$(WOLF_ROOT)/src/tls13.o\
$(WOLF_ROOT)/src/tls.o\
$(WOLF_ROOT)/src/wolfio.o\
$(WOLF_ROOT)/wolfcrypt/src/aes.o\
$(WOLF_ROOT)/wolfcrypt/src/arc4.o\
$(WOLF_ROOT)/wolfcrypt/src/asm.o\
$(WOLF_ROOT)/wolfcrypt/src/asn.o\
$(WOLF_ROOT)/wolfcrypt/src/blake2b.o\
$(WOLF_ROOT)/wolfcrypt/src/blake2s.o\
$(WOLF_ROOT)/wolfcrypt/src/camellia.o\
$(WOLF_ROOT)/wolfcrypt/src/chacha20_poly1305.o\
$(WOLF_ROOT)/wolfcrypt/src/chacha.o\
$(WOLF_ROOT)/wolfcrypt/src/cmac.o\
$(WOLF_ROOT)/wolfcrypt/src/coding.o\
$(WOLF_ROOT)/wolfcrypt/src/compress.o\
$(WOLF_ROOT)/wolfcrypt/src/cpuid.o\
$(WOLF_ROOT)/wolfcrypt/src/cryptocb.o\
$(WOLF_ROOT)/wolfcrypt/src/curve25519.o\
$(WOLF_ROOT)/wolfcrypt/src/des3.o\
$(WOLF_ROOT)/wolfcrypt/src/dh.o\
$(WOLF_ROOT)/wolfcrypt/src/dsa.o\
$(WOLF_ROOT)/wolfcrypt/src/ecc.o\
$(WOLF_ROOT)/wolfcrypt/src/ecc_fp.o\
$(WOLF_ROOT)/wolfcrypt/src/ed25519.o\
$(WOLF_ROOT)/wolfcrypt/src/error.o\
$(WOLF_ROOT)/wolfcrypt/src/fe_low_mem.o\
$(WOLF_ROOT)/wolfcrypt/src/fe_operations.o\
$(WOLF_ROOT)/wolfcrypt/src/ge_low_mem.o\
$(WOLF_ROOT)/wolfcrypt/src/ge_operations.o\
$(WOLF_ROOT)/wolfcrypt/src/hash.o\
$(WOLF_ROOT)/wolfcrypt/src/hmac.o\
$(WOLF_ROOT)/wolfcrypt/src/integer.o\
$(WOLF_ROOT)/wolfcrypt/src/logging.o\
$(WOLF_ROOT)/wolfcrypt/src/md2.o\
$(WOLF_ROOT)/wolfcrypt/src/md4.o\
$(WOLF_ROOT)/wolfcrypt/src/md5.o\
$(WOLF_ROOT)/wolfcrypt/src/memory.o\
$(WOLF_ROOT)/wolfcrypt/src/pkcs12.o\
$(WOLF_ROOT)/wolfcrypt/src/pkcs7.o\
$(WOLF_ROOT)/wolfcrypt/src/poly1305.o\
$(WOLF_ROOT)/wolfcrypt/src/pwdbased.o\
$(WOLF_ROOT)/wolfcrypt/src/random.o\
$(WOLF_ROOT)/wolfcrypt/src/ripemd.o\
$(WOLF_ROOT)/wolfcrypt/src/rsa.o\
$(WOLF_ROOT)/wolfcrypt/src/sha256.o\
$(WOLF_ROOT)/wolfcrypt/src/sha3.o\
$(WOLF_ROOT)/wolfcrypt/src/sha512.o\
$(WOLF_ROOT)/wolfcrypt/src/sha.o\
$(WOLF_ROOT)/wolfcrypt/src/signature.o\
$(WOLF_ROOT)/wolfcrypt/src/sp_arm32.o\
$(WOLF_ROOT)/wolfcrypt/src/sp_arm64.o\
$(WOLF_ROOT)/wolfcrypt/src/sp_armthumb.o\
$(WOLF_ROOT)/wolfcrypt/src/sp_c32.o\
$(WOLF_ROOT)/wolfcrypt/src/sp_c64.o\
$(WOLF_ROOT)/wolfcrypt/src/sp_cortexm.o\
$(WOLF_ROOT)/wolfcrypt/src/sp_int.o\
$(WOLF_ROOT)/wolfcrypt/src/sp_x86_64.o\
$(WOLF_ROOT)/wolfcrypt/src/srp.o\
$(WOLF_ROOT)/wolfcrypt/src/tfm.o\
$(WOLF_ROOT)/wolfcrypt/src/wc_encrypt.o\
$(WOLF_ROOT)/wolfcrypt/src/wc_pkcs11.o\
$(WOLF_ROOT)/wolfcrypt/src/wc_port.o\
$(WOLF_ROOT)/wolfcrypt/src/wolfevent.o\
$(WOLF_ROOT)/wolfcrypt/src/wolfmath.o


all	: wolfssllib client server test benchmark

wolfssllib	: $(OBJ)
	$(AR) r $(WOLFLIB) $(OBJ)
test	: wolfssllib ./user_settings.h
	$(CC) -o test $(CFLAGS) $(WOLF_ROOT)/wolfcrypt/test/test.c $(WOLFLIB) $(MQXLIB)
benchmark	: wolfssllib ./user_settings.h
	$(CC)  -o benchmark $(CFLAGS)  $(WOLF_ROOT)/wolfcrypt/benchmark/benchmark.c $(WOLFLIB) $(WOLFLIB) $(MQXLIB)
server	:  wolfssllib ./user_settings.h
	$(CC)  -o server $(CFLAGS)  $(WOLF_ROOT)/IDE/MQX/server-tls.c $(WOLFLIB) $(MQXLIB)
client  :  wolfssllib ./user_settings.h
	$(CC)  -o client $(CFLAGS) $(WOLF_ROOT)/IDE/MQX/client-tls.c $(WOLFLIB) $(MQXLIB)
clean	: 
	rm $(OBJ) $(WOLFLIB)
