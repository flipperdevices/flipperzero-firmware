# flipper-xremote
Advanced IR Remote App for Flipper Device 

## About
Navigation to the menu to press each button individually can be often uncomfortable because it requires scrolling to the desired button and selecting it. The idea behind `XRemote` is that all physical buttons are pre-mapped to specific category buttons, and a physical button directly sends an infrared signal. This allows the flipper device to be used as a remote rather than as a tool that has a remote.

`XRemote` also introduces a more user-friendly learning approach. Instead of having to manually name each button on the flipper when cloning a remote, the learning tool informs you upfront which buttons it will record. All you need to do is press the corresponding button on your existing remote, eliminating the need to name them individually.

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

## Progress

- [x] Application menu
- [x] Learn new remote
- [x] Signal analyzer
- [x] Use saved remote
    - [x] General button page
    - [x] Control buttons page
    - [x] Navigation buttons page
    - [x] Player buttons page
    - [ ] Custom buttons page
    - [ ] Full button list
    - [ ] Rename remote file
    - [ ] Delete remote file
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
