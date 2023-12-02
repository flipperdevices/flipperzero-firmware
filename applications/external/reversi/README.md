# Reversi game for Flipper Zero

![Game screen](images/game.png)
![Menu screen](images/menu.png)
![You win](images/win.png)

## Compile
1. Clone [firmware](https://github.com/flipperdevices/flipperzero-firmware)
2. Go to the `applications_user` directory
3. Create a symlink to this repo assuming that these two repos are on the same level:
```sh
ln -s ../../flipperzero-reversi flipperzero-reversi
```
4. From the main directory of the firmware run:
```sh
./fbt faps
```

## Install
1. Open folder `./build/f7-firmware-D/.extapps/` from the firmware directory
2. Open qFlipper
3. Open File Manager, SD Card/apps/games
4. Drag `game_reversi.fapp` to qFlipper

## Controls
The game controls should be intuitive.
Longs press on OK opens the menu to start a new game.

## Thanks to:
- [2048 game](https://github.com/eugene-kirzhanov/flipper-zero-2048-game)

## License
[MIT](LICENSE)
Copyright 2023 Dmitry Matyukhin
