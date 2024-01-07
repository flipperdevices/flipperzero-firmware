#pragma once

#include <stdlib.h>
#include <string.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format_i.h>
#include "../meal_pager_i.h"

#define MEAL_PAGER_SETTINGS_FILE_VERSION 2
#define CONFIG_FILE_DIRECTORY_PATH EXT_PATH("apps_data/meal_pager")
#define MEAL_PAGER_SETTINGS_SAVE_PATH CONFIG_FILE_DIRECTORY_PATH "/meal_pager.conf"
#define MEAL_PAGER_SETTINGS_SAVE_PATH_TMP MEAL_PAGER_SETTINGS_SAVE_PATH ".tmp"
#define MEAL_PAGER_SETTINGS_HEADER "Meal_Pager Config File"
#define MEAL_PAGER_SETTINGS_KEY_PAGER_TYPE "Pager Type"
#define MEAL_PAGER_SETTINGS_KEY_FIRST_STATION "First Station"
#define MEAL_PAGER_SETTINGS_KEY_LAST_STATION "Last Station"
#define MEAL_PAGER_SETTINGS_KEY_FIRST_PAGER "First Pager"
#define MEAL_PAGER_SETTINGS_KEY_LAST_PAGER "Last Pager"
#define MEAL_PAGER_SETTINGS_KEY_REPEATS "Repeats"
#define MEAL_PAGER_SETTINGS_KEY_HAPTIC "Haptic"
#define MEAL_PAGER_SETTINGS_KEY_LED "Led"
#define MEAL_PAGER_SETTINGS_KEY_SPEAKER "Speaker"
#define MEAL_PAGER_SETTINGS_KEY_SAVE_SETTINGS "SaveSettings"
#define MEAL_PAGER_TMP_FILE CONFIG_FILE_DIRECTORY_PATH "/tmp.sub"
#define MEAL_PAGER_SUBGHZ_FILE_TYPE "Flipper SubGhz RAW File"
#define MEAL_PAGER_SUBGHZ_FILE_VERSION 1
#define MEAL_PAGER_SUBGHZ_FILE_FREQUENCY "433920000"
#define MEAL_PAGER_SUBGHZ_FILE_ALT_FREQUENCY "433889000"
#define MEAL_PAGER_SUBGHZ_FILE_PRESET "FuriHalSubGhzPresetOok650Async"
#define MEAL_PAGER_SUBGHZ_FILE_Protocol "RAW"

bool meal_pager_save_subghz_buffer_file_start(
    void* context,
    FlipperFormat* ff,
    Storage* storage,
    char* frequency);

void meal_pager_save_subghz_buffer_stop(void* context, FlipperFormat* ff);

void meal_pager_save_settings(void* context);
void meal_pager_read_settings(void* context);
void meal_pager_set_max_values(void* context);