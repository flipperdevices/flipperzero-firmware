bl:
	docker-compose exec dev make -j$(nproc) -C bootloader

fw:
	docker-compose exec dev make -j$(nproc) -C firmware

hex: bootloader/.obj/f7/bootloader.hex firmware/.obj/f7/firmware.hex
	docker-compose exec dev srec_cat bootloader/.obj/f7/bootloader.hex -Intel firmware/.obj/f7/firmware.hex -Intel -o firmware/.obj/f7/full.hex -Intel

dfu: firmware/.obj/f7/full.hex
	docker-compose exec dev hex2dfu -i firmware/.obj/f7/full.hex -o firmware/.obj/f7/full.dfu -l "Flipper Zero F7"

all: bl fw hex dfu

flash: firmware/.obj/f7/full.dfu
	dfu-util -D firmware/.obj/f7/full.dfu -a 0

clean-bl:
	sudo rm -rf bootloader/.obj

clean-fw:
	sudo rm -rf firmware/.obj

clean: clean-bl clean-fw
	