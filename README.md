# ESP32 Marauder v0.3.2
<p align="center"><img alt="Marauder logo" src="https://github.com/justcallmekoko/ESP32Marauder/blob/master/pictures/marauder3L.jpg?raw=true" width="300"></p>
<p align="center">
  <b>A suite of WiFi/Bluetooth offensive and defensive tools for the ESP32</b>
  <br>
</p>

# Table Of Condensation
- [About](#about)
  - [YouTube](#youtube)
  - [Hackster Article](#hackster-article)
  - [Capabilities](#capabilities)
- [Do It Yourself](#do-it-yourself)
  - [Hardware](#hardware)
  - [Flashing Firmware](#flashing-firmware)
    - [Using Arduino IDE](#setting-up-arduino-ide)
- [Under Development](#under-development)
- [Special Thanks](#special-thanks)
- [For Sale Soon](#for-sale-soon)

# About
Sometimes you just gotta do what you gotta do. Am I right, ladies?
The ESP32 Marauder is a suite of WiFi/Bluetooth offensive and defensive tools created for the ESP32 and was originally inspired by [Spacehuhn's](https://github.com/Spacehuhn) [esp8266_deauther](https://github.com/Spacehuhn/esp8366_deauther) project. The tool itself serves as a portable device used to test and analyze WiFi and Bluetooth devices. Use this tool and its firmware with caution as the use of some of its capabilities without explicit consent from the target owner is unlawful in most countries. For more information about this project and how it's assembled, follow the video link below.

### YouTube
<p align="left"><a href="https://www.youtube.com/watch?v=BGFO1wA29o8"><img alt="Marauder logo" src="https://github.com/justcallmekoko/ESP32Marauder/blob/master/pictures/esp32marauder_thumbnail.jpg" width="500"></a></p>

### Hackster Article
You can check out the marauder article written [here](https://www.hackster.io/news/esp32-marauder-puts-a-bluetooth-wi-fi-pen-testing-toolkit-in-your-pocket-32d389f6e66f) as well as other interesting engineering projects.

## Capabilities
### Current
- Probe Request Sniff: Sniff for WiFi [probe request frames](https://mrncciew.com/2014/10/27/cwap-802-11-probe-requestresponse/) sent by wireless devices
- Beacon Sniff: Sniff [beacon frames](https://en.wikipedia.org/wiki/Beacon_frame) sent by wireless acccess points
- Beacon Spam Random: Spam hundreds of random beacon frames to wireless devices within range
- Rick Roll Beacon: Broadcast the rick roll lyrics as WiFi access points
- Bluetooth Sniffer: Sniff bluetooth devices within range
- Detect Card Skimmers: Detect bluetooth enabled credit card skimmers
- Packet Monitor: Show WiFi packet density on a given channel using a time bar graph
- Deauthentication Packet Sniffer: Detect deauthentication packets sent on all channels
- Draw: Just doodle on the screen or whatever

### Developing
- Emulate Bluetooth: Emulate a bluetooth device with a specific name

# Do It Yourself

## Hardware
This project requires the following hardware in order to work:
- Any ESP32 Development Board
- 2.8" TFT Touch Screen w/ ili9341
  
## Flashing Firmware
### Using Arduino IDE
1. Install the [Arduino IDE](https://www.arduino.cc/en/main/software)
2. In the Arduino IDE, go to `File`>`Preferences`
3. Add the following URL to `Additional Boards Manager URLs:`
    - https://dl.espressif.com/dl/package_esp32_index.json
4. Go to `Tools`>`Board`>`Boards Manager`, search for `esp32` and install `esp32 by Espressif Systems`
6. Install Spacehuhn's [SimpleList](https://github.com/spacehuhn/SimpleList) library in your Arduino IDE
    - Download the SimpleList repo
    - In the Arduino IDE, go to `Sketch`>`Include Library`>`Add .ZIP Library...` and add the SimpleList-master.zip you just downloaded
7. Install Bodmer's [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) library in your Arduino IDE
    - Download the TFT_eSPI repo
    - In the Arduino IDE, go to `Sketch`>`Include Library`>`Add .ZIP Library...` and add the TFT-eSPI-master.zip you just downloaded
    - Make the following modifications shown in [this issue](https://github.com/justcallmekoko/ESP32Marauder/issues/2#issuecomment-555695918) to the TFT_eSPI library you just installed
8. Follow [these instructions](https://github.com/me-no-dev/arduino-esp32fs-plugin) for installing ESP32 Spiffs Tool
9. Install the [CH340 Drivers](https://github.com/justcallmekoko/ESP32Marauder/blob/master/Drivers/CH34x_Install_Windows_v3_4.EXE)
10. Download or clone this repository
11. Open `esp32_marauder.ino`
12. Plug your ESP32 into a USB port and select the COM port under `Tools`>`Port`
12. Select `LOLIN D32` under `Tools`>`Boards`
13. Click `ESP32 Sketch Data Upload` and wait for the SPIFFS upload to finish
14. Click the upload button
  
# Under Development
Currently the ESP32 Marauder has limited firmware capabilities. Most of the work so far has been put into designing the hardware. New firmware features will be added over time and will eventually reach a point where this project will be a fully capable hacking machine.

# Special Thanks
- [Spacehuhn](https://github.com/spacehuhn) for an easy to use linked list library [SimpleList](https://github.com/spacehuhn/SimpleList)
  - Also a well designed menu structure. I adapted it to the TFT GUI because it worked perfectly
- [Bodmer](https://github.com/Bodmer) for a comprehensive TFT touch screen library [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)

# For Sale Now
You can buy the ESP32 Marauder using [this link](https://www.tindie.com/products/justcallmekoko/esp32-marauder/)

<a href="https://www.tindie.com/stores/justcallmekoko/?ref=offsite_badges&utm_source=sellers_justcallmekoko&utm_medium=badges&utm_campaign=badge_large"><img src="https://d2ss6ovg47m0r5.cloudfront.net/badges/tindie-larges.png" alt="I sell on Tindie" width="200" height="104"></a>
