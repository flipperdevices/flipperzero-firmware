# FlipperZero_NFC_Playlist:
The idea behind this app is to allow for you to test multiple copies of NFC's at once as a bulk test

At the moment this project is very basic and just reads the cards from the txt file and emulates the NFC's but this is very early days of development and i wanted to make a basic version of my idea first

## How it works (As of the moment will change):
When you start the app there is a start button when pressed starts the playlist

To set the playlist you need to create a file in ext/apps_data/nfc_playlist called playlist.txt you can find an example of the file in the repository

## How to build
This app was design, built and tested using the <a href="https://github.com/Flipper-XFW/Xtreme-Firmware">Xtreme firmware</a> i don't see why it wont work with other firmwares but do keep this in mind when building it with FBT/uFBT

## Ideas
- [ ] Add the ability to change playlist
- [ ] Make it so changed settings are saved (maybe make it so settings can be specified for each playlist changing the settings based on the playlist selected)