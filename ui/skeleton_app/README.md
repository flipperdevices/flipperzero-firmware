# skeleton_app
You can use this application as a starting point for creating your own application.  It contains the skeleton framework to get an application running on the Flipper Zero.

Please let me know any feedback! 
- Discord - https://discord.com/invite/NsjCvqwPAd  (@CodeAllNight)
- YouTube - https://youtube.com/@MrDerekJamison/playlists
- GitHub - https://github.com/jamisonderek/flipper-zero-tutorials
- Wiki - https://github.com/jamisonderek/flipper-zero-tutorials/wiki

## Overview

This application has three submenu items:
* Config
* Play
* About

## Config
The "Config" menu item currently has 2 settings.  The first setting has a selection of 3 options (and demonstrates how the numeric values associated with them don't have to be sequential.)  The other setting has a text field which is set by clicking OK while selecting the setting (depending on your application, you may want to do something else when an item is clicked, like bring up a hex selector.)

## Play
The "Play" screen is where you would put your primary application.  The current implementation renders some data.  When Left/Right buttons are clicked the value of x changes and the icon moves left/right.  Up/Down buttons don't do anything in our implementation.  As soon as the OK button is pressed, a tone is made based on the value of x.  Pressing the back button goes back to the menu.

## About

The "About" menu item contains information about your application, so people know what to do with it & how to contact you.

## Modifying the project

### application.fam

Set the `appid` to a unique value for your application. This should be lowercase letters, numbers, and underscores. Your icons header file will be impacted by this change (in app.c).

Set the `name` to the name to dispay in the UI when selecting an application to load.

Set the `entry_point` to the name of the method in your app.c file, that should get executed when your application is selected to be run. This method should take a (void\*) input parameter and return a uint32_t status code.

For most applications a 4K `stack_size` should be a good default.

Make sure `fap_icon` is the name of the PNG file with your 10x10 pixel icon that should be displayed when selecting your application.

The `fap_category` is the category that your application is mostly related to. If your primary purpose is not GPIO then change this category. This will determine what folder it is in when starting, as well as where people will locate your application in the Hub mobile app.

### app.png

This is a 10x10 PNG icon to display when selecting your application. If you want to rename this file to something other than `app.png`, be sure to also edit the application.fam `fap_icon` value.

### app.c

#### Icons file

Replace `my_app_id_42_icons.h` with your `appid`\_icons.h (based on the you set in application.fam)

#### TAG value

Change the #define TAG to be the name of the application to appear in log messages.

#### BACKLIGHT_ALWAYS_ON

Remove the `#define BACKLIGHT_ALWAYS_ON` statement if you don't want the backlight to automatically turn off after some duration.

#### MyApp and my_app

Replace all occurances of `MyApp` and `my_app_` with the name of your application.

#### FlipTheWorld

Replace `"Play"` with the action you want for your main applicaiton.

#### "This is a sample application"

Replace the about text in your application. Search for `This is a sample application`. You can use '\n' to force a newline.

#### my_app_view_draw_callback

Change the code in `..._view_draw_callback` to draw the primary screen for your application.

#### my_app_view_input_callback

Change the code in `..._view_input_callback` to handle user input in the primary screen for your application.

#### setting_1_values

Make sure the number of items in `setting_1_values` and `setting_1_names` is the same length. Change these settings to whatever values makes sense for your user to configure. You may also want to change `"Setting 1"` to be a more descriptive name for the setting and perhaps update the variable names as well.
