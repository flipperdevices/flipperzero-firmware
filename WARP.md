# WARP.md

This file provides guidance to WARP (warp.dev) when working with code in this repository.

## Build System (FBT - Flipper Build Tool)

FBT is the primary build system, wrapping SCons. All commands are invoked via `./fbt` from the firmware root.

### Common Build Commands

```bash
# Build firmware (default target)
./fbt

# Build and flash via debugger (SWD)
./fbt flash

# Build and flash via USB
./fbt flash_usb

# Build update package
./fbt updater_package

# Build with unit tests
./fbt FIRMWARE_APP_SET=unit_tests

# Format code before commit
./fbt format

# Run linters
./fbt lint

# Build external apps as FAPs
./fbt faps

# Build specific FAP by app ID
./fbt fap_<APPID>

# Debug with GDB
./fbt debug
```

### Development Commands

```bash
# Build and launch specific app on Flipper over USB
./fbt launch APPSRC=applications_user/path/to/app

# Build app without launching
./fbt build APPSRC=applications_user/path/to/app

# Generate VS Code configuration
./fbt vscode_dist

# Start CLI session
./fbt cli
```

## Testing

### Unit Tests

Unit tests run on-device to use actual hardware. Located in `applications/debug/unit_tests/`.

```bash
# Build firmware with unit tests
./fbt FIRMWARE_APP_SET=unit_tests updater_package

# Flash and run tests via CLI
./fbt flash_usb
./fbt cli
# In CLI: unit_tests

# Run specific test
# In CLI: unit_tests <test_name>
```

Test assets are in `applications/debug/unit_tests/resources/unit_tests/`.

## Architecture

### Furi Core (OS Abstraction)

Located in `furi/` directory. Provides RTOS primitives and OS-level abstractions:
- Threading (`core/thread.h`)
- Event loops (`core/event_loop.h`)
- Message queues, mutexes, semaphores
- Memory management (`core/memmgr.h`)
- Kernel functions (`core/kernel.h`)
- Logging (`core/log.h`)
- Records (service registry, `core/record.h`)
- Pub/sub messaging (`core/pubsub.h`)

Include via `#include <furi.h>` for core functionality.

### Furi HAL (Hardware Abstraction Layer)

Located in `targets/furi_hal_include/` and `targets/f7/furi_hal/` (for Flipper Zero hardware).
Provides hardware-specific APIs:
- GPIO, SPI, I2C, UART
- NFC/RFID hardware
- SubGHz radio (CC1101)
- USB, Bluetooth
- Display, buttons, vibro motor

Target-specific: `f7` for Flipper Zero, `f18` for other hardware.

### Application Structure

Applications are organized by type in `applications/`:
- `main/` - Main menu apps (NFC, SubGHz, infrared, iButton, etc.)
- `services/` - Background services (bt, gui, storage, loader, etc.)
- `settings/` - Settings apps
- `system/` - System utilities (updater, HID, etc.)
- `debug/` - Debug/test apps (unit_tests, etc.)
- `examples/` - Example apps

User apps go in `applications_user/`.

### Application Manifests

Each app has an `application.fam` manifest defining:
- **appid**: Unique identifier
- **apptype**: SERVICE, APP, PLUGIN, EXTERNAL, etc.
- **entry_point**: C function to run
- **stack_size**: Stack allocation in bytes
- **requires/conflicts**: Dependencies
- **resources**: SD card resources folder

### External Apps (FAPs)

FAPs (Flipper App Packages) are `.fap` files built separately from firmware. Set `apptype=FlipperAppType.EXTERNAL` in `application.fam`.

FAP-specific manifest fields:
- **fap_version**: App version
- **fap_icon**: 10x10 1-bit PNG icon
- **fap_category**: Subdirectory in apps folder
- **fap_icon_assets**: Folder for image assets
- **fap_private_libs**: Private libraries to build with app

FAPs use API version from `api_symbols.csv` for firmware compatibility.

## Coding Standards

### C Style (enforced by clang-format)

- 4 spaces for indentation (no tabs)
- **PascalCase** for types: `FuriHalUsb`, `SubGhzKeystore`
- **snake_case** for functions: `furi_hal_usb_init`, `subghz_keystore_read`
- File/package name prefixes content: `subghz_keystore.h` contains `SubGhzKeystore` type
- `_alloc` suffix: Allocate and init (constructor)
- `_free` suffix: Deinit and release (destructor)

### File Naming

- Directories: `[0-9A-Za-z_]+`
- Files: `[0-9A-Za-z_]+\.[a-z]+`
- Extensions: `.h`, `.c`, `.cpp`, `.cxx`, `.hpp`

### Code Formatting

Always run before commit:
```bash
./fbt format      # Format C/C++
./fbt format_py   # Format Python
./fbt format_all  # Format everything
```

### Contributing

- Read `CONTRIBUTING.md` and `CODING_STYLE.md`
- Ensure PR is compatible with firmware LICENSE (GPLv3)
- All code must pass CI/CD
- PRs must be reviewed by code owner
- Include `Co-Authored-By: Warp <agent@warp.dev>` in commit messages

## Build Configuration

Edit `fbt_options.py` or create `fbt_options_local.py` for persistent overrides.

Key options:
- `TARGET_HW`: Hardware target (7 for Flipper Zero)
- `COMPACT`: Optimize for size (0/1)
- `DEBUG`: Optimize for debugging (0/1)
- `FIRMWARE_APP_SET`: Which apps to include ("default" or "unit_tests")
- `COPRO_STACK_TYPE`: Bluetooth stack type ("ble_light" or "ble_full")

## Debugging

### With Hardware Debugger

Supported probes:
- Flipper Zero Wi-Fi Dev Board (Blackmagic)
- ST-Link (v2, v3)
- CMSIS-DAP compatible
- J-Link

```bash
./fbt flash debug  # Flash and attach GDB
./fbt blackmagic   # Debug with Blackmagic probe
./fbt openocd      # Start OpenOCD only
```

### FAP Debugging

1. Build firmware: `./fbt`
2. Flash: `./fbt flash`
3. Build FAP and run on Flipper
4. Attach debugger: `./fbt debug`

Debugger script `debug/flipperapps.py` provides FAP debugging support. Ensure `build/latest` points to correct debug/release build.

## Key Libraries

Located in `lib/`:
- `toolbox/` - Utility functions, data structures
- `subghz/` - SubGHz protocol implementations
- `infrared/` - Infrared protocol implementations
- `nfc/` - NFC protocol stack
- `lfrfid/` - Low-frequency RFID
- `flipper_format/` - FlipperFormat (.fff) file parsing
- `u8g2/` - Display graphics library
- `mbedtls/` - Cryptography

## Environment Variables

- `FBT_NOENV=1` - Skip toolchain setup, use system PATH
- `FBT_TOOLCHAIN_PATH` - Custom toolchain location
- `FBT_VERBOSE=1` - Enable verbose output
- `FBT_NO_SYNC=1` - Skip git submodule sync

## Resources

- Developer docs: https://developer.flipper.net/flipperzero/doxygen
- User docs: https://docs.flipper.net
- Discord: https://flipp.dev/discord
- Forum: https://forum.flipperzero.one/
