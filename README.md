# magspoof_flipper
WIP of MagSpoof for the Flipper Zero. Basic TX of saved files confirmed working against an MSR90 with an external H-bridge module mirroring Samy Kamkar's design. RFID coil output weaker; able to be picked up/detected by more compact mag readers such as Square, but yet to have success with it being decoded/parsed properly. Additional WIP investigation into alternate internal TX options (CC1101, ST25R3916, piezo) underway, courtesy of arha. Sample files with test data are included in `assets` for anyone wishing to experiment.

Disclaimer: use responsibly, and at your own risk. While in my testing, I've seen no reason to believe this could damage the RFID hardware, this is inherently driving the coil in ways it was not designed or intended for; I take no responsibility for fried/bricked Flippers. Similarly, please only use this with magstripe cards and mag readers you own — this is solely meant as a proof of concept for educational purposes. I neither condone nor am sympathetic to malicious uses of my code.

## Optional GPIO TX Module
For those desiring better TX than the internal RFID coil can offer, one can build the module below, consisting of an H-bridge, a capacitor, and a coil.

<img src="https://user-images.githubusercontent.com/20050953/215654078-1f4b370e-21b3-4324-b63c-3bbbc643120e.png" alt="Wiring diagram" title="Wiring diagram" style="height:320px">


## TODO
Known bugs:
- [ ] File format issues when Track 2 data exists but Track 1 is left empty; doesn't seem to be setting the Track 2 field with anything (doesn't overwrite existing data). However, `flipper_format_read_string()` doesn't seem to return `false`. Is the bug in my code, or with `flipper_format`?
- [ ] Attempting to play a track that doesn't have data results in a crash (as one might expect). Need to lock out users from selecting empty tracks in the config menu or do better error handling
- [ ] Custom text input scene with expanded characterset (Add Manually) has odd behavior when navigating the keys near the numpad

Emulation:
- [ ] Validate arha's bitmap changes, transition over to it fully
- [ ] General code cleanup
- [ ] Reverse track precompute & replay (should be simple with new bitmap approach; just iterate through bytes backwards, bits forwards?)
- [ ] Prefix/between/suffix addition to config menu
- [ ] Parameter tuning, find best defaults, troubleshoot improperly parsed TX
- [ ] Pursue skunkworks TX improvement ideas listed below

Scenes:
- [ ] Finish emulation config scene (reverse track functionality; possibly expand settings list to include prefix/between/suffix options)
- [ ] "Edit" scene (generalize "Add manually")
- [ ] "Rename" scene (generalize input_name)

File management:
- [ ] Update Add Manually flow to reflect new file format (currently only sets Track 2)
- [ ] Validation of card track data?
- [ ] Parsing loaded files into human-readable fields? (would we need to specify card type to decode correctly?)

## Skunkworks ideas
Internal TX improvements:
- [ ] Attempt downstream modulation techniques, in addition to upstream, like the LF RFID worker does when writing, for stronger signal
- [ ] Implement using the timer system, rather than direct-writing to pins
- [ ] Use the NFC (HF RFID) coil instead of or in addition to the LF coil (likely unfruitful from initial tests; we can enable/disable the oscillating field, but even with transparent mode to the ST25R3916, it seems we don't get low-enough-level control to pull it high/low correctly) 

External RX options:
1. UART-connected mag reader (bulky, but likely easiest to read over GPIO, and means one can read all tracks)
2. Square audio jack mag reader (this may be DOA; seems like newer versions of the Square modules have some form of preprocessing that also modifies the signal, perhaps in an effort to discourage folks using their hardware independent of their software. Thanks [@arha](https://github.com/arha) for your work investigating this)
3. Some read-head directly connected to GPIO, ADC'd, and parsed all on the Flipper. Likely the most compact and cheapest module option, but also would require the most work.
4. USB HID input likely infeasible; seems the FZ cannot act as an HID host.

## arha todo & notes
Attempting to exploit flipper hardware to some extent

- [X] Preprocess all MSR data into bitwise arrays, including manchester encoding. 
- [ ] Feed bits from timers
- [ ] Sync to the lfrfid timer and experiment representing a field flip with a few cycles of a high frequency carrier, like the 125khz lfrfid one. Perhaps mag readers' frontends will lowpass such signals, and keep only the low frequency component, in an attempt to drown out nearby noise?
- [X] Can the CC1101 radio be used in any way? Driving it from GD0 can achieve 50us, or about 10khz. Probably more with sync/packet mode. **Currently under testing**. The signal is extra noisy with a very wide bandwidth, but, in theory, it can work
- [ ] Can the 5V pin act as a coil driver? I've read reports it can drive 0.4A, other reports it can drive 2A. It boils down to bq25896 being fast enough. Ref: bq25896_enable_otg, which will probably need bypassing kernel libs and calling furi_hal_i2c_tx/furi_hal_i2c_tx whatever calls from Cube libs.
- [ ] Investigate transparent mode on 3916
- [ ] Can the piezo be used at its resonant frequency? I've seen LF signals being emulated with [nothing but headphones](https://github.com/smre/DCF77/blob/master/DCF77.py#L124) running a subharmonic; the wheel brake on some carts seems to react to audiofreq signals (or the RF emission from driving a speaker)

----
## Credits
This project interpolates work from [Samy Kamkar's original MagSpoof project](https://github.com/samyk/magspoof), [dunaevai135 & AlexYaro's Flipper hackathon project](https://github.com/dunaevai135/flipperzero-firmware), and the Flipper team's [LF RFID](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/applications/main/lfrfid) and [SubGhz](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/applications/main/subghz) apps.  

Many thanks to everyone who has helped in addition to those above, most notably: 
- [arha](https://github.com/arha) for bitmapping work and skunkworks testing (now a collaborator!)
- [Z4urce](https://github.com/Z4urce) for the provisional app icon
- [antirez](https://github.com/antirez) for bitmapping suggestions and general C wisdom
- [skotopes](https://github.com/skotopes) for RFID consultation
- [NVX](https://github.com/nvx) + dlz for NFC consultation
- davethepirate for EE insight and acting as a sounding board
- [cool4uma](https://github.com/cool4uma) for their work on custom text_input scenes 
- Everyone else I've had the pleasure of chatting with!
