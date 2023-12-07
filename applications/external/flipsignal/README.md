# FlipSignal

The FlipSignal application is still under development.  
The goal is to create an application that allows you to quickly send signals using a FlipBoard.

Currently the application sends a sub-ghz signal (RAW or Protocol), and then it sends an IR signal.  The configuration of signals is done via files rather than using the Config menu option.


## Installation
You need to copy your Sub-GHz files into your `SD Card/subghz` folder.

You need to copy your `flipboard.ir` file into your `SD Card/infrared` folder.  The `name:` field should be `Flip1`, `Flip2`, `Flip3` & `Flip4` for the 4 actions.  The actions are case-sensitive; so they must start with a capital F and then lowercase the remaining letters.  If the file is not created or no matching name exists, then a fallback file of `SD Card/infrared/assets/tv.ir` will be used along with default names (Power, Mute, Ch_prev and Ch_next).

- Action 1 will run `Flip1.sub` and then send IR signal `Flip1`.  If IR signal does not exist, it will send IR TV signal "Power".
- Action 2 will run `Flip2.sub` and then send IR signal `Flip2`.  If IR signal does not exist, it will send IR TV signal "Mute".
- Action 3 will run `Flip3.sub` and then send IR signal `Flip3`.  If IR signal does not exist, it will send IR TV signal "Ch_prev".
- Action 4 will run `Flip4.sub` and then send IR signal `Flip4`.  If IR signal does not exist, it will send IR TV signal "Ch_next".

## Updates
- V2.0 (was called v1.1): Flipper keyboard doesn't have a dash, so renamed the files to `Flip1.sub`, `Flip2.sub`, `Flip3.sub`, `Flip4.sub` and the IR entries to `Flip1`, `Flip2`, `Flip3`, `Flip4`.