# magspoof_flipper
WIP of MagSpoof for the Flipper Zero. Currently rewriting from the ground up; basic TX of saved files should now work over both RFID (using the Flipper's internal coil) and GPIO (pins A6 and A7: such that one can connect an H-bridge and external coil). A sample file with test data is included in `assets`, for anyone wishing to experiment. Using this README as coarse notes of what remains to be done; anyone is welcome to contribute!

Disclaimer: use responsibly, and at your own risk. While in my testing, I've seen no reason to believe this could damage the RFID hardware, this is inherently driving the coil in ways it was not designed or intended for; I take no responsibility for fried/bricked Flippers. Similarly, please only use this with magstripe cards and mag readers you own — this is solely meant as a proof of concept for educational purposes. I neither condone nor am sympathetic to malicious uses of my code.

## TODO
Emulation:
- Fix signal truncation issue! *Edit: Tentative fix in place*
- General code cleanup
- Reverse track precompute & replay
- Prefix/between/suffix addition to config menu
- Parameter tuning, find best defaults, troubleshoot improperly parsed TX
- Implement/integrate better bitmap than hacky first pass? Boilerplate from [antirez](https://github.com/antirez)'s better approach (from [ProtoView](https://github.com/antirez/protoview)) included at the bottom of `helpers/mag_helpers.c`
- Should the main timing-sensitive section be branchless? (Remove `if` and `switch` statements from the `FURI_CRITICAL...` section of `mag_spoof()`?)
- Pursue skunkworks TX improvement ideas listed below

Scenes:
- Finish emulation config scene (reverse track functionality; possibly expand settings list to include prefix/between/suffix options)
- "Edit" scene (generalize "Add manually")
- "Rename" scene (generalize input_name)

File management:
- Validation of card track data?
- Parsing loaded files into human-readable fields (would we need to specify card type to decode correctly?)
- Update Add Manually flow to reflect new file format (currently only sets Track 2)

Known bugs:
- From debug logging output, seems precomputed signal is getting truncated somehow! This is priority \#1 to fix. *Edit: Tentative fix in place*
- Custom text input scene with expanded characterset (Add Manually) has odd behavior when navigating the keys near the numpad
- File format issues when Track 2 data exists but Track 1 is left empty; doesn't seem to be setting the Track 2 field with anything (doesn't overwrite existing data). However, `flipper_format_read_string()` doesn't seem to return `false`. Is the bug in my code, or with `flipper_format`?
- Attempting to play a track that doesn't have data results in a crash (as one might expect). Need to lock out users from selecting empty tracks in the config menu or do better error handling

## Skunkworks ideas
Internal TX improvements:
- Attempt downstream modulation techniques, in addition to upstream, like the LF RFID worker does when writing, for stronger signal
- Implement using the timer system, rather than direct-writing to pins
- Use the NFC (HF RFID) coil instead of or in addition to the LF coil (likely unfruitful from initial tests; we can enable/disable the oscillating field, but even with transparent mode to the ST25R3916, it seems we don't get low-enough-level control to pull it high/low correctly) 

External RX options:
- UART-connected mag reader (bulky, but likely easiest to read over GPIO, and means one can read all tracks)
- Square audio jack mag reader (this may be DOA; seems like newer versions of the Square modules have some form of preprocessing that also modifies the signal, perhaps in an effort to discourage folks using their hardware independent of their software. Thanks [@arha](https://github.com/arha) for your work investigating this)
- Some read-head directly connected to GPIO, ADC'd, and parsed all on the Flipper. Likely the most compact and cheapest module option, but also would require the most work.
- USB HID input likely infeasible; seems the FZ cannot act as an HID host.

----
## Credits
This project interpolates work from [Samy Kamkar's original MagSpoof project](https://github.com/samyk/magspoof), [dunaevai135 & AlexYaro's Flipper hackathon project](https://github.com/dunaevai135/flipperzero-firmware), and the Flipper team's [LF RFID](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/applications/main/lfrfid) and [SubGhz](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/applications/main/subghz) apps.  

Many thanks to everyone who has helped in addition to those above, most notably: 
- [antirez](https://github.com/antirez) for bitmapping suggestions and general C wisdom
- [skotopes](https://github.com/skotopes) for RFID consultation
- [NVX](https://github.com/nvx) + dlz for NFC consultation
- davethepirate + [arha](https://github.com/arha) for EE insight and acting as sounding boards
- [cool4uma](https://github.com/cool4uma) for their work on custom text_input scenes 
- Everyone else I've had the pleasure of chatting with!
