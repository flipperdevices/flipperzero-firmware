## 1.9
 - Get revision, get config moved into main Gen4 menu
 - New function: Set Gen4 card shadow mode
 - New function: Set Gen4 card direct write to block 0 mode
 - Fixed: back button did not allow to exit from some scenes while the card is next to the Flipper HF RFID antenna
 - Gen4 protocol refactor
 - Gen4 UI refactor

## 1.8
 - Ultralight Various fixes and improvements

## 1.7
 - Gen2/CUID write support

## 1.6
 - Rework with new bit lib API

## 1.5
 - Fix incorrect max sector configuration

## 1.4 
 - Auth with password option moved into new submenu "Gen4 actions"
 - New function: Get gen4 card revision
 - New function: Get gen4 card config (shows only when debug ON)
 - New function: Set default config: for 06 A0 gen4 cards "revival". This revision of the cards may "fall into a coma" when interacting with it. In this state, she does not respond to any commands. This problem is solved by writing a standard config to the card

## 1.3
 - Fix incorrect gen4 password usage

## 1.2
 - Minimal changes for recent API updates

## 1.1
 - Rework application with new NFC API

## 1.0
 - Initial release
