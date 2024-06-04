/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * Configuration view
***/

/*** Includes ***/
#include <gui/modules/variable_item_list.h>

/*** Routines ***/

/** Sampling mode setting change function **/
void config_mode_change(VariableItem*);

/** Buffering setting change function **/
void config_buf_change(VariableItem*);

/** Beep option change function **/
void config_beep_change(VariableItem*);

/** Baudrate option change function **/
void config_baudrate_change(VariableItem*);

/** USB passthrough channel option change function **/
void config_passthru_chan_change(VariableItem* item);

/** SMM prefix option change function **/
void config_smm_pfx_change(VariableItem*);
