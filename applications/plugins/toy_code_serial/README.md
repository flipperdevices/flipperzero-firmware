# Flipper BP

Custom implementation of Intiface T-Code protocol via Serial-over-Bluetooth connection for Flipper Zero devices.

## What is it?
Flipper Zero devices are capable of communicating with other hardware solutions via different protocols or connections, including using GPIO pins. Therefore you could be able to use it as a periphery controller or proxy for devices that are incapable of connecting to the Intiface software by their own, because they lack wireless or proper wired connection.

This app allows Flipper Zero devices to use Serial-over-Bluetooth connection and T-Code protocol to communicate to Intiface software and you can use it as a starting point for developing support to your own periphery connected to the Flipper.

## How to use it?
Warning: currently all of this is Work In Progress, as user device configuration settings are not finished yet at Intiface software. As soon as this feature would be added, this README would be updated with more information.

0. Establish a Bluetooth communication to your Flipper device: enable debug logs, follow Flipper logs (usually with `./fbt cli -> log`), and pair Flipper device to your PC.
1. Run the application, choose `Flipper Internal` mode for testing and press central button.
2. Optionally: check with any serial terminal that your connection is successfully working - write any bytes into `19ed82ae-ed21-4c9d-4145-228e62fe0000` TX channel and check that callback is successfully receiving and parsing the data. If you want, you can refer to official T-Code [specification](https://docs.buttplug.io/docs/stpihkal/protocols/tcode/) to construct a valid vibration message to check the device.
3. Run Intiface Engine with a custom device configuration (see `device-config.json` or official Intiface documentation on this).
4. If your Flipper has Bluetooth enabled and paired with the computer/smartphone, Intiface should find the Flipper and propose to use it as a periphery. You can use Intiface playground for checking whether everything is working.
5. Enjoy (literally)

## How to build my own version of this app for my own additional periphery connected to the Flipper?
0. Explore how the current application is written. You should be able to develop a new view (like the current `internal`) and add to the `FBP` and `FBPAppView` in `fbp.h` additional information about your view, as well as a proper initialization of your view into `fbp.c` file.
1. Ensure that your periphery is working properly with the Flipper and you are able to properly control it from the code.
2. Develop a new view for the app that will use Serial-over-Bluetooth for receiving the commands and manipulate your periphery reacting to them. By default, there's a (potentially unstable and full of bugs) implementation of T-Code parser implemented for the Flipper, but you can develop your own protocol that Intiface software supports. 

Please, refer to the official Flipper Zero External Apps documentation for building the app, as well as to Intiface and Flipper Zero discords. 