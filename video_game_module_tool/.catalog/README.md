# Video Game Module Tool

Standalone firmware updater/installer for the Video Game Module.

## Features

- Install the official VGM firmware directly from Flipper Zero (firmware comes bundled with the application)
- Install custom VGM firmware files in UF2 format from SD card (see limitations)

## Limitations

When creating a custom UF2 firmware image, some limitations are to keep in mind:

- Non-flash blocks are NOT supported
- Block payloads MUST be exactly 256 bytes
- Payload target addresses MUST be 256 byte-aligned with no gaps
- Features such as file containers and extension tags are NOT supported
