# Flipper Zero Mimic Apple AirTag

- You can spoof flipper0 to act as an Apple AirTag by editing gap.c file in firmware and send data to be advertise simulating AirTag package.
- After all these changes flipper0 can still connect normally by Bluetooth without issues.

## Info
- Just replace gap.c with the modified one.
- Works with Official Flipper Firmware but should work with unleashed, RogueMaster too
- You need to sniff the nfc registration data in 14443a protocol between a real AirTag and Iphone process and change the advertise data. (You need new unpaired AirTag)

## Screenshot

<img src="https://github.com/culturally/flipper-zero-airtag/blob/main/screenshot.jpg?raw=true">

## Credits

- Found by https://github.com/Ocelot-Offensive-Security/
