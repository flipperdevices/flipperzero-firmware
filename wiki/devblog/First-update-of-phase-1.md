# What is done

## Peoples and management

1. We add many contributors within the **phase-1**. Also we start flash mob with "Welcome" issues and I like it! You can see [all welcome issues here](https://github.com/Flipper-Zero/flipperzero-firmware-community/issues?q=is%3Aissue+label%3Awelcome+) and get to know each other.
2. Make [discord server](https://flipperzero.one/discord)! Now you can chat and communicate by voice.
3. Create [developer blog](https://github.com/Flipper-Zero/flipperzero-firmware-community/wiki/Developer-blog), now you can read project updates in digest format.
4. Create [backlog](https://github.com/Flipper-Zero/flipperzero-firmware-community/issues?q=label%3Abacklog+) label and place some task that are not a priority at the moment.

## Environment

1. Add pipeline to automate wiki posting and remove this annoying routine [#63](https://github.com/Flipper-Zero/flipperzero-firmware-community/pull/63)
2. Started to add CI workflow:  [#70](https://github.com/Flipper-Zero/flipperzero-firmware-community/pull/70). Now GitHub pipelines check that `target_lo` and `target_f1` is successfully compiling.
3. Add Rust support into docker image: [#41](https://github.com/Flipper-Zero/flipperzero-firmware-community/pull/41) + [#68](https://github.com/Flipper-Zero/flipperzero-firmware-community/pull/68). Now you can build Rust code, link it with C and together, use Bindgen and Cbindgen.

## Core and stuff

1. Completely add `target_f1`, now you can build your code for [F1](https://github.com/Flipper-Zero/flipperzero-firmware-community/wiki/Hardware-version-F1B1C0.0) board.
2. Add implementation of [FURI](https://github.com/Flipper-Zero/flipperzero-firmware-community/wiki/FURI) (with many issues -- see [#59](https://github.com/Flipper-Zero/flipperzero-firmware-community/issues/59))...
3. ...and add many examples how to use FURI, HAL and do some funny things ([example page](https://github.com/Flipper-Zero/flipperzero-firmware-community/wiki/Application-examples)):
	1. [LED Blink](https://github.com/Flipper-Zero/flipperzero-firmware-community/wiki/Blink-app)
	2. [Writing to UART](https://github.com/Flipper-Zero/flipperzero-firmware-community/wiki/UART-write)
	3. [Communication between apps](https://github.com/Flipper-Zero/flipperzero-firmware-community/wiki/IPC-example)

## Hardware

1. We designed and manufactured [F2B0C1.1](https://github.com/Flipper-Zero/flipperzero-firmware-community/wiki/Hardware-version-F2B0C1.1)!

![F2B0C1.1 TODO: add photo](F2B0C1.1.jpg)

# What are we doing right now

1. Make UI and display driver [#98](https://github.com/Flipper-Zero/flipperzero-firmware-community/issues/98), and implement dummy display and UI emulator [#97](https://github.com/Flipper-Zero/flipperzero-firmware-community/issues/97). After do that evryone can develop UI features! 
2. We are continue designing FURI API and implementations [#59](https://github.com/Flipper-Zero/flipperzero-firmware-community/issues/59). If you have proposals or remarks about this component, or you don't understand what we do -- read [FURI and FURI AC description](https://github.com/Flipper-Zero/flipperzero-firmware-community/wiki/FURI) look at the [examples](https://github.com/Flipper-Zero/flipperzero-firmware-community/wiki/Application-examples) and feel free to comment in this issue or discuss it in [discord](https://flipperzero.one/discord).
3. We start to make big work of dynamic loading and linking applications. Flipper is different from many embedded system because we want to run user applications, load it by USB, Bluetooth, SD-card and other, and we need to implement it on small limited system without MMU. You can see progress and discuss on [#73](https://github.com/Flipper-Zero/flipperzero-firmware-community/issues/73)
4. We get interesting proposal about Zephyr OS [comment in #17](https://github.com/Flipper-Zero/flipperzero-firmware-community/issues/17#issuecomment-683929900) and port it on our new WB55 board [№89](https://github.com/Flipper-Zero/flipperzero-firmware-community/issues/89).
5. We make design of new flipper PCB with STM32WB55RB and new PMIC (we're using AXP173).
5. Creating unit test environment and pipelines for CI [#40](https://github.com/Flipper-Zero/flipperzero-firmware-community/issues/40). if you want to see how building and testing is working right now, see [Environment](https://github.com/Flipper-Zero/flipperzero-firmware-community/wiki/Environment) page.
6. Soon we have remote test and debug bench! I think i's very funny idea and useful for developers who haven't real harwdare and for running CI on hardware: [#96](https://github.com/Flipper-Zero/flipperzero-firmware-community/issues/96)
7. On next week we start to blow off magic smoke and breathe life in new F2B0C1.1 boards, stay tuned!

# We need help

1. Task about linting and control codestyle [#12](https://github.com/Flipper-Zero/flipperzero-firmware-community/issues/12) is stuck
2. Also we had massive discussion about integration with IDE. If you feel pain with our current developer environment and want to use your favorite IDE, welcome to [#18](https://github.com/Flipper-Zero/flipperzero-firmware-community/issues/18)!
3. What about my idea of wiki widgets to attach issues to wiki page? [#66](https://github.com/Flipper-Zero/flipperzero-firmware-community/issues/66)

# Our plans
