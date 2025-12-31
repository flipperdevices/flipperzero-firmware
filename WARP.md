# WARP.md

This file provides guidance to WARP (warp.dev) when working with code in this repository.

## Key commands and workflows

All commands below are run from the repository root unless noted otherwise.

### Build and flash firmware

- **Build default firmware distribution**
  - `./fbt`
  - Equivalent to building the main firmware distribution (`fw_dist`) using the Flipper Build Tool (FBT).

- **Common FBT high‑level targets** (see `documentation/fbt.md` for full list):
  - `./fbt fw_dist` – build and publish firmware to `dist/`.
  - `./fbt fap_dist` – build external plugins and publish to `dist/`.
  - `./fbt updater_package` – build a full self‑update package.
  - `./fbt updater_minpackage` – build a minimal self‑update package (firmware DFU only).

- **Flash firmware using SWD (debug probe)**
  - `./fbt flash`
  - Automatically detects supported probes (Wi‑Fi devboard/Blackmagic, ST‑Link, etc.).

- **Flash firmware over USB**
  - `./fbt flash_usb` – build, upload and install the update package over USB.
  - `./fbt flash_usb_full` – same, but includes radio stack and resources.

- **CLI session to a connected device**
  - `./fbt cli`
  - Starts a Flipper CLI session over USB, used for interacting with on‑device apps such as `unit_tests`.

- **Assets and resources build** (from `assets/ReadMe.md`)
  - `./fbt icons proto dolphin_internal dolphin_blocking dolphin_ext resources`
  - Rebuilds icons, protobufs, dolphin assets, and resources into the build output.

### Linting and formatting

FBT exposes convenient targets for C/C++ and Python formatting; an additional `scripts/lint.py` script enforces naming and permissions conventions.

- **C/C++ formatting and linting**
  - `./fbt lint` – run `clang-format` in check mode over C/C++ sources.
  - `./fbt format` – apply `clang-format` in‑place according to `.clang-format`.

- **Python formatting and linting**
  - `./fbt lint_py` – run `black` in check mode over Python sources, build scripts and manifests.
  - `./fbt format_py` – apply `black` formatting.

- **Extended C/C++ style and naming checks** (from `scripts/lint.py`)
  - Check only:
    - `python3 scripts/lint.py check applications furi lib targets`
  - Auto‑fix (format, filenames, permissions):
    - `python3 scripts/lint.py format applications furi lib targets`

### Unit tests running on device

Unit tests are implemented as an on‑device application (`applications/debug/unit_tests`) and driven via the CLI. See `documentation/UnitTests.md` for full details.

1. **Build firmware with unit tests enabled**
   - `./fbt FIRMWARE_APP_SET=unit_tests updater_package`

2. **Flash the resulting firmware**
   - Use any of the usual methods, e.g.:
     - `./fbt flash`
     - or `./fbt flash_usb`

3. **Run tests from the CLI**
   - Start a CLI session: `./fbt cli`
   - Run the full suite:
     - `unit_tests`
   - Run a single test (or a subset) by name:
     - `unit_tests <test_name>`
   - Test names are defined in `applications/debug/unit_tests/test_index.c`.

4. **Automated run of the whole suite from host** (from `scripts/testops.py`)
   - Wait for a device to appear on a serial port:
     - `python3 scripts/testops.py await_flipper`
   - Run the `unit_tests` CLI command and parse the result:
     - `python3 scripts/testops.py run_units`

### Host‑side hardware E2E tests (flippigator)

`scripts/flippigator` contains pytest‑based end‑to‑end tests that exercise a connected Flipper Zero device.

From the repo root:

- `cd scripts/flippigator`
- Run all tests:
  - `pytest`
- Target a specific serial/USB port:
  - `pytest --p <PORT>`
- Run tests for a specific module:
  - `pytest --m nfc`
- Run tests matching a pattern:
  - `pytest -k TestNfc`

### VS Code workspace setup (optional)

If you use VS Code locally, the repository includes preconfigured tasks and debug settings.

- Generate VS Code configuration (once):
  - `./fbt vscode_dist`
- Then open the repo root in VS Code and use the configured build and debug tasks.

## High‑level architecture

This repository is the official Flipper Zero firmware. The core building blocks are:

- A custom OS layer called **Furi Core** (`furi/`) providing OS‑level primitives and helpers.
- An RTOS and hardware abstraction layer under **targets** (RTOS, FuriHAL, startup and board‑specific code).
- **Applications and services** (`applications/`) that provide user‑facing functionality and background system services.
- **Libraries** (`lib/`) that implement reusable protocol stacks, filesystem drivers, graphics, cryptography and other shared functionality.
- **Assets** (`assets/`) that define icons, animations, Dolphin game assets, protobuf schemas and other data compiled into firmware resources.
- **Build system and tooling** (`./fbt`, `SConstruct`, `site_scons/`, `scripts/`) that orchestrate builds, flashing, tests and auxiliary workflows.

The top‑level `ReadMe.md` summarizes the main directories:

- `applications` – Applications and services used in firmware.
- `assets` – Assets consumed by applications and services.
- `furi` – Furi Core: OS‑level primitives and helpers.
- `documentation` – FBT and firmware documentation.
- `firmware` – Firmware SCons targets and related build glue.
- `lib` – Internal and third‑party libraries, drivers and utilities.
- `scripts` – Supplementary host‑side scripts and Python utilities.
- `targets` – Firmware for specific hardware targets (e.g. F7) and associated HAL/Furi integration.

Below is how these pieces fit together in typical workflows.

### Applications and services (`applications/`)

`applications/` contains all firmware applications grouped by responsibility (see `applications/ReadMe.md`):

- `main/` – User‑visible applications on the main Flipper menu (e.g. NFC, SubGhz, archive, GPIO). These are thin UI/front‑end layers that:
  - call into **services** for system APIs (storage, desktop, GUI, input, power, etc.), and
  - use protocol/domain libraries from `lib/` (e.g. `lib/nfc`, `lib/infrared`, `lib/subghz`).

- `services/` – Long‑running background services and system APIs, including:
  - communication and control surfaces (CLI, RPC, BLE, desktop service),
  - core system subsystems (GUI, input, loader, notification, power, storage).
  Services expose C APIs consumed both by apps in `applications/main/` and by other services.

- `settings/` – Small configuration UIs for system components (Bluetooth settings, desktop settings, power and storage settings, etc.). These are thin wrappers over the underlying services.

- `debug/` – Factory and developer tools that run on the device (battery tests, display tests, USB tests, `unit_tests`, etc.). These apps are useful for hardware bring‑up and diagnostics during development.

- `system/` – Utility apps not exposed directly in user menus (e.g. updater, storage migration tools, MFKey helper).

There is also an `applications_user/` tree intended for user or external applications that are not part of the main firmware tree, built via the same FBT/UFBT mechanisms.

When modifying or adding features:

- **UI and workflow changes** normally live in `applications/main/` (or in `settings/` if it’s a configuration UI).
- **New system‑wide capabilities or device services** should usually be implemented in `applications/services/` and then consumed by apps.
- **Low‑level protocol or hardware‑adjacent logic** should live in `lib/` (see below) and be wrapped by services/apps rather than used directly from UI code.

### Furi Core and targets (`furi/`, `targets/`, `firmware/targets/`)

- `furi/` implements Furi Core, the OS abstraction layer used throughout the codebase. It provides primitives such as threads/tasks, synchronization, logging, and other helpers that sit above FreeRTOS and hardware drivers.

- `targets/` defines concrete firmware targets and boot‑time configuration. For example, the `f7` target provides:
  - RTOS configuration and startup code,
  - FuriHAL integration for the STM32WB‑based Flipper Zero hardware,
  - wiring for services and applications to run on the device.

- `firmware/targets/` (nested under `firmware/`) contains SCons build descriptions for firmware targets, mapping high‑level FBT targets (e.g. `fw_dist`, `updater_package`) to specific board configurations under `targets/`.

As a rule of thumb, changes that:

- affect **board bring‑up, clocks, low‑level peripherals or boot flow** live under `targets/` and associated HAL code;
- affect **OS primitives or global infrastructure** live in `furi/`;
- affect **feature behavior** should be expressed in terms of Furi APIs and live under `applications/` and `lib/`.

### Libraries and shared code (`lib/`)

`lib/` holds both third‑party and Flipper‑specific libraries. `lib/ReadMe.md` documents the most important ones; key categories are:

- **RTOS and OS support** – `FreeRTOS-Kernel`, `FreeRTOS-glue`.
- **Protocol stacks and device‑level logic** – `infrared`, `nfc`, `subghz`, `lfrfid`, `ibutton`, etc.
- **Storage and filesystems** – `fatfs`, `littlefs`, `flipper_format`, `microtar`.
- **Graphics and UI support** – `u8g2`, helpers used by GUI and desktop services.
- **Cryptography and security** – `mbedtls` and related utilities.
- **Common utilities** – `toolbox`, `print`, `update_util`, C container library `mlib`, etc.

Typical flow:

- Libraries implement protocol‑ or system‑level operations in a hardware‑agnostic way (or in a way that assumes only HAL/Furi APIs).
- Services and applications in `applications/` wrap these libraries to provide user‑visible features.

When implementing new features, prefer to:

- place reusable protocol or data‑structure logic in a suitable subdirectory under `lib/`, and
- keep `applications/` code focused on orchestration, state machines, and UI.

### Assets and resources (`assets/`)

`assets/` provides the source assets that are compiled into firmware resources by FBT (see `assets/ReadMe.md`):

- **Dolphin assets** (`assets/dolphin/`) – graphics and data for the on‑device Dolphin "game"/mascot.
- **Icons and animations** (`assets/icons/`) – sprite sheets and bitmaps turned into C headers and compiled assets by FBT.
- **Protobuf schemas** (`assets/protobuf/`) – compiled into `.pb.c/.pb.h` via the `proto` FBT target.
- **Slideshows** (`assets/slideshow/`) – converted to `.slideshow` bundles via `scripts/slideshow.py`.

The main asset pipeline is invoked by the FBT command listed earlier. For delivering resources to a device without a full firmware update, there is a helper in `scripts/ReadMe.md`:

- `python3 scripts/storage.py -p <flipper_cli_port> send build/latest/resources /ext`

### Scripts and tooling (`scripts/`)

`scripts/` contains host‑side utilities that support development, flashing and testing (see `scripts/ReadMe.md`):

- **Flashing helpers** – scripts built on top of `STM32_Programmer_CLI` for OTP programming, core firmware flashing and option bytes management (`otp.py`, `ob.py`, etc.).
- **Resource delivery** – `storage.py` used to push resources from `build/latest/resources` onto device storage.
- **Slideshow tools** – `slideshow.py` for building `.slideshow` assets from PNG frames.
- **Linting** – `lint.py` described above.
- **Testing utilities** – `testops.py` for driving on‑device unit tests, `scripts/flippigator` for pytest‑based hardware E2E tests.

### Documentation (`documentation/`)

Key docs you may want to consult or reference programmatically:

- `documentation/fbt.md` – canonical reference for FBT commands, targets, configuration and environment.
- `documentation/UnitTests.md` – how unit tests are built, run and organized.
- `documentation/AppsOnSDCard.md` and `documentation/AppManifests.md` – architecture of external apps/FAPs and manifests.
- `documentation/KeyCombo.md` – hardware key combinations for boot modes and recovery.

### Additional project metadata

- `CODING_STYLE.md` – detailed C/C++ style and structural conventions used throughout the firmware.
- `CONTRIBUTING.md` – contribution workflow and expectations.
- `CODE_OF_CONDUCT.md` – community behavior guidelines.

Future Warp agents should consult these files when making non‑trivial structural or cross‑cutting changes to the firmware.
