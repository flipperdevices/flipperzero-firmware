# YouTube
My YouTube channel [@MrDerekJamison](https://youtube.com/@MrDerekJamison) has a bunch of videos for the Flipper Zero.

## Playlists
There are a few different playlists:
- [n00b](https://www.youtube.com/playlist?list=PLM1cyTMe-PYL8yUinRO8pKAf4DY6b4O6K) are videos about Flipper, Electronics, etc. that doesn't expect you to have any background knowledge.
- [1337](https://www.youtube.com/playlist?list=PLM1cyTMe-PYKgVdttrysDB3b37hrfL8yK) are videos that try to understand the topic.  For example, instead of just playing back a signal, we might look at the data inside the signal.
- [code](https://www.youtube.com/playlist?list=PLM1cyTMe-PYJaMQ6TWeK1mAWxORdjYJZ5) are videos for people looking to edit code in the Flipper or even write their own code.
- [demo](https://www.youtube.com/playlist?list=PLM1cyTMe-PYLwMGndLPgQQh0RNxqaMUCF) are videos with demos of using the Flipper Zero.



## Flipper Overview
- [intro video](https://youtu.be/nuRs_StGjxE) My initial Flipper Zero overview video.
- [demo video](https://youtu.be/8EuNqTr02eU) Flipper Zero ignites fireworks, replaying a "Read RAW" signal. (In the end of my overview video, I mentioned that I would make another video of the Flipper igniting fireworks.)


## BADUSB (Keyboard/Mouse emulator)
- [video](https://youtu.be/Ta7eVNS5opU) Demonstrates a firmware fix for DELETE and BACKSPACE keys.  If you have the fix applied, then you can send "CTRL-ALT DELETE" to unlock a PC.

## GPIO (General-Purpose Input/Output)
### Electronics
- [intro video](https://youtu.be/wfNIyLphR3Y) Introduction to LEDs (lights)
- [intro video](https://youtu.be/j5Jy3Wedw58) Introduction to breadboard/protoboard (for making circuit prototypes)
- [intro video](https://youtu.be/ldHCXL-D8HM) Introduction to pull-up/pull-down resistors.

### Sensors
- [demo video](https://youtu.be/iC5fBGwCPHw) Connecting lots of different sensors to Flipper Zero.
- [intro video](https://youtu.be/TCAT25kTq54) "Pins" - A tool for remembering which pins are for what.
- [1337 video](https://youtu.be/7VdVobQyRfo) Looking at UART signals from GPS module & using Logic Analyzer to decode UART. 

### GPIO
- [code video](https://youtu.be/v0PkcOtZdHg) Add Polling GPIO input to your [code](../gpio/gpio_polling_demo/README.md).
- [1337 video](https://youtu.be/cd9culIw79U) Connecting to a 7-Segment display and using the built-in Flipper Zero "GPIO Manual Control" app to display different numbers.

## Sub-GHz
### Sub-GHz ~ Overview
- [intro video](https://youtu.be/jNi-KlFTVkw) Introductory to Sub-GHz feature on Flipper.
  - Frequencies allowed
  - Frequency analyzer
  - Read
  - Read RAW
  - Save
  - Send
- [tool video](https://youtu.be/S0sgcDQrVOc) How to use a HackRF One to record and playback Sub-GHz signals from your Flipper.  This is really handy if you only have one Flipper Zero (but are doing things that typically takes two devices; like 2-player game).

### Sub-GHz ~ Rock paper scissors game
- [progress video](https://youtu.be/kP4-611YBWI) OUTDATED: Rock paper scissors game - First update on the game.
- [progress video](https://youtu.be/IzfrcORHvjA) Rock paper scissors game - Second update on the game.

### Sub-GHZ ~ Hacking Sub-GHz devices "Read"
- [video](https://youtu.be/r9pXts8KhtA) Sending different buttons by editing Flipper Zero "Read" files.

### Sub-GHZ ~ Hacking Sub-GHz devices "Read RAW"
- [intro video](https://youtu.be/ojpc7Q2fjS8) Introduction to RF signals & brief review of binary numbers.
- [signal video](https://youtu.be/7pPN2Ph8eJ0) Convert Sub-GHz "Read RAW" into CSV file.
  - one column of tone length in uS (1/1000 ms)
  - second column of silence length is uS (1/1000 ms)
- [signal video](https://youtu.be/LEyna1X_2dY) Decoding Sub-GHz "Read RAW" into binary data.
- [signal video](https://youtu.be/ihWnDep_Pfw) Hand-crafting a "Read RAW" save file.
  - Take a "Read RAW" file.
  - Convert to CSV
  - Decode into binary format
  - Edit the binary data (so we can trigger a different button press)
  - Reencode our data to a new "Read RAW" file.
- [demo video](https://youtu.be/-uG5dYobu-g) Play our hand-crafted "Read RAW" file to ignite some fuse.
  - Someone ignitied the first fuse.
  - We cpatured the signal, decoded it, changed to 3rd button, encoded it.
  - Now we play the new signal and ignite the 3rd fuse (without ever seeing the RAW signal from the remote)!!!
  - Remember: ONLY do this with devices you own!!!
- [signal video](https://youtu.be/SNiFvSeGzOQ) Looking at signals from a variety of remotes
  - Some have different preamble signatures
  - Some have fixed tone/silence lengths 
  - Some have fixed tone+silence lengths

### Sub-GHz ~ Developers adding Sub-GHz to thier applications
- [demo video](https://youtu.be/iPK7lXLIa1k) Sub-GHz demo part 1.
  - Application ["Subghz Demo"](../subghz/plugins/subghz_demo/README.md)
    - OK to send Counter
    - UP to send 440Hz tone
    - Long UP to send 880Hz tone
- [code video](https://youtu.be/2GwqbH1XMC4) Adding Sub-GHz into your own application.  Intended for developers.
- [code walkthru video](https://youtu.be/uRC6yLk1Zbc) Walkthru of the ["Subghz Demo"](../subghz/plugins/subghz_demo/README.md) code.

