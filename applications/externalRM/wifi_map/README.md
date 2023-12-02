# WIFI_MAP

![wifi_map](https://github.com/carvilsi/flipper0-wifi-map/blob/main/wifi_map.png?raw=true)

An ESP32 and FlipperZero wifi mapping.

Check the ESP32 side at: [esp32-wifi-map](https://github.com/carvilsi/esp32-wifi-map)

## Install, etc.

Clone this repo at `applications_user/` folder under `flipperzero-firmware`

`$ git clone git@github.com:carvilsi/flipper0-wifi-map.git`

### Build and flash

`$ ./fbt launch APPSRC=wifi_map`

### Logs

`$ minicom -D /dev/ttyACM0 -b 230400`

## Connection FlipperZero and ESP32

| Flipper Zero GPIO |    ESP32    |
|-------------------|-------------|
|      GND          |     GND     |
|      RX           | TXD GPIO_17 |
|      3V3          |     3V3     |

