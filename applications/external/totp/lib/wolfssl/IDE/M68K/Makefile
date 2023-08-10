NAME    = wolfssl
DEVICE  = -m5206e -gdwarf-2 -DMOD5441X -DMCF5441X
OLEVEL  = -O2
NBHEADERS = -I"$(NBROOT)/include" -I"$(NBROOT)/MOD5441X/include"

# location to put wolfssl.a created
OUTPUT  = $(NBROOT)/lib


CSRCS := ../../wolfcrypt/src/rsa.c \
        ../../wolfcrypt/src/asn.c \
        ../../wolfcrypt/src/aes.c \
        ../../wolfcrypt/src/sha.c \
        ../../wolfcrypt/src/sha256.c \
        ../../wolfcrypt/src/sha512.c \
        ../../wolfcrypt/src/integer.c \
        ../../wolfcrypt/src/tfm.c \
        ../../wolfcrypt/src/random.c \
        ../../wolfcrypt/src/logging.c \
        ../../wolfcrypt/src/memory.c \
        ../../wolfcrypt/src/coding.c \
        ../../wolfcrypt/src/hash.c \
        ../../wolfcrypt/src/kdf.c \
        ../../wolfcrypt/src/hmac.c \
        ../../wolfcrypt/src/md5.c \
        ../../wolfcrypt/src/wc_port.c \
        ../../wolfcrypt/src/wc_encrypt.c \
        ../../wolfcrypt/src/wolfmath.c \
        ../../wolfcrypt/src/sp_c32.c \
        ../../wolfcrypt/src/sp_int.c \
        ../../wolfcrypt/src/signature.c \
        ../../wolfcrypt/src/error.c \
        ../../src/wolfio.c \
        ../../src/ssl.c

INC = -I"./" -I"../../"
EXTRACFLAGS  = -DWOLFSSL_USER_SETTINGS $(INC) $(DEVICE) $(OLEVEL)
EXTRACFLAGS += -DNBMINGW -Wall -fno-rtti -fno-exceptions -falign-functions=4
EXTRACFLAGS += $(NBHEADERS)
EXTRACXXFLAGS = $(EXTRACFLAGS)

#LDEXTRA =
LIBOBJS = ./rsa.o \
          ./asn.o \
          ./aes.o \
          ./sha.o \
          ./sha256.o \
          ./sha512.o \
          ./integer.o \
          ./tfm.o \
          ./random.o \
          ./logging.o \
          ./memory.o \
          ./coding.o \
          ./hash.o \
          ./hmac.o \
          ./md5.o \
          ./wc_port.o \
          ./wc_encrypt.o \
          ./wolfmath.o \
          ./sp_c32.o \
          ./sp_int.o \
          ./signature.o \
          ./wolfio.o \
          ./ssl.o \
          ./error.o

#include $(NBROOT)/make/libmain.mak

all: compile link rmo

# compiling using g++ compiler to avoid mangled name complications if linking
# against other NetBurner libraries
compile:
	m68k-elf-g++ $(EXTRACFLAGS) -c $(CSRCS)

link:
	m68k-elf-ar -cr $(OUTPUT)/$(NAME).a $(LIBOBJS)

rmo:
	rm -f *.o

clean: rmo
	rm -f $(OUTPUT)/$(NAME).a

help:
	@echo "all     : execute compile, link, rmo"
	@echo "compile : create .o files"
	@echo "link    : create .a library from .o files"
	@echo "rmo     : remove all .o files"
	@echo "clean   : remove all .o files and .a library"

