#pragma once

#include <stdlib.h>
#include <string.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format_i.h>
#include "../hex_viewer.h"

#define HEX_VIEWER_SETTINGS_FILE_VERSION 1
#define CONFIG_FILE_DIRECTORY_PATH EXT_PATH("apps_data/hex_viewer")
#define HEX_VIEWER_SETTINGS_SAVE_PATH CONFIG_FILE_DIRECTORY_PATH "/hex_viewer.conf"
#define HEX_VIEWER_SETTINGS_SAVE_PATH_TMP HEX_VIEWER_SETTINGS_SAVE_PATH ".tmp"
#define HEX_VIEWER_SETTINGS_HEADER "HexViewer Config File"
#define HEX_VIEWER_SETTINGS_KEY_HAPTIC "Haptic"
#define HEX_VIEWER_SETTINGS_KEY_LED "Led"
#define HEX_VIEWER_SETTINGS_KEY_SPEAKER "Speaker"
#define HEX_VIEWER_SETTINGS_KEY_SAVE_SETTINGS "SaveSettings"

void hex_viewer_save_settings(void* context);
void hex_viewer_read_settings(void* context);

bool hex_viewer_open_file(void* context, const char* file_path);
bool hex_viewer_read_file(void* context);