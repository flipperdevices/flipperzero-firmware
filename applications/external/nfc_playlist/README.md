# FlipperZero_NFC_Playlist:
The idea behind this app is to allow for you to test multiple copies of NFC's at once as a bulk test
## How it works:
When starting the app you are greeted by a select file option where you choose the playlist you wanna run.

All the playlists should be placed in ext/apps_data/nfc_playlist and an example of how the data in the file should look can be found below.
```txt
/ext/nfc/link.nfc
/ext/nfc/link2.nfc
```
An example file can be found in the repository
## Settings:
- Emulate time (How long the NFC card will be emulated for)
- Delay time (How long the gap between the cards will be)
- LED indicator (Whether or not the LED's will be on)
- Skip errors (Makes it so you can make the emulation screen hide errors and skip delays between errors and emulation)
- Reset settings (Puts all the settings back to the defaults)
## Playlist editor:
- Create PLaylist (Creates a new playlist with the given name)
- Delete playlist (Deletes the selected playlist)
- Rename playlist (Renames the selected playlist to the new name provided)
- Add NFC Item (Adds the selected nfc item to the currently selected playlist)
- Remove NFC Item (Opens a menu allowing you to select a line to remove from the playlist)
- View playlist content (Allows you to view the contents of the playlist)
## Development plans/ideas:
Things i would like to add:
- Ability to remove cards from the playlist

These features are not guaranteed to be added but are being looked at as features to add

Any feedback is welcome and would be very much appreciated
