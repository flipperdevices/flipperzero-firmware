# BPM Tapper

[![FAP Build](https://github.com/ezod/flipperzero-bpm-tapper/actions/workflows/build.yml/badge.svg)](https://github.com/ezod/flipperzero-bpm-tapper/actions/workflows/build.yml)

A BPM Tapper for the Flipper Zero.

![ui](ui.png)

## Installation

1. Navigate to the [FAP Build](https://github.com/ezod/flipperzero-bpm-tapper/actions/workflows/build.yml)
   GitHub action workflow, and select the most recent run.
2. The FAP is built for both the `dev` and `release` channels of the official
   firmware. Download the artifact corresponding to your firmware version.
3. Extract `bpm_tapper.fap` from the ZIP file to `apps/Media` on your Flipper
   Zero SD card.

## Usage

Hit any button other than back repeatedly. Calculates based on the average of the last 8 inputs.

## Compiling

```
./fbt firmware_bpm_tapper
```

