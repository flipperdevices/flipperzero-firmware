# Programmer Calculator

This repository contains a calculator application designed to run on Flipper Zero.


<!-- ![Calculator App Screenshot](https://raw.githubusercontent.com/armixz/Flipper-Zero-Programmer-Calculator/main/app.png) -->

<p align="center">
  <img src="https://raw.githubusercontent.com/armixz/Flipper-Zero-Programmer-Calculator/main/img/1.png">
</p>

## Features ✨

- `Decimal to Binary`
- `Decimal to Hexadecimal`
- `Decimal to Charachter`
- `Hexadecimal to Binary`
- `Hexadecimal to Decimal`
- `Binary to Decimal`
- `Binary to Hexadecimal`

## Getting Started 🚀

To get started with this calculator program, clone the repository and compile the source files.

```bash
# clone `RogueMaster Firmware` repository:
git clone --recursive https://github.com/RogueMaster/flipperzero-firmware-wPlugins.git
cd flipperzero-firmware-wPlugins/
--or--
git clone https://github.com/flipperdevices/flipperzero-firmware.git
cd flipperzero-firmware/

# flash firmware directly to the Flipper 
# (Needs to be connected via USB, qFlipper closed)
./fbt flash_usb_full

# clone this repository (Calculator)
# move all files to `applications_user` directory
cd applications_user/
git clone https://github.com/armixz/Flipper-Zero-Programmer-Calculator.git

# build and launch the app
cd ~/path/to/root-directory eg. ~/flipperzero-firmware-wPlugins
./fbt launch APPSRC=programmercalc
```

## Usage 💡

- The FAP application is available in [flipc.org](https://flipc.org/armixz/Flipper-Zero-Programmer-Calculator).
- Use the keypad to input numbers.
- Toggle between different modes using MODE key.
- View results on the display.

## Contributing 🤝

Contributions to improve the calculator program are welcome. Please feel free to fork the repository and submit a pull request.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## Issues 🐛

If you find any bugs or issues, please create a new issue in the [issues section]() of the repository. Be sure to include detailed information and steps to reproduce the issue.

## License 📄

Distributed under the GPL-3.0 License. See `LICENSE` for more information.

## Acknowledgements 🎉

- Huge thanks to [@n-o-T-I-n-s-a-n-e](https://github.com/n-o-T-I-n-s-a-n-e) for being the inspiration and idea behind starting this project.
- This project uses libraries and tools from the Flipper Universal Registry Implementation (FURI), including Furi, Furi HAL, and GUI.
