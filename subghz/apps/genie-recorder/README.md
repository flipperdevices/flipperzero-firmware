# Genie garage door recorder

## Description
This program was written to allow the Flipper Zero to press buttons on a Genie garage door opened and record the rolling code. The goal is to capture all 65,536 signals (which hopefully repeats when it gets to the end).  Our click speed is current 2000ms per click + however long it takes to get the signal.  So if we assume it's 1000-1500/hr = about 3 days?

## Connecting to remote
<img src="wiring.png">
You only need to connect two wires between the Flipper and the remote.  One wire is GND on the Flipper. It should connect to the pin that is the same as the battery negative pad on the remote.  The second wire is the signal wire, it should connect to A7 on the Flipper Zero.  On the remote side, it should connect to a pin that transmits the signal.  If you remove the wires from the Flipper Zero and touch the two wires together, the remote should send a signal.
<p/><p/>
In my testing, a new CR2032 battery was able to send all 65,536 codes & still had power left over! IF YOU WANT TO USE THE FLIPPER 3V3 FOR A POWER SOURCE, THEN YOU MUST MAKE SURE THAT GND ON THE FLIPPER IS GOING TO GND ON THE REMOTE (and not the signal pin).  IN YOU ARE 100% SURE, THEN YOU CAN REMOVE THE BATTERY FROM YOUR REMOTE AND CONNECT 3V3 PIN ON THE FLIPPER TO THE BATTERY POSITIVE BAR ON THE REMOTE.
<p/><p/>
WARNING -- For my remote, the codes wrapped after 65,536 codes were sent.  I'm not sure if this is the case for all remotes.  If it doesn't wrap, it's possible that the remote could stop working (if the manufacture implemented OVR bits).
<p/><p/>
WARNING -- This could desync your remote from the receiver.
<p/><p/>
WARNING -- Don't run this near your garage.  There is no reason to open the physical garage door & you will likely burn out the motor.

## Running
Step 1. Deploy custom firmware (keeloq.c with te_short=200, te_long=400, te_delta=70)
Step 2. Connect a genie garage door opener to pins A7 and GND.
Step 3. Edit FREQUENCY if it is not broadcasting at 315MHz when button is held down.
Step 4. Run application
Step 5. Choose "Start" to start recording signals.
- You should see the current broadcast count (how many times the button was pressed)
- You should see the received signal count (how many times the signal was received)
- You should see the most recent Keeloq code received
Step 6. Let it run for a few weeks (the goal is to capture 65,536+ signals)
Step 7. Press the BACK button twice to exit the application.
Step 8. Copy the file "\apps_data\genie\keys.txt" from the SD card to your computer.