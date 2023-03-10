#include <stdlib.h>
#include <string.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format_i.h>
#include "../xremote.h"

#define XREMOTE_SETTINGS_FILE_VERSION 1
#define CONFIG_FILE_DIRECTORY_PATH EXT_PATH("apps_data/xremote")
#define XREMOTE_SETTINGS_SAVE_PATH CONFIG_FILE_DIRECTORY_PATH "/xremote.conf"
#define XREMOTE_SETTINGS_SAVE_PATH_TMP XREMOTE_SETTINGS_SAVE_PATH ".tmp"
#define XREMOTE_SETTINGS_HEADER "Xremote Config File"
#define XREMOTE_SETTINGS_KEY_HAPTIC "Haptic"
#define XREMOTE_SETTINGS_KEY_LED "Led"
#define XREMOTE_SETTINGS_KEY_SPEAKER "Speaker"
#define XREMOTE_SETTINGS_KEY_SAVE_SETTINGS "SaveSettings"

void xremote_save_settings(void* context);
void xremote_read_settings(void* context);