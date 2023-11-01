# Genie GIRUD-1T

These keys were captured from a Genie Intellicode remote from the [GIRUD-1T kit](https://www.geniecompany.com/garagedooropeneraccessories/GIRUD-1T).

This was decoded using the KeeLoq.c protocol decoder with a TE_short set to 200, a TE_long set to 400, and a TE_delta set to 70.  The capture was on the 315MHz channel using AM650.  After 65536 signals, the key repeated from the beginning.

Each entry in the TXT file is something like the following ``97A38C95007F1991`` which is the 64-bit key for the remote.  


If you have a custom firmware with TE_short set to 200, TE_long to 400, and TE_delta to 70 in [keeloq.c](https://github.com/flipperdevices/flipperzero-firmware/blob/c924693a84abe88a6c53e1e3b062f0a9ab1c5886/lib/subghz/protocols/keeloq.c#L16) you could use the following genie.sub file to transmit the key.

```c
Filetype: Flipper SubGhz Key File
Version: 1
Frequency: 315000000
Preset: FuriHalSubGhzPresetOok650Async
Protocol: KeeLoq
Bit: 64
Key: 97 A3 8C 95 00 7F 19 91
```

An easier way is to run the ``python genie.py`` file to generate a genie.sub file that uses the RAW protocol to transmit!  It produces files that work on any Flipper Zero firmware.  Edit the ``numbers = [0x97A38C95007F1991]`` in the script to be the value of the key you wish to use.

The script has been run to create the .SUB files in this folder.  In some cases two numbers were entered, so the .SUB file contains both sequences.  For example, you could pair with 0001 and 0002 files.  Then you can play the genie-0003.sub file to unlock the door.  The next code is typically 0xf or less.  If you play a future code, then the gap is reduced to 0x3 codes.  If you play invalid codes (from the past/beyond the future) the future code marker is left at its last position.  For example, if you were at code 0x0010 and played 0x200, then 0x0011-0x0013 and 0x201-0x203 will unlock the door.  If you then played 0x4200, then 0x0011-0x0013 and 0x201-0x203 will still unlock the door.  If you then played 0x001f, then 0x0011-0x0013 and 0x0020-0x0022 will unlock the door.  If you then played 0x0201, then 0x0011-0x0013 and 0x202-0x204 will unlock the door.  If you then played 0x200, then 0x0011-0x0013 and 0x201-0x203 will unlock the door.  It is not advised (since someone else with a Flipper Zero could do a Read RAW) but you can play the genie-0200_0201.sub, genie-4200_4201.sub, genie-8200_8201.sub & genie-c200_c201.sub to loop around and allow genie-0001.sub to become in the valid range again.