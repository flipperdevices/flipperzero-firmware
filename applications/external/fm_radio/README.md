# FM Radio - Flipper Zero
Application for flipperzero device to control radio boards.

## Supported boards
* [TEA5767](https://www.sparkfun.com/datasheets/Wireless/General/TEA5767.pdf)

## PIN
 - VCC = 3V3, Pin 9
 - GND = GND, Pin 18
 - SCL = C0, Pin 16
 - SDA = C1, Pin 15

## Controls
Up = Preset Up
Down = Preset Down
Left = Seek Down
Right = Seek Up
OK = Toggle Mute

## References
TEA5767 Datasheet - (https://www.sparkfun.com/datasheets/Wireless/General/TEA5767.pdf)


## TO-DO
Add User Settings Menu: 
-Standby/Mute On Exit: True/False
-RSSI Seek Strenght: 1-15
-Sound: Mono-L/Mono-R/Stereo
-User Editable Station List w/Names (Display on "Listen Now Page")
-Listen Now Page Causes Feedback (GetRadio Info call to update RSSI Stregth is the cause)
-Add the Preset Up/Down Icon.
-Redirect TEA5767 Audio To a GPIO and Playback on Flipper Internal Speaker
