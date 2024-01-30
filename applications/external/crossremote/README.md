# Flipper Zero Cross Remote

## Current State
- Infrared working
- SubGhz working
- Pause working
- IR Timing features in development

## What this is?
This app combines commands used in IR and SubGhz into playlists that can be run with one click
<br><br>

### What good is this?
Imagine you want to sit down and watch a movie after a long days work. <br>
Your probably do something like the following<br>
- Turn on your TV with the IR TV remote<br>
- Turn on your Bluray player with the Blueray IR remote<br>
- Turn on your surround sound with the speaker IR remote<br>
- Turn on your ceiling fan using a subGhz remote<br>
- Turn on your AC using another remote<br>
- etc<br>
<br>
Wouldn't it be nicer to simply click one button and let everything happen? This is what this app wants to do. <br>

### Features
- Select commands from saved subGhz transmissions
- Select commands registered with the IR App (choose from the created remote buttons)<br>
- Chain these commands using an easy UI<br>
- Save chained commands to a file<br>
- Add pauses, becaue target systems are not always fast enough for multiple commands<br>
- Run file containing chained IR & SubGhz commands<br>

### Limitations
SubGhz commands will stop working if you move/rename/delete the original files on your Flipper. This is because 
of how the Flippers SubGhz worker expects data. 

## How to install on Flipper Zero
- If you do not have one, download a firmware<br>
- Plug your Flipper Zero in via USB. <br>
- Copy the contents of this folder into the applications_user folder of your firmware. <br> 

Then run the command: 
 ```
.\fbt launch_app APPSRC=applications_user/xremote
 ```
The application will be compiled and copied onto your device. 

## Licensing
This code is open-source and may be used for whatever you want to do with it. 