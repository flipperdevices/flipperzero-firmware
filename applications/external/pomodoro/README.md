# Flipp Pomodoro
![Banner Image](docs/headline.gif)

>Boost Your Productivity with the Pomodoro Timer for Flipper Zero! Don't let your flipper get bored, let him help you instead.

---
* [Install](#install)
* [Guide](#guide)
* [Contributing](#contributing)
* [Development](#development)
    + [Track](#track)
    + [Build and Package](#build-and-package)

## Install
[![Total Downloads](https://img.shields.io/github/downloads/Th3Un1q3/flipp_pomodoro/total?style=for-the-badge)](https://github.com/Th3Un1q3/flipp_pomodoro/releases/latest)

Download from [releases](https://github.com/Th3Un1q3/flipp_pomodoro/releases/latest) and follow instructions there.

[![Latest Release Date](https://img.shields.io/github/release-date/Th3Un1q3/flipp_pomodoro?label=Latest%20release%20&style=for-the-badge) ![Latest Release](https://img.shields.io/github/v/release/Th3Un1q3/flipp_pomodoro?label=Latest%20version&style=for-the-badge) ![GitHub release (latest by date)](https://img.shields.io/github/downloads/Th3Un1q3/flipp_pomodoro/latest/total?label=Latest%20downloaded&style=for-the-badge)](https://github.com/Th3Un1q3/flipp_pomodoro/releases/latest)

## Guide

[About Pomodoro Technique](https://francescocirillo.com/products/the-pomodoro-technique)

### `Focus` Phase

![Working Screen](docs/working.png)

Do the following:
* Check your past notes
* Choose a task and note it down
* Work on the task until the Flipp Pomodoro rings
* If task is done during `Focus` phase
    * Review completed work
    * Plan a task to focus on next
    * Reflect what have you have learned
* If task is not complete by the end of the `Focus` phase
    * Place a bookmark and return to it during the next cycle
    * Think how to define task the way it would fit into a single phase
* If there is some distraction(remember: many things can wait 25 minutes)
    * Incomming message - take a note and answer during `Rest` phase
    * Genius idea - note a hint down, plan as a task or return to it when work is done
    * Everything unavoidable - feel free to leave the cycle, once it's mitigated you have task to work on

> Hint: By completing `Focus` phase your flipper gains good mood boost

### `Rest` Phase
![Resting Screen](docs/resting.png)

Do the following:
* Take a walk around or do a little stretch
* Take some fresh air
* Refill your drink
* Answer pending messages
* Talk to a colleague

## Contributing

[![GitHub Discussions](https://img.shields.io/github/discussions/Th3Un1q3/flipp_pomodoro?style=for-the-badge)](https://github.com/Th3Un1q3/flipp_pomodoro/discussions)

I welcome contributions to this project! If you're interested in helping out, here are a few ways to get started:
- Join [discussions](https://github.com/Th3Un1q3/flipp_pomodoro/discussions) to ask questions and share ideas with other contributors.
- If you've found a bug or have an idea for a new feature, please open an issue on [issue tracker](https://github.com/Th3Un1q3/flipp_pomodoro/issues). Before opening a new issue, please search the existing issues to see if someone has already reported the problem.
- If you're ready to start contributing code, please [fork](https://github.com/Th3Un1q3/flipp_pomodoro/fork) this GitHub repository and submit a [pull request](https://github.com/Th3Un1q3/flipp_pomodoro/pulls).

## Development

![GitHub commit activity (branch)](https://img.shields.io/github/commit-activity/m/Th3Un1q3/flipp_pomodoro?style=for-the-badge)

### Track
You can follow project development on the [project board](https://github.com/users/Th3Un1q3/projects/1).


###  Build and Package
Build application
```shell
# For standard(official) firmware
bash tools/build.sh

# For unleashed firmware
bash tools/build.sh -f unleashed 

# While flipper connected via USB and serial port is not bussy
# Build, run on flipper and keep the app in `Productivity` directory
bash tools/build.sh -f unleashed -i
```
