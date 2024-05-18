# MALVEKE **GAME BOY** Link Camera

## Introduction
MALVEKE app, this application allows you to extract your GAME BOY Camera pictures via WIFI for easy sharing with your phone, tablet, or PC. It's user-friendly; simply connect it to your GAME BOY and print as usual. The device will store the images and share them on a web server via WIFI. You will need a printer cable or Gameboy Color link cable for this.

## Instructions for use.

These instructions assume that you are starting at the Flipper Zero desktop. Otherwise, press the Back button until you are at the desktop.

- Press the **OK** button on the Flipper to open the main menu.
- Choose **Applications** from the menu.
- Choose **GPIO** from the submenu.
- Choose **MALVEKE GAME BOY Link-Camera**
- The Flipper Zero will show the loading screen of the application. 
- If the **MALVEKE** board is not connected, the following message will appear. You must connect it and press **OK**.
- **MALVEKE** will create a Wi-Fi network using the data displayed on the screen.
- On your computer or phone, search for the network and connect using the provided access credentials.
- When you connect, it will automatically open a browser window with the host. If this doesn't happen, you should manually enter the IP provided on the Flipper Zero as the **host** in your browser.
- Now, on your ***GAME BOY***, connect the Link Cable to **MALVEKE** **EXT2** port and proceed to print images as usual; they will appear in the web browser.
- In web you can Change palette and with button **A** Download image

## TODO
- Refactor Code
- Documentation
- Save transfered imagen in Flipper Zero