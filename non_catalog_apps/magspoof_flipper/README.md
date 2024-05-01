# magspoof_flipper
WIP of MagSpoof for the Flipper Zero. Basic TX of saved files confirmed working against an MSR90 with an external H-bridge module mirroring Samy Kamkar's design. Sample files are included in `resources`.

 RFID coil output weaker; able to be picked up/detected by more compact mag readers such as Square, but yet to have success with it being decoded/parsed properly. Additional investigation was made into alternate internal TX options (CC1101, ST25R3916, piezo); tentatively, RFID coil + speaker (`LF + P` config setting) results in the strongest internal TX tested to date but still weaker than a dedicated external module or an actual card swipe (and sounds like a dial-up modem from hell). For information on the state of internal TX &/or misc TODOs, known bugs, etc, confer `NOTES.md`.

**Disclaimer**: use responsibly, and at your own risk. ***I neither condone nor am sympathetic to malicious uses of my code.***  Please only use this with magstripe cards and mag readers you own — this is solely meant as a proof of concept for educational purposes. Similarly, if using internal TX: while in my testing, I've seen no reason to believe this could damage the RFID (or other) hardware, this is inherently driving the coil in ways it was not designed or intended for; I take no responsibility for fried/bricked Flippers.


## GPIO TX Module
For those desiring better TX than the internal RFID coil can offer, one can build the module below, consisting of an H-bridge, a capacitor, and a coil. Custom GPIO pin selection is a planned future feature.

<img src="https://user-images.githubusercontent.com/20050953/215654078-1f4b370e-21b3-4324-b63c-3bbbc643120e.png" alt="Wiring diagram" title="Wiring diagram" style="height:320px">


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
- [hummusec](https://github.com/hummusec) for testing of UART RX
- [xMasterX](https://github.com/xMasterX) and [WillyJL](https://github.com/Willy-JL) for keeping the app updated across API changes while I was away!
- Everyone else I've had the pleasure of chatting with!
