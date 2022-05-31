### Makefile to build the FreeRTOS library ###

# Build target (options: sim, board)

TARGET      = sim
SMALL       =

# Tools

CC          = xt-xcc
AS          = xt-xcc
AR          = xt-ar
XT_CORE     = $(patsubst %-params,%,$(notdir $(shell xt-xcc --show-config=core)))
CONFIGDIR   = $(shell xt-xcc --show-config=config)

# For platform-specific commands

include $(CONFIGDIR)/misc/hostenv.mk

# Source code and build locations

SRCROOT     = $(subst /,$(S),$(CURDIR))
TSTROOT     = $(abspath $(SRCROOT)$(S)..$(S)..$(S)..$(S)..$(S)..$(S)demos$(S)cadence$(S)sim$(SMALL))
BLDROOT     = $(TSTROOT)$(S)build
BLDDIR      = $(BLDROOT)$(S)$(XT_CORE)

FR_SRCDIR   = $(abspath $(SRCROOT)$(S)..$(S)..$(S)..)
FR_SRCDIR2  = $(FR_SRCDIR)$(S)portable$(S)MemMang
XT_SRCDIR   = $(SRCROOT)

vpath %.c $(FR_SRCDIR) $(FR_SRCDIR2) $(XT_SRCDIR)
vpath %.S $(XT_SRCDIR)

# File lists

FR_C_FILES  = $(notdir $(wildcard $(FR_SRCDIR)/*.c)) $(notdir $(wildcard $(FR_SRCDIR2)/*.c))
XT_C_FILES  = $(notdir $(wildcard $(XT_SRCDIR)/*.c))
XT_S_FILES  = $(notdir $(wildcard $(XT_SRCDIR)/*.S))

# List of all .o files that will go into the library

LIB_C_O     = $(patsubst %.c,%.o,$(XT_C_FILES) $(FR_C_FILES))
LIB_S_O     = $(patsubst %.S,%.o,$(XT_S_FILES))
LIB_O_LIST  = $(addprefix $(BLDDIR)/,$(LIB_C_O) $(LIB_S_O))

# Output files

OSLIB       = $(BLDDIR)$(S)libfreertos.a

# Build options

ifeq ($(TARGET),sim)
DFLAGS      = -DXT_SIMULATOR
endif
ifeq ($(TARGET),board)
DFLAGS      = -DXT_BOARD
endif

IFLAGS      = \
  -I$(FR_SRCDIR)$(S)..$(S)include -I$(FR_SRCDIR)$(S)..$(S)include$(S)private \
  -I$(XT_SRCDIR) -I$(TSTROOT)$(S)common$(S)config_files -I$(BLDDIR)

CFLAGS      = -O2 -g
CCFLAGS     = $(CFLAGS) -Wall -mno-coproc -mlongcalls -ffunction-sections -mno-l32r-flix $(DFLAGS)
ASFLAGS     = $(CCFLAGS)

# Include dependency rules (generated using -MD)

-include $(wildcard $(BLDDIR)/*.d)

# Targets

all : mkdir $(OSLIB)

mkdir : $(BLDDIR)/.mkdir

$(BLDDIR)/.mkdir :
	@$(MKPATH) $(BLDDIR)
	@echo "" > $@
	-$(CP) $(CONFIGDIR)/xtensa-elf/include/sys/reent.h $(BLDDIR)/reent.h

$(OSLIB) : $(LIB_O_LIST)
	$(AR) -rs $@ $^

$(BLDDIR)/%.o : %.c
	$(CC) $(CCFLAGS) $(IFLAGS) -MD -MF $(subst .o,.d,$@) -c -o $@ $<

$(BLDDIR)/%.o : %.S
	$(CC) $(ASFLAGS) $(IFLAGS) -MD -MF $(subst .o,.d,$@) -c -o $@ $<

clean :
	$(RM_R) $(BLDDIR)

clean_all :
	$(RM_R) $(BLDROOT)

.PHONY : all mkdir clean clean_all

