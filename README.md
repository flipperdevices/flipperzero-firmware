# magspoof_flipper
Very early WIP of MagSpoof for the Flipper Zero. Currently rewriting from the ground up.

Interpolates work from Samy Kamkar's original MagSpoof project, dunaevai135's Flipper hackathon project, and the Flipper team's LF RFID app.

Courses of action to try in the event the LF coil signal is too weak:
- Attempt downstream modulation techniques, in addition to upstream, like the LF RFID worker does when writing
- Introduce a subcarrier at ~125kHz, and OOK modulate it at the desired freq of bits (~4kHz)
- Implement using the timer system, rather than direct-writing to pins
- Use the NFC (HF RFID) coil instead or in addition to the LF coil
- Scrap all this and stick to using an external module for TX (could likely simplify to just a resistor and some coiled wire, rather than the full H-bridge build)