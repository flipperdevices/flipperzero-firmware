bootloader:
	$(MAKE) -C bootloader -j9 all

firmware:
	$(MAKE) -C firmware -j9 all

bootloader_clean:
	$(MAKE) -C bootloader -j9 clean

firmware_clean:
	$(MAKE) -C firmware -j9 clean

bootloader_flash:
	rm bootloader/.obj/f*/flash || true
	$(MAKE) -C bootloader -j9 flash

firmware_flash:
	rm firmware/.obj/f*/flash || true
	$(MAKE) -C firmware -j9 flash

all: bootloader firmware

clean: bootloader_clean firmware_clean

flash: bootloader_flash firmware_flash

debug:
	$(MAKE) -C firmware -j9 debug

.PHONY: all bootloader firmware debug
