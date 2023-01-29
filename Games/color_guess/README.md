# Flipper Zero Color Guessing Game
 <div style="text-align:center"><img src="assets/flipper_logo_orange.png"/></div>

## What this is?
As a web developer I enjoy guessing colours by HEX Code. This game is targeted at other Devs and graphic designers<br>
that also enjoy this. 
<br><br>
This app is still in development, currently all it does is allow you to set the color for the LED using a HEX code

### Mode 1
The LED will display a color and you must try and guess it by adjusting the HEX values on the screen. A timer will show<br>
how fast you were. 
(currently in development)

### Mode 2
You can define a color using the HEX code on-screen and the LED will display this color


## How to install on Flipper Zero
- If you do not have one, download a firmware<br>
- Plug your Flipper Zero in via USB. <br>
- Copy the contents of this folder into the applications_user folder of your firmware. <br> 

Then run the command: 
 ```
.\fbt launch_app APPSRC=applications_user/color_guess
 ```
The application will be compiled and copied onto your device. 

## Licensing
This code is open-source and may be used for whatever you want to do with it. 