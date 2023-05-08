# WIFI_MAP

An ESP32 and FlipperZero wifi mapping.

Check the ESP32 side at: [esp32-wifi-map](https://github.com/carvilsi/esp32-wifi-map)

## Build and flash

$ `./fbt launch_app APPSRC=wifi_map`

## Logs

$ `minicom -D /dev/ttyACM0 -b 230400`

## Connection FlipperZero and ESP32

| Flipper Zero GPIO | ESP32 |
| ----------------- | ----- |
|      GND          |  GND  |
|      RX           |  TX0  |
|      3V3          |  3V3  |
