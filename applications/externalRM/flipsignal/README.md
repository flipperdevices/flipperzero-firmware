# FlipSignal

The FlipSignal application is still under development.  
The goal is to create an application that allows you to quickly send signals using a FlipBoard.

Currently the application sends a sub-ghz signal (RAW or Protocol), and then it sends an IR signal.  The configuration of signals is done via files rather than using the Config menu option.


## Installation
You need to copy your Sub-GHz files into your `SD Card/subghz` folder.

You need to copy your `flipboard.ir` file into your `SD Card/infrared` folder.  The `name:` field should be `Flip-a1`, `Flip-a2`, `Flip-a3` & `Flip-a4` for the 4 actions.  The actions are case-sensitive; so they must start with a capital F and then lowercase the remaining letters.  If the file is not created or no matching name exists, then a fallback file of `SD Card/infrared/assets/tv.ir` will be used along with default names (Power, Mute, Ch_prev and Ch_next).

- Action 1 will run `Flip-a1.sub` and then send IR signal `Flip-a1`.  If IR signal does not exist, it will send IR TV signal "Power".
- Action 2 will run `Flip-a2.sub` and then send IR signal `Flip-a2`.  If IR signal does not exist, it will send IR TV signal "Mute".
- Action 3 will run `Flip-a3.sub` and then send IR signal `Flip-a3`.  If IR signal does not exist, it will send IR TV signal "Ch_prev".
- Action 4 will run `Flip-a4.sub` and then send IR signal `Flip-a4`.  If IR signal does not exist, it will send IR TV signal "Ch_next".