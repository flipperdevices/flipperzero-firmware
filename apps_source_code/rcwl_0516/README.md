# Rcwl-0516 Flippezero

This fap plugin (app) for RCWL-0516 radar.

RCWL-0516 is a doppler radar microwave motion sensor module

![RCWL-0516 board](./images/RCWL-0516-board.jpg)

## Connection

| rcwl-0516. | flipperzero |
| --- | --- |
| 3V3 | NC |
| GND | Pin 3 (A6) |
| OUT | Pin 2 (A7) |
| VIN | Pin 1 (5V) |
| CDS | NC |

## Installation

Make sure you have enough space and clone the source code:.

```bash
git clone --recursive https://github.com/flipperdevices/flipperzero-firmware.git
```

copy my folder to flipperzero-firmware/applications_user.

```bash
cd flipperzero-firmware

./fbt fap_RCWL_0516
```

you will find the app in build/f7-firmware-D/.extapps/,

or you will just see the output file dir in terminal after build

or you can just copy RCWL_0516.fap to your flipperzero /sdcard/app/GPIO and use it

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

## License

[MIT](https://choosealicense.com/licenses/mit/)
