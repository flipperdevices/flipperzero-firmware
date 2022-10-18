# Coffee-EEPROM-FAP

## Articles
- [Vendors Still Allow You to Hack Their Vending Machine](https://medium.com/@nic_whr/vendors-still-allow-you-to-hack-the-vending-machine-34ddad81dbad)

## Features
- Read credit on load
- Press -> right, set credit to 10 EUR
- Press . center, dump the EEPROM to serial (USB connected, log mode ON)
- Press <- left, erase the EEPROM

## Flipper Prototyping Board Schematic

- SDA -> Pin 15
- SCL -> Pin 16
- VCC (3.3V) -> Pin 9
- GND -> Pin 18

**Provide two 4k7 ohms Pull-Up resistors from SDA to VCC and one from SCL to VCC**

The upper 6 pins male header is not required (unless you want to use the PicKit2 either)

![alt text](https://github.com/wh00hw/Coffee-EEPROM-FAP/raw/master/assets/schema.jpg)


## PicKit2 Schematic

![alt text](https://github.com/wh00hw/Coffee-EEPROM-FAP/raw/master/assets/a74c600b-28eb-400c-b6e2-54530d133a6a.jpg)


## Demo

![alt text](https://github.com/wh00hw/Coffee-EEPROM-FAP/raw/master/assets/flipper.jpg)

## TODO
 - [ ] Write .bin dump loaded from file
 - [ ] Set any amount (see [CoffeeEditor](https://github.com/wh00hw/CoffeeEditor))
 - [ ] Threads for better UX (ex loading bar)

## Disclaimer
**I am not responsible for any illegal use of this tool. Edit the credit is like stealing.**

