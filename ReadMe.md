# Flipper Zero FW [ROGUEMASTER]
This firmware is a fork of [Unleashed/xMasterX](https://github.com/DarkFlippers/unleashed-firmware) and the main Flipper Devices FW! I will try to keep active development and updates from both in this build along with any other projects that can be found to be useful to the community. I try to keep this FW build the most cutting edge with updates from both and updates from active community projects.  

This software is for experimental purposes only and is not meant for any illegal activity/purposes. We do not condone illegal activity and strongly encourage keeping transmissions to legal/valid uses allowed by law.

## [Support Us!](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/SUPPORT.md)
Everyone gives much of their uncompensated free time to ensure the success of the Flipper Zero!
Thank you to all the supporters!

- [Support us if you like what you see](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/SUPPORT.md)! 😄🚀💸
- Donations: BTC: `3MPQbKmGRCstg4FjnadfHa3woCT94JkR2a`
- Donations: ETH: `0xC32Ea488DBeCF95992A5C81BD411e56Bd418BC5f`
- [Join THE Flipper Uncensored Discord](https://discord.gg/gF2bBUzAFe)

## Latest Updates

- Known Issues:	 `Chess`
- Last Synced/Checked [Unleashed/xMasterX](https://github.com/DarkFlippers/unleashed-firmware), changes in [changelog](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/CHANGELOG.md): `2022-11-27 01:44 EST`
- Last Synced/Checked [OFW](https://github.com/flipperdevices/flipperzero-firmware), changes in [commits](https://github.com/flipperdevices/flipperzero-firmware/commits/dev): `2022-11-27 01:44 EST`
- Updated: [USB HID Autofire (By pbek)](https://github.com/pbek/usb_hid_autofire)
- Added: [(WIP) added ISO15693 reading, saving and revealing from privacy mode (unlock) #1991 (By g3gg0)](https://github.com/flipperdevices/flipperzero-firmware/pull/1991)
- Updated: [Gui: proper navigation in file browser dialog #2014 (By skotopes)](https://github.com/flipperdevices/flipperzero-firmware/pull/2014)
- Updated: [HEX Viewer (By QtRoS)](https://github.com/QtRoS/flipperzero-firmware)
- Updated: [NRF24 Scanner (By vad7)](https://github.com/vad7/nrf24scan)
- Updated: [(WIP) added ISO15693 reading, saving and revealing from privacy mode (unlock) #1991 (By g3gg0)](https://github.com/flipperdevices/flipperzero-firmware/pull/1991)
- [Adding Slovenian, Bosnian and Croatian layout #187 (By StellaStoic)](https://github.com/DarkFlippers/unleashed-firmware/pull/187)
- [Add protocol Auriol hg0601a #451 (By LY2NEO)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/451)
- Added: [SubGhz: fix duration pricenton protocol #2054 (By Skorpionm)](https://github.com/flipperdevices/flipperzero-firmware/pull/2054)
- Added: [POCSAG protocol decoder for subghz #2055 (By Shuma)](https://github.com/flipperdevices/flipperzero-firmware/pull/2055)
- Added: [Dice (By Ka3u6y6a)](https://github.com/Ka3u6y6a/flipper-zero-dice)
- Added: [VB Lab Migration Assistant (By GMMan)](https://github.com/GMMan/flipperzero-vb-migrate) `Req: Vital Bracelet`
- Updated: [nfc: NTAG password auto capture (and other password-related changes) #1843 (By GMMan)](https://github.com/flipperdevices/flipperzero-firmware/pull/1843)
- Renamed some FAPs to fix ordering in Applications, delete your `/ext/apps` folder to get rid of duplicates
- Removed: [VB Lab Migration Assistant (By GMMan)](https://github.com/GMMan/flipperzero-vb-migrate) `Req: Vital Bracelet` (due to fap error)

## Install from Release
FLASH STOCK FIRST BEFORE UPDATING TO CUSTOM FIRMWARE!
- To avoid Application errors, delete /ext/apps before doing the RM firmware update
- To install new FW, extract the [latest release zip file](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/releases) to a folder, put the folder in the update folder on your SD card, and run the update file inside the folder using the Archive app (down from flipper desktop). If you were previously unleashed, you need to update your extend_range.txt file.`UPDATE IGNORE FLAG TO TRUE TO UNLEASH YOUR FLIPPER!!` On any update, you may need to update this file and your unirf map file, so keep backups. 😄

## Build
```bash
$ git clone --recursive https://github.com/RogueMaster/flipperzero-firmware-wPlugins.git
$ cd flipperzero-firmware-wPlugins/
$ ./fbt updater_package

# If building FAPS:
$ ./fbt fap_dist

# If building image assets:
$ ./fbt resources icons dolphin_ext
```

## How To
  
- [HERE IS A GUIDE FOR INSTALL (BY PINGYWON)](https://flipper.pingywon.com/)
- [HERE IS A NOOB GUIDE TO FLASH AND UNLOCK (BY interestingsoup)](https://interestingsoup.com/n00b-guide-flashing-flipper-zero-to-rougemaster/)
- [How to install firmware](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/documentation/HowToInstall.md)
- [How to build firmware](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/documentation/HowToBuild.md)
- [How to change Flipper name](https://github.com/RogueMaster/flipperzero-firmware/blob/420/documentation/CustomFlipperName.md)
- [How to use: NRF24 plugins](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/documentation/NRF24.md)
- [How to use: SentrySafe plugin](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/documentation/SentrySafe.md)
- [Barcode Generator readme](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/documentation/BarcodeGenerator.md)
- [How to extend SubGHz frequency range](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/documentation/DangerousSettings.md)
- [How to add extra SubGHz frequencies](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/documentation/SubGHzSettings.md)
- [💎 Extra plugins precompiled for Unleashed](https://github.com/UberGuidoZ/Flipper/tree/main/Applications/Unleashed)
- [Configure Sub-GHz Remote App](https://github.com/RogueMaster/flipperzero-firmware/blob/420/documentation/SubGHzRemotePlugin.md)
- [GAMES ONLY MODE PASSWORD: UP UP DOWN DOWN LEFT RIGHT LEFT RIGHT IN CLOCK](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/GAMES_ONLY.md)
- [FLIPPER PROJECT WISH LIST](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/RoadMap.md)
- [SAMPLE EDUCATIONAL PROJECTS 😄](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/RoadMap.md)
- Contact me on [Discord](https://discord.gg/gF2bBUzAFe) if you want a renamed Flipper DFU. I can do custom names too!
- [`FUN Links HERE` Collection for your Flipper SD](https://github.com/RogueMaster/awesome-flipperzero-withModules)


## All Changes/Features

- Animations: File `/ext/dolphin/manifest.txt` no longer will get overwritten. Any automatic building of animations is disabled [here](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/commit/fbe9175e0c828a54e651ee11f64f10f21e36a907).
- Animations: Hold Center to change Flipper idle animation. [Thanks to Zycenios](https://github.com/flipperdevices/flipperzero-firmware/commit/111786ef40e50a40d2e510595672b569d9b97bba) With changes by RogueMaster.
- Animations: iButton and RFID P0kem0n images [Thanks to Panzer00Z](https://github.com/Panzer00Z/flipperzero-firmware/)
- Animations: Idle animations will show all animations regardless of level and butthurt [Thanks to qqMajiKpp]
- Animations: Painting speech bubbles updated from FalsePhilosopher
- Animations: RM FW Update image [(Thanks to E_Surge)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/257)
- Animations: SubGHZ Scanning image with Pikachu [Thanks to Panzer00Z](https://github.com/Panzer00Z/flipperzero-firmware/blob/3a548ea9bb181c9348d8afb427890c411456134e/assets/icons/SubGhz/Scanning_123x52.png)
- Animations: Trimmed out the Flipper animations. `/ext/dolphin` folder on your Flipper should now be managed by you! [Copy this folder (RM Select)](https://github.com/RogueMaster/awesome-flipperzero-withModules/tree/rogue_main/dolphin-RMselect) or [this folder (RM minimal)](https://github.com/RogueMaster/awesome-flipperzero-withModules/tree/rogue_main/dolphin-minimal) if you don't want to do the work but want more animations.
- [Archive: File Browser Ordering (By Dig03)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/389)
- [Archive: Browser: Context menu to show file content (By askoriy)](https://github.com/DarkFlippers/unleashed-firmware/pull/139)
- Archive: FAPs are now launchable from Archive [By RogueMaster], thanks xMasterX for the suggestion
- Assets: Includes a NFC Level 50 Z3lda Amiibo
- Assets: Includes a NFC Rick Roll link
- Assets: Includes New Dolphin Animations: [Listed Here](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/tree/420/assets/resources/dolphin)
- Assets: Includes sample Music Player tunes
- Assets: Includes sample SubGHz assets for Crosswalk, Handicap Doors, Sextoys, Tesla Charge Port, and Unitree Go1 Robot Dog
- Assets: Includes Sonic Screw Driver sound for the Wav Player
- Assets: Running DolphinRestorer.fap on new install will auto-level to Level 7.
- [BadUSB: BadUSB as FAP #396 (By ESurge)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/396)
- BadUSB: Added ignore DUCKY_LANG cmd to retain compatibility with existing scripts [(Thanks to v1nc)](https://github.com/v1nc/flipperzero-firmware)
- BadUSB: Assets for Kiosk Evasion (By nocomp) and Wifi Stealer (By 7h30th3r0n3)
- BadUSB: [Dummy decoy/bad usb keyboard layout #1525 (By dummy-decoy)](https://github.com/flipperdevices/flipperzero-firmware/pull/1525)
- BadUSB: nb-NO Added norwegian keyboard layout [(By jd-raymaker)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/357)
- BadUSB: show script errors on screen [(By CromFr)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/200)
- BadUSB: sk-SK maping keybord for BadUsb [(By jaroslavmraz)](https://github.com/flipperdevices/flipperzero-firmware/pull/1619)
- Clock: Clock.fap loader apps and available as Favorites [Thanks to ESurge](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/336)
- Desktop: Hidden top bar [Thanks to ESurge](https://github.com/ESurge/)
- Desktop: Exclude icons on the left and show minimal battery [Thanks to skizzophrenic/Talking-Sasquach](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/360)
- Development free space thanks to removal of unused debug tools and [thanks to ESurge](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/46/files) for removal of first start assets.
- Dolphin: Assigned profile pic for levels 1-10 (Happy Lvl 1 Dolphin), 11-15 (Happy Lvl 2 Dolphin), 16-18 (Happy Lvl 3 Dolphin), 19-21 (Kid G0ku), 22-24 (Adult G0ku), 25-27 (SSJ G0ku) and 28-30 (SSJ3 G0ku)
- Dolphin: Expanded max level from 3 to 30 using [Roll20](https://roll20.net/compendium/dnd5e/Monsters#h-Experience%20Points), Increased max deed XP per action type from 15 to 45 exp daily & updated animation manifest for max level 30 for all animations (By RogueMaster)
- Dolphin: Internal manifest updated to have animations always available with max level 30
- Dolphin: Level animation sequence for level 3+ to be lvl1->2 for levels 1-20 and lvl2->3 for levels 21-30.
- Dolphin: Mood Stays Happy [(Thanks to biocage)](https://github.com/biocage/flipperzero-firmware), changed to Mood Stays Less Than 6 / Mood Stays High (but not in game mode) (By RogueMaster)
- Dolphin: Passport: DB Themed [(Thanks to Kuronons)]
- Dolphin: Passport: Show EXP [(By Dabolus)](https://github.com/Dabolus/flipperzero-firmware-rpg/)
- Dolphin: Changed daily MAX to 198 on all 7 Deed Types. Random Deed Selection used for MAX +3 EXP daily gain.
- Dolphin: Plugin Achivement +3 EXP for a total of up to 700 EXP daily. Plugin Achivements are:
	- Dice First Roll for d20+ = sides on dice (i.e. Nat 20 on d20)
	- Dice First Roll for d20+ = sides on dice - 1 (+1 EXP) (i.e. 19 on d20)
	- Games Only Mode from Lock Menu
	- Getting 2048 in 2048
	- Mouse Jacker Ducky Run
	- NRFSniffer Found Address
	- Stopwatch @ Alert
	- Tetris, Snake, or Flappy Bird Score For EXP
	- Zombiez for every 20 Zombie kills
- Dolphin: SD dolphin manifest updated to weight animations differently
- GPIO: Feature to read EEPROM of SFP Modules using I2C [(By marcusju)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/198)
- GPIO: [I²C-Scanner #1431 (By GitChris3004)](https://github.com/flipperdevices/flipperzero-firmware/pull/1431)
- iButton: [Fixed issue when loading iButton keys or U2F token from Archive app #382 (By ESurge)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/382)
- iButton: iButton.fap loader apps and available as Favorites [Thanks to ESurge](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/336)
- Icon Decode/Encode [(Thanks to PixlEmly)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/55/files)
- [Infrared: Infrared AS FAP #398 (By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/398) (With Thanks to ESurge) [Restored Infrared and RFID CLI #405 (By ESurge)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/405)
- Infrared: Universal AC, Audio, Fans & Projectors from [Unleashed/Eng1n33r](https://github.com/DarkFlippers/unleashed-firmware)
- Plugins: 2048, Arkanoid, Snake, and Tetris show score. Thanks to [whoamins](https://github.com/flipperdevices/flipperzero-firmware/commit/7feda832ede1ba8468eff2ca055fef3ddbdc16ac) and [DevMilanIan](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/188) With position changes by RogueMaster. Also all + Tic Tac Toe updated by [Unleashed/Eng1n33r](https://github.com/DarkFlippers/unleashed-firmware) for stability.
- Plugins: Icon for Clock [Thanks to Redlink](https://github.com/redlink2/flipperzero-firmware/tree/menuChanges)
- [RFID: LFRFID AS FAP #397 (By ROgueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/397) (With Thanks to ESurge) [Restored Infrared and RFID CLI #405 (By ESurge)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/405)
- Settings: "Lock W PIN + Off" add to UP menu [(By RogueMaster)]
- Settings: Actual PIN Lock [(By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/applications/desktop/desktop.c)
- Settings: Auto-Lock Options Added: 10s+15s+90s [(By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/applications/desktop/desktop_settings/scenes/desktop_settings_scene_start.c)
- Settings: Battery Meter on Desktop [Thanks to McAzzaMan](https://github.com/McAzzaMan/flipperzero-firmware/tree/BatteryPercentageView)
- Settings: Custom name with this compile: CUSTOM_FLIPPER_NAME=name ./fbt updater_package [By Unleashed/xMasterX](https://github.com/DarkFlippers/unleashed-firmware)
- Settings: Desktop => [Games Only Mode (By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/GAMES_ONLY.md)
- - [UP UP DOWN DOWN LEFT RIGHT LEFT RIGHT FROM CLOCK](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/GAMES_ONLY.md) (FULL LIST OF GAMES ONLY CONTROLS)
- Settings: LCD Timeout Options Added: 10s+90s+2min+5min+10min [(By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/applications/notification/notification_settings_app.c)
- Settings: Rename from App [(Thanks to E_Surge)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/409)
- Settings: Rename from SD `dolphin/name.txt` [(Thanks to E_Surge)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/259)
- Settings: Power: Fix for settings reload every second [(By lokiuox)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/364)
- Settings: Scan names will have timestamp instead of random name assigned for [NFC](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/lib/toolbox/random_name.c) and [SubGHz](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/applications/subghz/scenes/subghz_scene_read_raw.c) (By RogueMaster)
- Settings: Storage Info: [SD info: Add dynamic units and free % #1634 (By non-bin)](https://github.com/flipperdevices/flipperzero-firmware/pull/1634)
- Settings: Updated Dummy Mode mode to have access to 2048, Dice, Snake, Tetris & Zombiez [(By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/commit/bf964fffdd2c1d730623673987a6de32a3f7c92f)
- Settings: Updated GAMES ONLY mode to have access to 2048, Dice, Doom, Snake, Tetris & Zombiez [(By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/commit/bf964fffdd2c1d730623673987a6de32a3f7c92f)
- Settings: Updated HOLD UP to go to Primary Favorite [(By RogueMaster)]
- Settings: Updated HOLD DOWN to go to Secondary Favorite [(By RogueMaster)]
- Settings: Updated Left to go to Clock [(By RogueMaster)]
- Settings: Updated HOLD LEFT to go to SubGhz Remote [(By RogueMaster)]
- SubGHz: Auto Detect Raw in READ action (Needs To Be Enabled In Read Settings) [from perspecdev](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/152)
- SubGHz: [Add settings to subghz read functionality to allow setting RSSI threshold (raw only) (By PolymerPrints)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/184)
- SubGHz: Extended ranges enabled through flag in /ext/subghz/assets/extend_range.txt [from tkerrby](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/116)
- SubGHz: Moved setting_user file to setting_user.txt! This makes it changable from IOS app. (By RogueMaster)
- SubGHz: New frequency analyzer [(By ClusterM)](https://github.com/flipperdevices/flipperzero-firmware/pull/1501) [feedback mode (by darmiel)](https://github.com/darmiel/flipper-playlist/tree/feat/stealth-frequency-analyzer) [Quiet Mode (by Himura2la)](https://github.com/ClusterM/flipperzero-firmware/pull/1) [New frequency analyzer #1557 (By ClusterM)](https://github.com/flipperdevices/flipperzero-firmware/pull/1557)
- SubGHz: Protocols An-Motors, BFT Mitto, Came Atomo, FAAC SLH (Spa), HCS101, Keeloq, Keeloq Common, Nice Flor S, SecPlus v1+v2 and Star Line updates from [Eng1n33r](https://github.com/DarkFlippers/unleashed-firmware)
- SubGHz: Unlock from SD flag from [(cloudbreakdaniel)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/commit/97db0dc91ee3dff812b4dec0618e3f198de14405).  Update `subghz/assets/extend_range.txt` with [this file](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/assets/resources/subghz/assets/extend_range.txt) on SD.  **UPDATE IGNORE FLAG TO TRUE TO UNLEASH YOUR FLIPPER!!**
- U2F: U2F.fap loader apps and available as Favorites [Thanks to ESurge](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/336)

## Games
  
- [15 (By x27)](https://github.com/x27/flipperzero-game15)
- [2048 (By OlegSchwann)](https://github.com/OlegSchwann/flipperzero-firmware/tree/hackaton/game_2048/applications/game-2048) [(Score By DevMilanIan)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/186)
- [Arkanoid (By gotnull)](https://github.com/gotnull/flipperzero-firmware-wPlugins) [(Score By DevMilanIan)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/188)
- [BlackJack (By teeebor)](https://github.com/teeebor/flipper_games)
- [Chess (By Okalachev)](https://github.com/okalachev/flipperzero-firmware/tree/chess) Crashes 1st load if FW <~750KB or every load on larger FW  `Broken?`
- [Dice Roller Including SEX/WAR/8BALL/WEED DICE (By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/applications/dice/dice.c)
- [Dice (By Ka3u6y6a)](https://github.com/Ka3u6y6a/flipper-zero-dice)
- [Doom (By p4nic4ttack)](https://github.com/p4nic4ttack/doom-flipper-zero/)
- [Flappy Bird (By DroomOne)](https://github.com/DroomOne/flipperzero-firmware/tree/dev/applications/flappy_bird) [Flappy: Border hitboxes, bigger Pilars (By TQMatvey)](https://github.com/DarkFlippers/unleashed-firmware/pull/114) [Increase pilars line width to improve visibility (By ahumeniy)](https://github.com/DarkFlippers/unleashed-firmware/pull/140)
- [Game of Life (Updated to work by tgxn) (By itsyourbedtime)](https://github.com/tgxn/flipperzero-firmware/blob/dev/applications/game_of_life/game_of_life.c)
- [Heap Defence (By xMasterX)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/commit/fc776446de9fdd553b221c02668b925b689378d8) [(original by wquinoa & Vedmein)](https://github.com/Vedmein/flipperzero-firmware/tree/hd/svisto-perdelki)
- [Mandelbrot Set (By Possibly-Matt)](https://github.com/Possibly-Matt/flipperzero-firmware-wPlugins)
- [Minesweeper (By panki27)](https://github.com/panki27/minesweeper)
- [Monty Hall (By DevMilanIan)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/203)
- [Scorched Tanks (By jasniec)](https://github.com/jasniec/flipper-scorched-tanks-game)
- [Snake (By OlegSchwann)-OFW](https://github.com/flipperdevices/flipperzero-firmware/pull/829)(With updates from DrZlo13, xMasterX, QtRoS and RogueMaster) [Snake Score Saving (By JuanJakobo)](https://github.com/flipperdevices/flipperzero-firmware/pull/1922) [Turns anywhere (By TQMatvey)](https://github.com/DarkFlippers/unleashed-firmware/pull/125) [Food Spawns Anywwhere (By TQMatvey)](https://github.com/DarkFlippers/unleashed-firmware/pull/130)
- [Solitaire (By teeebor)](https://github.com/teeebor/flipper_games)
- [T-Rex (By gelin)](https://github.com/gelin/t-rex-runner) WIP
- [TAMA P1 (By GMMan)](https://github.com/GMMan/flipperzero-firmware/tree/tama-p1) requires [this rom](https://tinyurl.com/tamap1) IN `tama_p1` on SD as `rom.bin` to make it work.
- [Tanks (By Alexgr13)](https://github.com/alexgr13/flipperzero-firmware/tree/fork/dev/applications/tanks-game)
- [Tetris (By jeffplang)](https://github.com/jeffplang/flipperzero-firmware/tree/tetris_game/applications/tetris_game)
- [Tic Tac Toe (By gotnull)](https://github.com/gotnull/flipperzero-firmware-wPlugins)
- [Video Poker (By PixlEmly)](https://github.com/PixlEmly/flipperzero-firmware-testing/blob/420/applications/VideoPoker/poker.c)
- [Zombiez (Reworked By DevMilanIan)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/240) [(Original By Dooskington)](https://github.com/Dooskington/flipperzero-zombiez)

## Plugins

- [Air Mouse (By ginkage)](https://github.com/ginkage/FlippAirMouse/)
- [Authenticator/TOTP (By akopachov)](https://github.com/akopachov/flipper-zero_authenticator)
- [Bad Apple (By GMMan)](https://github.com/GMMan/flipperzero-badapple) `No working video.bin found`
- [Barcode Generator (By McAzzaMan)](https://github.com/McAzzaMan/flipperzero-firmware/tree/UPC-A_Barcode_Generator/applications/barcode_generator)
- [Bluetooth Remote (By Cutch)-OFW](https://github.com/flipperdevices/flipperzero-firmware/pull/1330)
- [BPM Tapper (By panki27)](https://github.com/panki27/bpm-tapper)
- [Calculator (By n-o-T-I-n-s-a-n-e)](https://github.com/n-o-T-I-n-s-a-n-e)
- [Ceasar Cipher (By panki27)](https://github.com/panki27/caesar-cipher)
- [Clock/Stopwatch (By CompaqDisc, Stopwatch & Sound Alert By RogueMaster)](https://gist.github.com/CompaqDisc/4e329c501bd03c1e801849b81f48ea61) [12/24HR (By non-bin)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/254) [Refactoring (By GMMan)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/256)
- [Counter (By Krulknul)](https://github.com/Krulknul/dolphin-counter)
- [DAP Link (By DrZlo13)-OFW](https://github.com/flipperdevices/flipperzero-firmware/pull/1897)
- [Deauther PWNDTOOLS V2.6.0 (By HEX0DAYS)](https://github.com/HEX0DAYS/FlipperZero-PWNDTOOLS) `Req: ESP8266` [Original](https://github.com/SpacehuhnTech/esp8266_deauther)
- [DHT Temp Monitor (By quen0n)](https://github.com/quen0n/FipperZero-DHT-Monitor) `Req: DHT11/DHT22(AM2302)/AM2301`
- [Distance Sensor (By Sanqui)](https://github.com/Sanqui/flipperzero-firmware/tree/hc_sr04)) `Req: HC-SR04` Ported/Modified by xMasterX
- [Dolphin Backup (By nminaylov)-OFW](https://github.com/flipperdevices/flipperzero-firmware/pull/1384) Modified by RogueMaster
- [Dolphin Restorer (By nminaylov)](https://github.com/flipperdevices/flipperzero-firmware/pull/1384) Cloned by RogueMaster
- [DSTIKE Deauther (By SequoiaSan)](https://github.com/SequoiaSan/FlipperZero-Wifi-ESP8266-Deauther-Module/tree/FlipperZero-Module-v2/FlipperZeroModule/FlipperZero-ESP8266-Deauth-App) `Req: ESP8266`
- [DTMF Dolphin (By litui)](https://github.com/litui/dtmf_dolphin)
- [Flashlight (By xMasterX)](https://github.com/xMasterX/flipper-flashlight)
- [GPS (By ezod)](https://github.com/ezod/flipperzero-gps) `Req: NMEA 0183`
- [HEX Viewer (By QtRoS)](https://github.com/QtRoS/flipperzero-firmware)
- [iButton Fuzzer (By xMasterX)](https://github.com/DarkFlippers/unleashed-firmware)
- [i2c Tools (By NaejEL)](https://github.com/NaejEL/flipperzero-i2ctools)
- [Lightmeter (By oleksiikutuzov)](https://github.com/oleksiikutuzov/flipperzero-lightmeter) `Req: BH1750`
- [IFTTT Virtual Button (By Ferrazzi)](https://github.com/Ferrazzi/FlipperZero_IFTTT_Virtual_Button) `Req: ESP8266 w/ IFTTT FW Flashed`
- [Metronome (By panki27)](https://github.com/panki27/Metronome)
- [Morse Code (By wh00hw)](https://github.com/DarkFlippers/unleashed-firmware/pull/144)
- [Mouse Jacker (By mothball187)](https://github.com/mothball187/flipperzero-nrf24/tree/main/mousejacker) ([Pin Out](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/tree/420/applications/mousejacker) from nocomp/Frog/UberGuidoZ) `Req: NRF24`
- [Mouse Jiggler (By Jacob-Tate)](https://github.com/Jacob-Tate/flipperzero-firmware/blob/dev/applications/mouse_jiggler/mouse_jiggler.c) (Original By MuddleBox)
- [Multi Converter (By theisolinearchip)](https://github.com/theisolinearchip)
- [Music Beeper (By DrZlo13)](https://github.com/flipperdevices/flipperzero-firmware/pull/1189) (With Changes By qqMajiKpp/Haseo)
- [Music Player (By DrZlo13)-OFW](https://github.com/flipperdevices/flipperzero-firmware/pull/1189)
- [NFC Magic (By gornekich)](https://github.com/flipperdevices/flipperzero-firmware/pull/1966)
- [NRF Sniff (By mothball187)](https://github.com/mothball187/flipperzero-nrf24/tree/main/nrfsniff) ([Pin Out](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/tree/420/applications/nrfsniff) from nocomp/Frog/UberGuidoZ) `Req: NRF24`
- [NRF24 Scanner (By vad7)](https://github.com/vad7/nrf24scan)
- [Ocarina (By invalidna-me)](https://github.com/invalidna-me/flipperzero-ocarina) [Here are the LOTZ Songs](https://www.zeldadungeon.net/wiki/Ocarina_of_Time_Songs)
- [Paint (By n-o-T-I-n-s-a-n-e)](https://github.com/n-o-T-I-n-s-a-n-e)
- [Password Generator (By anakod)](https://github.com/anakod/flipper_passgen)
- [PicoPass Reader (By Bettse)](https://github.com/flipperdevices/flipperzero-firmware/pull/1366)
- [Pomodoro Timer (By sbrin)](https://github.com/sbrin/flipperzero_pomodoro)
- [RFID Fuzzer (By Ganapati)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/245) [Changes by Unleashed/xMasterX](https://github.com/DarkFlippers/unleashed-firmware)
- [RF Remix (By ESurge)](https://github.com/ESurge/flipperzero-firmware-unirfremix) [(Original By jimilinuxguy)](https://github.com/jimilinuxguy/flipperzero-universal-rf-remote/tree/028d615c83f059bb2c905530ddb3d4efbd3cbcae/applications/jukebox) [(More protocols thanks to darmiel & xMasterX)](https://github.com/darmiel/flipper-playlist/blob/feat/unirf-protocols/applications/unirfremix/unirfremix_app.c)
- [RC2014 ColecoVision (By ezod)](https://github.com/ezod/flipperzero-rc2014-coleco)
- [SAM (By Unknown)][Original?](https://github.com/ctoth/SAM)
- [Sentry Safe (By H4ckd4ddy)](https://github.com/H4ckd4ddy/flipperzero-sentry-safe-plugin) ([Pin Out](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/tree/420/applications/sentry_safe) from [UberGuidoZ](https://github.com/UberGuidoZ/))
- [Signal Generator (By nminaylov)-OFW](https://github.com/flipperdevices/flipperzero-firmware/pull/1793)
- [Spectrum Analyzer (By jolcese)](https://github.com/jolcese/flipperzero-firmware/tree/spectrum/applications/spectrum_analyzer) [Updates (for testing) Thanks to theY4Kman](https://github.com/theY4Kman/flipperzero-firmware)
- [Sub-GHz Bruteforcer (By Ganapati & xMasterX)](https://github.com/derskythe/flipperzero-subbrute/tree/master)
- [Sub-GHz Playlist (By darmiel)](https://github.com/darmiel/flipper-playlist)
- [Temperature Sensor (By Mywk)](https://github.com/Mywk/FlipperTemperatureSensor) `Req: HTU2XD, SHT2X, SI702X, SI700X, SI701X or AM2320`
- [Temperature Sensor (By xMasterX)](https://github.com/DarkFlippers/unleashed-firmware/commit/9c4612e571db42f5e6123a3f159e01337453a6af) `Req: AM2320`
- [Timelapse (By theageoflove)](https://github.com/theageoflove/flipperzero-zeitraffer)
- [Tuning Fork (By besya)](https://github.com/besya/flipperzero-tuning-fork)
- [UART Echo (By DrZlo13)-OFW](https://github.com/flipperdevices/flipperzero-firmware/pull/831)
- [USB HID Autofire (By pbek)](https://github.com/pbek/usb_hid_autofire)
- [USB Keyboard (By huuck)](https://github.com/huuck/FlipperZeroUSBKeyboard)
- [WAV Player (By DrZlo13)](https://github.com/flipperdevices/flipperzero-firmware/tree/zlo/wav-player) Updated by Atmanos & RogueMaster To Work
- [WiFi (Deauther) V2 (By Timmotools)](https://github.com/Timmotools/flipperzero_esp8266_deautherv2) `Req: ESP8266` 
- [WiFi (Marauder) (By 0xchocolate)](https://github.com/0xchocolate/flipperzero-firmware-with-wifi-marauder-companion) `Req: ESP32 WITH MARAUDER FLASHED`
- [WiFi Scanner v.0.4 (By SequoiaSan)](https://github.com/SequoiaSan/FlipperZero-WiFi-Scanner_Module-ESP8266) `Req: ESP8266 or ESP32`
- [Wii EC Analyser (By csBlueChip)](https://github.com/csBlueChip/FlipperZero_WiiEC)
- [Zero Tracker (By DrZZlo13)](https://github.com/DrZlo13/flipper-zero-music-tracker)

## Open PRs Checked Out & Not Merged In Main
  
- [Automatic shutdown on idle #1647 (By SHxKenzuto)](https://github.com/flipperdevices/flipperzero-firmware/pull/1647)
- [Decode RAW recordings #1667 (By qistoph)](https://github.com/flipperdevices/flipperzero-firmware/pull/1667)
- [NFC - Machine Readable Travel Documents #1866 (By qistoph)](https://github.com/flipperdevices/flipperzero-firmware/pull/1866)
- [nfc: NTAG password auto capture (and other password-related changes) #1843 (By GMMan)](https://github.com/flipperdevices/flipperzero-firmware/pull/1843)
- [Snake Plugin: Store game state on close and restore it on restart, show highscore #1922 (By JuanJakobo)](https://github.com/flipperdevices/flipperzero-firmware/pull/1922)
- [Improve file name filtering #2047 (By gsurkov)](https://github.com/flipperdevices/flipperzero-firmware/pull/2047)
- [NFC: Accept non-parsed apps in Mifare DESFire. #2041 (By mh-)](https://github.com/flipperdevices/flipperzero-firmware/pull/2041)
- [Allow "Detect reader" for unsaved card #2045 (By niki-timofe)](https://github.com/flipperdevices/flipperzero-firmware/pull/2045)
- [Gui: proper navigation in file browser dialog #2014 (By skotopes)](https://github.com/flipperdevices/flipperzero-firmware/pull/2014)
- [(WIP) added ISO15693 reading, saving and revealing from privacy mode (unlock) #1991 (By g3gg0)](https://github.com/flipperdevices/flipperzero-firmware/pull/1991)
- [SubGhz: fix duration pricenton protocol #2054 (By Skorpionm)](https://github.com/flipperdevices/flipperzero-firmware/pull/2054)
- [POCSAG protocol decoder for subghz #2055 (By Shuma)](https://github.com/flipperdevices/flipperzero-firmware/pull/2055)
