#pragma once

#include <stdlib.h>
#include <string.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format_i.h>
#include "../flippertag.h"

#define FLIPPERTAG_SETTINGS_FILE_VERSION 1
#define CONFIG_FILE_DIRECTORY_PATH EXT_PATH("apps_data/flippertag")
#define FLIPPERTAG_SETTINGS_SAVE_PATH CONFIG_FILE_DIRECTORY_PATH "/flippertag.conf"
#define FLIPPERTAG_SETTINGS_SAVE_PATH_TMP FlipperTag_SETTINGS_SAVE_PATH ".tmp"
#define FLIPPERTAG_SETTINGS_HEADER "FlipperTag Config File"
#define FLIPPERTAG_SETTINGS_KEY_HAPTIC "Haptic"
#define FLIPPERTAG_SETTINGS_KEY_LED "Led"
#define FLIPPERTAG_SETTINGS_KEY_SPEAKER "Speaker"
#define FLIPPERTAG_SETTINGS_KEY_SAVE_SETTINGS "SaveSettings"

void flippertag_save_settings(void* context);
void flippertag_read_settings(void* context);