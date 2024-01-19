## Roadmap

- Store images to onboard ESP32-CAM SD card (partially completed, #24).
- Camera preview GUI overlay (#21).
- Full screen 90 degree and 270 degree fill (#6).

## v1.5

- Remove usage of image no longer found in the Flipper Zero firmware build.

## v1.4

- Add RC builds to repo.
- Improve FAP code.
- Improve Firmware code (requires reflash).
- Improve Firmware flashing utility code.
- Improve GitHub actions code.
- Look to mitigate issue "Mirrored Image" #27.

## v1.3.1 (patch)

- Addressed new linting issue with "ufbt" tools.

## v1.3

- Important: Firmware Update Required! Ensure you update your firmware to fully utilize the new features. Backwards compatibility should be ok.
- New Feature: Introducing the Firmware Flash utility, simplifying the firmware flashing process. Refer to the project readme for detailed instructions. (Closes #26)
- Enhancement: Flash functionality now remains active during camera preview, making it easier to take pictures in areas of low light.
- Bug Fix: Addressed picture inversion issue reported by user leedave. Thanks for your contribution! (Closes #23)
- Code Refinement: Enhanced firmware code for readability and maintainability. Separated concerns into individual files for a more organized structure.
- Technical Improvements: Implemented general code enhancements and introduced syntactic sugar for cleaner, more efficient code.
- Work in Progress: Added a new test function for saving pictures to the onboard ESP32-CAM SD card. This feature is under development and will allow users to save pictures directly to the SD card in the future. Tracked under feature request #24.

## v1.2

- Save image support. When the center button is pressed take a picture and save it to the "DCIM" folder at the root of your SD card. The image will be saved as a bitmap file with a timestamp as the filename ("YYYYMMDD-HHMMSS.bmp").
- Camera flash support. Flashes the ESP32-CAM onboard LED when a picture is taken if enabled in the settings.
- Move the camera dithering type to the settings scene as a new configurable option.
- Add "Flash" option to the settings scene as a new configurable option.
- Update documentation to reflect changes.
- Update firmware with new dithering options set.
- Update firmware with new flash support.
- Update repo to reflect <https://github.com/CodyTolene/Flipper-Zero-Development-Toolkit> for easier tooling.

## v1.1

- Support and picture stabilization for all camera orientations (0 degree, 90 degree, 180 degree, and 270 degree).
- Rename "Scene 1" to "Camera". No UX changes there.
- Clean up unused "Scene 2". This was inaccessible to users previously and unused.
- Add new dithering variations (requires the latest firmware installation, see here for the installation guide <https://github.com/CodyTolene/Flipper-Zero-Camera-Suite#firmware-installation>):
  - "Jarvis Judice Ninke" dithering option
  - "Stucki" dithering option.
  - "Floyd-Steinberg" dithering option.
  - Cycle through the dithering options with the center button on the Flipper Zero.
- Resolves issue <https://github.com/CodyTolene/Flipper-Zero-Camera-Suite/issues/7>
- Resolves issue <https://github.com/CodyTolene/Flipper-Zero-Camera-Suite/pull/17>

## v1.0

- Builds upon Z4urce's software found here (updated 6 months ago): <https://github.com/Z4urce/flipperzero-camera>
- Utilizes the superb C boilerplate examples laid out by leedave (updated last month): <https://github.com/leedave/flipper-zero-fap-boilerplate>
- Builds upon the "Camera" software into the new "Camera Suite" application with new usage:
  - Add a scene for a guide.
  - Add a scene for settings.
  - Add ability to rotate the camera orientation.
