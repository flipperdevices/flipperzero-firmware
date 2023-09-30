# flipper-xremote
Advanced IR Remote App for Flipper Device 

### Currently under development, additional details coming soon..

## Idea
With the current infrared application, users must navigate through the menu to locate each button individually. This requires scrolling to the desired button and selecting it, which can be uncomfortable. The idea behind `XRemote` is that all physical buttons are pre-mapped to specific category buttons, and pressing a physical button directly sends an infrared signal. This allows the flipper device to be used as a remote rather than as a tool that has a remote.

`XRemote` also introduces a more user-friendly learning approach. Instead of having to manually name each button on the flipper when cloning a remote, the learning tool informs you upfront which buttons it will record. All you need to do is press the corresponding button on your existing remote, eliminating the need to name them individually.

The application is compatible with standard `.ir` files. However, to ensure functionality, button names within these files must align with the predefined naming scheme.

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
- [ ] Learn new remote
- [ ] IR signal analyzer
- [x] Use saved remote
    - [x] General button page
    - [x] Control buttons page
    - [x] Navigation buttons page
    - [x] Player buttons page
    - [ ] Custom buttons page
    - [ ] Full button list
    - [ ] Edit remote file
    - [ ] Delete remote file
- [x] Application settings
    - [x] Load settings from the file
    - [x] Store settings to the file
    - [x] Vertical/Horizontal menu
    - [x] IR command repeat count
    - [x] GUI to change settings
    - [ ] Return button configuration

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

<p align="center">
Saved remote control apps
</p>
<p align="center">
    <img src="https://github.com/kala13x/flipper-xremote/blob/main/screens/saved_remote_apps.png" alt="XRemote main menu">
</p>
