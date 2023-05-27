# Metronome

[![FAP Build](https://github.com/ezod/flipperzero-metronome/actions/workflows/build.yml/badge.svg)](https://github.com/ezod/flipperzero-metronome/actions/workflows/build.yml)

A metronome for the Flipper Zero.

![ui](ui.png)

## Installation

1. Navigate to the [FAP Build](https://github.com/ezod/flipperzero-metronome/actions/workflows/build.yml)
   GitHub action workflow, and select the most recent run.
2. The FAP is built for both the `dev` and `release` channels of the official
   firmware. Download the artifact corresponding to your firmware version.
3. Extract `metronome.fap` from the ZIP file to `apps/Media` on your Flipper
   Zero SD card.

## Features

- BPM adjustable, fine and coarse (hold pressed)
- Selectable amount of beats per bar
- Selectable note length
- First beat is pronounced
- Progress indicator
- LED flashes accordingly
- 3 different settings: Beep, Vibrate, Silent (push Down to change)
