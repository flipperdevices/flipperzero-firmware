# Bad USB `.kl` Format

The `.kl` files tell Bad USB what keys to press to make the correct characters
appear on screen. There are two formats, version 1 and version 2.

## Version 1

Version 1 has no file header. The entire format is a table of 127 16-bit
[Keys](#key-format). The Keys are layed out so that a 7-bit ASCII character
code can be used to index the table and find the relevant Key. To indicate that
a character is not mapped, two zero bytes (`00 00`) may be used. This format is
in little-endian. 

## Version 2

Version 2 attempts to lift the limitations of version 1, by allowing non-ASCII
characters to be defined, and allowing to define that a character should be
entered by a sequence of keys, rather than being limited to just a single key.

For example, to enter the codepoint U+00E9 “é” LATIN SMALL LETTER E WITH ACUTE
on the US-International keyboard layout, one first needs to first the
apostrophe key (here key 51), followed by the “E” key (here key 8). This file
format allows such characters to be defined.

The layout of the version 2 file format is as follows:

    +========+==================+=================+
    | Header | Codepoint Ranges | Sequences Table |
    +========+====================================+

All data is stored in little-endian.

### Header

The Header starts with the magic byte sequence FF 4B 4C, followed by the
version number, with is 02. The FF is to disambiguate it with the first file
format, as that is not a valid Usage ID on the USB HID Keyboard/Keypad Page.
After that, the amount of Codepoint Ranges are specified in an unsigned 16-bit
integer, and the length of the Sequences Tables is specified in an unsigned
16-bit integer.

    +------------+------------+------------+------------+
    | Header Magic Bytes: FF 4B 4C         | Version: 2 |
    +------------+------------+------------+------------+
    | # of Codepoint Ranges   | Sequences Table length  |
    +------------+------------+------------+------------+

### Codepoint Ranges

The Codepoint Ranges section of any number of Codepoint Range definitions. The
amount of Codepoint Ranges is specified in the header.

A Codepoint Range starts with the Range Specifier, a 32-bit unsigned integer
that specifies which unicode codepoints it handles. The Codepoint Range handles
the codepoints starting at Range Specifier and ending at Range Specifier +
0x100, exclusive. The least significant byte of Range Specifier must always be
00.

It is then followed by 256 Key Offsets, which are 16-bit unsigned integers that
indicate at which offset in the Sequences Table the Key Sequence can be found.

    +------+------+------+------+------+------+------+------+     +------+------+
    | Range Specifier           | Key Offset  | Key Offset  | ... | Key Offset  |
    +------+------+------+------+------+------+------+------+     +------+------+

To indicate that a codepoint is unmapped, it is recommended is to store a
zero-length Key Sequence at offset zero of the Sequences Table, and point to
that.

### Sequences Table

The Sequences Table consist of any number of Key Sequences. A Key Sequence is
an array of Keys, as described by [Key Format](#key-format), terminated by a
16-bit null.

    +---+---+---+---+     +---+---+---+---+---+---+---+---+     +---+---+---+---+
    | Key   | Key   | ... | Key   | 00 00 | Key   | Key   | ... | Key   | 00 00 | ...
    +---+---+---+---+     +---+---+---+---+---+---+---+---+     +---+---+---+---+

## Key Format

    +----------------+----------------+
    | Usage ID       | Modifier Flags |
    +----------------+----------------+

A key is two bytes wide, where the low byte is the Usage ID as found in the
[HID USB Keyboard/Keypad Page][HID Usage Tables]. In the high byte, the
modifier keys that need to be pressed for that key to be typed are found:

```c
enum HidKeyboardMods {
    KEY_MOD_LEFT_CTRL = (1 << 8),
    KEY_MOD_LEFT_SHIFT = (1 << 9),
    KEY_MOD_LEFT_ALT = (1 << 10),
    KEY_MOD_LEFT_GUI = (1 << 11),
    KEY_MOD_RIGHT_CTRL = (1 << 12),
    KEY_MOD_RIGHT_SHIFT = (1 << 13),
    KEY_MOD_RIGHT_ALT = (1 << 14),
    KEY_MOD_RIGHT_GUI = (1 << 15),
};
```

[HID Usage Tables]: https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
