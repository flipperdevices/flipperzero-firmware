# USB HID Autofire

[GitHub](https://github.com/pbek/usb_hid_autofire)

[![Build](https://github.com/pbek/usb_hid_autofire/actions/workflows/build.yml/badge.svg)](https://github.com/pbek/usb_hid_autofire/actions/workflows/build.yml)

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
