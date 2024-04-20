/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.3
 *
 * Configuration view
***/

/*** Includes ***/
#include <gui/modules/variable_item_list.h>



/*** Routines ***/

/** Sampling mode setting change function **/
void config_mode_change(VariableItem *);

/** Buffering setting change function **/
void config_buf_change(VariableItem *);

/** Beep option change function **/
void config_beep_change(VariableItem *);
