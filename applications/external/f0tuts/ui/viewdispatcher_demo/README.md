# viewdispatcher demo
This is a UI demo of the ViewDispatcher class.  It is based off the wiki code for the [ViewDispatcher] (https://github.com/jamisonderek/flipper-zero-tutorials/wiki/User-Interface#viewdispatcher) section of the User-Interface page.

The two [View](https://github.com/jamisonderek/flipper-zero-tutorials/wiki/User-Interface#view) objects registers draw and input callbacks and set different screen orientations.

The draw callback does some basic text drawing on a [Canvas](https://github.com/jamisonderek/flipper-zero-tutorials/wiki/User-Interface#canvas).

The input callback changes the X and Y position of a cursor "^" based on the key that is pressed.  You can see that when the ViewPort orientation is rotated, both the screen and the keypress is translated automatically.  Pressing the back button uses a [Message Queue](https://github.com/jamisonderek/flipper-zero-tutorials/wiki/Message-Queue) to signal that application should exit.  Pressing OK button switches the [View](https://github.com/jamisonderek/flipper-zero-tutorials/wiki/User-Interface#view).
