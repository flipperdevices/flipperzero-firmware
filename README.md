# Flipboard
<img src="./images/banner.png">

## Starting out
- [Overview](#overview)
- [Applications](#applications)
- [Installing Apps](#installing-apps)
  - [Official Store](#official-store)
  - [Pre-installed on some firmware](#pre-installed-on-some-firmware)
  - [Building with VSCode (Visual Studio Code)](#building-with-vscode-visual-studio-code)
  - [Building with FBT](#building-with-fbt)
  - [Building with UFBT](#building-with-ufbt)
- [Running the application](#running-the-application)
- [Support](#support)

## Overview

The FlipBoard is a versatile device for the Flipper Zero that can enable all kinds of features. It has 4 buttons that connect to the Flipper Zero via separate GPIO pins, so you can react to any combinations of button up/down events. Each button has a 24-bit color LED that is programmable using the WS2812b protocol. There is also a small single-color LED on the board you can use, for example, to signal a status change.

The FlipBoard applications are written in C and compiled into FAP (Flipper Application Packages).  The GPIO and LED drivers can also be used in JavaScript applications that run on the Flipper Zero!

You can order your FlipBoard from [@MakeItHackin](https://www.youtube.com/makeithackin) using [tindie](https://www.tindie.com/products/32844/) or [Etsy](https://www.etsy.com/listing/1601295558/).  Be sure to also join our [Discord server](https://discord.gg/KTThkQHj5B).

## Applications
Currently there are four applications written for the FlipBoard that demonstrate many of the key features:

- [FlipKeyboard](./flipkeyboard/README.md) - A MacroPad that types using USB or BLE.  Also a music keyboard.
- [FlipBlinky](./flipblinky/README.md) - Turn the FlipBoard into blinky badge.
- [FlipSignal](./flipsignal/README.md) - Send IR and Sub-GHz signals.
- Simon memory game
  - [Tutorial](./simon-tutorial/README.md) step-by-step instructions to make your own game!
  - [Completed game](./simon-tutorial/completed/step-16/flipsimon/README.md) version of the game that is ready to play.

## Installing Apps

Here are some options for installation:
- [Official Store](#official-store) (coming soon?)
- [Pre-installed on some firmware](#pre-installed-on-some-firmware) (like RogueMaster)
- [Visual Studio Code](#building-with-vscode-visual-studio-code)
- [Build FAP with FBT tool](#building-with-fbt)
- [Build FAP with uFBT tool](#building-with-ufbt)

### Official Store

We hope to have the FlipBoard applications available in the official Flipper Zero store soon.  This will make it easy to install the applications on your Flipper Zero without needing to build the applications yourself.

### Pre-installed on some firmware

Currently the RogueMaster firmware has the FlipBoard applications pre-installed.  This is a great option if you want to try out the applications without building them yourself.

### Building with VSCode (Visual Studio Code) 

This is the recommended configuration if you are going to be editing the existing applications or writing your own.  You will get code completion and be able to step into the firmware code, finding out how a specific function works.

**Prerequisites:**

Most of the prerequisites links have a default "Download" button on the landing page (look for a button that says `Download` or has a download icon).  For each of the installers, after you accept the license agreement, you can just click "Next" through the rest of the install process.

- [Install Python](https://www.python.org/downloads/)
- [Install GIT tools](https://git-scm.com/downloads)
- [Install VSCode](https://code.visualstudio.com/download)

Configure VSCode:
- Step 1: Make sure you have installed the above prerequisites.
- Step 2: Create a folder called `repos` in your home directory.  You can use any folder you want, but this tutorial assumes you are using `/repos` folder.  If you are using Windows, you can use `C:\repos` or `C:\Users\yourname\repos` or any other folder you want.
- Step 3: Open VSCode.
- Step 4: Clone the Flipboard repository.
  - Step 4a: `View`/`Command Palette`/`Git: Clone`
  - Step 4b: Enter the URL of the flipboard repository `https://github.com/jamisonderek/flipboard.git` then click **Clone from URL** and choose your `/repos` folder as the target.  This will create a folder called `flipboard` in your `repos` folder.
  - Step 4c: When prompted if you would like to open the repository, choose `Open`.
  - Step 4d: When prompted if you trust the authors, choose `Yes` (or choose `No` if you prefer.)
- Step 5: **Recursively clone** the firmware repository.   Be sure you choose `Git: Clone (Recursive)` from the `Command Palette` menu.  This is different than the `Git: Clone` command you ran in the previous step!
  - Step 5a: `View`/`Command Palette`/`Git: Clone (Recursive)`
  - Step 5b: Enter the URL of the firmware you would like to run on your Flipper and choose your `/repos` folder as the target.
    - Official: `https://github.com/flipperdevices/flipperzero-firmware.git`
    - Momentum: `https://github.com/Next-Flip/Momentum-Firmware.git`
    - Unleashed: `https://github.com/DarkFlippers/unleashed-firmware.git`
    - RogueMaster: `https://github.com/RogueMaster/flipperzero-firmware-wPlugins.git`
    - Xtreme: `https://github.com/Flipper-XFW/Xtreme-Firmware.git`
  - NOTE: THIS IS RECURSIVELY CLONING THE REPOSITORY.  This may take a few minutes to complete, depending on the speed of your internet connection.
  - Step 5c: When prompted if you would like to open the repository, choose `Open`.
  - Step 5d: When prompted if you trust the authors, choose `Yes` (so you will be able to build the code later.)
  - You may get a warning about "13 submodules".  This is normal, you can safely ignore this warning.
- Step 6: Open GIT tools
  - Step 6a: Open the GIT CMD window (or any new command window should be fine.)
  - Step 6b: Type `git` and press enter.
  - Step 6c: If you get a message that says `git is not recognized as an internal or external command`, then you need to add the git tools to your path.
- Step 7: Install VSCode tools
  - Step 7a: Close VSCode and reopen it.
  - Step 7b: In VSCode, make sure the Explorer window is open (`View`/`Explorer`).
  - Step 7c: Right click on `fbt` and choose `Open in Integrated Terminal`.  A new command terminal should appear at the bottom right of the screen.
  - Step 7d: Type `./fbt vscode_dist` in the Integrated Terminal and press enter.
  - Step 7e: If prompted if you would like to install the recommended extensions, choose `Install`.
  - Step 7f: Close VSCode and reopen it.
  - Step 7g: If prompted if you would like to install the recommended extensions, choose `Install`.
  - Step 7h: Be sure the extensions `CMAKE` and `CMAKE TOOLS` are disabled for the workspace, otherwise Ctrl+Shift+B may not show the build options (and instead show things about `cmake`)!
    - Step 7h1: Click on the `Extensions` icon on the left side of the screen.
    - Step 7h2: Type `cmake` in the search box.
    - Step 7h3: Click on the `CMAKE` extension.
    - Step 7h4: If it has an `Install` button you are done.
    - Step 7h5: If it has a `Disable` button, click on the `Disable` button.
- Step 7: Build and deploy
  - Step 7a: Connect your Flipper to the computer.
  - Step 7b: Press `Ctrl+Shift+B` to bring up the build options.
  - Step 7c: Choose `[Debug] Flash (USB, with resources)`.
  - Step 7d: The firmware and FAPs should get built.
  - Step 7e: The firmware should get installed on the Flipper Zero.
- Step 8: Copy the applications from the flipboard repository to the firmware repository's `application_user` folder.
  - Step 8a: Copy the folder `flipblinky` from `/repos/flipboard/flipblinky` to `./applications_user/flipblinky`.
  - Step 8b: Copy the folder `flipsignal` from `/repos/flipboard/flipsignal` to `./applications_user/flipsignal`.
  - Step 8c: Copy the folder `flipkeyboard` from `/repos/flipboard/flipkeyboard` to `./applications_user/flipkeyboard`.
  - Step 8d: Copy the folder `flipsimon` from `/repos/flipboard/simon-tutorial/completed/step-16/flipsimon` to `./applications_user/flipsimon`.

Build and deploy the application:
  - Step 1: Connect your Flipper to the computer.
  - Step 2: In VSCode, make sure the Explorer window is open (`View`/`Explorer`).
  - Step 2: Open the file `./applications_user/flipblinky/app.c` (or whichever application you want to build)
  - Step 3: Press `Ctrl+Shift+B` to bring up the build options (or `Terminal`/`Run Task...`).
  - Step 4: Choose `[Debug] Launch App on Flipper`.
  - Step 5: The application should get built, the FAP installed on the Flipper Zero, and then the application should get launched.

### Building with FBT

If you prefer a different editor than VSCode, using FBT may be a good option.  FBT is a command line tool that can build the firmware and applications.  This is the same tool used by VSCode.

Prerequisites:
- [Install Python](https://www.python.org/downloads/)
- [Install GIT tools](https://git-scm.com/downloads)

Configure FBT:
- Step 1: Make sure you have installed the above prerequisites.
- Step 2: Create a folder called `repos` in your home directory.  You can use any folder you want, but this tutorial assumes you are using `/repos` folder.
- Step 3: Clone the Flipboard repository.
  - Step 3a: Open a command prompt.
  - Step 3b: Change directory to your `/repos` folder (`cd /repros`).
  - Step 3c: Clone the flipboard repository `git clone https://github.com/jamisonderek/flipboard.git`
- Step 4: Clone the firmware repository recursively.
  - Step 4a: Open a command prompt.
  - Step 4b: Change directory to your `/repos` folder.
  - Step 4c: Clone the firmware repository you would like to run on your Flipper.
    - Official: `git clone --recursive https://github.com/flipperdevices/flipperzero-firmware.git`
    - Momentum: `git clone --recursive https://github.com/Next-Flip/Momentum-Firmware.git`
    - Unleashed: `git clone --recursive https://github.com/DarkFlippers/unleashed-firmware.git`
    - RogueMaster: `git clone --recursive https://github.com/RogueMaster/flipperzero-firmware-wPlugins.git roguemaster-firmware`
    - Xtreme: `git clone --recursive https://github.com/Flipper-XFW/Xtreme-Firmware.git`
- Step 5: Deploy the firmware
  - Step 5a: Connect your Flipper to the computer.
  - Step 5b: Open a command prompt.
  - Step 5c: Change directory to the firmware repository you cloned (e.g. `cd Momentum-Firmware`).
  - Step 5d: Run the command `./fbt FORCE=1 flash_usb_full`
- Step 6: Copy the applications from the flipboard repository to the firmware repository's `application_user` folder.
  - Step 6a: Copy the folder `flipblinky` from `/repos/flipboard/flipblinky` to `./applications_user/flipblinky`.
  - Step 6b: Copy the folder `flipsignal` from `/repos/flipboard/flipsignal` to `./applications_user/flipsignal`.
  - Step 6c: Copy the folder `flipkeyboard` from `/repos/flipboard/flipkeyboard` to `./applications_user/flipkeyboard`.

Build and deploy the application:
  - Step 1: Connect your Flipper to the computer.
  - Step 2: Open a command prompt.
  - Step 3: Change directory to the firmware repository you cloned.
  - Step 4: Run the command `./fbt launch APPSRC=./applications_user/flipblinky/app.c` (or whichever application you want to build)

### Building with uFBT
uFBT is a lightweight tool that can quickly build the application.  This method does not work with Unleashed or Xtreme firmware.  You must use FBT or VSCode for those firmwares.

Prerequisites:
- [Install Python](https://www.python.org/downloads/)
- [Install GIT tools](https://git-scm.com/downloads)

Prepare environment:
- Step 1: Make sure you have installed the above prerequisites.
- Step 2: Create a folder called `repos` in your home directory.  You can use any folder you want, but this tutorial assumes you are using `/repos` folder.
- Step 3: Clone the Flipboard repository.
  - Step 3a: Open a command prompt.
  - Step 3b: Change directory to your `/repos` folder (`cd /repros`).
  - Step 3c: Clone the flipboard repository `git clone https://github.com/jamisonderek/flipboard.git`
- Step 4: Install ubft tool.
  - Linux & macOS: `python3 -m pip install --upgrade ufbt`
  - Windows: `py -m pip install --upgrade ufbt`
- Step 5: Set environment to match your firmware.
  - Step 5a: Connect your Flipper to the computer.
  - Step 5b: Open a command prompt.
  - Step 5c: Change directory to the flipboard application. (`cd /repos/flipboard/flipblinky`)
  - Step 5d: Run the command based on your firmware:
    - Official: `ufbt update --index-url=https://update.flipperzero.one/firmware/directory.json`
    - Unleashed: `ufbt update --index-url=https://up.unleashedflip.com/directory.json`
    - Momentum: `ufbt update --index-url=https://up.momentum-fw.dev/firmware/directory.json`
- Step 6: Build and deploy the application.
  - Step 6a: Connect your Flipper to the computer.
  - Step 6b: Open a command prompt.
  - Step 6c: Change directory to the flipboard application. (`cd /repos/flipboard/flipblinky`)
  - Step 6d: Run the command `ufbt launch`.

## Support

If you have need help, we are here for you.  Also, we would love your feedback on cool ideas for future FlipBoard applications!

Please visit my Flipper Zero Discord server [Flipper Zero Tutorials (Unofficial)](https://discord.gg/KTThkQHj5B) and ask your questions in the `#flipboard` or `#general` channel.
