<h2>

[Flipper Zero Firmware](https://github.com/flipperdevices/flipperzero-firmware/blob/dev/ReadMe.md) <= READ THIS READ ME</h2>
#### Thank you to all the supporters; this firmware is a fork of [Unleashed/Eng1n33r](https://github.com/Eng1n33r/flipperzero-firmware) & [the main Flipper Devices FW](https://github.com/flipperdevices/flipperzero-firmware)! I will try to keep active development and updates from both in this build along with any other projects that can be found to be useful to the community. I try to keep this FW build the most cutting edge with updates from both and updates from active community projects. All features and projects pulled are listed in expandable sections below. Please do [support us](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/SUPPORT.md), [Eng1n33r](https://github.com/Eng1n33r/flipperzero-firmware) & [Flipper Devices](https://github.com/flipperdevices/flipperzero-firmware)! Everyone gives much of their free time to ensure the success of the Flipper Zero!

<b>Latest Updates:</b>

- TOFIX - FAP Loader disabled so the FW compiles. Compiling requires api_symbols.csv manipulation and some FAPs are not working, we are aware.
- Latest OFW updates: [#1524](https://github.com/flipperdevices/flipperzero-firmware/pull/1524), [#1492](https://github.com/flipperdevices/flipperzero-firmware/pull/1492), [#1496](https://github.com/flipperdevices/flipperzero-firmware/pull/1496), [#1520](https://github.com/flipperdevices/flipperzero-firmware/pull/1520), [#1523](https://github.com/flipperdevices/flipperzero-firmware/pull/1523), [#1513](https://github.com/flipperdevices/flipperzero-firmware/pull/1513), [#1462](https://github.com/flipperdevices/flipperzero-firmware/pull/1462), [#1528](https://github.com/flipperdevices/flipperzero-firmware/pull/1528) & [#1438](https://github.com/flipperdevices/flipperzero-firmware/pull/1438)
- Excluded [App RPC Bug Fixes and redesign #1491](https://github.com/flipperdevices/flipperzero-firmware/pull/1491) since it breaks compile

<table width="100%" border="0" cellspacing="0">
  <tr> <td colspan=2> <h3>This software is for experimental purposes only and is not meant for any illegal activity/purposes. We do not condone illegal activity and strongly encourage keeping transmissions to legal/valid uses allowed by law.</h3> </td> </tr>
  <tr> <td>
<br><b>FLASH STOCK FIRST BEFORE UPDATING TO CUSTOM FIRMWARE<b>

<details>
  <summary><B>HOW TOs</b></summary><br/>
  
- [- HERE IS A GUIDE FOR INSTALL (BY PINGYWON)](https://flipper.pingywon.com/)
- [- HERE IS A NOOB GUIDE TO FLASH AND UNLOCK (BY interestingsoup)](https://interestingsoup.com/n00b-guide-flashing-flipper-zero-to-rougemaster/)
- [- How to install firmware](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/documentation/HowToInstall.md)
- [- How to build firmware](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/documentation/HowToBuild.md)
- [- BadUSB: how to use different keyboard layouts](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/documentation/BadUSBLayouts.md)
- [- Configure UniversalRF Remix App](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/documentation/UniRFRemix.md)
- [- How to use: NRF24 plugins](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/documentation/NRF24.md)
- [- How to use: SentrySafe plugin](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/documentation/SentrySafe.md)
- [- Barcode Generator readme](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/documentation/BarcodeGenerator.md)
- [- How to extend SubGHz frequency range](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/documentation/DangerousSettings.md)
- [- How to add extra SubGHz frequencies](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/documentation/SubGHzSettings.md)
</details>

      
- To install new FW, extract the [latest release zip file](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/releases) to a folder, put the folder in the update folder on your SD card and run the update file inside the folder using the Archive app (down from flipper desktop). If you were previously unleashed, you need to update your extend_range.txt file. **UPDATE IGNORE FLAG TO TRUE TO UNLEASH YOUR FLIPPER!!** On any update, you may need to update this file and your unirf map file, so keep backups. 😄
    - [<h2>GAMES ONLY MODE PASSWORD: UP UP DOWN DOWN LEFT CENTER LEFT CENTER FOR MAIN MENU</h2>](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/GAMES_ONLY.md)

<br>[FLIPPER PROJECT WISH LIST](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/RoadMap.md) - [SAMPLE EDUCATIONAL PROJECTS 😄](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/RoadMap.md)
<br>[`FUN Links HERE` Collection for your Flipper SD](https://github.com/RogueMaster/awesome-flipperzero-withModules)
</td> <td>

<h2>DONATIONS ACCEPTED 😄🚀💸</h2>
BTC: <B>3MPQbKmGRCstg4FjnadfHa3woCT94JkR2a</B><br>
ETH: <B>0xC32Ea488DBeCF95992A5C81BD411e56Bd418BC5f</B>
    <br> <br>
    
Contact me on [Discord](https://discord.gg/gF2bBUzAFe) if you want a renamed Flipper DFU. I can do custom names too!
<br><h2>[Join my Flipper/Amiibo Discord](https://discord.gg/gF2bBUzAFe)</h2>
<details>
  <summary><B>You should clone with</B></summary></br>

  ```shell
$ git clone --recursive https://github.com/RogueMaster/flipperzero-firmware-wPlugins.git
$ cd flipperzero-firmware-wPlugins/
$ ./fbt resources icons
$ ./fbt --with-updater updater_package

# If building FAPS:
$ ./fbt plugin_dist FIRMWARE_APP_SET=ext_apps
```
</details>
      </td> </tr>
      <tr> <td>

<details>
  <summary><B>All Changes/Features</b></summary><br/>
  
- Animations: Hold Center to change flipper idle animation. [Thanks to Zycenios](https://github.com/flipperdevices/flipperzero-firmware/commit/111786ef40e50a40d2e510595672b569d9b97bba) With changes by RogueMaster.
- Animations: Idle animations will show all animations regardless of level and butthurt [Thanks to qqMajiKpp]
- Animations: SubGHZ Scanning image with Pikachu [Thanks to Panzer00Z](https://github.com/Panzer00Z/flipperzero-firmware/blob/3a548ea9bb181c9348d8afb427890c411456134e/assets/icons/SubGhz/Scanning_123x52.png)
- Assets: Includes a NFC Level 50 Link Amiibo
- Assets: Includes a NFC Rick Roll link
- Assets: Includes sample Music Player tunes
- Assets: Includes sample SubGHz assets for Crosswalk, Handicap Doors, Sextoys and Tesla Charge Port
- Assets: Includes Sonic Screw Driver sound for the Wav Player
- Assets: Running DolphinRestorer.fap on new install will auto-level to the end of Level 2.
- BadUSB: DUCKY_LANG keyword support [(Thanks to v1nc)](https://github.com/v1nc/flipperzero-firmware)(Supports US, DE, FR, [HU by terzinnorbert](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/135), [BE By Bramard](https://github.com/v1nc/flipperzero-firmware/pull/4/files))
- Development free space thanks to removal of unused debug tools and [thanks to ESurge](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/46/files) for removal of first start assets.
- Dolphin: Assigned profile pic for levels 1-10, 11-20 and 21-30 respectively from default lvl 1, 2 & 3
- Dolphin: Expanded max level from 3 to 30 using [Roll20](https://roll20.net/compendium/dnd5e/Monsters#h-Experience%20Points), Increased max deed XP per action type from 15 to 45 exp daily & updated animation manifest for max level 30 for all animations (By RogueMaster)
- Dolphin: Internal manifest updated to have animations always available with max level 30
- Dolphin: Level animation sequence for level 3+ to be lvl1->2 for levels 1-20 and lvl2->3 for levels 21-30.
- Dolphin: Mood Stays Happy [(Thanks to biocage)](https://github.com/biocage/flipperzero-firmware)
- Dolphin: SD dolphin manifest updated to weight animations differently
- Icon Decode/Encode [(Thanks to PixlEmly)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/55/files)
- NFC: Display UL PWD_AUTH payload / ntag-pwd-capture [(Thanks to GMMan)](https://github.com/flipperdevices/flipperzero-firmware/pull/1471)
- Plugins: Fixed sound decay issues on music player [Thanks to qqMajiKpp]
- Plugins: Menu, Icons and Raycast exit [Thanks to Redlink](https://github.com/redlink2/flipperzero-firmware/tree/menuChanges)
- Plugins: Snake and Tetris show score. [Thanks to whoamins](https://github.com/flipperdevices/flipperzero-firmware/commit/7feda832ede1ba8468eff2ca055fef3ddbdc16ac) With changes by RogueMaster.
- RFID: Direct Emulation from [(atomsmasha)](https://github.com/flipperdevices/flipperzero-firmware/pull/1371)
- Settings: Actual PIN Lock [(By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/applications/desktop/desktop.c)
- Settings: Auto-Lock Options Added: 10s+15s+90s [(By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/applications/desktop/desktop_settings/scenes/desktop_settings_scene_start.c)
- Settings: Battery Meter on Desktop [Thanks to McAzzaMan](https://github.com/McAzzaMan/flipperzero-firmware/tree/BatteryPercentageView)
- Settings: Desktop => [Games Only Mode (By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/GAMES_ONLY.md) [(Thanks to Astrrra for Inverted Mode to Mimic)](https://github.com/wetox-team/flipperzero-firmware/commit/ce91582b7417c5d7a9d8416c17a102d3a5868238)
- - [UP UP DOWN DOWN LEFT CENTER LEFT CENTER FOR MAIN MENU](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/GAMES_ONLY.md)<== FULL LIST OF GAMES ONLY CONTROLS
- Settings: Favorite Game by holding UP on Desktop [Thanks to gotnull](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/57)
- Settings: Hold Down for Games Menu [(Thanks to ESurge)](https://github.com/ESurge/flipperzero-firmware-wPlugins)
- Settings: LCD Timeout Options Added: 10s+90s+2min+5min+10min [(By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/applications/notification/notification_settings_app.c)
- Settings: Renamed flippers have the correct name for Bluetooth pairing [(By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/firmware/targets/f7/furi_hal/furi_hal_version.c#L95)
- Settings: Scan names will have timestamp instead of random name assigned for [NFC](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/lib/toolbox/random_name.c) and [SubGHz](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/applications/subghz/scenes/subghz_scene_read_raw.c) (By RogueMaster)
- SubGHz: Auto Detect Raw in READ action (Needs To Be Enabled In Read Settings) [from perspecdev](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/152)
- SubGHz: Extended ranges enabled through flag in /ext/subghz/assets/extend_range.txt [from tkerrby](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/pull/116)
- SubGHz: Protocols An-Motors, BFT Mitto, Came Atomo, FAAC SLH (Spa), HCS101, Keeloq, Keeloq Common, Nice Flor S, SecPlus v1+v2 and Star Line updates from [Eng1n33r](https://github.com/Eng1n33r/flipperzero-firmware)
- SubGHz: Unlock from SD flag from [(cloudbreakdaniel)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/commit/97db0dc91ee3dff812b4dec0618e3f198de14405).  Update `subghz/assets/extend_range.txt` with [this file](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/assets/resources/subghz/assets/extend_range.txt) on SD.  **UPDATE IGNORE FLAG TO TRUE TO UNLEASH YOUR FLIPPER!!**

 </details>


<details>
  <summary><B>Open PRs Checkedout & Not Merged In Main:</b></summary><br/>
  
- [`ELF Loader`/[FL-2627] Plugins API #1387 (By hedger/DrZlo13)](https://github.com/flipperdevices/flipperzero-firmware/pull/1387)
- [NFC: Display UL PWD_AUTH payload / ntag-pwd-capture (Thanks to GMMan)](https://github.com/flipperdevices/flipperzero-firmware/pull/1471)
- [Add skylanders (/activision) mifare classic to identification procedure #1497 (By equipter)](https://github.com/flipperdevices/flipperzero-firmware/pull/1497)
- [New frequency analyzer #1501 (By ClusterM)](https://github.com/flipperdevices/flipperzero-firmware/pull/1501)
- [Add a Mifare Classic info screen to parser output #1504 (By Astrrra)](https://github.com/flipperdevices/flipperzero-firmware/pull/1504)
- [BadUSB: add SYSRQ keys #1460 (By rck)](https://github.com/flipperdevices/flipperzero-firmware/pull/1460)
- [I²C-Scanner #1431 (By GitChris3004)](https://github.com/flipperdevices/flipperzero-firmware/pull/1431)
- [nfc: collect nonces during mf classic emulation #1484 (By indutny)](https://github.com/flipperdevices/flipperzero-firmware/pull/1484)

</details>

</td><td>

<details>
  <summary><B>GAMES</b></summary><br/>
  
- - Games `HIDDEN` if they dont work well; can be added under [applications/meta/application.fam](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/applications/meta/application.fam#L54)
- `FAP` [2048 (By OlegSchwann)](https://github.com/OlegSchwann/flipperzero-firmware/tree/hackaton/game_2048/applications/game-2048)
- `FAP` [Arkanoid (By gotnull)](https://github.com/gotnull/flipperzero-firmware-wPlugins)
- [Chess (By Okalachev)](https://github.com/okalachev/flipperzero-firmware/tree/chess) Crashes 1st load if FW <~750KB or every load on larger FW `HIDDEN "INTAPP_chess_game"`
- [Chip8 Emulator (By mega8bit)](https://github.com/mega8bit/flipperzero-firmware) Updated by ESurge. Add SD folder `chip8`, [Get GAMES HERE](https://johnearnest.github.io/chip8Archive/) `HIDDEN "INTAPP_Chip8"`
- `FAP` [Dice Roller Including SEX/WAR/8BALL/WEED DICE (By RogueMaster)](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/blob/unleashed/applications/dice/dice.c)
- `FAP` [Flappy Bird (By DroomOne)](https://github.com/DroomOne/flipperzero-firmware/tree/dev/applications/flappy_bird)
- `FAP` [Game of Life (Updated to work by tgxn)(By itsyourbedtime)](https://github.com/tgxn/flipperzero-firmware/blob/dev/applications/game_of_life/game_of_life.c) `HIDDEN "GAME_GameOfLife"`
- `FAP` [Mandelbrot Set (By Possibly-Matt)](https://github.com/Possibly-Matt/flipperzero-firmware-wPlugins) `HIDDEN "GAME_MandelbrotSet"`
- `FAP` [RayCast (Bt Zlo)](https://github.com/flipperdevices/flipperzero-firmware/tree/zlo/raycast-game-engine)
- `FAP` Snake [OFW]
- `FAP` [Tanks (By Alexgr13)](https://github.com/alexgr13/flipperzero-firmware/tree/fork/dev/applications/tanks-game) `HIDDEN "GAME_Tanks"`
- `FAP` [Tetris (By jeffplang)](https://github.com/jeffplang/flipperzero-firmware/tree/tetris_game/applications/tetris_game)
- `FAP` [Tic Tac Toe (By gotnull)](https://github.com/gotnull/flipperzero-firmware-wPlugins)
- `FAP` [Video Poker (By PixlEmly)](https://github.com/PixlEmly/flipperzero-firmware-testing/blob/unleashed/applications/VideoPoker/poker.c)
</details>

<details>
  <summary><B>PLUGINS</b></summary><br/>

- - PLUGINS `HIDDEN` if they have uncommon hardware dependancies
- `FAP` [Barcode Generator (By McAzzaMan)](https://github.com/McAzzaMan/flipperzero-firmware/tree/UPC-A_Barcode_Generator/applications/barcode_generator)
- `FAP` [Bluetooth Remote (By Cutch)[OFW]](https://github.com/flipperdevices/flipperzero-firmware/pull/1330)
- [Clock/Stopwatch (By CompaqDisc, Stopwatch & Sound Alert By RogueMaster)](https://gist.github.com/CompaqDisc/4e329c501bd03c1e801849b81f48ea61)
- [DSTIKE Deauther (By SequoiaSan)](https://github.com/SequoiaSan/FlipperZero-Wifi-ESP8266-Deauther-Module/tree/FlipperZero-Module-v2/FlipperZeroModule/FlipperZero-ESP8266-Deauth-App) `HIDDEN "INTAPP_esp8266_deauth"` due to `Req: ESP8266`
- `FAP` [Dolphin Backup (By nminaylov)](https://github.com/flipperdevices/flipperzero-firmware/pull/1384) Modified by RogueMaster
- `FAP` [Dolphin Restorer (By nminaylov)](https://github.com/flipperdevices/flipperzero-firmware/pull/1384) Cloned by RogueMaster
- [HID Analyzer (By Ownasaurus)](https://github.com/Ownasaurus/flipperzero-firmware/tree/hid-analyzer/applications/hid_analyzer)
- `FAP` [Mouse Jacker (By mothball187)](https://github.com/mothball187/flipperzero-nrf24/tree/main/mousejacker) ([Pin Out](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/tree/unleashed/applications/mousejacker) from nocomp/Frog/UberGuidoZ) `Req: NRF24`
- `FAP` [Mouse Jiggler (By Jacob-Tate)](https://github.com/Jacob-Tate/flipperzero-firmware/blob/dev/applications/mouse_jiggler/mouse_jiggler.c) (Original By MuddleBox)
- `FAP` Music Player [OFW]
- `FAP` [NRF Sniff (By mothball187)](https://github.com/mothball187/flipperzero-nrf24/tree/main/nrfsniff) ([Pin Out](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/tree/unleashed/applications/nrfsniff) from nocomp/Frog/UberGuidoZ) `Req: NRF24`
- [PicoPass Reader (By Bettse)](https://github.com/flipperdevices/flipperzero-firmware/pull/1366)
- [RF Remix (By ESurge)](https://github.com/ESurge/flipperzero-firmware-unirfremix) [(Original By jimilinuxguy)](https://github.com/jimilinuxguy/flipperzero-universal-rf-remote/tree/028d615c83f059bb2c905530ddb3d4efbd3cbcae/applications/jukebox)
- `FAP` [Sentry Safe (By H4ckd4ddy)](https://github.com/H4ckd4ddy/flipperzero-sentry-safe-plugin) ([Pin Out](https://github.com/RogueMaster/flipperzero-firmware-wPlugins/tree/unleashed/applications/sentry_safe) from [UberGuidoZ](https://github.com/UberGuidoZ/))
- `FAP` [Spectrum Analyzer (By jolcese)](https://github.com/jolcese/flipperzero-firmware/tree/spectrum/applications/spectrum_analyzer) [Updates (for testing) Thanks to theY4Kman](https://github.com/theY4Kman/flipperzero-firmware)
- `FAP` [Touch Tunes Remote (By jimilinuxguy)](https://github.com/jimilinuxguy/flipperzero-universal-rf-remote/tree/028d615c83f059bb2c905530ddb3d4efbd3cbcae/applications/jukebox) `HIDDEN "APPS_TouchTunes"`
- `FAP` [WAV Player (By Zlo)](https://github.com/flipperdevices/flipperzero-firmware/tree/zlo/wav-player) Updated by Atmanos & RogueMaster To Work
- [WiFi (Marauder) (By 0xchocolate)](https://github.com/0xchocolate/flipperzero-firmware-with-wifi-marauder-companion) `REQUIRES WIFI DEV BOARD WITH MARAUDER FLASHED`
- [WiFi Scanner v.0.4 (By SequoiaSan)](https://github.com/SequoiaSan/FlipperZero-WiFi-Scanner_Module-ESP8266) `HIDDEN "INTAPP_wifi_scanner"` due to `Req: ESP8266 or WIFI DEV BOARD`
</details>

## Conflicting PRs Not Merged:
- [Fixed building for users with space in windows username #1437 (By SzymonLisowiec)](https://github.com/flipperdevices/flipperzero-firmware/pull/1437)
- [App RPC Bug Fixes and redesign #1491 (By skotopes)](https://github.com/flipperdevices/flipperzero-firmware/pull/1491)

</td></tr></table>
