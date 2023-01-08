# Flipper Zero BT Serial Example App

This is a very simple Serial-over-Bluetooth app for Flipper Zero. You can use it (but better not to :D) as a reference app for building your own solutions. 

## What does it do?
This application hijacks the default Serial-over-Bluetooth connection of a Flipper Zero device (that is usually used for communication with mobile app) and set a custom callback on data received over the connection. Therefore, by modifying this app you can implement your own custom logic on using Serial connection over Bluetooth. As an example - see this repository: https://github.com/maybe-hello-world/flipper-bp

## How to use?
0. Optional: enable debug log on the Flipper device: System - Log level - Debug
1. Compile and upload the app on the flipper.
2. Enable bluetooth and pair your device with the flipper.
3. Run the application on the flipper (optionally: connect flipper via usb and use `fbt cli -> log` to observe the logs)
4. On your device launch an app that supports serial profile over bluetooth (e.g., for Linux: bluetoothctl) and use these channels for connection:
```
tx: 19ed82ae-ed21-4c9d-4145-228e62fe0000
rx: 19ed82ae-ed21-4c9d-4145-228e61fe0000
```
Specifically, try to write into `tx` channel some bytes.
5. Observe bytes received on the Flipper's side. 

## How to modify it for my own app?
0. Optional: find a normal Flipper Bluetooth Serial reference app instead of this junk. If it's impossible - go to step 1.
1. The application substites the default serial callback of the Flipper RPC connection. In the fbs.c modify the `bt_serial_event_callback` function to introduce your own custom logic of processing the data received (or sent) over Bluetooth.

## How to build?
See [this](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/AppsOnSDCard.md) official documentation on how to build external applications for Flipper Zero.


