# Genie garage door recorder

## Description
This program was written to allow the Flipper Zero to press buttons on a Genie garage door opened and record the rolling code. The goal is to capture all 65,536 signals (which hopefully repeats when it gets to the end).  Our click speed is current 2000ms per click + however long it takes to get the signal.  So if we assume it's 1000-1500/hr = about 3 days?

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