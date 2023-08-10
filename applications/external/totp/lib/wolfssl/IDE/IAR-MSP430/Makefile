PROGRAM = wolfcrypt
WOLFSSL_SRC_DIR = ../..
TI_COMPILER_PATH = /opt/ti/msp430-gcc/bin

WOLFSSL_CFLAGS += -I$(WOLFSSL_SRC_DIR) -I. -DWOLFSSL_USER_SETTINGS

SRC_FILES  = $(wildcard $(WOLFSSL_SRC_DIR)/src/*.c)
SRC_FILES += $(wildcard $(WOLFSSL_SRC_DIR)/wolfcrypt/src/*.c)
SRC_FILES := $(filter-out %bio.c %misc.c %evp.c, $(SRC_FILES))

SRC = main.c \
         $(SRC_FILES)

CFLAGS += $(WOLFSSL_CFLAGS) -O3 -mmcu=msp430f5659 -I/opt/ti/msp430-gcc/include -L/opt/ti/msp430-gcc/include -mlarge -mcode-region=either -fdata-sections -ffunction-sections -Wl,--gc-sections

CC = $(TI_COMPILER_PATH)/msp430-elf-gcc
STRIP = $(TI_COMPILER_PATH)/msp430-elf-strip

$(PROGRAM): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) $(LDLIBS) -o $@
	$(STRIP) $@

clean:
	rm -f $(PROGRAM)

install:
	mspdebug tilib "prog $(PROGRAM)" --allow-fw-update
