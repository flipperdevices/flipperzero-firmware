# Hardware Communication
## Flipper
- Through UART
	- 5v: 5v
	- Tx: 13
	- Rx: 14
	- GND: 18

## RPI 0 W
- Through UART (must have bluetooth disabled)
	- 5v: 5v
	- Tx: GPIO 14
	- Rx: GPIO 15
	- GND: 6

##### Disable Bluetooth
In /boot/config.txt add:
```
dt-overlay=disable-bt
```

## Interface
| Flipper | Jumpers | RPI    |
| ------- | ------- | -------|
| 5v      | Brown   | 5v     |
| Tx      | Gray    | Rx     |
| Rx      | White   | Tx     |
| GND     | Black   | GND    |

#### Flipper Wiring
<img src='attachments/flipperGPIO.png' alt='Wires connected to corresponding flipper GPIO ports' height="250" width="400"/>

#### RPI Wiring
<img src='attachments/rpiGPIO.png' alt='Wires connected to corresponding RPI GPIO ports' height="160" width="400"/>