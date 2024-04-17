# Flipper Zero Spotify Remote

## An app for the Flipper Zero that lets you control Spotify playback using the Flipper Zero and WiFi Devboard
Thank you to [Derek Jamison](https://github.com/jamisonderek) for debugging help!

### Demo
https://github.com/benjamindrussell/flipper-spotify-remote/assets/46113261/9d0cc41b-262d-4086-8f4b-a16d3f9a65fe

### Installation
This app is included in the [RogueMaster Firmware](https://github.com/RogueMaster/flipperzero-firmware-wPlugins), or you can build it from source using [ufbt](https://github.com/flipperdevices/flipperzero-ufbt) or [fbt](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/fbt.md)

I am working on getting the [Build Flipper Application Package](https://github.com/marketplace/actions/build-flipper-application-package-fap) GitHub workflow working so you can get the .fap directly from this repo

You will also need to install my [ESP32 Spotify Remote](https://github.com/benjamindrussell/esp32-spotify-remote) on the WiFi Devboard

### Setup 
1. Go to the [Spotify Developer Site](https://developer.spotify.com/) and make an account
2. Go to your dashboard and make a new app
3. Copy your Client ID and Client Secret into the [ESP32 Code](https://github.com/benjamindrussell/esp32-spotify-remote)
4. You will have to add http://IP_ADDRESS/callback as a reirect uri in your spotify dashboard, where IP_ADDRESS is the ip that appears on the Flipper Screen for each new ip since I wasn't able to have it work with a static ip on the ESP32
<img width="536" alt="image" src="https://github.com/benjamindrussell/flipper-spotify-remote/assets/46113261/7116d5d3-048f-4c29-b4f5-e02061d8fe7a">


