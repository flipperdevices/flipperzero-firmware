# Flipper Zero LED Pulse Animations

## What this is?
This app is a proof-of-concept to show abilities of the LED light integrated in the Flipper Zero device. 
<br><br>
By default the Flipper Zero firmware framework offers various controls to the LED, including<br>
- Selection of few basic colors (red, green, blue, yellow, magenta, cyan etc)<br>
- Very limited Selection of color intensity (mainly 0 and 255)<br>
- Blinking sequences per color (mainly fast and slow)
<br>
The Goal of this application is to show the potential the LED light could have. Just like the pixels of a screen, the appearance of the LED<br>
is controlled by the intensity settings (0-255) of the three basic colors (red, green, blue). In total this offers more than 16 million possible<br>
colors in appearance. <br>
<br>

## What this does
The application allows you to play a pulse animation in the three basic colors red, green and blue, as well as a combined colors yellow and white. 

## How to install on Flipper Zero
- If you do not have one, download a firmware<br>
- Plug your Flipper Zero in via USB. <br>
- Copy the contents of this folder into the applications_user folder of your firmware. <br> 

Then run the command: 
 ```
.\fbt launch_app APPSRC=applications_user/ledpulser
 ```
The application will be compiled and copied onto your device. 

## Licensing
This code is open-source and may be used for whatever you want to do with it. 

## Source
[https://github.com/leedave/Leeds-Flipper-Zero-Applications]