# Flipperzero Race Game

Race game inspired by Race game in BrickGame 9999 in 1.


Source is heavily base on tetris game from [jeffplang's repo](https://github.com/jeffplang/flipperzero-firmware/blob/tetris_game/applications/tetris_game/tetris_game.c) and [this article](https://yakovlev.me/hello-flipper-zero/).

## Build instructions

First clone the repos

```bash
git clone https://github.com/flipperdevices/flipperzero-firmware.git
cd flipperzero-firmware
git clone https://github.com/zyuhel/flipperzero-racegame.git ./applications_user/flipperzero-racegame

```

Build the firmware
```bash
./fbt
```

Build application
```bash
./fbt fap_racegame
```

Now it will be available in *build/f7-firmware-D/.extapps* folder in file racegame.fap. Copy it to flipper and enjoy.

