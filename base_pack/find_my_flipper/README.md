# FindMy Flipper - FindMy SmartTag Emulator

This app extends the functionality of the FlipperZero's bluetooth capabilities, enabling it to act as an Apple AirTag or Samsung SmartTag, or even both simultaneously. It utilizes the FlipperZero's BLE beacon to broadcast a SmartTag signal to be picked up by the FindMy Network. I made this to serve as a versatile tool for tracking purposes, offering the ability to clone existing tags, generate OpenHaystack key pairs for integration with Apple's FindMy network, and tune the device's beacon broadcast settings.

## Features

1. Tag Emulation: Clone your existing Apple AirTag or Samsung SmartTag to the FlipperZero, or generate a key pair for use with the FindMy network without owning an actual AirTag.
2. Customization: Users can adjust the interval between beacon broadcasts and modify the transmit power to suit their needs, optimizing for both visibility and battery life.
3. Efficient Background Operation: The app is optimized to run in the background, ensuring that your FlipperZero can still be tracked with minimal battery usage and without stopping normal use.

## Usage Guide

### Step 1: Installation
- **Option A:** Use the released/precompiled firmware appropriate (FAP) for your device.
- **Option B:** Build the firmware yourself using `fbt/ufbt`.
- Both Installation options require you to be running a dev build of firmware. When release gets access to the extra BLE beacon this will change, thank you!
### Step 2: Obtaining SmartTag Data

#### Option A: Open Haystack Method
1. **Generate a Tag:** Download the `generate_keys.py` file and execute it in your terminal. (You will need cryptography ```python3 -m pip install cryptography```)
2. **Follow Prompts:** During execution, you'll be prompted for inputs. By the end, you'll obtain a **Private Key**, **Public Key**, **Payload**, and **MAC Address**.
   - **Private Key** is necessary to receive location reports from Apple.
   - **MAC Address** should be registered in the FlipperZero app:
     1. Open the app and navigate to the config menu.
     2. Choose "register tag" and enter the MAC Address when prompted.
     3. A payload dialog will appear next. Enter your **Payload** here.
     4. Click save.
3. **Configuration Completion:** With this setup, your device is ready for Open Haystack. Proceed with the specific steps for Open Haystack or MaclessHaystack based on your setup.
   - Don't Own a Mac: https://github.com/dchristl/macless-haystack
   - Own a Mac: https://github.com/seemoo-lab/openhaystack

#### Option B: Cloning Existing Tag
1. **Pair a Tag:** First, pair an AirTag or Samsung SmartTag with your device.
2. **Enter 'Lost' Mode:** Keep the tag away from the device it's registered to for approximately 15 minutes.
3. **Download nrfConnect:** Install nrfConnect from the Apple App Store or Google Play Store.
4. **Filter and Scan:**
   - Open the app, click on filters, and exclude all except for the brand of your tag (Apple/Samsung).
   - Adjust the RSSI to the lowest setting (-40 dBm).
   - Initiate a scan. Wait for your SmartTag to appear as a "FindMy" device.
5. **Capture Data:** Click **Raw** or **View Raw** to capture your **payload** and note your tag's **MAC Address**. Immediately remove the tag's battery to prevent key/MAC rotation.
6. **Enter Data in FlipperZero App:** Input the captured **payload** and **MAC Address** into the FlipperZero app.

### Step 3: Configuration
- Upon launching the app, choose whether to clone an AirTag or SmartTag, generate a new Open Haystack key pair, or adjust broadcast settings.

### Step 4: Tracking
- Once the app is configured, your FlipperZero can be tracked using the relevant platform's tracking service (FindMy app for Apple devices, SmartThings for Samsung devices, and respective web browsers).


Customization

- Beacon Interval: Adjust how frequently your FlipperZero broadcasts its presence.
- Transmit Power: Increase or decrease the signal strength to balance between tracking range and battery life.

Background Use

The app is designed to have a negligible impact on battery life, even when running in the background. This allows for continuous tracking without the need for frequent recharging.

Compatibility

- Apple devices for AirTag tracking via the FindMy network.
- Any device that supports Samsung SmartTag tracking, including web browsers (previously FindMyMobile).

Thanks

- Huge thanks to all the people that contributed to the OpenHaystack project, supporting projects, and guides on the subject. This wouldn't be a thing without any of you!

Legal and Privacy

This app is intended for personal and educational use. Users are responsible for complying with local privacy laws and regulations regarding tracking devices. The cloning and emulation of tracking tags should be done responsibly and with respect to the ownership of the original devices.

Disclaimer

This project is not affiliated with Apple Inc. or Samsung. All product names, logos, and brands are property of their respective owners. Use this app responsibly and ethically.
