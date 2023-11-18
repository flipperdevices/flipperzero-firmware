# 🐬 Flipper Zero - Pokemon Trading in Game Boy

## Introduction

This is a Pokemon exchange application from Flipper Zero to Game Boy (Generación I). Flipper Zero emulates a "Slave" Game Boy connected to a Game Link Cable to be able to exchange any Pokemon from the First Generation (Red, Blue, Yellow) to a real Game Boy.

It is a Proof of Concept (POC) for using views, GPIO, and FURI (Flipper Universal Registry Implementation).


## Connection: Flipper Zero GPIO - Game Boy

The pins should be connected as follows:

| Cable Game Link (Socket) | Flipper Zero GPIO |
| ------------------------ | ----------------- |
| 6 (GND)                  | 8 (GND)           |
| 5 (CLK)                  | 6 (B2)            |
| 3 (SI)                   | 7 (C3)            |
| 2 (SO)                   | 5 (B3)            |

## How does it work?

The method used to communicate 2 Game Boys is based on the SPI protocol, which is a very simple serial communication protocol in which a master device communicates with one or more slave devices. The protocol is bidirectional and synchronous, and uses three basic signals:

- A clock signal (CLK).
- An output signal (Serial Out or SO).
- An input signal (Serial In or SI).

In the Game Boy, games store data in an internal shift register that is used to send and receive information. The SPI protocol used by the Game Boy uses the clock signal to indicate when data is being transferred.

The Game Boy link protocol is synchronous and requires the slave device to respond at the same rate as the master device. The master device supplies an 8KHz clock (data transfer rate of 1KB/s). The time window for responding is only **~120μs**. However, the slave device has no restrictions and can respond when it receives data. The clock can vary and there is no lower limit.


##  Tested In
- Game Boy Color (GBC)
- Game Boy Advance (GBA)


