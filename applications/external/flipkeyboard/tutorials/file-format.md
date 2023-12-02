# File Format

All of the FlipBoard applictions save their settings file in the `SD Card/apps_data/flipboard` folder.  The file `flipboard_file.h` defines the methods to load and save the model (which is all of the configuration data for the main application).  The `flipboard_file.c` file implements these methods.  It uses the name of the model to create the file name.  

- The FlipKeyboard's `app_config.h` has `#define FLIPBOARD_APP_NAME "keyboard"` which defines the model name as 'keyboard', so the file is `SD Card/apps_data/flipboard/keyboard.txt`.
- The FlipSignal's `app_config.h` has `#define FLIPBOARD_APP_NAME "signal"` which defines the model name as `signal`, so the file is `SD Card/apps_data/flipboard/signal.txt`.
- The FlipBlinky's `app_config.h` has `#define FLIPBOARD_APP_NAME "blinky"` which defines the model name as `blinky`, so the file is `SD Card/apps_data/flipboard/blinky.txt`.

## Keyboard.txt
The settings file (`keyboard.txt`) is a simple text file with a list of key-value pairs.  When loading the settings file, each 'ButtonId' is read in and the corresponding settings are loaded.  

```
#
ButtonId: 1
Fields: 31
ColorUp: 16711680
ColorDown: 16777215
Frequency: 261.626007
Message: cls¶¶¶dir g:¶
KeystrokeCount: 0
#
ButtonId: 2
Fields: 31
ColorUp: 65280
ColorDown: 16719616
Frequency: 277.183014
Message: 
KeystrokeCount: 15
Keystrokes: 227 1 21 1 1 10 6 1 16 1 7 1 40 1 1 7 7 1 12 1 21 1 44 1 10 1 563 1 40 1
#
```

### ButtonId
The ButtonId is a value from 1-15, which is a binary expansion of the 4 buttons on the FlipBoard (1=leftmost, 2=mid-left, 4=mid-right, 8=rightmost).  These values are combined, so the leftmost+rightmost button is 9, and the mid-left+mid-right buttons is 6.

### Fields
The Fields value is a bitfield that indicates which fields are in use.

### ColorUp
The ColorUp value is the color to use when the button is not pressed.  It is a 24-bit RGB value, written in decimal.

### ColorDown
The ColorDown value is the color to use when the button is pressed.  It is a 24-bit RGB value, written in decimal.

### Frequency
The Frequency value is the frequency of the tone to play when the button is pressed.  It is a float value, written in decimal.

### Message
The Message value is the text to send to the computer when the button is pressed.  It is a string value.

### KeystrokeCount & Keystrokes
The KeystrokeCount value is the number of keystrokes to send to the computer when the button is pressed & should match the length of the Keystrokes filed.  The Keystrokes value is a list of keystrokes to send to the computer when the button is pressed.  Each keystroke is made up of two values, the first is the key_code to send for the keystroke, the second is the number of times to press it.


# Download the file
You can download the file to your computer using qFlipper, edit it using a text editor, then upload the changes to the Flipper Zero.

- Click on the Files tab
- Open `SD Card`
- Open `apps_data`
- Open `flipboard`
- Right click on the file and choose 'Download`
- Open the file with a text editor and make changes.
- Drag the file back to the `flipboard` folder to upload it back to the Flipper Zero.
