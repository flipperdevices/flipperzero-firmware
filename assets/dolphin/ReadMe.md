# Dolphin assets {#dolphin_assets}

Dolphin assets are split into 3 parts:

- blocking - Essential animations that are used for blocking system notifications. They are packed to `assets_dolphin_blocking.[h,c]`.
- internal  - Internal animations that are used for idle dolphin animation. Converted to `assets_dolphin_internal.[h,c]`.
- external  - External animations that are used for idle dolphin animation. Packed to resource folder and placed on SD card.

# Files

- `manifest.txt` - contains the animation enumeration and the values used for random idle animation selection. Starting point for Dolphin.
- `meta.txt`     - contains data that describes how animation is drawn.
- `frame_X.png`  - animation frame.

## File manifest.txt

Flipper Format File with ordered keys.

Header:

```
Filetype: Flipper Animation Manifest
Version: 1
```

All fields below are required for each animation entry.

- `Name` - name of animation. Must be exact animation directory name.
- `Min butthurt`, `Max butthurt` - inclusive range of dolphin's butthurt used to filter animations for random idle selection.
- `Min level`, `Max level` - inclusive range of dolphin's level used to filter animations for random idle selection. Setting both values to 0 excludes the animation from random idle selection, but it can still be selected by exact name.
- `Weight` - relative chance of this animation being chosen from the valid idle animations. A weight of 0 effectively disables random selection for this entry.

These values only affect random idle selection. Blocking animations are invoked by exact name for system notifications, so their `Min butthurt`, `Max butthurt`, `Min level`, `Max level`, and `Weight` values are unused and are set to 0 in `blocking/manifest.txt`. Internal and external animations can participate in random idle selection, so their values determine eligibility and relative selection probability. Some animations can also be excluded from random selection, such as `L1_NoSd_128x49`.

## File meta.txt

Flipper Format File with ordered keys.

Header:

```
Filetype: Flipper Animation
Version: 1
```

- `Width` - animation width in px (<= 128)
- `Height` - animation height in px (<= 64)
- `Passive frames` - number of bitmap frames for passive animation state
- `Active frames` - number of bitmap frames for active animation state (can be 0)
- `Frames order` - order of bitmap frames where first N frames are passive and following M are active. Each X number in order refers to bitmap frame, with name frame\_X.bm. This file must exist. Any X number can be repeated to refer same frame in animation.
- `Active cycles` - cycles to repeat of N active frames for full active period. E.g. if frames for active cycles are 6 and 7, and active cycles is 3, so full active period plays 6 7 6 7 6 7. Full period of passive + active period are called *total period*.
- `Frame rate` - number of frames to play for 1 second.
- `Duration` - total amount of seconds to play 1 animation.
- `Active cooldown` - amount of seconds (after passive mode) to pass before entering next active mode.

- `Bubble slots` - amount of bubble sequences.
- Any bubble sequence plays whole sequence during active mode. There can be many bubble sequences and bubbles inside it. Bubbles in 1 bubble sequence have to reside in 1 slot. Bubbles order in 1 bubble sequence is determined by occurrence in file. As soon as frame index goes out of EndFrame index of bubble - next animation bubble is chosen. There can also be free of bubbles frames between 2 bubbles.

- `Slot` - number to unite bubbles for same sequence.
- `X`, `Y` - are coordinates of left top corner of bubble.
- `Text` - text in bubble. New line is `\n`
- `AlignH` - horizontal place of bubble corner (Left, Center, Right)
- `AlignV` - vertical place of bubble corner (Top, Center, Bottom)
- `StartFrame`, `EndFrame` - frame index range inside whole period to show bubble.

### Understanding of frame indexes

For example we have

```
Passive frames: 6
Active frames: 2
Frames order: 0 1 2 3 4 5 6 7
Active cycles: 4
```

Then we have indexes

```
                        passive(6)            active (2 * 4)
Real frames order:   0  1  2  3  4  5     6  7  6  7  6  7  6  7
Frames indexes:      0  1  2  3  4  5     6  7  8  9  10 11 12 13
```
