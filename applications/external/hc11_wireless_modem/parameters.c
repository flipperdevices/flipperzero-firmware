/***
 * HC-11 wireless modem emulator for the Flipper Zero
 * Version: 0.1
 *
 * Parameters
***/

/*** Includes ***/
#include <storage/storage.h>

#include "common.h"

/*** Parameters ***/

/** Files and paths **/
const char* config_file = STORAGE_APP_DATA_PATH_PREFIX "/" CONFIG_FILE;

/** Submenu item names **/
const char* submenu_item_names[] = {"Configuration", "USB serial passthrough", "About"};

/** Channel setting parameters **/
const char* config_channel_label = "HC-11 channel";
const uint8_t config_channel_values[] = {1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                                         11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
const char* config_channel_names[] = {"C001", "C002", "C003", "C004", "C005", "C006", "C007",
                                      "C008", "C009", "C010", "C011", "C012", "C013", "C014",
                                      "C015", "C016", "C017", "C018", "C019", "C020"};
const uint8_t nb_config_channel_values = COUNT_OF(config_channel_values);

/** Address setting parameters **/
const char* config_address_label = "HC-11 address";
const uint8_t config_address_values[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
const char* config_address_names[] = {
    "A000",
    "A001",
    "A002",
    "A003",
    "A004",
    "A005",
    "A006",
    "A007",
    "A008",
    "A009",
    "A010",
    "A011",
    "A012",
    "A013",
    "A014",
    "A015"};
const uint8_t nb_config_address_values = COUNT_OF(config_address_values);

/** TX power setting parameters **/
const char* config_txpower_label = "TX power";
const uint8_t config_txpower_values[] = {1, 2, 3, 4, 5, 6, 7, 8};
const char* config_txpower_names[] = {"P1", "P2", "P3", "P4", "P5", "P6", "P7", "P8"};
const uint8_t nb_config_txpower_values = COUNT_OF(config_txpower_values);

/** Mode setting parameters **/
const char* config_mode_label = "HC-11 mode";
const uint8_t config_mode_values[] = {1, 3};
const char* config_mode_names[] = {"FU1 (RX)", "FU3"};
const uint8_t nb_config_mode_values = COUNT_OF(config_mode_values);

/** USB passthrough channel setting parameters **/
const char* config_passthru_chan_label = "USB channel";
const uint8_t config_passthru_chan_values[] = {0, 1};
const char* config_passthru_chan_names[] = {"0", "1"};
const uint8_t nb_config_passthru_chan_values = COUNT_OF(config_passthru_chan_values);

/** LED parameters **/
const uint16_t min_led_flash_duration = 15; /*ms*/

/** USB serial passthrough view timings **/
const uint16_t passthru_view_update_every = 250; /*ms*/
