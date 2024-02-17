# Video Game Module Tool

This app is a standalone firmware updater/installer for the Video Game Module

## Features

- Install the official Video Game Module firmware directly from Flipper Zero (firmware comes bundled with the app).
- Install custom Video Game Module firmware files in UF2 format from a microSD card (see limitations).

## Limitations

When creating a custom UF2 firmware image, keep in mind the following limitations:
- Non-flash blocks are NOT supported.
- Block payloads MUST be exactly 256 bytes.
- Payload target addresses MUST be 256 byte-aligned with no gaps.
- Features such as file containers and extension tags are NOT supported.
