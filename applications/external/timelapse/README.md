#Simple timelapse/intervalometer app for Flipper Zero, works via GPIO pins.

## Control:
 - Up and down - time. 
 - Left and right - number of frames 
 - Long press arrows - ±10 frames/seconds 
 - OK - start/pause 
 - Long press OK - turn on/off the backlight 
 - Back - reset 
 - Long press back - exit

1 frame - simple timer, 0 frames - infinite mode, -1 frames - BULB mode

When the timer is running, all buttons are blocked except OK.

## What you need:
  - two EL817C optocouplers
  - pin header connector 1x3 2,54mm male
  - some wire
  - heat shrink
  - camera remote connector
  
## How to assemble
Take optocouplers, connect according to the scheme: https://theageoflove.ru/uploads/2022/11/camera_cable_en.jpg
Camera pinout can be found here: https://www.doc-diy.net/photo/remote_pinout/