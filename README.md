

# <img src="quac.png" alt="logo" width="30" style="image-rendering: pixelated;"/> Quac! Remote
## QUick ACtion Remote Control for Flipperzero
This app allows you to organize previously recorded signals, of any type*, so that you can quickly and easily play them back. No more needing to recall whether that door is Sub-GHz or RFID! Just navigate to that action in **Quac** and press OK to send!

The app does not provide any recording functionality - you must use the existing Flipperzero apps to create the saved files for your action/device. Additionally, you must manage the folder structure manually on your SD card.

```
* Only Sub-Ghz (.sub) and RFID (.rfid) files are supported at this time, IR is coming soon!
```

## Screenshots
<table align="center">
  <tr>
    <td align="center">Main View</td>
    <td align="center">"Work" Actions</td>
  </tr>
  <tr>
    <td><img src="screenshots/screenshot_1.png"/></td>
    <td><img src="screenshots/screenshot_2.png"/></td>
  </tr>
</table>

## Navigation / Controls
- Pressing `OK` on a folder label will open/navigate to that folder and display it's contents.
- Pressing `OK` on an action (indicated as a label surrounded by a border) will transmit that signal.
- Pressing `Back` will take you up one folder
- Pressing `Up` and `Down` will, you know, select things up and down...

## File System Layout
The key to organizing your Quac interface is to organize your `/ext/apps_data/quac` folder structure. Every individual file is assigned a button, and every folder/directory is a "group". Selecting a group will show you the contents of that folder/directory, whether they are individual files or more folders. There is no limit on the number of actions or folders - nest as deep as you want!

You can organize your files by device type, or by function. For example, you may have a folder of "TV" actions, which correspond to Channel Up, Channel Down, Volume Up, Volume Down, etc. Or you may have a "Work Access" folder, which contains files/actions that correspond to Parking Gate, Garage Door, Lobby Entrance. 

The files in a folder can be of mixed types. This is Quac's main strength! So continuing with the "Work Access" example, the Parking Gate can be Sub-Ghz and the Garage Door can be RFID.

## Sorting and Naming
The list view is based on the sorted file and folder order. This is enforced by sorting the actual filenames. When there are cases where you need to enforce a sorted order, you can prepend the file and folder names with `XX_` where `X` is a digit between 0-9. This will let you place an action called `On` before `Off`, even though when sorted alphabeticaly, `Off` would come before `On`. Therefore, you would name your files `00_On.rfid` and `01_Off.rfid` - assuming these are RFID saved signals.

Here's an example file layout for the screenshots above:
```
/ext/apps_data/quac
  /00_Work
    -00_Parking_Gate.sub
    -01_Garage_Door.rfid
    -02_Office.rfid
  -01_Front_Door.rfid
  /02_Home_Theater
     -00_Power_On.ir
     -01_Power_Off.ir
```

When the action files and folders are rendered for display, any `XX_` prefix will be stripped. All underscores will be replaced with spaces. Extensions will be stripped. Casing is preserved.

## Signal playback
The signal files are played back as recorded. During playback/transmit, the LED light will flash green until the action is complete. For RFID signals, they are continuously played back for 3 seconds.

## On deck... maybe?
- Playlist support: Thinking a `.qpl` file (Quac playlist file - just a text file), that lists the full path of every command you wish to play back. Playlists would be rendered as buttons. Support a `pause [ms]` command so you can space out the actions inside of a playlist.
- Hidden file/folder support: Hide files and folders if they start with a `.`. This allows you to place/organize the files used by a playlist in your current directory structure, without them appearing as buttons/groups in the app.
- Horizontal layout
- Customize length of RFID playback. (Per file? hmm...)
- Improved UI

## Building / Install
This app is currently built with `ufbt`, intended for the stock firmware. I have not tested this on other firmwares. The `.fap` file can be found in the Releases section on the right.
