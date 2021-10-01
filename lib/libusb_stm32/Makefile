CMSIS       ?= CMSIS
CMSISDEV    ?= $(CMSIS)/Device
CMSISCORE   ?= $(CMSIS)/CMSIS/Include $(CMSIS)/CMSIS/Core/Include
FLASH       ?= st-flash
TOOLSET     ?= arm-none-eabi-
CC           = $(TOOLSET)gcc
LD           = $(TOOLSET)gcc
AR           = $(TOOLSET)gcc-ar
OBJCOPY      = $(TOOLSET)objcopy
DFU_UTIL    ?= dfu-util
STPROG_CLI  ?= ~/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI
OPTFLAGS    ?= -Os

ifeq ($(OS),Windows_NT)
	RM = del /Q
	fixpath = $(strip $(subst /,\, $1))
else
	RM = rm -f
	fixpath = $(strip $1)
endif

MODULE      ?= libusb.a
CFLAGS      ?= -mcpu=cortex-m3
DEFINES     ?= STM32F1 STM32F103x6

ARFLAGS      = -cvq
LDFLAGS      = --specs=nano.specs -nostartfiles -Wl,--gc-sections -L demo
INCLUDES     = $(CMSISDEV)/ST $(CMSISCORE) inc
CFLAGS2     ?= $(CFLAGS) -mthumb -std=gnu99 $(OPTFLAGS)

OBJDIR       = obj
SOURCES      = $(wildcard src/*.c) $(wildcard src/*.S)
OBJECTS      = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(notdir $(basename $(SOURCES)))))
DSRC         = $(wildcard demo/*.c) $(wildcard demo/*.S) $(STARTUP)
DOBJ         = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(notdir $(basename $(DSRC)))))
DOUT         = cdc_loop

SRCPATH      = $(sort $(dir $(SOURCES) $(DSRC)))
vpath %.c $(SRCPATH)
vpath %.S $(SRCPATH)
vpath %.s $(SRCPATH)


help all:
	@echo 'Usage: make target [program]'
	@echo 'Available targets are:'
	@echo '  bluepill'
	@echo '  stm32f103x6   CDC loopback demo for STM32F103 based boards'
	@echo '  32l100c-disco'
	@echo '  stm32l100xc   CDC loopback demo for STM32L100xC based boards'
	@echo '  32l476rg-nucleo'
	@echo '  stm32l476rg   CDC loopback demo for STM32L476xG based boards'
	@echo '  stm32l052x8   CDC loopback demo for STM32L052x8 based boards'
	@echo '  32f429zi-nucleo'
	@echo '  stm32f429xi   CDC loopback demo for STM32F429xI based boards'
	@echo '  stm32f401xc   CDC loopback demo for STM32F401xC based boards'
	@echo '  stm32f401xe   CDC loopback demo for STM32F401xE based boards'
	@echo '  cmsis         Download CMSIS 5 and stm32.h into a $$(CMSIS) directory'
	@echo '  doc           DOXYGEN documentation'
	@echo '  module        static library module using following envars (defaults)'
	@echo '                MODULE  module name ($(MODULE))'
	@echo '                CFLAGS  mcu specified compiler flags ($(CFLAGS))'
	@echo '                DEFINES mcu and module specified defines ($(DEFINES))'
	@echo '                        see USB Device HW driver and core API section for the'
	@echo '                        compile-time control macros'
	@echo ' '
	@echo 'Environmental variables (defaults):'
	@echo '  CMSIS         Path to the CMSIS V4 or CMSIS V5 root folder ($(CMSIS))'
	@echo '  CMSISCORE     Path to the CMSIS Core include folder(s) ($(CMSISCORE))'
	@echo '  CMSISDEV      Path to the CMSIS Device folder ($(CMSISDEV))'
	@echo '  FLASH         st-link flash utility ($(FLASH))'
	@echo ' '
	@echo 'Examples:'
	@echo '  make bluepill program'
	@echo '  make module MODULE="usbd.a" CFLAGS="-mcpu=cortex-m4" DEFINES="STM32L4 STM32L476xx USBD_VBUS_DETECT"'

cmsis: $(CMSISDEV)/ST

$(CMSISDEV)/ST: $(CMSIS)
	@git clone --recurse-submodules --depth 1 https://github.com/dmitrystu/stm32h.git $@

$(CMSIS):
	@git clone --depth 1 https://github.com/ARM-software/CMSIS_5.git $@

$(OBJDIR):
	@mkdir $@

program: $(DOUT).hex
	$(FLASH) --reset --format ihex write $(DOUT).hex

program_dfu: $(DOUT).bin
	$(DFU_UTIL) -d 0483:DF11 -a 0 -D $(DOUT).bin -s 0x08000000

program_stcube: $(DOUT).hex
	$(STPROG_CLI) -c port=SWD reset=HWrst -d $(DOUT).hex -hardRst

demo: $(DOUT).hex $(DOUT).bin

$(DOUT).bin : $(DOUT).elf
	@echo building $@
	@$(OBJCOPY) -O binary $< $@

$(DOUT).hex : $(DOUT).elf
	@echo building $@
	@$(OBJCOPY) -O ihex $< $@

$(DOUT).elf : $(OBJDIR) $(DOBJ) $(OBJECTS)
	@echo building $@
	@$(LD) $(CFLAGS2) $(LDFLAGS) -Wl,--script='$(LDSCRIPT)' -Wl,-Map=$(DOUT).map $(DOBJ) $(OBJECTS) -o $@

clean: $(OBJDIR)
	@$(RM) $(DOUT).*
	@$(RM) $(call fixpath, $(OBJDIR)/*.*)

doc:
	doxygen

module: clean
	$(MAKE) $(MODULE)

$(MODULE): $(OBJDIR) $(OBJECTS)
	@$(AR) $(ARFLAGS) $(MODULE) $(OBJECTS)

$(OBJDIR)/%.o: %.c
	@echo compiling $<
	@$(CC) $(CFLAGS2) $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCLUDES)) -c $< -o $@

$(OBJDIR)/%.o: %.S
	@echo assembling $<
	@$(CC) $(CFLAGS2) $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCLUDES)) -c $< -o $@

$(OBJDIR)/%.o: %.s
	@echo assembling $<
	@$(CC) $(CFLAGS2) $(addprefix -D, $(DEFINES)) $(addprefix -I, $(INCLUDES)) -c $< -o $@

.PHONY: module doc demo clean program help all program_stcube cmsis

stm32f103x6 bluepill: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103x6.s' \
						LDSCRIPT='demo/stm32f103x6.ld' \
						DEFINES='STM32F1 STM32F103x6 USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m3'

stm32f303xe 32f303re-nucleo: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F3xx/Source/Templates/gcc/startup_stm32f303xe.s' \
						LDSCRIPT='demo/stm32f303xe.ld' \
						DEFINES='STM32F3 STM32F303xE USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m4'

stm32f303xc: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F3xx/Source/Templates/gcc/startup_stm32f303xc.s' \
						LDSCRIPT='demo/stm32f303xc.ld' \
						DEFINES='STM32F3 STM32F303xC USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m4'

stm32f105xb: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f105xc.s' \
						LDSCRIPT='demo/stm32f105xb.ld' \
						DEFINES='STM32F1 STM32F105xC USBD_VBUS_DETECT USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m3 -Wp,-w'

stm32f107xb: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f107xc.s' \
						LDSCRIPT='demo/stm32f105xb.ld' \
						DEFINES='STM32F1 STM32F107xC HSE_25MHZ USBD_VBUS_DETECT USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m3 -Wp,-w'

stm32l052x8: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32L0xx/Source/Templates/gcc/startup_stm32l052xx.s' \
						LDSCRIPT='demo/stm32l052x8.ld' \
						DEFINES='STM32L0 STM32L052xx USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m0plus'

stm32l100xc 32l100c-disco: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32L1xx/Source/Templates/gcc/startup_stm32l100xc.s' \
						LDSCRIPT='demo/stm32l100xc.ld' \
						DEFINES='STM32L1 STM32L100xC USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m3'

stm32l476xg 32l476rg-nucleo: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32L4xx/Source/Templates/gcc/startup_stm32l476xx.s' \
						LDSCRIPT='demo/stm32l476xg.ld' \
						DEFINES='STM32L4 STM32L476xx USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m4'

stm32f429xi 32f429zi-nucleo: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f429xx.s' \
						LDSCRIPT='demo/stm32f429xi.ld' \
						DEFINES='STM32F4 STM32F429xx USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m4'

stm32f429xi_hs 32f429zi-nucleo_hs: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f429xx.s' \
						LDSCRIPT='demo/stm32f429xi.ld' \
						DEFINES='STM32F4 STM32F429xx USBD_PRIMARY_OTGHS USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m4'

stm32l433cc: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32L4xx/Source/Templates/gcc/startup_stm32l433xx.s' \
						LDSCRIPT='demo/stm32l433xc.ld' \
						DEFINES='STM32L4 STM32L433xx USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m4'

stm32wb55: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32WB/Source/Templates/gcc/startup_stm32wb55xx_cm4.s' \
						LDSCRIPT='demo/stm32wb55xc.ld' \
						DEFINES='STM32WB STM32WB55xx USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m4'


stm32f070xb: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F0xx/Source/Templates/gcc/startup_stm32f070xb.s' \
						LDSCRIPT='demo/stm32f070xb.ld' \
						DEFINES='STM32F0 STM32F070xB USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m0'

stm32g431xb 32g431rb-nucleo: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32G4xx/Source/Templates/gcc/startup_stm32g431xx.s' \
						LDSCRIPT='demo/stm32g431xb.ld' \
						DEFINES='STM32G4 STM32G431xx USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m4'

stm32f446xc 32f446re-nucleo:  clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f446xx.s' \
						LDSCRIPT='demo/stm32f446xc.ld' \
						DEFINES='STM32F4 STM32F446xx USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m4'

stm32f446xc_hs 32f446re-nucleo_hs:  clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f446xx.s' \
						LDSCRIPT='demo/stm32f446xc.ld' \
						DEFINES='STM32F4 STM32F446xx USBD_SOF_DISABLED USBD_PRIMARY_OTGHS' \
						CFLAGS='-mcpu=cortex-m4'

stm32f373xc: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F3xx/Source/Templates/gcc/startup_stm32f373xc.s' \
						LDSCRIPT='demo/stm32f373xc.ld' \
						DEFINES='STM32F3 STM32F373xC USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m4'

stm32l053x8 32l053r8-nucleo: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32L0xx/Source/Templates/gcc/startup_stm32l053xx.s' \
						LDSCRIPT='demo/stm32l052x8.ld' \
						DEFINES='STM32L0 STM32L053xx USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m0plus'

stm32f405xg: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f405xx.s' \
						LDSCRIPT='demo/stm32f405xg.ld' \
						DEFINES='STM32F4 STM32F405xx USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m4'

stm32f405xg_hs: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f405xx.s' \
						LDSCRIPT='demo/stm32f405xg.ld' \
						DEFINES='STM32F4 STM32F405xx USBD_SOF_DISABLED USBD_PRIMARY_OTGHS' \
						CFLAGS='-mcpu=cortex-m4'

stm32f401xc: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f401xc.s' \
						LDSCRIPT='demo/stm32f401xc.ld' \
						DEFINES='STM32F4 STM32F401xC USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m4'

stm32f401xe: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f401xe.s' \
						LDSCRIPT='demo/stm32f401xe.ld' \
						DEFINES='STM32F4 STM32F401xE USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m4'

stm32f745xe: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f745xx.s' \
						LDSCRIPT='demo/stm32f745xe.ld' \
						DEFINES='STM32F7 STM32F745xx USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m7'

stm32f745xe_hs: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f745xx.s' \
						LDSCRIPT='demo/stm32f745xe.ld' \
						DEFINES='STM32F7 STM32F745xx USBD_SOF_DISABLED USBD_PRIMARY_OTGHS' \
						CFLAGS='-mcpu=cortex-m7'

stm32f042f6: clean
	@$(MAKE) demo STARTUP='$(CMSISDEV)/ST/STM32F0xx/Source/Templates/gcc/startup_stm32f042x6.s' \
						LDSCRIPT='demo/stm32f042x6.ld' \
						DEFINES='STM32F0 STM32F042x6 USBD_SOF_DISABLED' \
						CFLAGS='-mcpu=cortex-m0 -DUSBD_PINS_REMAP'
