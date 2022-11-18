# USB HID Autofire

[GitHub](https://github.com/pbek/usb_hid_autofire) |
[Latest release](https://github.com/pbek/usb_hid_autofire/releases/latest)
[Changelog](CHANGELOG.md) |
[License](LICENSE.md)

[![Build](https://github.com/pbek/usb_hid_autofire/actions/workflows/build-test.yml/badge.svg)](https://github.com/pbek/usb_hid_autofire/actions/workflows/build-test.yml)

This is a simple Flipper Zero application to send left-clicks as a USB HID device.

## Building

```shell
cd applications_user
git clone https://github.com/pbek/usb_hid_autofire.git

cd ..

# Build the application
./fbt fap_usb_hid_autofire

# Build and launch the application
./fbt launch_app APPSRC=usb_hid_autofire
```
