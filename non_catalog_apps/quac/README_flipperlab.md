# Quac! Remote

## QUick ACtion Remote Control for Flipperzero

This app allows you to organize previously recorded signals, of any type, so that you can quickly and easily play them back. No more needing to recall whether that door is Sub-GHz or RFID! Just navigate to that action in **Quac** and press OK to send!

The app does not provide any recording functionality - you can use the existing Flipperzero apps to create the saved files or import from existing files. Quac! provides some basic functionality to manage your files. Or, you can manage the folder structure manually on your SD card on your PC.

* Supported files include: Sub-Ghz (.sub), RFID (.rfid), Infrared (.ir), and NFC (.nfc)

## Features

* Playback of rfid, sub-ghz, IR, and NFC signals
* Easy navigation
* Flexible signal organization
* In-app file management
* Playlist support
* Flexible naming/sorting, hidden file/folder support
* Customizable UI

## Signal playback

The signal files are played back as recorded. During playback/transmit, the LED light will flash blue until the action is complete. For RFID and NFC signals, they are continuously played back for the durations specified in the Settings.

## Signal Organization

The key to organizing your Quac interface is to organize your **/ext/apps_data/quac** folder structure. The UI is derived directly from the filesystem structure. Every individual file/signal is assigned a button. This is an "action". And every folder/directory is a logical group of more files/folders. Selecting a group in the UI will show you the contents of that folder. There is no limit on the number of actions or folders - nest as deep as you want!

You can organize your files by device type, or by function. For example, you may have a folder of "TV" actions, which correspond to Channel Up, Channel Down, Volume Up, Volume Down, etc. Or you may have a "Work Access" folder, which contains files/actions that correspond to Parking Gate, Garage Door, Lobby Entrance.

The files in a folder can be of mixed types. **This is Quac's main strength!** So continuing with the "Work Access" example, the Parking Gate can be Sub-Ghz and the Garage Door can be RFID.

## Playlists

You can chain multiple signal playback actions together by creating a playlist. Simply create a text file which contains a list of paths to the signals you wish to transmit - they will be played sequentially. Playlist names show up as clickable button, like all other individual signals/actions.

Errors found in the playlist will halt playback and vibrate the Flipperzero. Blank lines are ignored.

## Settings

The settings menu will appear as the last item when you are viewing the "root" directory. Within the settings you can control:

* Layout: Switch between Horizontal and Vertical layout
* Show Icons: Toggles display of all icons
* Show Headers: Toggles display of header/folder text at the top, giving you room for one more item on screen!
* RFID Duration: Changes the length of time a RFID signal is transmitted. Can be overridden, per RFID file in a Playlist

**More information can be found in the Git repository**
