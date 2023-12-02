# flipperzero-lightmeter

[![FAP: Build and lint](https://github.com/oleksiikutuzov/flipperzero-lightmeter/actions/workflows/build-and-lint.yml/badge.svg)](https://github.com/oleksiikutuzov/flipperzero-lightmeter/actions/workflows/build-and-lint.yml)

Download: [![FAP Factory](https://flipc.org/api/v1/oleksiikutuzov/flipperzero-lightmeter/badge?root=application)](https://flipc.org/oleksiikutuzov/flipperzero-lightmeter?root=application)

Repository for source code and hardware files for Lightmeter app for Flipper Zero

<img src="images/framed_gui_main.png" width="500px">

<img src="images/framed_gui_lux_meter.png" width="500px">

## Supported sensors

- BH1750
- MAX44009

## Wiring

```
VCC -> 3.3V
GND -> GND
SCL -> C0
SDA -> C1
```

## Sensor module

<img src="module/back.jpg" width="500px">

### If you want to build this module, you'll need (it's quite over-engineered, sorry :D)
1. BH1750 Sensor
2. [Module PCB](https://github.com/oleksiikutuzov/flipperzero-lightmeter/blob/main/module/module_v2_gerber.zip)
3. [Enclosure](https://github.com/oleksiikutuzov/flipperzero-lightmeter/blob/main/module/module_v2_enclosure.stl)
4. 4-pin female header
5. 10-pin male header
6. 2x M3 threaded inserts (max diameter 5.3 mm, max height 4 mm)
7. 2x M3x5 screws

## TODO
- [ ] Save settings to sd card
- [ ] Color measurement with TCS34725 sensor

## References
App inspired by [lightmeter](https://github.com/vpominchuk/lightmeter) project for Arduino by [vpominchuk](https://github.com/vpominchuk).