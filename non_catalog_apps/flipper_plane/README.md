# Paper Plane

Port of [this game](https://ticalc.org/archives/files/fileinfo/341/34139.html) to the Flipper Zero.

## Gameplay

![plane flying down](./img/gameplay2.png)

Your goal is to fly a paper plane through obstacles and get to the bottom of the map.

Your score is dictated by how far down the level you get.

![plane flying left](./img/gameplay1.png)

## Building

This game can be built with the [ufbt](https://github.com/flipperdevices/flipperzero-ufbt) toolkit.

Install:

```bash
python3 -m pip install --upgrade ufbt
```

Build:

```bash
ufbt update # update ufbt to the latest version
ufbt # builds the application
```

Run:

```bash
ufbt launch # be sure your flipper is plugged in and detected
```

## Credits

This game is a port of [Paper Plane](https://ticalc.org/archives/files/fileinfo/341/34139.html) from the TI-83+/84+ series of graphing calculators, which itself is a port of a minigame from Wario Ware Inc. on the GBA.

The art and map are direct conversions from the calculator version.

Most of the Flipper Zero specific code comes from [this project](https://github.com/Rrycbarm/t-rex-runner), which was a huge help in figuring how things work.