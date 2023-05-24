
# How to Build by yourself

## Install required software

- Git - [Download](https://git-scm.com/downloads) for Windows, on Linux/Mac install via package manager (`brew`, `apt`, ...)

For development:

- Git
- VSCode

## Clone the Repository

You should clone with

```shell
git clone --recursive https://github.com/RogueMaster/flipperzero-firmware-wPlugins.git
```
## VSCode integration

`fbt` includes basic development environment configuration for VS Code. Run `./fbt vscode_dist` to deploy it. That will copy the initial environment configuration to the `.vscode` folder. After that, you can use that configuration by starting VS Code and choosing the firmware root folder in the "File > Open Folder" menu.

## Building

Check out [documentation/fbt.md](fbt.md) for details on building and flashing firmware.

```sh
./fbt COMPACT=1 DEBUG=0 updater_package
```

Check `dist/` for build outputs.

Use **`flipper-z-{target}-update-{suffix}.tgz`** to flash your device.


# Build on Windows

Check out `documentation/fbt.md` for details on building and flashing firmware. 

### Compile everything for development

```sh
./fbt.cmd FIRMWARE_APP_SET=debug_pack updater_package
```

Production standard Options

```sh
./fbt.cmd COMPACT=1 DEBUG=0 updater_package
```

**You may need to change** `/` **to** `\` **in front of fbt command (Only for Windows)!**

Check `dist/` for build outputs.

Use **`flipper-z-{target}-update-{suffix}.tgz`** to flash your device.

If compilation fails, make sure all submodules are all initialized. Either clone with `--recursive` or use `git submodule update --init --recursive`.
