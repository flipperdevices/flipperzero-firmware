# UART Terminal for Flipper Zero
[Flipper Zero](https://flipperzero.one/) app to control various devices via UART interface.

## Capabilities
- Read log and command output by uart
- Send commands by uart
- Set baud rate
- Quick commands

## Connecting
| Flipper Zero pin | UART interface |
| ---------------- | -------------- |
| 13 TX            | RX             |
| 14 RX            | TX             |
|8, 18 GND         | GND            |

Info: If possible, do not power your devices from 3V3 (pin 9) Flipper Zero. It does not support hot plugging.

## How to install
Copy the contents of the repository to the applications_user/uart_terminal folder Flipper Zero firmware and build app with the command ./fbt fap_uart_terminal.

Or use the tool uFBT for building applications for Flipper Zero.

## How it works


![1f](https://user-images.githubusercontent.com/122148894/211161450-6d177638-3bfa-42a8-9c73-0cf3af5e5ca7.jpg)


![2f](https://user-images.githubusercontent.com/122148894/211161456-4d2be15b-4a05-4450-a62e-edcaab3772fd.jpg)


![3f](https://user-images.githubusercontent.com/122148894/211161458-2f9b0d80-f501-4db3-9242-1b3c11729132.jpg)


![4f](https://user-images.githubusercontent.com/122148894/211161461-4507120b-42df-441f-9e01-e4517aa83537.jpg)

## INFO:

70-80% of the source code is taken from the Wifi Marauder project. Many thanks to the developers of the Wifi Marauder project.
