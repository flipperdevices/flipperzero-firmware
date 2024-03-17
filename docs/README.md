# flipper-xremote

Advanced IR Remote App for Flipper Device

## Idea

Navigation to the menu to press each button individually can be often uncomfortable because it requires scrolling to the desired button and selecting it. The idea behind XRemote is that all physical buttons are pre-mapped to specific category buttons, and a physical button directly sends an infrared signal. This allows the flipper device to be used as a remote rather than as a tool that has a remote.

## Learn new remote

XRemote also introduces a more user-friendly learning approach. Instead of having to manually name each button on the flipper when cloning a remote, the learning tool informs you upfront which buttons it will record. All you need to do is press the corresponding button on your existing remote, eliminating the need to name them individually.

## Custom Layout

To customize your layout, open the saved remote file, select Edit in the menu, and configure which infrared commands should be transmitted when physical buttons are pressed or held. These changes will be stored in the existing remote file, which means that the configuration of custom buttons can be different for all remotes.

## Standard file support

The application is compatible with standard .ir files. However, to ensure functionality, names within these files must align with the predefined naming scheme. If the button is not highlighted when pressed or the notification LED does not light up, the button with the appropriate name cannot be found in the file.

## Button schema

Button name | Description
------------|-------------------
Power       | Power
Setup       | Setup/Settings
Input       | Input/Source
Menu        | Menu
List        | List
Info        | Info
Mode        | Mode
Back        | Back
Ok          | Enter/Ok
Up          | Up
Down        | Down
Left        | Left
Right       | Right
Mute        | Mute
Vol_up      | Volume up
Vol_dn      | Volume down
Ch_next     | Next channel
Ch_prev     | Previous channel
Next        | Jump forward
Prev        | Jump backward
Fast_fo     | Fast forward
Fast_ba     | Fast backward
Play_pa     | Play/Pause
Pause       | Pause
Play        | Play
Stop        | Stop

## Alternative button names

In addition to the predefined names, XRemote uses alternative button names to make it as easy as possible to interact with different types of IR dumps. That means if a button with the appropriate name is not found in the file, the application will try to find the same button with alternative names. Ensure this feature is enabled in the application settings before you use it.

The application stores and reads alternate names from the following file:
SD_Card/apps_data/flipper_xremote/alt_names.txt

If the Alt-Names option is enabled in the config and the file does not exist, it will be created automatically with default values during the application's startup. You can edit, add, or remove any button or alternate name values from this file. Button names must either have only the first uppercase or be entirely lowercase. As for alternate names, they are case-insensitive. The button can have one or several comma-separated alternate names.

## Default alternative names

Button name | Alternative names
------------|-------------------
Power       | shutdown,off,on,standby
Setup       | settings,config,cfg
Input       | source,select
Menu        | osd,gui
List        | guide
Info        | display
Mode        | aspect,format
Back        | return,exit
Ok          | enter,select
Up          | uparrow
Down        | downarrow
Left        | leftarrow
Right       | rightarrow
Mute        | silence,silent,unmute
Vol_up      | vol+,volume+,volup,+
Vol_dn      | vol-,volume-,voldown,-
Ch_next     | ch+,channel+,chup
Ch_prev     | ch-,channel-,chdown
Next        | next,skip,ffwd
Prev        | prev,back,rewind,rew
Fast_fo     | fastfwd,fastforward,ff
Fast_ba     | fastback,fastrewind,fb
Play_pa     | playpause,play,pause