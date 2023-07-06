# LoRA Terminal for Flipper Zero
[Flipper Zero](https://flipperzero.one/) app to control Lora Breakout board.

## Capabilities
- Read log and command output by uart
- Send commands by uart
- Set baud rate
- Fast commands

## Connecting
| Flipper Zero pin | UART interface  |
| ---------------- | --------------- |
| 13 TX            | RX              |
| 14 RX            | TX              |
|8, 18 GND         | GND             |

Info: If possible, do not power your devices from 3V3 (pin 9) Flipper Zero. It does not support hot plugging.

## Keyboard
LoRA_terminal uses its own special keyboard for work, which has all the symbols necessary for working in the console.

To accommodate more characters on a small display, some characters are called up by holding.

![kbf](https://user-images.githubusercontent.com/122148894/212286637-7063f1ee-c6ff-46b9-8dc5-79a5f367fab1.png)


## How to install
Copy the contents of the repository to the applications_user/LoRA_terminal folder Flipper Zero firmware and build app with the command ./fbt fap_LoRA-Term.

Or use the tool [uFBT](https://github.com/flipperdevices/flipperzero-ufbt) for building applications for Flipper Zero.

## How it works


## INFO:
Source code is taken from the [UART Terminal](https://github.com/cool4uma/UART_Terminal) project. Many thanks to the developers of the Wifi Marauder project and the UART Terminal project.
