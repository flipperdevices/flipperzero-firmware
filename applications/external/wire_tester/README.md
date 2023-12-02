# Flipper Wire Tester
This is a simple Flipper Zero app that beeps if a wire is continuous.

![Screenshot](screen_1.png)

## Installation
Copy the `wire_tester` directory to the `applications_user` directory of your
favorite firmware. Run `./fbt launch APPSRC=applications_user/wire_tester` to
install and start the application on your Flipper.


## Usage
Find the app under `Apps` -> `GPIO` -> `[GPIO] Wire Tester` and launch it. Test
a wire by connecting it to pins 6 and 8. If the wire is continuous, you will
see the LED turn green and the flipper will beep.


## Licensing
Source in this repository is licensed under the 2-clause BSD license, see
`LICENSE` for details.
