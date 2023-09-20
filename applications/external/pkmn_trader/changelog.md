# Changelog - Patch Notes

## Version 1.2.3
- **Refactor and UI cleanup:** Convert to Flipper Zero UI modules for simpler interface, reduce binary by ~10 kbyte.
- **Add Features:** Add ability to set custom pokemon nickname or default, add ability to set OT name and ID.
- **Bug Fixes:** Fix strange issue with exp gain causing traded pokemon to de-level and result in incorrect stats.

## Version 1.2.2
- **Extended Functionality:** Add support to set level, select moves, set up EV/IV to a few predefined configurations, set up stats based on level and EV/IV settings, set nickname to default pokemon name.

## Version 1.2.1
- **Add github action to build**

## Version 1.2.0
- **Cleanup data structs:** This refactors the main data blocks for defining pokemon, the icon, their species/hex value, as well as the large trade array in to more human friendly structs. Laying some groundwork to be able to adjust pokemon details pre-trade by @kbembedded .
- **Bug Fixes:** Fix furi crash, Fixes #9 by @kbembedded .
