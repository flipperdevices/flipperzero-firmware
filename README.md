# USB HID Autofire

[GitHub](https://github.com/pbek/usb_hid_autofire)

This is a simple Flipper Zero application to send left-clicks as a USB HID device.

## Building

```shell
cd applications_user
git clone https://github.com/pbek/usb_hid_autofire.git

# Build the application
./fbt fap_usb_hid_autofire

# Build and launch the application
./fbt launch_app APPSRC=usb_hid_autofire
```
