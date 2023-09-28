# flipper-xremote
Advanced IR Remote App for Flipper Device 

### Currently under development, additional details coming soon..

## Idea
With the current infrared application, users must navigate through the menu to locate each button individually. This requires scrolling to the desired button and selecting it, which can be uncomfortable. The idea behind XRemote is that all physical buttons are pre-mapped to specific category buttons, and pressing a physical button directly sends an infrared signal. This allows the flipper device to be used as a remote rather than as a tool that has a remote.

`XRemote` also introduces a more user-friendly learning approach. Instead of having to manually name each button on the flipper when cloning a remote, the learn tool informs you upfront which buttons it will record. All you need to do is press the corresponding button on your existing remote, eliminating the need to name them individually.

The application is compatible with standard `.ir` files. However, to ensure functionality, button names within these files must align with predefined names. For a list of supported name definitions, please refer to the [views/remote_common_view.h](https://github.com/kala13x/flipper-xremote/blob/main/views/xremote_common_view.h) file.

## Progress

- [x] Application menu
- [ ] Learn new remote
- [ ] Use saved remote
    - [x] Saved remote general button page 
    - [ ] Saved remote control buttons page
    - [x] Saved remote navigation buttons page
    - [x] Saved remote player buttons page
    - [ ] Saved remote custom buttons page
    - [ ] Saved remote full button list
    - [ ] Delete saved remote file
    - [ ] Edit saved remote file
- [ ] Application settings
    - [ ] Vertical/Horizontal menu setting
    - [ ] Infrared command repeat count

## Screens

<p align="center">
Main menu
</p>
<p align="center">
    <img src="https://github.com/kala13x/flipper-xremote/blob/main/screens/app_menu.png" alt="XRemote main menu">
</p>

<p align="center">
Saved remote menu and control
</p>
<p align="center">
    <img src="https://github.com/kala13x/flipper-xremote/blob/main/screens/control.png" alt="XRemote main menu">
</p>

<p align="center">
Saved remote menu and playback
</p>
<p align="center">
    <img src="https://github.com/kala13x/flipper-xremote/blob/main/screens/playback.png" alt="XRemote main menu">
</p>
