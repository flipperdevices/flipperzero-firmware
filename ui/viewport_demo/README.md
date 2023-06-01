# viewport demo
This is a UI demo of the ViewPort class.  It is based off the wiki code for the [ViewPort] https://github.com/jamisonderek/flipper-zero-tutorials/wiki/User-Interface#viewport section of the User-Interface page.

The draw callback does some basic text drawing on a [Canvas](https://github.com/jamisonderek/flipper-zero-tutorials/wiki/User-Interface#canvas).

The input callback changes the X and Y position of a cursor "^" based on the key that is pressed.  You can see that when the ViewPort orientation is rotated, both the screen and the keypress is translated automatically.  Pressing the back button uses a [Message Queue](https://github.com/jamisonderek/flipper-zero-tutorials/wiki/Message-Queue) to signal that application should exit.
