## Version 1.9 - 29/05/2024

- Correctly display the bytes at the left-hand side of the serial traffic screen in the passthrough view
- Free the UART receive stream buffer in the main thread rather than in the receive thread
- Added code to turn on the +5V pin as well as the PC1 pin when the app is running - currently compiled out
- Minor corrections

## Version 1.8 - 08/05/2024

- Added serial traffic monitor in the USB serial passthrough view
- Added USB serial passthrough channel setting
- Added utility to decode LRF serial traffic logged in the Flipper Zero CLI in real time

## Version 1.7 - 04/05/2024

- Added USB serial passthrough function

## Version 1.6 - 29/04/2024

- Added baudrate setting
- Faster automatic SMM sampling rate
- Display the supply voltage and receiver temperature in the LRF info view
- Keep the backlight on when testing the LRX laser or the IR pointer
- Undocumented feature can be enabled with a special configuration file (manufacturer only)

## Version 1.5 - 24/04/2024

- Added LRX laser tester
- Added IR pointer tester

## Version 1.4 - 21/04/2024

- Added automatic SMM sampling mode
- Sampling buffer sizeable by number of samples as well as time
- Added buffering indicator
- Added return rate indicator
- Save diagnostic data files in a dedicated folder

## Version 1.3 - 17/04/2024

- Added menu option to save the LRF diagnostic data
- LED flashes to show serial port activity
- Modularized the code

## Version 1.2 - 12/04/2024

- Changed the power control pin from pin #1 (+5V) to #15 (C1)
- Added Pointer ON/OFF toggle
- Automatically start sampling when entering Sample
- Automatically read the LRF's identification when entering LRF information
- Save and restore the last selected submenu option

## Version 1.1 - 10/04/2024

- Added LRF information display
- Save and restore the configuration
- Nicer about screens

## Version 1.0 - 06/04/2024

Initial release
