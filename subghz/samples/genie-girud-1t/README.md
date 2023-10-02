# Genie GIRUD-1T

These keys were captured from a Genie Intellicode remote from the [GIRUD-1T kit](https://www.geniecompany.com/garagedooropeneraccessories/GIRUD-1T).

This was decoded using the KeeLoq.c protocol decoder with a TE_short set to 200, a TE_long set to 400, and a TE_delta set to 70.  The capture was on the 315MHz channel using AM650.  After 65536 signals, the key repeated from the beginning.

Each entry in the TXT file is something like the following ``97A38C95007F1991`` which is the 64-bit key for the remote.  You would need to make a file similar to the following to transmit the key. 

**Be sure your TE_short is set to 200, TE_long to 400, and TE_delta to 70 in [keeloq.c](https://github.com/flipperdevices/flipperzero-firmware/blob/c924693a84abe88a6c53e1e3b062f0a9ab1c5886/lib/subghz/protocols/keeloq.c#L16) so the signal is sent correctly, otherwise it will not work!!!**

```c
Filetype: Flipper SubGhz Key File
Version: 1
Frequency: 315000000
Preset: FuriHalSubGhzPresetOok650Async
Protocol: KeeLoq
Bit: 64
Key: 97 A3 8C 95 00 7F 19 91
```

You can run the genie.py file to generate a genie.sub file that uses the RAW protocol to transmit, so it works without modified firmware.  
Edit the ``info = 0x0D94C5EC007F1991`` to be the value of the key you wish to use.