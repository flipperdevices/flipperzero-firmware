#ifndef STORAGE_H
#define STORAGE_H

/* Module to save and load settings to and from Flipper's internal storage */
#include "gravity_app_i.h"
#include <storage/storage.h>

bool save_settings(GravityApp* app);
bool load_settings(GravityApp* app);
bool save_data(GravityApp* app);
bool load_data(GravityApp* app);

#endif