# Bluetooth Cookie Clicker
The goal of this application is use the Flipper Zero's bluetooth connectivity to send mouse clicks to your phone at a given frequency.  If you are playing a game, such as Cookie Clicker 2, this will result in a 'screen tap' being simulated.  Only connect to phones you own (unless your friends ask you to connect your Flipper Zero to their phone, so they can unlock more achievements in their games!)

## Easy Setup
The easiest way is to use [FlipC.org](https://flipc.org/build) to build the project and install on your Flipper.  If the application isn't already in the [Tools](https://flipc.org/category/tools) section; you can go to [Build](https://flipc.org/build) and build it yourself.

```
git:   jamisonderek/flipper-zero-tutorials
settings:
  root:  /hid/hid_cookie
```
Then click on "Let's FAP!"

Once it is built, you will be able to choose to Download the FAP or install directly on your Flipper Zero!

## Local Setup (Windows)
Setup your development machine with the following.  You can watch my video at [YouTube](https://www.youtube.com/watch?v=gqovwRkn2xw)
- VSCode
- Git
- Recursive Clone of the Flipper Zero firmware
- Clone of this tutorial project : https://github.com/jamisonderek/flipper-zero-tutorials


Once everything is cloned, copy the hid_cookie project into your applications_user folder... 
- Open the firmware project in Visual Studio Code
- Right click on "applications_user"
- Choose "Open in Integrated Terminal"
- Type:
   ```
   New-Item -Path hid_cookie -ItemType Junction -Value ..\..\flipper-zero-tutorials\hid\hid_cookie 
   ```

Once everything is copied, build and run the project...
- Open the build task window (Ctrl+Shift+B)
- Choose "Build update bundle"
- Open the "applications_user\hid_cookie\hid.c" file
- Open the build task window (Ctrl+Shift+B)
- Choose "Launch app on Flipper"

## Using the app
Once the application is launched, it will show a menu that includes directions for using the application.  Press the OK button to launch the instructions.  Press the DOWN button to read more instructions.  Press the BACK button to return to the menu.
