# Flipper Mfkey32

![](https://thumb.tildacdn.com/tild3332-3839-4061-b663-363464303432/-/resize/214x/-/format/webp/noroot.png)

## What
This Flipper application ("FAP") cracks Mifare Classic 1K keys on your Flipper Zero. No companion app/desktop needed.

## How
1. Use the Detect Reader function to save nonces on your Flipper from the reader
2. Use the Mfkey32 app to crack the keys
3. Scan the Mifare Classic card

All cracked nonces are automatically added to your user dictionary, allowing you to clone Mifare Classic 1K cards upon re-scanning them.

## Builds
Published: https://github.com/noproto/FlipperMfkey/releases

Manual: :information_source: Refer to the build flags necessary in mfkey32.c

## Why
This was the only function of the Flipper Zero that was [thought to be impossible on the hardware](https://old.reddit.com/r/flipperzero/comments/is31re/comment/g72077x/). You can still use other methods if you prefer them.

## Misc Stats
1. RAM used: **135 KB**, RAM free: 5 KB (original was ~53,000 KB, 99.75% RAM usage eliminated)
2. Disk used: (None)
3. Time per unsolved key:

| Category | Time |
| -------- | ---- |
| Best (real world) | N/A |
| Average | 255 seconds |
| Worst possible (expected) | 510 seconds |

Writeup: Coming soon

## Developers
noproto, AG
