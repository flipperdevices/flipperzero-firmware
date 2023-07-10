<div align="center">
  <img align="center" src=".github/images/flipper-dev.png" />
  <h2 align="center">Flipper Zero - Camera Suite</h2>
  <p align="center">
    Firmware and software to run an ESP32-CAM module on your Flipper Zero device.
  </p>
  <a href="https://shop.flipperzero.one/">
    <img src=".github/images/flipper-zero-buy-now.svg" />
  </a>
  <a href="https://docs.flipperzero.one/">
    <img src=".github/images/flipper-zero-docs.svg" />
  </a>
</div>

---

## Table of Contents <a name="index"></a>

- [Previews](#previews)
- [Hardware Requirements](#hardware-requirements)
- [Hardware Installation](#hardware-installation)
- [Firmware Installation](#firmware-installation)
- [Software Installation](#software-installation)
- [Software Guide](#software-guide)
- [Attributions](#attributions)

## Previews <a name="previews"></a>

This section is coming soon...

<p align="right">[ <a href="#index">Back to top</a> ]</p>

## Hardware Requirements <a name="hardware-requirements"></a>

Requires an ESP32-CAM module (I've personally used these: [Amazon link 1][amazon-esp32-cam-link-1] | [Amazon link 2][amazon-esp32-cam-link-2] ).

<p align="right">[ <a href="#index">Back to top</a> ]</p>

## Hardware Installation <a name="hardware-installation"></a>

This section is coming soon...

<p align="right">[ <a href="#index">Back to top</a> ]</p>

## Firmware Installation <a name="firmware-installation"></a>

This section is coming soon...

<p align="right">[ <a href="#index">Back to top</a> ]</p>

## Software Installation <a name="software-installation"></a>

1. Connect your Flipper Zero via USB, or insert your MicroSD.
2. Download "camerasuite.fap" from the release section.
3. Move "camerasuite.fap" into `~\apps\gpio\` on your Flipper Zero MicroSD:
   ```
   .                            # The Flipper Zero MicroSD root.
   ├── apps                     # The Flipper Zero Applications folder.
   |   ├── gpio                 # The Flipper Zero General Purpose Input/Output folder.
   |   |   ├── camerasuite.fap  # The Camera Suite application.
   ```
4. Reinsert your MicroSD into your Flipper Zero if you took it out.
5. Plug in your ESP32-CAM module to your Flipper Zero.
6. Press the "Power" button on your Flipper Zero to turn it on.
7. Open the application "[ESP32-CAM] Camera Suite":
   ```
   Applications > GPIO > [ESP32-CAM] Camera Suite
   ```
8. That's it! Follow the on screen instructions to continue.

<p align="right">[ <a href="#index">Back to top</a> ]</p>

## Software Guide <a name="software-guide"></a>

This section is coming soon...

<p align="right">[ <a href="#index">Back to top</a> ]</p>

## Attributions <a name="attributions"></a>

This project is based on/forked from the [Fliper Zero Camera Application][flipperzero-camera]
by [Z4urce][github-profile-z4urce] combined with the [Flipper Zero Boilerplate Application][flipper-zero-fap-boilerplate]
by [Dave Lee][github-profile-leedave].

<p align="right">[ <a href="#index">Back to top</a> ]</p>

Fin. Thanks for looking and happy programming friend!

Cody

<!-- LINKS -->

[amazon-esp32-cam-link-1]: https://amzn.to/3NCoQUq
[amazon-esp32-cam-link-2]: https://amzn.to/46IuAF9
[flipper-zero-fap-boilerplate]: https://github.com/leedave/flipper-zero-fap-boilerplate
[flipperzero-camera]: https://github.com/Z4urce/flipperzero-camera
[github-profile-z4urce]: https://github.com/Z4urce
[github-profile-leedave]: https://github.com/leedave