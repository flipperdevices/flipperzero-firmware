# What is done

## Peoples and management

1. We add many contributors within the **phase-1**. Also we start flash mob with "Welcome" issues and I like it! You can see [all welcome issues here](https://github.com/Flipper-Zero/flipperzero-firmware-community/issues?q=is%3Aissue+label%3Awelcome+) and get to know each other.
2. Make [discord server](https://flipperzero.one/discord)! Now you can chat and communicate by voice.
3. Create [developer blog](https://github.com/Flipper-Zero/flipperzero-firmware-community/wiki/Developer-blog), now you can read project updates in digest format.

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

1. We designed and manufactured  [F2B0C1.1](https://github.com/Flipper-Zero/flipperzero-firmware-community/wiki/Hardware-version-F2B0C1.1)!

![F2B0C1.1](F2B0C1.1.jpg)

# What are we doing right now

1. Make UI and display driver #(), Реализация дисплея и эмулятор интерфейса
2. Продолжаем пилить фури и загрузчик
3. Обсуждения ядра не особо шевелятся, есть зефир, мб кто-нибудь предложит что-нибудь
4. Пилим под блютузную плату
5. Делаем тесты
6. Делаем стенд удаленной отладки
7. Запукаем новые платы
