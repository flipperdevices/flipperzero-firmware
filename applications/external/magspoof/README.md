# magspoof_flipper by Zachary Weiss
WIP of MagSpoof for the Flipper Zero. Basic TX of saved files confirmed working against an MSR90 with an external H-bridge module mirroring Samy Kamkar's design. RFID coil output weaker; able to be picked up/detected by more compact mag readers such as Square, but yet to have success with it being decoded/parsed properly. Additional investigation into alternate internal TX options (CC1101, ST25R3916, piezo) underway; tentatively, RFID coil + speaker (`LF + P` config setting) results in the strongest internal TX tested to date but still weaker than a dedicated external module or an actual card swipe (and sounds like a dial-up modem from hell). Sample files with test data are included in `assets` for anyone wishing to experiment.

Disclaimer: use responsibly, and at your own risk. While in my testing, I've seen no reason to believe this could damage the RFID (or other) hardware, this is inherently driving the coil in ways it was not designed or intended for; I take no responsibility for fried/bricked Flippers. Similarly, please only use this with magstripe cards and mag readers you own — this is solely meant as a proof of concept for educational purposes. I neither condone nor am sympathetic to malicious uses of my code.

## Hummus's Fork
I made this fork initially to add reading capability using UART magnetic card readers.

Things that changed in this fork:
- Added a basic card reading ability
- Added a function to parse a new MagDevice from a Card String (%Track1?;Track2;Track3?;)
- Swapped the pins between A6 to A7 on the card that I'm using, might add this to configuration scene later on
- Adapted some of the APIs to the most recent firmware changes.

## Optional GPIO TX Module
For those desiring better TX than the internal RFID coil can offer, one can build the module below, consisting of an H-bridge, a capacitor, and a coil.

<img src="https://user-images.githubusercontent.com/20050953/215654078-1f4b370e-21b3-4324-b63c-3bbbc643120e.png" alt="Wiring diagram" title="Wiring diagram" style="height:320px">


## TODO
Known bugs:
- [X] File format issues when Track 2 data exists but Track 1 is left empty; doesn't seem to be setting the Track 2 field with anything (doesn't overwrite existing data). However, `flipper_format_read_string()` doesn't seem to return `false`. Is the bug in my code, or with `flipper_format`?
  - [X] Review how it's done in [unirfremix (Sub-GHz Remote)](https://github.com/DarkFlippers/unleashed-firmware/blob/dev/applications/main/unirfremix/unirfremix_app.c), as IIRC that can handle empty keys, despite using the `flipper_format` lib for parsing.
- [X] Attempting to play a track that doesn't have data results in a crash (as one might expect). Need to lock out users from selecting empty tracks in the config menu or do better error handling (*Doesn't crash now, but still should probably prevent users from being able to select*)
- [ ] Custom text input scene with expanded characterset (Add Manually) has odd behavior when navigating the keys near the numpad

Emulation:
- [X] Validate arha's bitmap changes, transition over to it fully
- [X] Test piezo TX (prelim tests promising)
- [ ] General code cleanup
- [X] Reverse track precompute & replay
- [ ] Parameter tuning, find best defaults, troubleshoot improperly parsed TX
- [ ] Pursue skunkworks TX improvement ideas listed below
- [ ] Remove or reimplement interpacket 
  - [ ] Verify `furi_delay_us` aliasing to `64us`

Scenes:
- [X] Finish emulation config scene (reverse track functionality; possibly expand settings list to include prefix/between/suffix options)
- [ ] "Edit" scene (generalize `input_value`)
- [ ] "Rename" scene (generalize `input_name`)

File management:
- [ ] Update Add Manually flow to reflect new file format (currently only sets Track 2)
- [ ] Validation of card track data?
- [ ] Parsing loaded files into human-readable fields? (would we need to specify card type to decode correctly?)

## Skunkworks ideas
Internal TX improvements:
- [ ] Attempt downstream modulation techniques in addition to upstream, like the LF RFID worker does when writing.
- [ ] Implement using the timer system, rather than direct-writing to pins
- [X] Use the NFC (HF RFID) coil instead of or in addition to the LF coil (likely unfruitful from initial tests; we can enable/disable the oscillating field, but even with transparent mode to the ST25R3916, it seems we don't get low-enough-level control to pull it high/low correctly) 
- [ ] Add "subcarriers" to each half-bit transmitted (wiggle the pin high and low rapidly)
  - [ ] Piezo subcarrier tests
  - [ ] LF subcarrier tests
  - [X] Retry NFC oscillating field? 

External RX options:
1. [TTL / PS/2 mag reader connected to UART](https://www.alibaba.com/product-detail/Mini-portable-12-3-tracks-usb_60679900708.html) (bulky, harder to source, but likely easiest to read over GPIO, and means one can read all tracks)
2. Square audio jack mag reader (this may be DOA; seems like newer versions of the Square modules have some form of preprocessing that also modifies the signal, perhaps in an effort to discourage folks using their hardware independent of their software. Thanks arha for your work investigating this)
3. Some [read-head](https://www.alibaba.com/product-detail/POS-1-2-3-triple-track_60677205741.html) directly connected to GPIO, ADC'd, and parsed all on the Flipper. Likely the most compact and cheapest module option, but also would require some signal-processing cleverness.
4. USB HID input over pre-existing USB C port infeasible; seems the FZ cannot act as an HID host (MCU is the STM32WB55RGV6TR).
5. Custom USB HID host hat based on the [MAX3421E](https://www.analog.com/en/products/max3421e.html) (USB Host Controller w/ SPI), like the [Arduino USB Host Shield](https://docs.arduino.cc/retired/shields/arduino-usb-host-shield). Would be a large but worthwhile project in its own right, and would let one connect any USB HID reader they desire (or other HID devices for other projects). Suggestion credit to arha.
6. Implement a software/firmware USB host solution over GPIO like [esp32_usb_soft_host (for ESP32)](https://github.com/sdima1357/esp32_usb_soft_host) or [V-USB (for AVR)](https://www.obdev.at/products/vusb/index.html). Suggestion credit to arha. Also a massive undertaking, but valuable in and of itself.

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
This project interpolates work from [Samy Kamkar](https://github.com/samyk/)'s [original MagSpoof project](https://github.com/samyk/magspoof), [Alexey D. (dunaevai135)](https://github.com/dunaevai135/) & [Alexandr Yaroshevich](https://github.com/AYaro)'s [Flipper hackathon project](https://github.com/dunaevai135/flipperzero-firmware/tree/dev/applications/magspoof), and the [Flipper team](https://github.com/flipperdevices)'s [LF RFID](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/applications/main/lfrfid) and [SubGhz](https://github.com/flipperdevices/flipperzero-firmware/tree/dev/applications/main/subghz) apps.  

Many thanks to everyone who has helped in addition to those above, most notably: 
- [arha](https://github.com/arha) for bitmapping work, skunkworks testing, and innumerable suggestions/ideas/feedback (now a collaborator!)
- [Zalán Kórósi (Z4urce)](https://github.com/Z4urce) for an earlier app icon
- [Salvatore Sanfilippo (antirez)](https://github.com/antirez) for bitmapping suggestions and general C wisdom
- [skotopes](https://github.com/skotopes) for RFID consultation
- [Tiernan (NVX)](https://github.com/nvx) + dlz for NFC consultation
- davethepirate for EE insight and acting as a sounding board
- [cool4uma](https://github.com/cool4uma) for their work on custom text_input scenes 
- Everyone else I've had the pleasure of chatting with!
