# FlipperZero_NFC_Playlist:
The idea behind this app is to allow for you to test multiple copies of NFC's at once as a bulk test

## Know issue:
At the end of some playlists it will cause the flipperZero to crash there is no eta when it come to the fix of this as i still trying to figure out what is causing the issue, it appears to of started after an update to the firmware so is probably just a compatibility issue

Any help in finding or fixing the issue would be amazing

## How it works:
When starting the app you are greeted by a select file option where you choose the playlist you wanna run.

All the playlists should be placed in ext/apps_data/nfc_playlist and an example of how the data in the file should look can be found below.
```txt
/ext/nfc/link.nfc
/ext/nfc/link2.nfc
```
An example file can be found in the repository

## How to build
This app was design, built and tested using the <a href="https://github.com/Flipper-XFW/Xtreme-Firmware">Xtreme firmware</a> so keep that in mind when building the FAP for yourself

## Settings:
- Emulate time (How long the NFC card will be emulated for)
- Delay time (How long the gap between the cards will be)
- LED indicator (Whether or not the LED's will be on)
- Reset settings (Puts all the settings back to the defaults)

## Playlist editor:
- Delete playlist (Deletes the selected playlist)
- Rename playlist (Renames the selected playlist the new name provided)