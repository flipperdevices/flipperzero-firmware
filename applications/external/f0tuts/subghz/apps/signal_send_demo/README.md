# start_here
You can use this application as a starting point for creating your own application.  We updated it to send some Sub-GHz signals.

# Overview
This application has the following submenu items:
* Config
* Send Princeton
* Send Nice FLO
* Flip the world
* About

# Config
The "Config" menu item currently has 1 setting.  You can modify the menu to contain multiple settings if needed.

# Send Princeton
This sends a Princeton signal with a hard-coded SN+Button.  The app.c file sets the value with ``send_princeton(0x967AB4, freq);`` It currently is the same as this [princeton.sub](./princeton.sub) file.

# Send Nice FLO
This sends a Nice FLO signal with a hard-coded value.  The app.c file sets the value with ``send_nice_flo(0xDA1, freq);`` It currently is the same as this [nice_flo.sub](./nice_flo.sub) file.

# Flip the world
The "Flip the world" is where you would put your primary application.  It currently just renders the UI. The back button goes back to the main menu.

# About
The "About" menu item contains information about your application, so people know what to do with it & how to contact you.