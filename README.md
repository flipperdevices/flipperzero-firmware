# magspoof_flipper
WIP of MagSpoof for the Flipper Zero. Currently rewriting from the ground up. Interpolates work from Samy Kamkar's original MagSpoof project, dunaevai135's Flipper hackathon project, and the Flipper team's LF RFID app.  

Many thanks to everyone who has helped in addition to those above, most notably: antirez for bitmapping suggestions, skotopes for RFID consultation, NVX + dlz for NFC consulation, davethepirate for EE insight and being a sounding board, and cool4uma for their work on custom text_input scenes — as well as everyone else I've had the pleasure of chatting with.

Using this README as coarse notes of what remains to be done; anyone is welcome to contribute!

## TODO
Emulation:
- Finish refactor from hardcoded test scene to mag_helpers (most notable change: precomputing bit output akin to devBioS's "RedSpoof" implementation of MagSpoof)
- Multi-track emulation, reverse track emulation
- Experimentation on timing and other parameters (zero prefix/between/suffix, interpacket delay, reverse vs non-reverse track, etc)
- Implement/integrate better bitmap than hacky first pass? antirez's better approach (from ProtoView) included at bottom of mag_helpers
- External TX option(s) — interface with original H-bridge design, also perhaps singular coil. Does GPIO have sufficient output for this? Need a capacitor to discharge from?
- Pursue skunkworks TX improvement ideas listed below

Scenes:
- Non-hardcoded emulation scene (using mag_helpers functions) that play loaded card data
- Emulation config scene. Be able to select between RFID / GPIO H-bridge / GPIO plain coil(?), modify timing (clock and interpacket), select track(s) to be emulated, toggle reverse track (?)
- Improved saved info display (better text wrapping options? remove and just include that info on the emulate scene? decode data to fields?)
- Edit saved card scene

File management:
- What is best way to save track data, and designate which tracks are in a file? Just use end sentinels to determine when loaded, or split it out into different fields?
- Parsing loaded files into relevent fields (would we need to specify card type as well, to decode correctly?)
- Modify manual add scene to allow editing and renaming of existing files
- Validation of card track data?
- Better cleanup / management of data during add manually

Known bugs:
- Currently there's a few functions that are unused, while the refactor is in progress. To avoid compilation errors relating to the unused functions, one must comment out `-Werror` in `site_scons/cc.scons` (or comment out the unused functions, the former is just easier/faster).
- Custom text input scene with expanded characterset (Add Manually) has odd behavior when navigating the keys near the numpad
- Track 1 data typically starts with a `%` sign. Unless escaped, it won't be displayed when printed, as C considers it a special character. To confirm: how does this impact the emulation when iterating through the chars? Does it get played correctly?

## Skunkworks ideas
Internal TX improvements:
- Attempt downstream modulation techniques, in addition to upstream, like the LF RFID worker does when writing, for stronger signal
- Implement using the timer system, rather than direct-writing to pins
- Use the NFC (HF RFID) coil instead of or in addition to the LF coil (likely unfruitful from initial tests; we can enable/disable the oscillating field, but even with transparent mode to the ST25R3916, it seems we don't get low-enough-level control to pull it high/low correctly) 

External RX options (What is simplest read module?):
- Some UART mag reader (bulky, but likely easiest to read over GPIO, and means one can read all tracks)
- Square audio jack mag reader (compact, but will be harder to decode from GPIO. Also only will read track 2 without modification)
- USB HID input feasible? Flipper seemingly can't act as an HID host, is there any way to circumvent this or is it due to a hardware incompatibility? This would be the easiest / best option all-around if feasible. 
