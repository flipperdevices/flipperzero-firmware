# Prerequisite
You don't need to be an engineer in embedded systems to understand this article.
But here is a short list of the requirements to follow this tutorial:
- C programming basics
- no fear about Command line interfaces
- A Flipper Zero
- Interest in embedded development

You will need to flash the Flipper Zero and this article don't aim to troubleshoot your installation especially with windows drivers.
If you have any problems with the connection between your computer and your device, please refer to the [documentation](https://docs.flipper.net/qflipper/windows-debug).

# Introduction

![](screenshoots/flipper_zero.jpeg)

You heard about the [Flipper Zero](https://flipperzero.one/) and bought it for fun.
You played with it and now you want to develop your first application to take the full advantage of the device.

The Flipper Zero is a device which target cybersecurity enjoyers.
It is a fully open-source project which is highly inspired by the [pwnagotchi](https://pwnagotchi.ai/).
You can, with some knowledge, build your own Flipper Zero from scratch.
All the [hardware schematics](https://docs.flipper.net/development/hardware/schematic), [case](https://docs.flipper.net/development/blueprints/flipper-zero) and [firmware](https://github.com/flipperdevices/flipperzero-firmware) are available.
Moreover, the community has very important part in the Flipper Zero, for example there are several [awesome lists on Github](https://github.com/djsime1/awesome-flipperzero).


## Features

The Flipper Zero is packaged with several 
- Radio signal
- RFID
- NFC
- Infrared
- GPIO
- Ibutton
- Bad USB
- U2F

## Firmware and Softwares
### Firmware

### Software

A desktop and a mobile application will help you to interact with your Flipper Zero.
The QFlipper is the desktop application which let you upgrade the firmware with a USB-C connection.
It is a pretty simple application but it also allow you to debug your applications.

The mobile application is more complete.
It has a builtin hub to install applications on your device.
You can also save your files that you registered on your Flipper Zero. 

# Tutorial
## Tools and Installations

### Requirements

A quite recent system is needed to build and flash your applications.

- Windows 10+ with PowerShell and Git
- macOS 12+ with Command Line tools
- Ubuntu 20.04+ with build-essential and Git

If you want more information, please refer to the [documentation](https://github.com/flipperdevices/flipperzero-firmware#requirements).

### Firmware

The first step is to clone the [firmware repository](https://github.com/flipperdevices/flipperzero-firmware) in order to have all the tools needed and the good version of the firmware.
The recursive flag is required because this project uses submodules.

```
git clone --recursive https://github.com/flipperdevices/flipperzero-firmware.git
cd flipperzero-firmware
```

### Visual Studio Code (Optional)

In this tutorial we will use [Visual Studio Code](https://code.visualstudio.com/)(VScode) to edit and flash your application.
The Flipper Zero has a great integration with it.

Then, to configure your VScode you have to launch this command.

```
./fbt vscode_dist
```

Now if you launch VScode in this directory, it will notify you to install all the needed plugins.

### Build and Flash
## Create your application
### Overview

In this tutorial, we will build together a simple id card application with a minimalist icon.
It is a good first application to learn how to use the GUI with some texts and images and simple inputs.
The application will display your name, your phone number and your e-mail.

All the code and resources are available on my [Github](https://github.com/alanretgue/Flipper_Zero-id_card/tree/master).

For this tutorial, I use windows because of several troubles with the `fbt` script on Ubuntu 20.04. 

### Application manifest

Let's start your project.
The first file to create is the `application.fam`.
It is a manifest file which will help the compiler to build correctly the application.
Here you can find a first minimal example:

```
App(
    appid="id_card",
    name="Id Card",
    apptype=FlipperAppType.EXTERNAL,
    entry_point="id_card_app",
    cdefines=["APP_ID_CARD"],
    requires=["gui"],
    stack_size=1 * 1024,
    fap_icon_assets="images",
    fap_icon="images/icon_10x10.png",
    fap_category="Misc",
)
```

- `appid`: Id of the application, used to compile and flash it.
- `name`: Name of the application, displayed menus.
- `apptype`: Type of the application, `EXTERNAL` specify to the compiler to make a `.fap` application.
- `entry_point`: The entry point of your application, like your `main` function in C.
- `cdefines`: C Preprocessor definitions to declare it globally.
- `requires`: Applications required by your application, specify the app ID.
- `stack_size`: Stack size allocated to your application.
- `fap_icon_assets`: Directory for your images and assets. Mandatory if you want to use images in your application.
- `fap_icon`: Icon for your application, must be 10x10 px.
- `fap_category`: Category of your application, your executable will be placed in this subdirectory.

The [Official documentation](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/AppManifests.md) can give you more information about this file.

### Source code
#### Include files
#### Main function
#### Callbacks
##### Inputs
##### GUI
# Going further
