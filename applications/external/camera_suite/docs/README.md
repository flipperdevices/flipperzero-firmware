## Flipper Zero - Camera Suite

Software to run an ESP32-CAM module on your Flipper Zero device.

Full setup, wiring guide, etc. in the main project README here: https://github.com/CodyTolene/Flipper-Zero-Camera-Suite

Firmware is needed for the ESP32-CAM module, see here for more information: https://github.com/CodyTolene/Flipper-Zero-Camera-Suite#firmware-installation

## Software Guide

Button mappings:

**Up** = Contrast Up

**Down** = Contrast Down

**Left** = Toggle invert.

**Right** = Toggle dithering on/off.

**Center** = Take a picture and save to the "DCIM" folder at the root of your SD card. Image will be saved as a bitmap file with a timestamp as the filename ("YYYYMMDD-HHMMSS.bmp"). If flash is on in the settings (enabled by default) the ESP32-CAM onboard LED will light up when the picture is taken.

**Back** = Go back.

Settings:

**Orientation** = Rotate the camera image 90 degrees counter-clockwise starting at zero by default (0, 90, 180, 270). This is useful if you have your camera module mounted in a different orientation than the default.

**Flash** Toggle the ESP32-CAM onboard LED on/off when taking a picture.

**Dithering Type** Change between the Cycle Floyd–Steinberg, Jarvis-Judice-Ninke, and Stucki dithering types.

**Haptic FX** = Toggle haptic feedback on/off.

**Sound FX** = Toggle sound effects on/off.

**LED FX** = Toggle LED effects on/off.
