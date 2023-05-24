# Bar code scanner emulator
### Scanner work at COM-port emulation. 
### Characteristics
* Speed: 19200 (Bit Rate)
* VID: 1155 | PID: 22336
* Name: Flipper {Your Flipper name}
* Manufacturer Name: Flipper Devices Inc.
* SerialNumber=flip_{Your Flipper name}

## Installation
[![FAP Factory](https://fap.playmean.xyz/api/v1/polarikus/flipper-zero_bc_scanner_emulator/badge)](https://fap.playmean.xyz/polarikus/flipper-zero_bc_scanner_emulator)

1. Copy bc_scanner.fap file in Flipper SDCard (apps folder)
2. Open bc_scanner app, for creating bcscanner folder, or create a folder manually via qFlipper 
3. Add bar code files (.bc) in "bcscanner" folder

## GUI
### App icon and name
![Icon](https://gitlab.idcloud.space/flipper-zero/bc_scanner_e/-/wikis/uploads/4c4fabf431947ef31a6e4e8103154b0f/icon.png)
### File Select
After open app - select bar code to be emulated

![Catalog](https://gitlab.idcloud.space/flipper-zero/bc_scanner_e/-/wikis/uploads/a03a1fa3e8bea95510eca7e10af5b5cd/catalog.png)
### Emulation
1. Click "Run" Button

![Run](https://user-images.githubusercontent.com/59930161/214501072-33a32970-7f1c-4f3b-b6f0-39bb4f2201ce.png)
![Work](https://user-images.githubusercontent.com/59930161/214501068-ad11e74f-b501-408b-939b-8d2d7748e542.png)

When sending completed You will see 100% on the display:

![End](https://user-images.githubusercontent.com/59930161/214501074-f567af0c-16d3-4287-81e6-a96ebb6a57f2.png)

2. If you need to resend, click "Run" again 
3. To exit - press the "Back" button on the flipper

## Create Bar Code File
1. Open the .txt file in an editor that recognizes GS special characters, such as sublime text
2. Add a barcode in any format, such as GS1 format
3. Save file and change extension on .bc
4. Copy file in Flipper SDCard, to "/bcscanner" folder

File example [test_evotor.bc](https://gitlab.idcloud.space/flipper-zero/bc_scanner_e/-/wikis/uploads/bfcf94d5c6510ec051369b077b8e94a1/test_evotor.bc)
