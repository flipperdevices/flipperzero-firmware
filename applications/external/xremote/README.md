<p align="center">Advanced IR Remote App for Flipper Device</p>

<p align="center">
  Version 1.3 - <a href="https://github.com/kala13x/flipper-xremote/blob/main/docs/changelog.md">Changelog</a>
</p>

<p align="center">
    <img src="https://github.com/kala13x/flipper-xremote/blob/main/.flipcorg/banner.png" alt="XRemote">
</p>

## Idea

Navigation to the menu to press each button individually can be often uncomfortable because it requires scrolling to the desired button and selecting it. The idea behind `XRemote` is that all physical buttons are pre-mapped to specific category buttons, and a physical button directly sends an infrared signal. This allows the flipper device to be used as a remote rather than as a tool that has a remote.

## Learn new remote

`XRemote` also introduces a more user-friendly learning approach. Instead of having to manually name each button on the flipper when cloning a remote, the learning tool informs you upfront which buttons it will record. All you need to do is press the corresponding button on your existing remote, eliminating the need to name them individually.

## Custom Layout

To customize your layout, open the saved remote file, select `Edit` in the menu, and configure which infrared commands should be transmitted when physical buttons are pressed or held. These changes will be stored in the existing remote file, which means that the configuration of custom buttons can be different for all remotes.

<table align="center">
    <tr>
        <td align="center">Edit custom page buttons</td>
    </tr>
    <tr>
        <td><img src="https://github.com/kala13x/flipper-xremote/blob/main/screens/custom_layout.png" alt="XRemote edit layout"></td>
    </tr>
</table>

## Standard file support

The application is compatible with standard `.ir` files. However, to ensure functionality, names within these files must align with the predefined naming scheme. If the button is not highlighted when pressed or the notification LED does not light up, the button with the appropriate name cannot be found in the file.

Button name | Description
------------|-------------------
`Power`     | Power
`Eject`     | Eject
`Setup`     | Setup/Settings
`Input`     | Input/Source
`Menu`      | Menu
`List`      | List
`Info`      | Info
`Mode`      | Mode
`Back`      | Back
`Ok`        | Enter/Ok
`Up`        | Up
`Down`      | Down
`Left`      | Left
`Right`     | Right
`Mute`      | Mute
`Vol_up`    | Volume up
`Vol_dn`    | Volume down
`Ch_next`   | Next channel
`Ch_prev`   | Previous channel
`Next`      | Jump forward
`Prev`      | Jump backward
`Fast_fo`   | Fast forward
`Fast_ba`   | Fast backward
`Play_pa`   | Play/Pause
`Pause`     | Pause
`Play`      | Play
`Stop`      | Stop

## Alternative button names
In addition to the predefined names, `XRemote` uses alternative button names to make it as easy as possible to interact with different types of IR dumps. That means if a button with the appropriate name is not found in the file, the application will try to find the same button with alternative names. Ensure this feature is enabled in the application settings before you use it.

The application stores and reads alternate names from the following file:
```
SD Card/apps_data/flipper_xremote/alt_names.txt
```

If the `Alt-Names` option is enabled in the config and the file does not exist, it will be created automatically with default values during the application's startup. You can edit, add, or remove any button or alternate name values from this file. Button names must either have only the first uppercase or be entirely lowercase. As for alternate names, they are case-insensitive. The button can have one or several comma-separated alternate names.

This is the default `alt_names.txt` file:

```
Filetype: XRemote Alt-Names
Version: 1
# 
Power: shutdown,off,on,standby
Setup: settings,config,cfg
Input: source,select
Menu: osd,gui
List: guide
Info: display
Mode: aspect,format
Back: return,exit
Ok: enter,select
Up: uparrow
Down: downarrow
Left: leftarrow
Right: rightarrow
Mute: silence,silent,unmute
Vol_up: vol+,volume+,volup,+
Vol_dn: vol-,volume-,voldown,-
Ch_next: ch+,channel+,chup
Ch_prev: ch-,channel-,chdown
Next: next,skip,ffwd
Prev: prev,back,rewind,rew
Fast_fo: fastfwd,fastforward,ff
Fast_ba: fastback,fastrewind,fb
Play_pa: playpause,play,pause
```

## Installation options

1. Install the latest stable version directly from the official [application catalog](https://lab.flipper.net/apps/flipper_xremote).
2. Install on any firmware from the community driven application catalog [FlipC](https://flipc.org/kala13x/flipper-xremote).
3. Manually install using `.fap` file:  
   - Download the `.fap` file from the [Releases](https://github.com/kala13x/flipper-xremote/releases) section of this repository.
   - Write the `.fap` file to an SD card using [qFlipper](https://docs.flipper.net/qflipper) or any your preferred SD card writer.

## Build options

1. If you already have the flipper zero firmware cloned on the Linux:
   - Connect your Flipper device to your computer using a USB cable.
   - Use deploy script from this repository to build and run the application on the device:

    ```bash
    ./deploy.sh --fw=/path/to/the/firmware -b -l
    ```

    Do not use `-l` (link) option of you are building the project directly from the `applications_user` directory of the firmware.
2. If you don't have the firmware or the Linux please refer to the [official documentation](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/documentation/AppsOnSDCard.md) for build instructions.

## Progress

- [x] Application menu
- [x] Learn new remote
- [x] Signal analyzer
- [x] Use saved remote
  - [x] General button page
  - [x] Control buttons page
  - [x] Navigation buttons page
  - [x] Player buttons page
  - [x] Custom buttons page
  - [x] Edit custom layout
  - [x] Alternative button names
  - [ ] Add or remove button
  - [ ] All buttons page
- [x] Application settings
  - [x] GUI to change settings
  - [x] Load settings from the file
  - [x] Store settings to the file
  - [x] Vertical/horizontal views
  - [x] IR command repeat count
  - [x] Exit button behavior
  - [x] Enable/disable alt names

## Screens

<table align="center">
    <tr>
        <td align="center">Main menu</td>
        <td align="center">Saved remote menu</td>
    </tr>
    <tr>
        <td><img src="https://github.com/kala13x/flipper-xremote/blob/main/screens/app_menu.png" alt="XRemote main menu"></td>
        <td><img src="https://github.com/kala13x/flipper-xremote/blob/main/screens/saved_remote_menu.png" alt="XRemote saved remote menu"></td>
    </tr>
</table>

<table align="center">
    <tr>
        <td align="center">Saved remote control apps</td>
    </tr>
    <tr>
        <td><img src="https://github.com/kala13x/flipper-xremote/blob/main/screens/saved_remote_apps.png" alt="XRemote IR applicatoions"></td>
    </tr>
</table>

<table align="center">
    <tr>
        <td align="center">Learn mode</td>
        <td align="center">Received signal</td>
    </tr>
    <tr>
        <td><img src="https://github.com/kala13x/flipper-xremote/blob/main/screens/learn_mode.png" alt="XRemote learn mode"></td>
        <td><img src="https://github.com/kala13x/flipper-xremote/blob/main/screens/signal_view.png" alt="XRemote received signal"></td>
    </tr>
</table>

<table align="center">
    <tr>
        <td align="center">Settings</td>
    </tr>
    <tr>
        <td><img src="https://github.com/kala13x/flipper-xremote/blob/main/screens/settings_menu.png" alt="XRemote settings menu"></td>
    </tr>
</table>
