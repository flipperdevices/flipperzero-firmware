## Doing a direct unleashed fork to implement FAPS and all the old stuff will be re-implemented.

To Be Added/To Verify Present:

- Games & Plugins?
- Assets: Running DolphinRestorer.fap on new install will auto-level to the end of Level 2.
- GPIO: Feature to read EEPROM of SFP Modules using I2C [(By marcusju)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/198)
- Development free space thanks to removal of unused debug tools and [thanks to ESurge](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/46/files) for removal of first start assets.
- Plugins: Menu, Icons, and Raycast exit [Thanks to Redlink](https://github.com/redlink2/flipperzero-firmware/tree/menuChanges)
- Settings: Favorite Game by holding UP on Desktop [Thanks to gotnull](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/57)
- Settings: Hold Down for Games Menu [(Thanks to ESurge)](https://github.com/ESurge/flipperzero-firmware-wPlugins)
- Settings: Rename from SD `dolphin/name.txt` [(Thanks to E_Surge)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/259)
- Settings: Scan names will have timestamp instead of random name assigned for [NFC](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/lib/toolbox/random_name.c) and [SubGHz](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/applications/subghz/scenes/subghz_scene_read_raw.c) (By RogueMaster)
- SubGHz: Auto Detect Raw in READ action (Needs To Be Enabled In Read Settings) [from perspecdev](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/152)
- SubGHz: [Add settings to subghz read functionality to allow setting RSSI threshold (raw only) (By PolymerPrints)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/184)

Implemented:

- Animations: Hold Center to change Flipper idle animation. [Thanks to Zycenios](https://github.com/flipperdevices/flipperzero-firmware/commit/111786ef40e50a40d2e510595672b569d9b97bba) With changes by RogueMaster.
- Animations: iButton and RFID P0kem0n images [Thanks to Panzer00Z](https://github.com/Panzer00Z/flipperzero-firmware/)
- Animations: Idle animations will show all animations regardless of level and butthurt [Thanks to qqMajiKpp]
- Animations: RM FW Update image [(Thanks to E_Surge)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/257)
- Animations: SubGHZ Scanning image with Pikachu [Thanks to Panzer00Z](https://github.com/Panzer00Z/flipperzero-firmware/blob/3a548ea9bb181c9348d8afb427890c411456134e/assets/icons/SubGhz/Scanning_123x52.png)
- Assets: Includes a NFC Level 50 Gan0n Amiibo
- Assets: Includes a NFC Rick Roll link
- Assets: Includes New Dolphin Animations: [Rick Roll, Matrix & Swim animations (By qqMajiKpp)], [Rukamon 2x and Agumon animations (By Syrius)], [Mew (By Arkaivos)], [Eye of the Flipper (By Kuronons)], [Shodan (By qqMajiKpp)], [Sirene & The Witch (By Haseo)], [Dino & A New Hope (By Haseo)], [Earth Arcadia (By Kuronons)], [Kuronons Black Flags Collection (21 Animations)(By Kuronons)], [D.Va (By Haseo)], [GITS (By Haseo)], [Slayers (By qqMajiKpp)], [P0liwhirl (By Panzer00Z)], [RogueMaster CFW Animation (By Kuronons)], [Nyan Cat (By Haseo)], [L (By Kuronons)], [Laughing Man (By Kuronons)], [B0ws3r (By Haseo)], [Kuronons Black Flags Collection VOL 2 (9 Animations)(By Kuronons)], [Thanks for all the fish (By qqMajiKpp)], [OCP, Skynet and Weyland (By Kuronons)], [Mario (By Haseo)], [Umbrella (By Haseo)], [Trioptimum (By qqMajiKpp)], [Tyrell & Cyberdyne (By Kuronons)], [Allen & Maha (By Haseo)], [Starfield (By qqMajiKpp)] & [DJ (By ut1s)]
- Assets: Includes sample Music Player tunes
- Assets: Includes sample SubGHz assets for Crosswalk, Handicap Doors, Sextoys, Tesla Charge Port, and Unitree Go1 Robot Dog
- Assets: Includes Sonic Screw Driver sound for the Wav Player
- BadUSB: Added ignore DUCKY_LANG cmd to retain compatibility with existing scripts [(Thanks to v1nc)](https://github.com/v1nc/flipperzero-firmware)
- BadUSB: Assets for Kiosk Evasion and Wifi Stealer
- BadUSB: sk-SK maping keybord for BadUsb [(By jaroslavmraz)](https://github.com/flipperdevices/flipperzero-firmware/pull/1619)
- BadUSB: pt-BR maping keybord for BadUsb [(By web-mancha)](https://github.com/Eng1n33r/flipperzero-firmware/pull/65)
- BadUSB: show script errors on screen [(By CromFr)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/200)
- Dolphin: Assigned profile pic for levels 1-10, 11-20 and 21-30 respectively from default lvl 1, 2 & 3
- Dolphin: Expanded max level from 3 to 30 using [Roll20](https://roll20.net/compendium/dnd5e/Monsters#h-Experience%20Points), Increased max deed XP per action type from 15 to 45 exp daily & updated animation manifest for max level 30 for all animations (By RogueMaster)
- Dolphin: Internal manifest updated to have animations always available with max level 30
- Dolphin: Level animation sequence for level 3+ to be lvl1->2 for levels 1-20 and lvl2->3 for levels 21-30.
- Dolphin: Mood Stays Happy [(Thanks to biocage)](https://github.com/biocage/flipperzero-firmware), changed to Mood Stays Less Than 6 / Mood Stays High (but not in game mode) (By RogueMaster)
- Dolphin: Passport: DB Themed [(Thanks to Kuronons)]
- Dolphin: Passport: Show EXP [(By Dabolus)](https://github.com/Dabolus/flipperzero-firmware-rpg/)
- Dolphin: Changed daily MAX to 198 on all 7 Deed Types. Random Deed Selection used for MAX +3 EXP daily gain.
- Dolphin: Plugin Achivement +3 EXP for a total of up to 700 EXP daily. Plugin Achivements are:
- - Dice First Roll for d20+ = sides on dice (i.e. Nat 20 on d20)
- - Dice First Roll for d20+ = sides on dice - 1 (+1 EXP) (i.e. 19 on d20)
- - Games Only Mode from Lock Menu
- - Getting 2048 in 2048
- - Mouse Jacker Ducky Run
- - NRFSniffer Found Address
- - Stopwatch @ Alert
- - Tetris, Snake, or Flappy Bird Score For EXP
- - Zombiez for every 20 Zombie kills
- Dolphin: SD dolphin manifest updated to weight animations differently
- Icon Decode/Encode [(Thanks to PixlEmly)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/55/files)
- IR: Universal AC, Audio, Fans & Projectors from [Unleashed/Eng1n33r](https://github.com/Eng1n33r/flipperzero-firmware)
- Plugins: 2048, Arkanoid, Snake, and Tetris show score. Thanks to [whoamins](https://github.com/flipperdevices/flipperzero-firmware/commit/7feda832ede1ba8468eff2ca055fef3ddbdc16ac) and [DevMilanIan](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/188) With position changes by RogueMaster. Also all + Tic Tac Toe updated by [Unleashed/Eng1n33r](https://github.com/Eng1n33r/flipperzero-firmware) for stability.
- Settings: Actual PIN Lock [(By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/applications/desktop/desktop.c)
- Settings: Auto-Lock Options Added: 10s+15s+90s [(By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/applications/desktop/desktop_settings/scenes/desktop_settings_scene_start.c)
- Settings: Battery Meter on Desktop [Thanks to McAzzaMan](https://github.com/McAzzaMan/flipperzero-firmware/tree/BatteryPercentageView)
- Settings: Custom name with this compile: CUSTOM_FLIPPER_NAME=name ./fbt updater_package [By Unleashed/xMasterX](https://github.com/Eng1n33r/flipperzero-firmware)
- Settings: Desktop => [Games Only Mode (By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/GAMES_ONLY.md) [(Thanks to Astrrra for Inverted Mode to Mimic)](https://github.com/wetox-team/flipperzero-firmware/commit/ce91582b7417c5d7a9d8416c17a102d3a5868238)
- - [UP UP DOWN DOWN LEFT RIGHT LEFT RIGHT FROM CLOCK](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/GAMES_ONLY.md)<== FULL LIST OF GAMES ONLY CONTROLS
- Settings:  "DUMB Mode" is now "Lock W PIN + Off" [(By RogueMaster)]
- Settings: LCD Timeout Options Added: 10s+90s+2min+5min+10min [(By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/applications/notification/notification_settings_app.c)
- SubGHz: Extended ranges enabled through flag in /ext/subghz/assets/extend_range.txt [from tkerrby](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/116)
- SubGHz: Moved setting_user file to setting_user.txt! This makes it changable from IOS app. (By RogueMaster)
- SubGHz: New frequency analyzer [(By ClusterM)](https://github.com/flipperdevices/flipperzero-firmware/pull/1501) [feedback mode (by darmiel)](https://github.com/darmiel/flipper-playlist/tree/feat/stealth-frequency-analyzer)
- SubGHz: Protocols An-Motors, BFT Mitto, Came Atomo, FAAC SLH (Spa), HCS101, Keeloq, Keeloq Common, Nice Flor S, SecPlus v1+v2 and Star Line updates from [Eng1n33r](https://github.com/Eng1n33r/flipperzero-firmware)
- SubGHz: Unlock from SD flag from [(cloudbreakdaniel)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/commit/97db0dc91ee3dff812b4dec0618e3f198de14405).  Update `subghz/assets/extend_range.txt` with [this file](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/420/assets/resources/subghz/assets/extend_range.txt) on SD.  **UPDATE IGNORE FLAG TO TRUE TO UNLEASH YOUR FLIPPER!!**