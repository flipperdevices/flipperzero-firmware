<picture>
    <source media="(prefers-color-scheme: dark)" srcset="/.github/assets/dark_theme_banner.png">
    <source media="(prefers-color-scheme: light)" srcset="/.github/assets/light_theme_banner.png">
    <img
        alt="Shows an illustrated sun in light mode and a moon with stars in dark mode"
        src="/.github/assets/light_theme_banner.png">
</picture>

[![Build](https://github.com/flipperdevices/flipperzero-firmware/actions/workflows/build.yml/badge.svg)](https://github.com/flipperdevices/flipperzero-firmware/actions/workflows/build.yml)
[![Discord](https://img.shields.io/discord/740930220399525928.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://flipperzero.one/discord)

# Commonly Asked Question

- [Flipper Zero Official Website](https://flipperzero.one). Simple way to explain your friends what Flipper can do.
- [Flipper Zero Firmware Update](https://update.flipperzero.one). Improvements for your dolphin: latest firmware releases, upgrade tools for PC and Mobile.
- [User Documentation](https://docs.flipperzero.one). Learn more about your dolphin: specs, usage guides, and everything that you wanted to ask.

# Development

## Requirements

Officially supported development platforms:

- Windows 10+ with PowerShell and Git (x86_64)
- macOS 12+ with Command Line tools (x86_64, arm64)
- Ubuntu 20.04+ with build-essential and Git (x86_64)

Officially supported In-circuit Debuggers(optional, but highly recommended):

- Flipper WiFi Board
- ST-Link
- J-Link

Everything else will be taken care of by Flipper Build System.

## Cloning Source Code

Ensure that you have enough space and clone source code with Git:

```shell
$ git clone --recursive https://github.com/flipperdevices/flipperzero-firmware.git
```

## Building

Build firmware using Flipper Build Tool:

```shell
$ ./fbt
```

## Flashing Firmware using In-circuit Debugger

Connect your In-circuit Debugger to the Flipper and flash firmware using Flipper Build Tool:

```shell
$ ./fbt flash
```

## Flashing Firmware using USB

Ensure that your flipper is working, connect it using USB cable and flash firmware using Flipper Build Tool:

```shell
$ ./fbt flash_usb
```

## Documentation We Love

- [Code Of Conduct](/CODE_OF_CONDUCT.md) - how not to spoil fun
- [Coding Style](/CODING_STYLE.md) - how to make your code nice
- [Flipper Build Tool](/documentation/fbt.md) - building, flashing, and debugging Flipper software
- [Applications](/documentation/AppsOnSDCard.md), [Application Manifest](/documentation/AppManifests.md) - developing, building, deploying, and debugging Flipper applications
- [Hardware combos and Un-bricking](/documentation/KeyCombo.md) - recovering your Flipper from most nasty situations
- [Flipper File Formats](/documentation/file_formats) - everything about how Flipper stores your data and how you can work with it
- [Universal Remotes](/documentation/UniversalRemotes.md) - contributing your infrared remote to universal remotes
- And much more in the [Documentation](/documentation) folder

# Links

- Discord: [flipp.dev/discord](https://flipp.dev/discord)
- Website: [flipperzero.one](https://flipperzero.one)
- Forum: [forum.flipperzero.one](https://forum.flipperzero.one/)
- Kickstarter: [kickstarter.com](https://www.kickstarter.com/projects/flipper-devices/flipper-zero-tamagochi-for-hackers)

# Project structure

- `applications`    - Applications and services used in firmware
- `assets`          - Assets used by applications and services
- `furi`            - Furi Core: OS-level primitives and helpers
- `debug`           - Debug tool: GDB-plugins, SVD-file and etc
- `documentation`   - Documentation generation system configs and input files
- `firmware`        - Firmware source code
- `lib`             - Our and 3rd party libraries, drivers, etc.
- `scripts`         - Supplementary scripts and python libraries home

Also, pay attention to `ReadMe.md` files inside those directories.
