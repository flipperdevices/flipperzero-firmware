### Welcome to Flipper Zero Jerky Firmware repo!
Based on clean [Flipper Devices](https://github.com/flipperdevices/flipperzero-firmware) + [Unleashed Firmware](https://github.com/DarkFlippers/unleashed-firmware)

The main goal is to make any features possible in this device without any limitations!

# What's changed

* Grabbed ibutton, lfrfid, nfc intercom-keys (grabbed by Jerky, wetox-team)
* BadUSB scripts (Android PIN bruteforce, Kiosk evation bruteforce, Windows wifi/web passwords information)
* SubGhz gates bruteforce (Came, Lift Master, Handicap)
* SubGhz jammers (can be illegal in your country, for educational purposes only!)
* SubGhz just for fun (sex-toys manipulation, Tesla charge opener, etc.)
* Some IR TV remotes (Samsung, Sharp, don't forget about Universal Remote)
* Cleaned some standard animations, added new

[![GitHub Last Commit](https://img.shields.io/github/last-commit/pieceofsys/flipperzero-firmware/dev?logo=github)](https://github.com/pieceofsys/flipperzero-firmware/commits/dev)

# What's changed by DarkFlippers
* SubGHz regional TX restrictions removed
* SubGHz frequecy range can be extended in settings file (Warning: It can damage flipper's hardware)
* Many rolling code protocols now have the ability to save & send captured signals
* FAAC SLH (Spa) & BFT Mitto (secure with seed) manual creation
* Sub-GHz static code brute-force plugin
* LFRFID Fuzzer plugin
* Custom community plugins and games added
* Extra SubGHz frequencies + extra Mifare Classic keys
* Picopass/iClass plugin included in releases
* Recompiled IR TV Universal Remote for ALL buttons
* Universal remote for Projectors, Fans, A/Cs and Audio(soundbars, etc.)
* BadUSB keyboard layouts
* Customizable Flipper name
* SubGHz -> Press OK in frequency analyzer to use detected frequency in Read modes
* SubGHz -> Long press OK button in SubGHz Frequency analyzer to switch to Read menu 
* Other small fixes and changes throughout
* See other changes in changelog and in readme below

### Current modified and new SubGHz protocols list:
- HCS101
- An-Motors
- CAME Atomo
- FAAC SLH (Spa) [External seed calculation required]
- BFT Mitto [External seed calculation required]
- Keeloq [Not ALL systems supported yet!]
- Nice Flor S
- Security+ v1 & v2
- Star Line (saving only)

[![GitHub Last Commit](https://img.shields.io/github/last-commit/DarkFlippers/unleashed-firmware/dev?logo=github)](https://github.com/DarkFlippers/unleashed-firmware/commits/dev)

# Links
* Official Docs: [http://docs.flipperzero.one](http://docs.flipperzero.one)
* Official Forum: [forum.flipperzero.one](https://forum.flipperzero.one/)

# Project structure
- `applications`    - Applications and services used in firmware
- `assets`          - Assets used by applications and services
- `furi`            - Furi Core: os level primitives and helpers
- `debug`           - Debug tool: GDB-plugins, SVD-file and etc
- `documentation`   - Documentation generation system configs and input files
- `firmware`        - Firmware source code
- `lib`             - Our and 3rd party libraries, drivers and etc...
- `site_scons`      - Build helpers
- `scripts`         - Supplementary scripts and python libraries home

Also pay attention to `ReadMe.md` files inside those directories.
