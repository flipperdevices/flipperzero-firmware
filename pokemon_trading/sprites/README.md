# Sprite Assets

In this folder are the Pokemon sprite sources. They should all be 3 digits and numbered in the national Pokedex order.


## Building

The final sprites on the SD card are expected to be in the "fxbm" format as used by [https://github.com/flipperdevices/flipperzero-game-engine](https://github.com/flipperdevices/flipperzero-game-engine). This is XBM format with a short header with the size, width, and height.

In order to generate the fxbm formatted file, the following steps can be used from the root directory of the project (do not perform the steps from this directory, it won't work):
```
git clone https://github.com/flipperdevices/flipperzero-game-engine
python3 flipperzero-game-engine/scripts/sprite_builder.py sprites files
cd files
xxd -i 000.fxbm > ../missingno_i.h
rm 000.fxbm
rm all_sprites.fxbm
for I in *; do cat $I >> all_sprites.fxbm; rm $I; done
cd -
```
