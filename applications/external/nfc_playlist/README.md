# FlipperZero_NFC_Playlist:
The idea behind this app is to allow for you to test multiple copies of NFC's at once as a bulk test

## How it works (As of the moment will change):
When starting the app you are greeted by a select file option where you choose the playlist you wanna run.

All the playlists should be placed in ext/apps_data/nfc_playlist and an example of how the data in the file should look can be found below.
```txt
/ext/nfc/link.nfc
/ext/nfc/link2.nfc
```
An example file can be found in the repository

## Ideas
- [X] Add the ability to change playlist
- [ ] Make it so changed settings are saved (maybe make it so settings can be specified for each playlist changing the settings based on the playlist selected)