# Flipper Zero Firmware

[![Discord](https://img.shields.io/discord/740930220399525928.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](http://flipperzero.one/discord)

![Show me the code](https://habrastorage.org/webt/eo/m0/e4/eom0e4btudte7nrhnyic-laiog0.png)

Welcome to [Flipper Zero](https://flipperzero.one/)'s Firmware repo!
Our goal is to create nice and clean code with good documentation, to make it a pleasure for everyone to work with.

# Update firmware

[Get Latest Firmware from Update Server](https://update.flipperzero.one/)


Flipper Zero's firmware consists of three components:

- Core2 firmware set - proprietary components by ST: FUS + radio stack.
- Core1 Bootloader - controls basic hardware initialization and loads firmware
- Core1 Firmware - HAL + OS + Drivers + Applications

All 3 of them must be flashed in order described.

## With STLink

### Core2 flashing procedures

Prerequisites:

- Linux / macOS
- Terminal
- STM32_Programmer_CLI added to $PATH

One liner: `./flash_core2_ble.sh`

### Core1 Bootloader + Firmware

Prerequisites:

- Linux / macOS
- Terminal
- [arm-gcc-none-eabi](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
- openocd

One liner: `./flash_core1_main.sh`

## With USB DFU 

1. Download latest [Firmware](https://update.flipperzero.one)

2. Reboot Flipper to Bootloader 
 - Press and hold `← Left` + `↩ Back` for reset 
 - Release `↩ Back` and keep holding `← Left` until blue LED lights up
 - Release `← Left`
<!-- ![Switch to DFU sequence](https://habrastorage.org/webt/uu/c3/g2/uuc3g2n36f2sju19rskcvjzjf6w.png) -->

3. Run `dfu-util -D full.dfu -a 0`

# Build from source

## Docker
### Prerequisites

1. Install [Docker Engine and Docker Compose](https://www.docker.com/get-started)
2. Clone the repo:
   ```sh
   git clone https://github.com/flipperdevices/flipperzero-firmware
   cd flipperzero-firmware
   ```
3. Prepare the container:
   ```sh
   docker-compose up -d
   ```

### Compile everything

```sh
docker-compose exec dev make
```

### Flash everything

```sh
docker-compose exec dev make whole
```

### Compile bootloader

```sh
docker-compose exec dev make -C bootloader
```

Bootloader compilation results:
* `bootloader/.obj/f7/bootloader.elf`
* `bootloader/.obj/f7/bootloader.hex`
* `bootloader/.obj/f7/bootloader.bin`
* **`bootloader/.obj/f7/bootloader.dfu`** - should be used to flash

### Compile firmware

```sh
docker-compose exec dev make -C firmware
```

Firmware compilation results:
* `firmware/.obj/f7/firmware.elf`
* `firmware/.obj/f7/firmware.hex`
* `firmware/.obj/f7/firmware.bin`
* **`firmware/.obj/f7/firmware.dfu`** - should be used to flash

### Concatenate bootloader and firmware

You might want to do this to distribute the firmware as a single file.

That's exactly how we generate our `full` builds.

1. Concatenate HEX files:
   ```sh
   docker-compose exec dev srec_cat \
    bootloader/.obj/f7/bootloader.hex -Intel \
    firmware/.obj/f7/firmware.hex -Intel \
    -o firmware/.obj/f7/full.hex -Intel
   ```
2. Convert HEX to DFU:
   ```sh
   docker-compose exec dev hex2dfu \
    -i firmware/.obj/f7/full.hex \
    -o firmware/.obj/f7/full.dfu \
    -l "Flipper Zero F7"
   ```

Finally, you will have **`firmware/.obj/f7/full.dfu`** file that can be distributed and flashed.

## MacOS
### Prerequisites
1. Install `gcc-arm-embedded`
```
brew install --cask gcc-arm-embedded
```

2. Install `open-ocd` if you want use debugger
```
brew install open-ocd
```

3. Build hex2dfu from source. Clone repo, build executable file and copy it to `PATH`
```
git clone git@github.com:rusdacent/hex2dfu.git
cd hex2dfu
gcc hex2dfu.c ED25519/*.c -o hex2dfu
sudo cp hex2dfu /usr/local/bin
```

### Compile firmware
```
make
```

### Flash to device via STLink
```
make flash
```

### Flash firmware to device via USB
Before you need install `dfu-util` on your mac:
```
brew install dfu-util
```
Then you need build firmware code via `make` command and then you can flash on your Flipper:
```
dfu-util -a 0 -D firmware/.obj/f7/firmware.dfu
```
Where `f7` - target

### Launch GNU Debugger
```
make debug
```

## Linux
### Prerequisites
First, let's install `gcc-arm-embedded`. This is harder than it looks since [the ppa repository](https://launchpad.net/gcc-arm-embedded) is marked deprecated.
1. Download latest gcc-arm-none-eabi from [arm site](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
```
wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2
```
2. Then unzip it to any directory and move/link your executables to the directory that is in the PATH. For example (please, replace `ggcc-arm-none-eabi-10.3-2021.10` to your version):
```
sudo tar xjf gcc-arm-none-eabi-10.3-2021.10-x86_64-linux.tar.bz2 -C /usr/share/
sudo ln -s /usr/share/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gcc /usr/bin/arm-none-eabi-gcc 
sudo ln -s /usr/share/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-g++ /usr/bin/arm-none-eabi-g++
sudo ln -s /usr/share/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-gdb /usr/bin/arm-none-eabi-gdb
sudo ln -s /usr/share/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-size /usr/bin/arm-none-eabi-size
sudo ln -s /usr/share/gcc-arm-none-eabi-10.3-2021.10/bin/arm-none-eabi-objcopy /usr/bin/arm-none-eabi-objcopy
```

If your unzip command fail with error `bzip2: Cannot exec: No such file or directory` you need install bzip2 before:
```
sudo apt-get install bzip2
```
3. Build hex2dfu from source. Clone repo, build executable file and copy it to `PATH`
```
git clone https://github.com/rusdacent/hex2dfu.git
cd hex2dfu 
gcc hex2dfu.c ED25519/*.c -o hex2dfu 
mv ./hex2dfu /usr/local/bin/hex2dfu 
```
4. Install `open-ocd` if you want use debugger
```
sudo apt install openocd
```

### Compile firmware
```
make
```

### Flash to device via STLink
```
make flash
```

### Flash firmware to device via USB
Before you need install `dfu-util` on your mac:
```
sudo apt install dfu-util
```
Then you need build firmware code via `make` command and then you can flash on your Flipper:
```
dfu-util -a 0 -D firmware/.obj/f7/firmware.dfu
```
Where `f7` - target

# Links
* Discord: [flipp.dev/discord](https://flipp.dev/discord)
* Website: [flipperzero.one](https://flipperzero.one)
* Kickstarter page: [kickstarter.com](https://www.kickstarter.com/projects/flipper-devices/flipper-zero-tamagochi-for-hackers)
* Forum: [forum.flipperzero.one](https://forum.flipperzero.one/)

# Folders structure

- applications - application and services
  * accessor - Wiegand server
  * archive - Archive and file manager 
  * bt - BLE service and application
  * cli - Console service
  * debug_tools - different tools that we use on factory and for debug
  * dialogs - service for showing GUI dialogs
  * dolphin - dolphin service and supplientary apps
  * gpio-tester - GPIO control application
  * gui - GUI service
  * ibutton - ibutton application, onewire keys and more
  * input - input service
  * irda - irda application, controls your IR devices 
  * irda_monitor - irda debug tool 
  * lfrfid - LF RFID application
  * lfrfid-debug - LF RFID debug tool
  * loader - application loader service
  * menu - main menu service
  * music-player - music player app (demo)
  * nfc - NFC application, HF rfid, EMV and etc
  * notification - notification service 
  * power - power service
  * power-observer - power debug tool
  * scened-app-example - c++ application example 
  * storage - storage service, internal + sdcard
  * storage_settings - storage settings app
  * subghz - subghz application, 433 fobs and etc
  * tests - unit tests and etc
- assets - assets used by applications and services
  * compiled - compilation results
  * icons - source icons images
- bootloader - bootloader for flipper
  * src - bootloader sources
  * targets - targets' hal and implementation
- core - core libraries: home for furi
- debug - debug helpers, plugins and tools
- docker - docker image sources (used for automated firmware build)
- documentation - documentation generation system configs and input files
- firmware - firmware for flipper
  * targets - targets' hal and implementation
- lib - different libraries and drivers that apps and firmware uses
  * ST25RFAL002 - ST253916 driver and NFC hal
  * STM32CubeWB - STM32WB hal
  * app-scened-template - scened template app library
  * app-template - template app library
  * callback-connector - callback connector library
  * common-api - common api delaration library
  * cyfral - cyfral library
  * drivers - drivers that we wrote
  * fatfs - external storage file system
  * fnv1a-hash - fnv1a hash library 
  * irda - irda library
  * littlefs - internal storage file system
  * mlib - algorithms and containers 
  * nfc_protocols - nfc protocols library
  * onewire - one wire library 
  * qrcode - qr code generator library
  * subghz - subghz library
  * toolbox - toolbox of things that we are using but don't place in core
  * u8g2 - graphics library that we use to draw GUI
- make - make helpers
- scripts - supplimentary scripts
