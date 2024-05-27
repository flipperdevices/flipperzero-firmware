# Changelog - Patch Notes

## Version 2.0
**Bug Fixes**  
- Older MALVEKE style pinout no longer breaks OK button, MALVEKE users now able to modify traded Pokemon

**New Features**  
- Generation II support (Gold, Silver, Crystal)  
  - Ability to configure name, level, held item, moveset, EV/IV, shininess, gender, pokerus, Unown form, OT ID/name
- Custom Sprite Art for all Pokemon, all sprites are the full 56x56 px size  
- Better state synchronization during trades, able to request canceling a trade
- Exit confirmation to prevent accidentally losing configuration  
- Add main menu to select generation and pinout  
- Adjust Trade and Select views to show full sprite, with proper transparency

**Refactor**
- Sprites moved to file on SD card  
- Create accessors for generically modifying Pokemon struct data  

## Version 1.6
- **Change Name:** All the application names for GAME BOY/MALVEKE were standardized for better readability on the screen.

## Version 1.5
- **Add Features:** Incorporate flipper-gblink library; Add support for MALVEKE board as well as custom pin selection; If MALVEKE board is detected, default to that pinout, otherwise use the original documented pinout.
- **BUG:** The current MALVEKE pinout and interrupt use breaks the OK button after entering the trade screen.

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
