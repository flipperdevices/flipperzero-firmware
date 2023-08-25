# Flipper Zero Forth

This is a plugin for the Flipper Zero that ports
[pForth](https://github.com/philburk/pforth).

Once started the plugin will add a `forth` command to the CLI that can be used
to call up a basic forth shell.

Calling the `forth` command with a file path as parameter will cause that file
to be executed as a forth script. Examples are provided under `forth_scripts`.

To use one of the provided scripts (for example `notif_success.fth`), copy it
to your SD card. Then run the plugin and execute `forth /ext/<path to your
file>` (for example `forth /ext/notif_success.fth`) on the CLI.

## Warning

This plugin is in the early stages of development. It will inevitably have
bugs. You have been warned.

## Dictionary

The dictionary for pForth has been pre-compiled and the pForth code has been
modified to be able to load that pre-compiled dictionary without copying it
around in memory.

To recreate the dictionary you will need a pForth, that has been compiled as a
32-bit binary. To recreate the dictionary, follow these instructions:

- Download the pForth source code.
- Go into the `platforms/unix/` directory.
- Set the `WIDTHOPT` parameter in the `Makefile` to `-m32`.
- Compile pForth using `make`.
- Modify the `loadp4th.fth` file in `fth/` to your liking.
- Remove the `pforth.dic` and `pfdicdat.h` files if they are present.
- Compile the dictionary by running `make pfdicdat.h`.
- Modify the `pfdicdat.h` file such that it is compatible with the inplace
  dictionary loading. This includes removing the `const` modifier from
  `MinDicNames` and `MinDicCode` and setting a liberal size for `MinDicNames`
  and `MinDicCode`. A size of 20000 + 16 for the former, and 40000 + 16 for the
  latter should suffice.
- Copy the modifed `pfdicdat.h` into `lib/pforth/` in the plugin's source code
  directory.

## Acknowledgements

The code in `lib/pforth/` has been taken from Phil Burk's pForth, which can be
found here: https://github.com/philburk/pforth.
