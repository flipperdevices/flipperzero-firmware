# flipperzero-lightmeter 

[![Build FAP](https://github.com/oleksiikutuzov/flipperzero-lightmeter/actions/workflows/build_fap.yml/badge.svg?branch=main)](https://github.com/oleksiikutuzov/flipperzero-lightmeter/actions/workflows/build_fap.yml) [![Check Code Format](https://github.com/oleksiikutuzov/flipperzero-lightmeter/actions/workflows/lint.yml/badge.svg?branch=main)](https://github.com/oleksiikutuzov/flipperzero-lightmeter/actions/workflows/lint.yml)

<img src="images/framed_gui_main.png" width="500px">  

## Wiring

```
VCC -> 3.3V
GND -> GND
SCL -> C0
SDA -> C1
```

## TODO:
- [x] Ambient light metering
- [x] Make it work with multiple scenes
- [x] Fixed Aperture mode
- [x] Fixed Shutter speed mode 
- [x] ISO correction 
- [x] Info screen
- [x] Pinout screen
- [x] ND correction
- [x] Popup when sensor not detected
- [x] Compensation for diffusion dome 
- [ ] Cleanup code
- [x] Draw suitable icon

## TODO for later
- [ ] Save settings to sd card

## References
App inspired by [lightmeter](https://github.com/vpominchuk/lightmeter) project for Arduino by [vpominchuk](https://github.com/vpominchuk).
