# WIFI_MAP

An ESP32 and FillperZero wifi mapping

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
