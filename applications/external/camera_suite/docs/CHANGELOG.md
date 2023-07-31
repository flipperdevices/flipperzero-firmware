## Roadmap

- Save image support.
- Full screen 90 degree and 270 degree fill.
- Camera flash support.
- In-camera GUI.

## v1.1 (current)

- Support and picture stabilization for all camera orientations (0 degree, 90 degree, 180 degree, and 270 degree).
- Rename "Scene 1" to "Camera". No UX changes there.
- Clean up unused "Scene 2". This was inaccessible to users previously and unused.
- Add new dithering variations (requires the latest firmware installation, see here for the installation guide https://github.com/CodyTolene/Flipper-Zero-Camera-Suite#firmware-installation):
  - "Jarvis Judice Ninke" dithering option
  - "Stucki" dithering option.
  - "Floyd-Steinberg" dithering option.
  - Cycle through the dithering options with the center button on the Flipper Zero.
- Resolves issue https://github.com/CodyTolene/Flipper-Zero-Camera-Suite/issues/7
- Resolves issue https://github.com/CodyTolene/Flipper-Zero-Camera-Suite/pull/17

## v1.0

- Builds upon Z4urce's software found here (updated 6 months ago): https://github.com/Z4urce/flipperzero-camera
- Utilizes the superb C boilerplate examples laid out by leedave (updated last month): https://github.com/leedave/flipper-zero-fap-boilerplate
- Builds upon the "Camera" software into the new "Camera Suite" application with new usage:
  - Add a scene for a guide.
  - Add a scene for settings.
  - Add ability to rotate the camera orientation.
