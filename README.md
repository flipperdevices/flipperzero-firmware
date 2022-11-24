![Image](assets/dialer.jpg)

## DTMF Dolphin

DTMF (Dual-Tone Multi-Frequency) dialer, Bluebox, and Redbox.

Now in a release-ready state for both Dialer, Bluebox, and Redbox (US/UK) functionality!

Please note that using the current tone output method, the 2600 tone is scaled about 33 Hz higher than it should be. This is a limitation of the current sample rate.

This is for educational/entertainment purposes only. While the code generates sine waves, the buzzer on the Flipper Zero uses PWM which interferes with the cleanness of the signal. While the tones generated sound fairly accurate, they may not be detected by DTMF applications. This is a hardware limitation.

### Educational Links:

* http://www.phrack.org/issues/25/7.html#article
* https://en.wikipedia.org/wiki/Dual-tone_multi-frequency_signaling
* https://en.wikipedia.org/wiki/Blue_box
* https://en.wikipedia.org/wiki/Red_box_(phreaking)
