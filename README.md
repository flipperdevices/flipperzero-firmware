# Flipper Zero Meal Pager Tool

## What this is?
This app triggers restaurant pagers in a brute force manner, useful to test if devices are still functional. 
<br><br>

## Supported Pagers
- Retekess T119
- Retekess TD157
- Retekess TD165
- Retekess TD174

### Features
- Select range of stations
- Select range of pagers

## How to install on Flipper Zero
- If you do not have one, download a firmware<br>
- Plug your Flipper Zero in via USB. <br>
- Copy the contents of this folder into the applications_user folder of your firmware. <br> 

Then run the command: 
 ```
.\fbt launch APPSRC=applications_user/meal_pager
 ```
The application will be compiled and copied onto your device. 

## Thank you notes
- [Moeker](https://github.com/moeker) and his awesome [pagger tool](https://github.com/meoker/pagger), couldn't have done this without
- [xb8](https://github.com/xb8/t119bruteforcer) for the useful data collected
- [Xenobyte, ShotokanZH](https://twitter.com/xenobyte_/status/1558123251276070912) for their super interesting research