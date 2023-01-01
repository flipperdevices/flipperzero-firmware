# magspoof_flipper
Very early WIP of MagSpoof for the Flipper Zero. Currently rewriting from the ground up.

Interpolates work from Samy Kamkar's original MagSpoof project, dunaevai135's Flipper hackathon project, and the Flipper team's LF RFID app.

Courses of action to try in the event the LF coil signal is too weak:
- Attempt downstream modulation techniques, in addition to upstream, like the LF RFID worker does when writing
- Introduce a subcarrier at ~125kHz, and OOK modulate it at the desired freq of bits (~4kHz)
- Implement using the timer system, rather than direct-writing to pins
- Use the NFC (HF RFID) coil instead of or in addition to the LF coil (this is promising in my mind; Samsung Wallet's discontinued magstripe emulation would've been over their NFC coil, most likely)
- Scrap all this and stick to using an external module for TX (could likely simplify to just a resistor and some coiled wire, rather than the full H-bridge build)

Other misc things to investigate / build:
- File format, manual add, saving / loading
- Ideal timing / speed
- Precomputing bit output, and then sending ("RedSpoof" by devBioS does this, as they say they had timing issues when computing the bits live)
- Reverse-track emulate?
- Tuning of parameters like pre-signal zeros?
- "Interpacket delay" like the RedSpoof implementation?
- (Less important) Any way to easily wrap text on screen, without having to manually calculate the number of chars that fit and splicing the string accordingly into lines?


HF coil notes:
~~NFC reader field can be turned on / off with `furi_hal_nfc_field_on();` and `furi_hal_nfc_field_off();` respectively, as seen in nfc_scene_field.c (used for debug purposes). Initial tests with `furi_hal_nfc_field_on();` are promising signal-wise, but the delay introduced by the wake/sleep initialization renders it impossible to toggle rapidly. At a lower level, that consists of `furi_hal_nfc_exit_sleep();` and `st25r3916TxRxOn();` to turn on, and `st25r3916TxRxOff();` and `furi_hal_nfc_start_sleep();` to turn off. May be worth trying directly (wake from sleep at setup, toggle on and off corresponding with bit direction, send to sleep on exit). Initial tests have been difficult to get work as some of the st25r3916 symbols are unresolved; need to figure out how to import/call it properly, or how to get another layer lower of control.~~ Testing with furi_hal_nfc_ll_rxtx_on(); and furi_hal_nfc_ll_rxtx_off(); does indeed create a nice strong signal on my 'scope (thanks @dlz#7721 for finding the wrapped functions), but no response from a mag reader; makes sense, was a long shot -- next step NFC testing would be lower-level control that lets us pull the coil high/low, rather than just producing the standard 13.56MHz signal and OOK modulating it.

