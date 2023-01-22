# magspoof_flipper
WIP of MagSpoof for the Flipper Zero. Currently rewriting from the ground up; basic TX of saved files should now work over both RFID (using the Flipper's internal coil) and GPIO (pins A6 and A7: such that one can connect an H-bridge and external coil). A sample file with test data is included in `assets`, for anyone wishing to experiment. Using this README as coarse notes of what remains to be done; anyone is welcome to contribute!

Disclaimer: use responsibly, and at your own risk. While in my testing, I've seen no reason to believe this could damage the RFID hardware, this is inherently driving the coil in ways it was not designed or intended for; I take no responsibility for fried/bricked Flippers. Similarly, please only use this with magstripe cards and mag readers you own — this is solely meant as a proof of concept for educational purposes, and I neither condone nor am sympathetic to malicious uses of my code.

## TODO
Emulation:
- Clean up old testing scenes, remove deprecated helpers
- Reverse track precomputation
- Does the main timing-sensitive section need to be branchless? (Remove `if`s from the `FURI_CRITICAL...` section of `mag_spoof()`?)
- Implement/integrate better bitmap than hacky first pass? antirez's better approach (from ProtoView) included at bottom of `helpers/mag_helpers.c`
- Pursue skunkworks TX improvement ideas listed below

Scenes:
- Complete emulation config scene (include reverse track functionality; possibly expand settings list to include prefix/between/suffix options)
- Improved saved info display (better text wrapping options? remove and just include that info on the emulate scene? decode data to fields?)
- Edit saved card scene

File management:
- Parsing loaded files into relevent fields (would we need to specify card type as well, to decode correctly?)
- Modify manual add scene to allow editing and renaming of existing files
- Validation of card track data?
- Update Add Manually flow to reflect new file format (currently only sets Track 2, and Info/Emulate scene only displays Track 2)

Known bugs:
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

----
## Credits
This project interpolates work from [Samy Kamkar's original MagSpoof project](https://github.com/samyk/magspoof), [dunaevai135's Flipper hackathon project](https://github.com/dunaevai135/flipperzero-firmware), and the Flipper team's [LF RFID](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/applications/main/lfrfid) and [SubGhz](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/applications/main/subghz) apps.  

Many thanks to everyone who has helped in addition to those above, most notably: 
- [antirez](https://github.com/antirez) for bitmapping suggestions and general C wisdom
- [skotopes](https://github.com/skotopes) for RFID consultation
- [NVX](https://github.com/nvx) + dlz for NFC consultation
- davethepirate + [arha](https://github.com/arha) for EE insight and acting as sounding boards
- [cool4uma](https://github.com/cool4uma) for their work on custom text_input scenes 
- Everyone else I've had the pleasure of chatting with!
