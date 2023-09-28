# flipper-xremote
Advanced IR Remote App for Flipper Device 

### Currently under development, additional details coming soon..

## Idea
Currently, to use the buttons in the infrared application, you have to find all the buttons one by one in the menu, scroll to the button you want, and then click it. This makes using the remote a little uncomfortable. The idea behind XRemote is that all physical buttons are pre-mapped to specific category buttons, and pressing a physical button directly sends an infrared signal. This allows the flipper device to be used as a remote rather than as a tool that has a remote.

The learning approach of XRemote is also different from the current application. Now, in order to clone the remote, you have to write the names of all the buttons one by one with a flipper, which is very inconvenient. The idea of XRemote is that it will tell you in advance which button it is going to record, and you only have to press the remote, so you will no longer have to name all the buttons.

The application works with standard `.ir` files, but for the buttons to work, the names in the file must match the predefined names of XRemote. Check out [views/remote_common_view.h](https://github.com/kala13x/flipper-xremote/blob/main/views/xremote_common_view.h) file for supported name definitions.

## Process

- [ ] Learn new remote
- [ ] Use saved remote
    - [x] Saved remote general category
    - [ ] Saved remote control category
    - [x] Saved remote navigation category
    - [x] Saved remote player category
    - [ ] Saved remote custom category
    - [ ] Saved remote full button list
    - [ ] Delete saved remote file
    - [ ] Edit saved remote file
- [ ] Application settings
    - [ ] Vertical/Horizontal setting
    - [ ] Command repeat setting

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
