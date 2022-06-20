# Flipper Build Tool

FBT is the entry point for most firmware-related commands and utilities.
It is invoked by `./fbt` in firmware project root directory. Internally, it is a wrapper around [scons](https://scons.org/) build system.

## NB 
FBT constructs all referenced environments & their targets' dependency trees on startup. So, to keep startup time as low as possible, we're hiding construction of certain targets behind command-line options.

## Invoking FBT
To build with FBT, call it specifying configuration options & targets to build. For example,

`./fbt --with-updater COMPACT=1 DEBUG=0 VERBOSE=1 updater_package copro_dist`

To run cleanup (think of `make clean`) for specified targets, all `-c` option.

## FBT targets
FBT keeps track of internal dependencies, so you only need to build the highest-level target you need, and FBT will make sure everything it needs is up-to-date.

### High-level (what you most likely need)
- `fw_dist` - build & publish firmware to `dist` folder
- `updater_package` - build self-update package. _Requires `--with-updater` option_
- `copro_dist` - bundle Core2 FUS+stack binaries for qFlipper
- `debug` - build and flash firmware, then attach with gdb with firmware's .elf loaded
- `debug_updater` - attach gdb with updater's .elf loaded. _Requires `--with-updater` option_
- `debug_other` - attach gdb loading an externally-built .elf. Specify .elf to debug with `OTHER_ELF=path/to/other.elf`
- `openocd` - just start OpenOCD

### Firmware targets
- `firmware_extapps` - build all plug-ins as separate .elf files
    - `firmware_snake_game`, etc - build single plug-in as .elf by its name
    - Check out `--extra-ext-apps` for force adding extra apps to external build 
- `firmware_flash` - flash current version to attached device with OpenOCD
- `firmware_cdb` - generate compilation database
- `firmware_all`, `updater_all` - build basic set of binaries

### Assets
- `resources` - build resources and their Manifest
    - `dolphin_ext` - process dolphin animations for SD card 
- `icons` - generate .c+.h for icons from png assets
- `proto` - generate .pb.c+.pb.h for .proto sources
- `proto_ver` - generate .h with protobuf version 
- `dolphin_internal`, `dolphin_blocking` - generate .c+.h for corresponding dolphin assets
 

## Command-line parameters

- `--options optionfile.py` (default value `fbt_options.py`) - load file with multiple configuration values
- `--with-updater` - enables updater-related targets and dependency tracking. Enabling this options introduces extra startup time costs, so use it when bundling update packages. Or if you have a fast computer and don't care about a few extra seconds of startup time
- `--extra-ext-apps=app1,app2,appn` - forces listed apps to be built as external with  `firmware_extapps` target


## Configuration 
Default configuration variables are set in configuration file `fbt_options.py`. 
Values set on command-line have higher precedence over configuration file.

You can find out available options with `./fbt -h`.
