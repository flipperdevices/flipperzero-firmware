# Changelog - Patch Notes

## Version 1.4
- **Bug Fixes:** More robust trade logic fixes issues with names, remove ability to use numbers in Pokemon/Trainer names as the game itself will not allow that, fix trade animation not always being animated, make FAP icon 1bpp.
- **Add Features:** Implement trade patch list that Game Boy expects and uses, add ability to return to main menu to modify a Pokemon traded to the Flipper and re-enter trade without the Game Boy needing to power cycle and re-connect through the Link Club, add back debug logging.
- **Trade Refactor:** Eliminate extraneous code, improve robustness of state tracking during trades, isolate Trade's scope to the compilation unit, add notes on exchanged bytes during a trade, improve timing of animation during trade, reduce time spent in interrupt context, follow same setup/hold times for data exchange that the Game Boy uses, reduce use of magic numbers, clean up and improve code tracking real world time

## Version 1.3
- **Refactor and UI cleanup:** Convert to Flipper Zero UI modules for simpler interface, reduce binary size.
- **Add Features:** Add ability to set custom Pokemon nickname or default, add ability to set OT name and ID, add ability to select Pokemon type(s). Note that, an evolution as well as a couple of different attacks will cause this to be overwritten with the Pokemon's default values.
- **Bug Fixes:** Fix strange issue with exp gain causing traded Pokemon to de-level and result in incorrect stats.

## Version 1.2.2
- **Extended Functionality:** Add support to set level, select moves, set up EV/IV to a few predefined configurations, set up stats based on level and EV/IV settings, set nickname to default Pokemon name.

## Version 1.2.1
- **Add GitHub action to build**

## Version 1.2.0
- **Cleanup data structs:** This refactors the main data blocks for defining Pokemon, the icon, their species/hex value, as well as the large trade array in to more human friendly structs. Laying some groundwork to be able to adjust Pokemon details pre-trade by @kbembedded .
- **Bug Fixes:** Fix furi crash, Fixes #9 by @kbembedded .
