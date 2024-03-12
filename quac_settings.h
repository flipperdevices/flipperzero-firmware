#pragma once

#include "quac.h"

/** Set the default Settings for Quac */
void quac_set_default_settings(App* app);

/** Load the Settings from the .quac.conf file. If not found,
 * then load the defaults.
*/
void quac_load_settings(App* app);

/** Save the current settings to the .quac.conf file */
void quac_save_settings(App* app);
