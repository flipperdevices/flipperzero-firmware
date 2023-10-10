# FM Radio - Flipper Zero
Application for flipperzero device to control radio boards.

<img src="https://github.com/coolshrimp/flipperzero-firmware-wPlugins/blob/420/applications/external/fm_radio/images/Screenshot1.png" width="250">
<img src="https://github.com/coolshrimp/flipperzero-firmware-wPlugins/blob/420/applications/external/fm_radio/images/Screenshot2.png" width="250">

## Supported boards
* [TEA5767](https://www.sparkfun.com/datasheets/Wireless/General/TEA5767.pdf)

## PIN
 - VCC = 3V3, Pin 9
 - GND = GND, Pin 18
 - SCL = C0, Pin 16
 - SDA = C1, Pin 15

## Controls
Up = Preset Up<br>
Down = Preset Down<br>
Left = Seek Down<br>
Right = Seek Up<br>
OK = Toggle Mute

## References
TEA5767 Datasheet - (https://www.sparkfun.com/datasheets/Wireless/General/TEA5767.pdf)


## TO-DO
Add User Settings Menu: <br>
-Standby/Mute On Exit: True/False<br>
-RSSI Seek Strenght: 1-15<br>
-Sound: Mono-L/Mono-R/Stereo<br>
-User Editable Station List w/Names (Display on "Listen Now Page")<br>
-Add the Preset Up/Down Icon.<br>
-Redirect TEA5767 Audio To a GPIO and Playback on Flipper Internal Speaker


Created By: Coolshrimp
https://CoolshrimpModz.com