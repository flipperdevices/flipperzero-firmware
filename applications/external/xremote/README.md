<p align="center">Advanced IR Remote App for Flipper Device</p>
<p align="center">
    <img src="https://github.com/kala13x/flipper-xremote/blob/main/screens/flipperzero.png" alt="XRemote">
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


## Installation options

1. Install the latest stable version directly from the [application catalog](https://lab.flipper.net/apps/flipper_xremote).

2. Manually install using `.fap` file:  
   - Download the `.fap` file from the [Releases](https://github.com/kala13x/flipper-xremote/releases) section of this repository.
   - Write the `.fap` file to an SD card using [qFlipper](https://docs.flipper.net/qflipper) or any your preferred SD card writer.

## Build options

1. If you already have the flipper zero firmware cloned on the Linux:
   - Connect your Flipper device to your computer using a USB cable.
   - Use deploy script from this repository to build and run the application on the device:

    ```bash
    ./deploy.sh --fw=/path/to/the/firmware
    ```
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
  - [ ] Add or remove button
  - [ ] All buttons page
- [x] Application settings
  - [x] GUI to change settings
  - [x] Load settings from the file
  - [x] Store settings to the file
  - [x] Vertical/horizontal views
  - [x] IR command repeat count
  - [x] Exit button behavior

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
