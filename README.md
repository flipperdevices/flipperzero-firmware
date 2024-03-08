# FindMy Flipper - FindMy SmartTag Emulator

This app extends the functionality of the FlipperZero's bluetooth capabilities, enabling it to act as an Apple AirTag or Samsung SmartTag, or even both simultaneously. It utilizes the FlipperZero's BLE beacon to broadcast a SmartTag signal to be picked up by the FindMy Network. I made this to serve as a versatile tool for tracking purposes, offering the ability to clone existing tags, generate OpenHaystack key pairs for integration with Apple's FindMy network, and tune the device's beacon broadcast settings.

## Features

1. Tag Emulation: Clone your existing Apple AirTag or Samsung SmartTag to the FlipperZero, or generate a key pair for use with the FindMy network without owning an actual AirTag.
2. Customization: Users can adjust the interval between beacon broadcasts and modify the transmit power to suit their needs, optimizing for both visibility and battery life.
3. Efficient Background Operation: The app is optimized to run in the background, ensuring that your FlipperZero can still be tracked with minimal battery usage and without stopping normal use.

Usage

- Installation: Use the released/precompiled FAP for you firmware or build using fbt/ufbt.
- Obtaining SmartTag Data (Open Haystack): To generate a tag, download the generate_keys.py file and run it in terminal. Follow the prompts, at the end you should end up with a **Private Key**, **Public Key**, **Payload**, and **MAC Address**. The **Private Key** is required to obtain the location reports from Apple. The **MAC Address** is to be entered into the FlipperZero app, open the config menu and select register tag. A MAC Address entry dialog should appear, enter your MAC here. Next a payload dialog will appear, enter the **Payload** data directly into this field. Once doe click save. You should be all set to use OpenHaystack. From here follow the OpenHaystack Steps or the MaclessHaystack steps depending on your setup.
- Obtaining SmartTag Data (Cloning Existing Tag): Cloning an existing tag is the far easier and safer method for using your Flipper on the FindMy network. This is also easier to track and works easily with android phones as well as apple devices. First pair an AirTag or Samsung SmartTag to your device. Next have the tag enter "lost" mode, this is typically done by keeping the tag away from the device that initially registered it for ~15min. After some time has passed download and install the free **nrfConnect** app from the Apple Appstore or Google Play Store. Open the app and click on filters, then exclude everything except for the brand of tag you are cloning (Apple/Samsung). It may help to lower the RSSI to the lowest setting (-40 dBm). Finally click scan and wait until you see your smarttag, it should appear as a "FindMy" device. Click **Raw** or **View Raw** and copy or screenshot this data. This is your **payload**. Also take note of your tags **MAC Address**. Once you have this data immediently remove the battery from your tracker so that the keys/mac don't rotate/change. Now just enter the **payload** and **MAC Address** you obtained from nrfConnect into the FlipperZero app your youre set!
- Configuration: Upon launching the app, you'll be able to select whether to clone an AirTag or SmartTag, generate a new OpenHaystack key pair, and adjust the broadcast settings.
- Tracking: Once configured, your FlipperZero can be tracked using the respective platform's tracking service (FindMy app for Apple devices, SmartThings for Samsung, and respective web browsers).

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
