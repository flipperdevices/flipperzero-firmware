# GAME BOY Pokemon Trading MALVEKE

## Introduction

Tool for trading Pokemon from the Flipper Zero to Generation I and II Game Boy games (Red, Blue, Yellow, Gold, Silver, and Crystal). In addition to creating and trading Pokemon to a Game Boy game, the Flipper can be used to modify an existing Pokemon traded to the Flipper and trading it back to the Game Boy!

No longer causes the OK button to break when using the pinout for older MALVEKE (MLVK2.5) PCBs! Users of MALVEKE PCBs Rev. <= 2.5 no longer need to reset the Flipper after a trade and are able to modify the traded Pokemon!

See the project's [README on GitHub](https://github.com/EstebanFuentealba/Flipper-Zero-Game-Boy-Pokemon-Trading/blob/main/README.md) for more detailed instructions on the application and all of the customization options for Pokemon.

## Connection: Flipper Zero GPIO - Game Boy

See the project's [README on GitHub](https://github.com/EstebanFuentealba/Flipper-Zero-Game-Boy-Pokemon-Trading/blob/main/README.md) for Link Cable interface adapters available on Tindie.

The Original pinout is as follows:

| Cable Game Link (Socket) | Flipper Zero GPIO |
| ------------------------ | ----------------- |
| 6 (GND)                  | 8 (GND)           |
| 5 (CLK)                  | 6 (B2)            |
| 3 (SI)                   | 7 (C3)            |
| 2 (SO)                   | 5 (B3)            |

Using the "Select Pinout" option, the Original, MALVEKE (MLVK2.5), or any custom pin configuration can be selected as well.


## How Does it Work?

Game Boy and Game Boy color games communicate using a synchronous serial protocol; similar to SPI without a chip select line. The Pokemon games themselves arbitrate which unit is driving the clock; this application lets the Game Boy drive the clock every time. Like SPI, data is full-duplex bidirectional, and uses 3 signals:

- A clock signal (CLK).
- An output signal (Serial Out or SO).
- An input signal (Serial In or SI).

The Game Boy itself uses a shift register to clock data in and out.

## Tested to Support Game Boy models
- Game Boy Color (GBC)
- Game Boy Advance (GBA)
- Game Boy Advance SP (GBA SP)
- Analogue Pocket
