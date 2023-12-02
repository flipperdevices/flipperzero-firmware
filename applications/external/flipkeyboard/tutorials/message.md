# Tutorial: Editing the Message that is typed.

The message is the text that gets sent to the computer when FlipBoard buttons are pressed.  This is an easy way to type a bunch of text.

## Entering a Message using the Flipper Zero

![Message Keyboard](./message-keyboard.png)

Entering a message is done via `button_config->text_input` which uses `#include <gui/modules/text_input.h>` for the on-screen keyboard module.  This TextInput module is fairly restricted on the characters it exposes. This means, that while you can enter any message you would like, the TextInput keyboard may not expose the characters you want to type.

You can hold a button down to invert the case, but no symbols or other characters are exposed. 

In a future tutorial, we can create our own keyboard module, with an interface matching text_input.h but exposing most of the characters supported by the font.

## Editing the Message contents

In this tutorial, we will edit the Message contents using a text editor on our computer.

You will recall from [file-format](./file-format.md) that the `SD Card/apps_data/flipboard/keyboard.txt` file is a simple text file with a list of key-value pairs, as shown below:
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
```

- Step 1. Download this file to your computer using qFlipper.

- Step 2. Scroll down to the `ButtonId` matching the button you want to edit.  Remember the 3rd button has a `ButtonId` of 4, and the 4th button has a `ButtonId` of 8.

- Step 3. Edit the `Message:` key with the value you would like to type, such as `cls¶¶¶dir g:¶`.  You to enter any content you want.  

  - **NOTE:** The font used on the Flipper Zero does not have all of the characters, so once you edit the Message, you may not be able to properly view or edit it using the Flipper Zero.

- Step 4. The [Pilcrow](https://en.wikipedia.org/wiki/Pilcrow) `¶` character is used to represent pressing the `Enter` key, so you can use it to insert newlines into your message.  We use this special character because the name-value pairs in the file are separated by newlines, so we can't use a newline to represent a newline.

- Step 5. Save the file.

- Step 6. Upload the updated file to your Flipper Zero, dragging the file back onto the qFlipper window.