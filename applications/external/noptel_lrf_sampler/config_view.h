/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.3
 *
 * Configuration view
***/

/*** Includes ***/
#include <gui/modules/variable_item_list.h>

/*** Routines ***/

/** Frequency setting change function **/
void config_freq_change(VariableItem*);

/** Averaging time change function **/
void config_avg_change(VariableItem*);

/** Beep option change function **/
void config_beep_change(VariableItem*);
