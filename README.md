# Flipboard

<img src="./images/banner.png">

The FlipBoard v1.1 is a versatile device for the Flipper Zero that can enable all kinds of features. It has 4 buttons that connect via separate GPIO pins, so you can react to any combinations of button up/down events. Each button has a 24-bit color LED that is programmable using the WS2812b protocol. There is also a small single-color LED on the board you can use, for example, to signal a status change.

There are a couple of applications, such as flipkeyboard (a musical keyboard) and flipblinky (visual effects), that demonstrate some of the features. We will be adding more applications as time progresses, along with tutorials on how to write your own applications. Please let us know your feedback.

You can order your FlipBoard from [https://tindie.com/stores/MakeItHackin](https://tindie.com/stores/MakeItHackin).

## Applications

- [FlipKeyboard](./flipkeyboard/README.md)
- [FlipBlinky](./flipblinky/README.md)
- [FlipSignal](./flipsignal/README.md)

## Installing Apps

There are multiple ways to install applications on your Flipper Zero.  If you just want to run the apps without making any customizations, the easiest way is to install a pre-built FAP (Flipper Application Package) file.  If you want to customize the app & follow along with the various tutorials, you can install the source code and build it yourself.

### Using flipc.org (Chrome, Edge, Opera)

For best experience, use the latest version of Chrome, Microsoft Edge or Opera.  Other browsers will only give you a file you need to manually install (since they don't support the [Web Serial API](https://caniuse.com/web-serial)).

Flipc.org is a website that allows you to install FAP files directly from the web.  This is an unofficial website and is not affiliated with the Flipper Zero team, run by a single person.  It is provided as a convenience for users who want to install apps without having to build them from source.

<img src="images/flipc-install.gif" width=50%/>

- Step 1: **Load the web page** for the app you want to install.  Other people may have "forked" the app and made their own version, so make sure you are using the correct link.  In some cases, their changes may improve the product, but in other cases, they may have introduced bugs or malicious code.  **Use at your own risk.**
  - https://flipc.org/jamisonderek/flipboard?branch=main&root=flipblinky
  - https://flipc.org/jamisonderek/flipboard?branch=main&root=flipsignal
  - https://flipc.org/jamisonderek/flipboard?branch=main&root=flipkeyboard

- Step 2: Click on the underlined text under FIRMWARE and **choose the firmware** that matches what you are running on your Flipper Zero. (Official, Unleashed, RogueMaster, Xtreme)
- Step 3: Click on the underlined text under CHANNEL and **choose the current channel** you are running on your Flipper Zero.  (Release, Release Candidate, Development)
- Step 4: **Connect your Flipper Zero** to your computer. Make sure nothing is running on your computer that is using the serial port.  In particular, close qFlipper, close https://lab.flipper.net, and close any CLI tools that communicate with the Flipper Zero.
- Step 5: Click on the orange **Install button**.
  - The application should get installed.
  - The Install button should change to an orange Run of Flipper button.
  - You can try "Run on Flipper" to run the application, but this will not work for all firmware versions.

### Using flipc.org (Other browsers)

For best experience, use the latest version of Chrome, Microsoft Edge or Opera.  Other browsers will only give you a file you need to manually install (since they don't support the [Web Serial API](https://caniuse.com/web-serial)).

Flipc.org is a website that allows you to install FAP files directly from the web.  This is an unofficial website and is not affiliated with the Flipper Zero team, run by a single person.  It is provided as a convenience for users who want to install apps without having to build them from source.

<img src="images/flipc-download.gif" width=50%/>

- Step 1: **Load the web page** for the app you want to install.  Other people may have "forked" the app and made their own version, so make sure you are using the correct link.  In some cases, their changes may improve the product, but in other cases, they may have introduced bugs or malicious code.  **Use at your own risk.**
  - https://flipc.org/jamisonderek/flipboard?branch=main&root=flipblinky
  - https://flipc.org/jamisonderek/flipboard?branch=main&root=flipsignal
  - https://flipc.org/jamisonderek/flipboard?branch=main&root=flipkeyboard
- Step 2: Click on the underlined text under FIRMWARE and **choose the firmware** that matches what you are running on your Flipper Zero. (Official, Unleashed, RogueMaster, Xtreme)
- Step 3: Click on the underlined text under CHANNEL and **choose the current channel** you are running on your Flipper Zero.  (Release, Release Candidate, Development)
- Step 4: **Connect your Flipper Zero** to your computer. Make sure nothing is running on your computer that is using the serial port.  In particular, close qFlipper, close https://lab.flipper.net, and close any CLI tools that communicate with the Flipper Zero.
- Step 5: Click on the orange **Download button**. The browser should automatically download the file and put it in your web download folder.
- Step 6: **Install the FAP** onto your Flipper Zero using one of the methods below.
  - [Using qFlipper](#copying-fap-using-qflipper)
  - [Using SD Card](#copying-fap-using-sd-card)

### Copying FAP using qFlipper

If you are using a browser that does not support the Web Serial API, you will need to manually copy the FAP file to your Flipper Zero.

<img src="images/qflipper-copy.gif" width=50%/>

- Step 1: Download the FAP file to your computer.
- Step 2: Connect your Flipper Zero to your computer.
- Step 3: Launch [qFlipper](https://docs.flipper.net/qflipper) application.
- Step 4: Click the `Files` tab in qFlipper.
- Step 5: Navigate to `SD Card/apps/GPIO` folder (or whatever category your FAP is).
- Step 6: Drag the FAP file from your computer to the qFlipper window.

### Copying FAP using SD Card

If you are using a browser that does not support the Web Serial API, you will need to manually copy the FAP file to your Flipper Zero.

<img src="images/eject-sd.gif" width=50%/>

- Step 1: Download the FAP file to your computer.
- Step 2: Navigate to `Settings`/`Storage`/`Unmount SD Card`/`Unmount`.
- Step 3: Eject the SD Card from your Flipper Zero & insert it into your computer.

<img src="images/copy-sd.gif" width=50%/>

- Step 4: Drag the file into `/apps/GPIO` folder (or whatever category your FAP is).
- Step 5: Eject the SD Card from your computer & insert it back into your Flipper Zero.

### Building with VSCode (Visual Studio Code) 

### Building with FBT

### Building with UFBT

## Running the application

<img src="images/running-app.gif" width=50%/>

- Step 1: Make sure you have followed the step to [install the application](#installing-apps).
- Step 2: On the Flipper Zero, navigate to the `Apps` menu.
- Step 3: Select the type of application (like GPIO) you want to run.
- Step 4: Select the application you want to run.

## Support

If you have any questions, please contact us on Discord at https://discord.com/invite/NsjCvqwPAd