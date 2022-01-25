# Dolphin assets

Dolphin assets contains of 3 directories:
- essential. Animations that compiled into firmware. Consist of blocking animations, which have to be available always. Animations inside always refered by animation name. Converted to assets\_dolphin\_esssential.[h,c]. These blocking animation can be loaded by name, not by random selection.
- internal. Animations that compiled into firmware. Consist of idle and system animation, such as "NoSd". Converted to assets\_dolphin.[h,c]
- external. Idle animations that placed on SD card. Just copied to resources directory.

`manifest.txt` file consist of data for random animation selection, such as level, animation name and butthurt. By this file on SD card Flipper understands animation directory name.
`meta.txt` file describes how animations should be drawn.
`frame_X.bm` files - are bitmap data files. This is not a raw data, it is prefixed with compression info.

#### File manifest.txt
Key-value order of this file is strict - you can't mess it.
- `Name` - name of animation. Must be exact as animation directory name.
- `Min butthurt`, `Max butthurt` - range of mood of dolphin for this animation.
- `Min level`, `Max level` - range of levels to play animation at. If 0, this animation doesn't participate in random idle animation selection. It can only be selected by precise name. This is used for blocking animation, which brings critical information.
- `Weight` - chance of this animation to be choosen at random animation selection.

Some of animations can be discarded from random animation selection, such as L1\_NoSd\_128x49.

#### File meta.txt
Key-value order of this file is strict - you can't mess it.
- `Width` - width of animation (less than 128)
- `Height` - height of animation (less than 64)
- `Passive frames` - number of bitmap frames for passive animation state
- `Active frames` - number of bitmap frames for active animation state (can be 0)
- `Frames order` - order of bitmap framesm where first N frames are passive and following M are active. Each X number in order refers to bitmap frame, with name frame\_X.bm. This file must exist. Any X number can be repeated to refer same frame in animation.
- `Active cycles` - cycles to repeat of N active frames for full active period. E.g. if frames for active cycles are 6 and 7, and active cycles is 3, so full active period plays 6 7 6 7 6 7. Full period of passive + active period are called *total period*.
- `Frame rate` - number of frames to play for 1 second.
- `Duration` - total amount of seconds to play 1 animation.
- `Active cooldown` - amount of seconds (after passive mode) to pass before entering next active mode.

- `Bubble slots` - amount of bubble sequences.
- Any bubble sequence plays whole sequence during active mode. There can be many bubble sequences and bubbles inside it. Bubbles in 1 bubble sequence have to reside in 1 slot. Bubbles order in 1 bubble sequence is determined by occurance in file. As soon as frame index goes out of EndFrame index of bubble - next animation bubble is choosen. There can also be free of bubbles frames between 2 bubbles.

- `Slot` - number to unite bubbles for same sequence.
- `X`, `Y` - are coordinates of left top corner of bubble.
- `Text` - text in bubble. New line is `\n`
- `AlignH` - horizontal place of bubble corner (Left, Center, Right)
- `AlignV` - vertical place of bubble corner (Top, Center, Bottom)
- `StartFrame`, `EndFrame` - frame index range inside whole period to show bubble.

#### Understanding of frame indexes
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

