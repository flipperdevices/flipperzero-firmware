* Finish restructuring menus
    * Add purge strategy UI to scan
    * New menu for Mana
    * New menu for purge strategy
    * Maybe a UI for stalk
    * Update set MAC to use hex UI module
    * BLE Purge strategy UI - Used in SCAN, GET, SET, & anywhere else relevant
* Crashing
    * Scan - View - Select - Back
    * I think my scene exit functions need to include updating the selected menu item and suchforth to avoid crash

* Re-implement console/command execution
* Re-implement cls/clear screen
* Refactor console logic - to keep console recording during menu navigation

* Settings->BLE_PURGE_STRAT needs to become a submenu where each purge element can be enabled/disabled

get/set:
SSID_LEN_MIN |
SSID_LEN_MAX |
DEFAULT_SSID_COUNT |
CHANNEL |                                                             
MAC |
ATTACK_PKTS |
ATTACK_MILLIS |
MAC_RAND |
EXPIRY |
HOP_MODE                                                         
SCRAMBLE_WORDS |
BLE_PURGE_STRAT |
BLE_PURGE_MAX_RSSI |
BLE_PURGE_MIN_AGE  

mana:
Auth: None, WEP, WPA
Mana Mode: Normal, Loud, <todo>
verbose
Run: Status, Start, Stop
Clear Mana Cache


purge strategy:
