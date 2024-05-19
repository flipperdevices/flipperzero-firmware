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
- Send a Range of Signals
- Select range of stations
- Select range of pagers

## How to install on Flipper Zero
- If you do not have one, download a firmware onto your PC via git<br>
- Plug your Flipper Zero in via USB. <br>
- Copy the contents of this folder into the applications_user folder of your firmware. <br> 

## What does the blinking mean

### Yellow Blinking LED
Means that the SubGhz Code is being generated for the configured range

### Purple Blinking LED
Means that SubGhz Signals are being sent

## Settings Explanation

### Pager Type
Sets the current Pager type your targeting. Each model uses a different encoding, you cannot trigger multiple models at once

### First Station / Last Station
Each Station has a unique id. This is important so many stations can run in one location without conflicting each other. Use these settings to define a range of stations to trigger

I do not recommend a lager range than 10

### Fist Pager / Last Pager
The range of numbers on the pagers to be triggered. Most stations don't have many pagers, so a range of 0 - 31 probably is enough for all.

### Signal Repeat
How many times a single pager trigger is sent. Usually a signal is sent multiple times to combat radio noise that can cause a signal not to be recognised. 
This is the total number of signals, so a setting of 0 will not send anything. More signals take longer, less have a higher risk of no effect. Set a number between 1 and 10. 


## Can this Brute-Force Attacks
This is a Proof-of-Concept. In Theory it could, but I wouldn't recommend trying it. Its annoying for people targeted and it could get you into trouble. Seriously, don't be that person, nobody will like your for it. 
Appart from that, most pagers support 8191 Stations. Triggering ~30 Pagers per station would easily take a long time. That when only sending each trigger once. It is recommended is to repeat the signal approx 10x, so that would already take all day. 
Also your Flipper Zero will crash in that time, as the generated signals use RAM which is limited in the device. There may be ways to work around this, but I think its better to have such a limitation.

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