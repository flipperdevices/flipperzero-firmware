#pragma once

#include <stdlib.h>
#include <string.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format_i.h>
#include "../meal_pager.h"

#define MEAL_PAGER_SETTINGS_FILE_VERSION 1
#define CONFIG_FILE_DIRECTORY_PATH EXT_PATH("apps_data/meal_pager")
#define MEAL_PAGER_SETTINGS_SAVE_PATH CONFIG_FILE_DIRECTORY_PATH "/meal_pager.conf"
#define MEAL_PAGER_SETTINGS_SAVE_PATH_TMP MEAL_PAGER_SETTINGS_SAVE_PATH ".tmp"
#define MEAL_PAGER_SETTINGS_HEADER "Meal_Pager Config File"
#define MEAL_PAGER_SETTINGS_KEY_PAGER_TYPE "Pager Type"
#define MEAL_PAGER_SETTINGS_KEY_FIRST_STATION "First Station"
#define MEAL_PAGER_SETTINGS_KEY_LAST_STATION "Last Station"
#define MEAL_PAGER_SETTINGS_KEY_FIRST_PAGER "First Pager"
#define MEAL_PAGER_SETTINGS_KEY_LAST_PAGER "Last Pager"
#define MEAL_PAGER_SETTINGS_KEY_HAPTIC "Haptic"
#define MEAL_PAGER_SETTINGS_KEY_LED "Led"
#define MEAL_PAGER_SETTINGS_KEY_SPEAKER "Speaker"
#define MEAL_PAGER_SETTINGS_KEY_SAVE_SETTINGS "SaveSettings"

void meal_pager_save_settings(void* context);
void meal_pager_read_settings(void* context);