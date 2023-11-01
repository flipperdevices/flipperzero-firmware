# Marauder Firmware
To update your ESP32 firmware, you can use [ESP32Marauder](https://github.com/justcallmekoko/ESP32Marauder/wiki/Flipper-Zero)

Talking Sasquach made a [video](https://youtu.be/uc5_hVmJ-3Y?t=354) that walks you through the process.
- Install Git & Python3.
- Install the "[CP210x Universal Windows Driver](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads)" from Silicon Labs.
- Open PowerShell
- git clone https://github.com/SkeletonMan03/FZEasyMarauderFlash
- cd FZEasyMarauderFlash
- run `pip3 install -r requirements.txt`. 
- Connect the devboard or ESP32 board via USB.
- Press and hold the `BOOT` button on the module, press and release the `RESET` button.
- Release the BOOT button. 
- run `python3 EasyInstall.py`. 
- Select the option of what you want to do
  - I choose "Update all files"
  - then I choose "Flash Marauder on ESP32-WROOM" for the WROOM module.
  - or "Flash Marauder on ESP32 Wemos D1 Mini" for the D1 Mini module.
