# Flipper bootloader

What it does?

- [+] Hardware initialization
- [ ] Firmware CRC check
- [+] Firmware update
- [ ] Interactive UI
- [+] Boot process LED indicators
- [ ] FS check
- [ ] Recovery mode

# Platforms

| Name      | Bootloader    | Firmware      | Reset | DFU           |
|           | Address       | Address       | Combo | Combo         |
---------------------------------------------------------------------
| F2B0C1.1  | 0x08000000    | 0x00008000    | L+R   | L+R, hold R   |

Platform independend code and headers in `src`and `platforms/include` folders.

# Building

## With dev docker image:

`docker-compose exec dev make -C bootloader`

## With toolchain installed in path:

`make`

## Build Options

- `DEBUG` - 0/1 - enable or disable debug build. Default is 1.
- `PLATFORM` - string - platform to build. Default is `F2B0C1.1`.

# Flashing 

Using stlink(st-flash):

`make flash`

# Debug

Using stlink (st-util + gdb):

`make debug`
