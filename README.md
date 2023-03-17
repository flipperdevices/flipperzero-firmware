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

Manual: Copy the fap/ directory to applications_user/mfkey32/ and build it with fbt

## Why
This was the only function of the Flipper Zero that was [thought to be impossible on the hardware](https://old.reddit.com/r/flipperzero/comments/is31re/comment/g72077x/). You can still use other methods if you prefer them.

## Misc Stats
1. RAM used: **135 KB**, RAM free: 5 KB (original was ~53,000 KB, 99.75% RAM usage eliminated)
2. Disk used: (None)
3. Time per unsolved key:

| Category | Time |
| -------- | ---- |
| Best (real world) | 31 seconds |
| Average | 5.1-8.85 min |
| Worst possible (expected) | 10.2-14.14 min |

NB: Keys that are already in the system/user dictionary or nonces with already found keys are cracked instantly. This means on average cracking an arbitrary number of nonces from the same reader will take 5.1 to 8.85 minutes (1 unknown key).

Writeup: Coming soon

## Developers
noproto, AG
