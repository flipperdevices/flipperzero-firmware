# Flipper Zero Meal Pager Tool

## What this is?
This app triggers restaurant pagers in a brute force manner, useful to test if devices are still functional. 
<br><br>

## Supported Pagers
- Retekess T119
- Retekess TD157 (coming soon)
- Retekess TD165 (coming soon)
- Retekess TD174 (coming soon)

### Features
- Select range of stations (needs improving for full range)
- Select range of pagers (needs improving for full range)

## How to install on Flipper Zero
- If you do not have one, download a firmware onto your PC via git<br>
- Plug your Flipper Zero in via USB. <br>
- Copy the contents of this folder into the applications_user folder of your firmware. <br> 

## What does the blinking mean

### Yellow Blinking LED
Means that the SubGhz Code is being generated for the configured range

### Purple Blinking LED
Means that SubGhz Signals are being sent

## Can this Brute-Force Attacks
This is a Proof-of-Concept. In Theory it could, but I wouldn't recommend trying it. Its annoying for people targeted and it could get you into trouble. Seriously, don't be that person, nobody will like your for it. 
Appart from that, most pagers support 8191 Stations. Triggering ~30 Pagers per station would easily take a long time. That when only sending each trigger once. It is recommended is to repeat the signal approx 10x (currently not possible, but possible that stations don't react to a single call), so that would already take all day. 
Chances are also, that your Flipper Zero will crash in that time.

## Does this even work
I don't know. It's based on intel collected from other people. The Flipper sends data, I checked that with a second flipper. But if the data actually triggers something is not sure. 

Then run the command: 
 ```
.\fbt launch APPSRC=applications_user/meal_pager
 ```
The application will be compiled and copied onto your device. 

## Thank you notes
- [Moeker](https://github.com/moeker) and his awesome [pagger tool](https://github.com/meoker/pagger), couldn't have done this without
- [xb8](https://github.com/xb8/t119bruteforcer) for the useful data collected
- [Xenobyte, ShotokanZH](https://twitter.com/xenobyte_/status/1558123251276070912) for their super interesting research