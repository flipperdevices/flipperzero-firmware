# <img src="quac.png" alt="logo" width="30" style="image-rendering: pixelated; image-rendering -moz-crisp-edges; image-rendering: crisp-edges;"/> Quac! Remote
## QUick ACtion Remote Control for Flipperzero
This app allows you to organize previously recorded signals, of any type*, so that you can quickly and easily play them back. No more needing to recall whether that door is Sub-GHz or RFID! Just navigate to that action in **Quac** and press OK to send!

The app does not provide any recording functionality - you must use the existing Flipperzero apps to create the saved files for your action/device. Additionally, you must manage the folder structure manually on your SD card.

```
* Only Sub-Ghz (.sub) and RFID (.rfid) files are supported at this time, IR is coming soon!
```

## Features
* [Playback of rfid, sub-ghz, (and soon IR) signals](README.md#signal-playback)
* [Easy navigation](README.md#navigation--controls)
* [Flexible signal organization](README.md#signal-organization) - utilizing the SDcard filesystem
* [Playlist support](README.md#playlists)
* [Flexible naming/sorting, hidden file/folder support](README.md#sorting-and-naming)
* [Customizable UI](README.md#settings)

## Screenshots
<table>
  <tr>
    <td><img src="screenshots/screenshot_1.png" width="256px"/></td>
    <td rowspan="2"><img src="screenshots/screenshot_4_90.png" width="128px"/></td>
  </tr>
  <tr>
    <td><img src="screenshots/screenshot_3.png" width="256px"/></td>
  </tr>
</table>

## Navigation / Controls
- Pressing `OK` on a folder label will open/navigate to that folder and display it's contents.
- Pressing `OK` on an action (indicated as a label surrounded by a border) will transmit that signal.
- Pressing `Back` will take you up one folder
- Pressing `Up` and `Down` will, you know, select things up and down...

## Signal playback
The signal files are played back as recorded. During playback/transmit, the LED light will flash blue until the action is complete. For RFID signals, they are continuously played back for 3 seconds.

## Signal Organization
The key to organizing your Quac interface is to organize your `/ext/apps_data/quac` folder structure. The UI is derived directly from the filesystem structure. Every individual file/signal is assigned a button. This is an "action". And every folder/directory is a logical group of more files/folders. Selecting a group in the UI will show you the contents of that folder. There is no limit on the number of actions or folders - nest as deep as you want!

You can organize your files by device type, or by function. For example, you may have a folder of "TV" actions, which correspond to Channel Up, Channel Down, Volume Up, Volume Down, etc. Or you may have a "Work Access" folder, which contains files/actions that correspond to Parking Gate, Garage Door, Lobby Entrance. 

The files in a folder can be of mixed types. **This is Quac's main strength!** So continuing with the "Work Access" example, the Parking Gate can be Sub-Ghz and the Garage Door can be RFID.

Here's an example file layout for the screenshots above:
```
/ext/apps_data/quac
  /00_Work
    00_ParkingGate.sub
    01_Garage_door.rfid
    02_Office.rfid
  01_Front_Door.rfid
  /02_Jam_Lists
     00_Gate_Spam.qpl
     01_TVs_OFF.qpl
     02_my_Playlist.qpl
```

## Playlists
You can chain multiple signal playback actions together by creating a playlist. Simply create a text file which contains a list of paths to the signals you wish to transmit - they will be played sequentially. Playlist names show up as clickable button, like all other individual signals/actions. Playlist files feature the following:
* Comments: lines that start with a `#` are ignored
* `pause <ms>` on a line will pause the playback by the specified millisecond duration
* Signal file names can be fully specified or local to the current directory

Errors found in the playlist will halt playback and vibrate the F0. Blank lines are ignored.

Here's an example playlist called `/ext/apps_data/quac/arrive_home.qpl`:
```
# Home arrival playlist
.exterior_light_on.sub
.side_door_unlock.sub
pause 2500
/ext/apps_data/quac/03_Stereo/00_On.sub
/ext/apps_data/quac/06_Lights/Disco_Ball.sub
```
The first two `.sub` files live in the `/ext/apps_data/quac` folder, which is where `arrive_home.qpl` is located, and will not show up in any UI screen since they are hidden (start with a `.`). Next, we pause the playlist for 2.5 sec. The last two files live elsewhere, but can still be referenced by the playlist.

## Sorting and Naming
The list view UI is based on the sorted file and folder order. This is enforced by sorting the actual filenames. When there are cases where you need to force a specific order, you can prepend the file and folder names with `XX_` where `X` is a digit between 0-9. This will let you place an action called `On` before `Off`, even though when sorted alphabeticaly, `Off` would come before `On`. Therefore, you would name your files `00_On.rfid` and `01_Off.rfid`. When the files and folders are rendered for display, any `XX_` prefix will be stripped. All underscores will be replaced with spaces. Extensions will be stripped. Casing is preserved. Additionally, all files and folders that begin with a `.` will be ignored when drawing the UI. However, they can still be referenced in playlists. This keeps the UI unclutterd.

## Settings
<img src="screenshots/screenshot_2.png" width="256px"/>

The settings menu will appear as the last item when you are viewing the "root" directory. Within the settings you can control:
- Layout: Switch between Horizontal and Vertical layout
- Show Icons: Toggles display of all icons
- Show Headers: Toggles display of header/folder text at the top, giving you room for one more item on screen!
- RFID Duration: Changes the length of time a RFID signal is transmitted. Can be overridden, per `.rfid` file in a Playlist

## On deck...
- IR Signal support

## Building / Install
This app is currently built with `ufbt`, intended for the stock firmware. I have not tested this on other firmwares. The `.fap` file can be found in the Releases section on the right.
