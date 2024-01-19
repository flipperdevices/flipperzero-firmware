
# Changelog
## TODO:
- Add settings options to toggle hardware feedback
- Maybe take a look at the board verifier algo and try to make faster/multi-thread or anything to allow better maps

## Version 1.1.0 - 1/11/2024

Added haptic / led functionality

## Added
- Haptic feedback on all button presses.
    - Out of bounds movement
    - Ok to clear tiles
    - Holding back for flags
    - Different haptic feedback on win/loss
- LED changes on win loss
    - Initially LED is just reset
    - Set to red on loss
    - Set to blue on win
- Sound on some presses

## Version 1.0.0 - 1/10/2024

Initial Full release for the game.

## Added
- Mine sweeper game with settings menu to choose variable board dimensions from 16x7 to 32x32.
- Ability to set difficulty, width, and height for the game board in settings.
- Ability to enable solvable boards only
- The core game functionality is complete for the game with multiple types of button presses registered for different inputs.
- The game will save your settings (besides "enable solvable") when you leave the game.
