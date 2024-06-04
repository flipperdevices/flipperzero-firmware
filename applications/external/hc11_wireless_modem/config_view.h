/***
 * HC-11 wireless modem emulator for the Flipper Zero
 * Version: 0.1
 *
 * Configuration view
***/

/*** Includes ***/
#include <gui/modules/variable_item_list.h>

/*** Routines ***/

/** Channel option change function **/
void config_channel_change(VariableItem*);

/** Address option change function **/
void config_address_change(VariableItem*);

/** TX power option change function **/
void config_txpower_change(VariableItem*);

/** Mode option change function **/
void config_mode_change(VariableItem*);

/** USB passthrough channel option change function **/
void config_passthru_chan_change(VariableItem* item);
