* Finish restructuring menus
    * Add purge strategy UI to scan
    * New menu for purge strategy
    * Maybe a UI for stalk
    * Update set MAC to use hex UI module
    * BLE Purge strategy UI - Used in SCAN, GET, SET, & anywhere else relevant

* Re-implement console/command execution
* Re-implement cls/clear screen
* Refactor console logic - to keep console recording during menu navigation
* Flipper protocol for ESP32-Gravity to allow more sophisticated UIs
    * e.g. get all settings info, in bytes, from a single command
* Just In Time view memory allocation (now that there are lots of them)
* Change scene #defines to enums

* Settings->BLE_PURGE_STRAT needs to become a submenu where each purge element can be enabled/disabled

purge strategy:
MIN_RSSI
MAX_AGE
Unselected
Unnamed
