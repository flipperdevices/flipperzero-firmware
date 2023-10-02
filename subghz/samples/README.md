# Sub-GHz Samples
These are samples captured by various remotes I own.
- [Chevy HHR 2006](./chevy-hhr-2006/README.md) vehicle keyfob.
- [Quantum Fire](./quantum-fire/README.md) fireworks controller.
- [X10](./x10/README.md) x10.com controller.
- [Genie GIRUD-1T](./genie-girud-1t/README.md) garage door opener.

Check out my [YouTube videos](../../youtube/README.md#sub-ghz) for more information on how to use these files.


## Understanding the data
- The file format is [documented](https://flipperdevices.github.io/flipperzero-firmware/md_docs_file_formats__sub_ghz_file_formats.html) on the flipperdevices github account.

- There are two types of .sub files.
  - Files where the protocol is known.
  - Files where the protocol is unknown.

- For unknown protocols, the "Filetype:" will be "Flipper SubGhz RAW File".
  - You will see "RAW_Data:" lines.  The positive numbers are the length of tones (in uS, which is 1/1000 millisecond.)  The negative numbers are the length of the silences (in uS).
  - You can interpret and create RAW files to send a different button; see these [videos](../../youtube/README.md#sub-ghz--hacking-sub-ghz-devices-read-raw).

- For known protocols, the "Filetype:" will be the name of the protocol.  Often for known protocols the file has a line called "Key:" with the data needed for transmitting the code.
  - You can edit the "Key" data to send a different button; see these [videos](../../youtube/README.md#sub-ghz--hacking-sub-ghz-devices-read).
