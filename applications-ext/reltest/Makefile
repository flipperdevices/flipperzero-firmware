$(error Now you're on your own, please bring the Makefile (I think you only need the includes) to match CMakeLists.txt)

CROSS?=arm-none-eabi-
CC=$(CROSS)gcc
LD=$(CROSS)gcc
STRIP=$(CROSS)strip
SIZE=$(CROSS)size

SRC=main.c start.c snake-game.c

OPT?=0

CFLAGS=-mcpu=cortex-m3 -mthumb -O$(OPT) -ggdb3 \
	-mword-relocations -mlong-calls -fno-common -nostdlib

LDFLAGS=-r -Bsymbolic -nostartfiles \
	-mcpu=cortex-m3 -mthumb -mlong-calls -fno-common -nostdlib \
	-T ../application-ext.ld

OBJS=$(SRC:.c=.o)
DEPS=$(SRC:.c=.d)

all: plugin.elf

-include $(DEPS)

%.o: %.c
	@echo " CC $<"
	@$(CC) -MMD $(CFLAGS) -o $@ -c $<

plugin.elf: $(OBJS)
	@echo " LINK $@"
	@$(LD) $(LDFLAGS) -o $@ $^
	@$(STRIP) -g -o plugin-striped.elf $@
	@$(SIZE) --common $@

.PHONY: clean all list

clean:
	@echo " CLEAN"
	@rm -fR $(OBJS) $(DEPS) *.elf
	@rm -f plugin.elf.list plugin.elf.read

list:
	@echo " Creating list..."
	@$(CROSS)objdump -hpxdSsrt plugin.elf > plugin.elf.list
	@$(CROSS)readelf -atSln plugin.elf > plugin.elf.read