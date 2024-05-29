# Flipper Zero UHF RFID App

## Overview
This app is designed to work with the M6e Nano UHF RFID Reader and the Flipper Zero. This app supports reading up to 150 tags per second, writing to tags (EPC, TID, Reserved, and User Memory Banks), viewing all tag information, saving tags, and more!

## Features
- Read and view up to 150 UHF RFID tags at once!
  - EPC, TID, User, and Reserved Memory Banks
  - Can cycle through multiple tags from a single read!
- Save, rename, delete and view UHF tags on your flipper!
  - EPC, TID, User, and Reserved Memory Banks
- Write UHF tags 
  - EPC Memory Bank
  - TID Memory Bank (not supported if tag is locked)
  - User Memory Bank
  - Reserved Memory Bank 
- UHF RFID Reader Configuration
  - Set RF Power
  - Set antenna type (WIP)
- Features coming soon: 
  - Support for other UHF RFID Modules (YRM1001)
  - Support for custom baud rate, region, protocol, and more!
  - Set custom size for user/reserved memory banks to read
  - Custom PCB with M6e Nano module and external antenna support!

## Hardware Requirements

Currently, this Flipper Zero application requires an M6e Nano compatible UHF RFID Reader. I recommend one of the following options:
- SparkFun Simultaneous RFID Reader
- The M7E Hecto that just came out should work too, however, this I haven't been able to test.

Additionally, a Raspberry Pi Zero is required in order to run the ThingMagic Mercury API
- I hope to eliminate the need for the RPi, however, I thought having it could allow for easier adoption of different UHF RFID readers. 

## Coming Soon
In the future, I plan on developing a custom pcb that utilizes the M6e Nano module offering a powerful UHF Reader that plugs right into the Flipper Zero! Additionally, I plan to add support for other UHF RFID modules such as the YRM1001 module. Some other feature enhancements to the configuration menu will also be added including custom baud rate, power and antenna selection, UHF RFID module selection, and selecting/setting custom UHF RFID tag types (adjusting read/write size for memory banks).
