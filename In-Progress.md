* Finish restructuring menus
    * New menu for fuzz
    * New menu for deauth
    * New menu for scan
    * New menu for Mana
    * Maybe a UI for stalk
    * BLE Purge strategy UI - Used in SCAN, GET, SET, & anywhere else relevant
* Crashing
    * Scan - View - Select - Back
* Help
    * Command info
    * About
    * Get Started
    * Commands
    * Help
* Re-implement console/command execution
* Re-implement cls/clear screen
* Refactor console logic - to keep console recording during menu navigation

* Settings->BLE_PURGE_STRAT needs to become a submenu where each purge element can be enabled/disabled
* I think my scene exit functions need to include updating the selected menu item and suchforth to avoid crash

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
