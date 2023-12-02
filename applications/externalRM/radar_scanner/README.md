# Flipper Zero Radar Scanner

## Overview

The Flipper Zero Radar Scanner is an application designed for the Flipper Zeros GPIO capabilities, utilizing the RCWL-0516 Microwave Radar module to detect the presence and movement of living beings or people. This powerful radar module is capable of detecting movement at a range of approximately 8 meters, depending on the environment, and can even detect humans through walls, ceilings, and floors up to 5-6 meters away.

## Features

- **Easy Hardware Setup**: Connect the RCWL-0516 module to your Flipper Zero by supplying 5 volts to the VIN pin with ground connected. The OUT pin of the module should be connected to Pin 7 on the Flipper Zero. Make sure to enable the "5V on GPIO" option when powering the radar module from the Flipper Zero. Be mindful of 5v and GND as reversing these will damage the radar module.

- **Bi-Directional Detection**: The RCWL-0516 module is bi-directional, meaning it can detect movement from both the front and back sides. The front side with components is the primary detection side and should be oriented away from you. You can modify the detection range by adding metal shielding to focus detection where the Flipper is pointing.

- **Adjustable Sensitivity**: If the sensitivity is too high, you can solder a resistor to the module's pad to reduce it, allowing you to fine-tune the detection sensitivity to your needs.

- **User-Friendly Interface**: The app starts in an "inactive" or "standby" state. Press the OK button to activate it. The Flipper display will show "Active," and the app will begin monitoring for movement.

- **Real-Time Feedback**:
  - When movement is detected, the text "No presence" will change to "Presence detected."
  - An alarm will sound to alert you.
  - The LED will change from green ("clear") to red ("detected").
  - The Flipper will vibrate twice to notify you.
  
- **Muted Mode**: Press the Down button to enter "muted" mode. In this mode, detection still triggers text changes, LED status, and vibrations, but there is no sound. You can toggle this mode on and off.

- **Pause and Resume**: Press the OK button to pause detection and return the Flipper to "standby" mode. Press it again to resume monitoring for movement.

- **Exit the App**: Use the Back key to exit the app and return to the Flipper Zero's main menu.

## Getting Started
1. Install the app by moving ```radar_scanner.fap``` into Applications/external

**Download the version for your firmware:**

```OFW_radar_scanner.fap -> Official```
```radar_scanner.fap -> Xtreme```
```UL_radar_scanner.fap -> Unleashed```
```RM_radar_scanner.fap -> Rogue Master```

2. Power on the Flipper Zero and navigate to GPIO to enable 5V.

3. Navigate to the GPIO applications folder and launch ```[GPIO] Radar Scanner```

4. Press Ok to start and monitor for real-time feedback and respond accordingly.

5. Use the Down button to toggle "muted" mode on and off.

6. Press the OK button to pause or resume detection.

7. Press the Back key to exit the app.

## Note

- The range and detection capabilities may vary depending on environmental factors.

- Experiment with sensitivity adjustments and metal shielding to optimize detection for your specific use case.

- Be aware of privacy and legal considerations when using radar-based detection systems.

- Enjoy your Flipper Zero Radar Scanner and explore its various applications!

