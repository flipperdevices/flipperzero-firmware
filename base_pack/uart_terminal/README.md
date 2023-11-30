# UART Terminal for Flipper Zero 
[Flipper Zero](https://flipperzero.one/) app to control various devices via UART interface.
## Download fap
| **FW Official** | **FW Unleashed** |
| - | - |
| [![FAP Factory](https://flipc.org/api/v1/cool4uma/UART_Terminal/badge)](https://flipc.org/cool4uma/UART_Terminal) | [![FAP Factory](https://flipc.org/api/v1/cool4uma/UART_Terminal/badge?firmware=unleashed)](https://flipc.org/cool4uma/UART_Terminal?firmware=unleashed) |

## Capabilities
- Reading from UART in text or hex mode
- Sending commands
- Sending AT commands
- Sending fast commands
- Sending binary packets (in hex)
- Baudrate selection
- UART pins selection (2 options)

## Connecting
| Device UART interface  | Flipper Zero pin (default) | Flipper Zero pin (option) |
| :----------------: | :---------------: | :---------------: |
| RX | 13 TX | 15 TX |
| TX | 14 RX | 16 TX |
| GND  | 8, 18 GND | 8, 18 GND |

Info: If possible, do not power your devices from 3V3 (pin 9) Flipper Zero. It does not support hot plugging.

## Keyboard
UART_terminal uses its own special keyboard for work, which has all the symbols necessary for working in the console.

To accommodate more characters on a small display, some characters are called up by holding.

![kbf](https://user-images.githubusercontent.com/122148894/212286637-7063f1ee-c6ff-46b9-8dc5-79a5f367fab1.png)

## Hex mode
Hexadecimal mode is useful for working with simple binary protocols.
You can see the bytes being received in hexadecimal in the terminal window.
You can also send binary packets to UART. A simplified keyboard is used to enter packet bytes.

![004](https://github.com/rnadyrshin/UART_Terminal/assets/110516632/d5d3c09b-accc-4e6f-b777-81e751284108)

## How to install
Copy the contents of the repository to the applications_user/uart_terminal folder Flipper Zero firmware and build app with the command ./fbt fap_uart_terminal.

Or use the tool [uFBT](https://github.com/flipperdevices/flipperzero-ufbt) for building applications for Flipper Zero.


## How it works


![1f](https://user-images.githubusercontent.com/122148894/211161450-6d177638-3bfa-42a8-9c73-0cf3af5e5ca7.jpg)


![2f](https://user-images.githubusercontent.com/122148894/211161456-4d2be15b-4a05-4450-a62e-edcaab3772fd.jpg)


![4f](https://user-images.githubusercontent.com/122148894/211161461-4507120b-42df-441f-9e01-e4517aa83537.jpg)

## INFO:

~60% of the source code is taken from the [Wifi Marauder](https://github.com/0xchocolate/flipperzero-firmware-with-wifi-marauder-companion) project. Many thanks to the developers of the Wifi Marauder project.
