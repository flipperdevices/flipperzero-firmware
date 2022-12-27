<picture>
    <source media="(prefers-color-scheme: dark)" srcset="/.github/assets/dark_theme_banner.png">
    <source media="(prefers-color-scheme: light)" srcset="/.github/assets/light_theme_banner.png">
    <img
        alt="Shows an illustrated sun in light mode and a moon with stars in dark mode"
        src="/.github/assets/light_theme_banner.png">
</picture>

- [Flipper Zero Official Website](https://flipperzero.one). Simple way to explain your friends what Flipper can do.
- [Flipper Zero Firmware Update](https://update.flipperzero.one). Improvements for your dolphin: latest firmware releases, upgrade tools for PC and Mobile.
- [User Documentation](https://docs.flipperzero.one). Learn more about your dolphin: specs, usage guides, and everything that you wanted to ask.

# Contributing

Our main goal is to build healthy, sustainable community around flipper and to be open to any new ideas and contributions. Also we do have couple rules and taboos here, so please read this chapter carefully and especially [Code Of Conduct](/CODE_OF_CONDUCT.md).

## I need help

The best place to search for an answers is our [User Documentation](https://docs.flipperzero.one) but if you can not find answers there check our [Discord Server](https://flipp.dev/discord) or [Forum](https://forum.flipperzero.one/).

## I want to report issue

If you found issue and you want to report it please check our [Issues](https://github.com/flipperdevices/flipperzero-firmware/issues). Make sure that description contains information about used versions, platform, and way to reproduce issue.

## I want to contribute code

Before opening PR make sure that your changes really must be in the firmware itself. Many ideas can easily be implemented as external applications and place to flipper application catalog. If you are not sure you can ask on [Discord Server](https://flipp.dev/discord) or in [Issues](https://github.com/flipperdevices/flipperzero-firmware/issues) and we'll help you to find right place for your code. Also please read [Contribution Guide](/CONTRIBUTING.md), [Coding Style](/CODING_STYLE.md) and ensure that your code is compatible with project [License](/LICENSE). After you make sure that everything is good to go feel free to open [Pull Request](https://github.com/flipperdevices/flipperzero-firmware/pulls).

# Development

Flipper Zero Firmware is written in C with some bits and pieces written in armv7m assembly language. Intermediate level will be the best for comfortable programming. Flipper applications can be developer using C/C++.  

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

## Documentation

- [Flipper Build Tool](/documentation/fbt.md) - building, flashing, and debugging Flipper software
- [Applications](/documentation/AppsOnSDCard.md), [Application Manifest](/documentation/AppManifests.md) - developing, building, deploying, and debugging Flipper applications
- [Hardware combos and Un-bricking](/documentation/KeyCombo.md) - recovering your Flipper from most nasty situations
- [Flipper File Formats](/documentation/file_formats) - everything about how Flipper stores your data and how you can work with it
- [Universal Remotes](/documentation/UniversalRemotes.md) - contributing your infrared remote to universal remotes
- [Firmware RoadMap](/documentation/RoadMap.md)
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
