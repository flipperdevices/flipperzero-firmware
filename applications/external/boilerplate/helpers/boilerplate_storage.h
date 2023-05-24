#include <stdlib.h>
#include <string.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format_i.h>
#include "../boilerplate.h"

#define BOILERPLATE_SETTINGS_FILE_VERSION 1
#define CONFIG_FILE_DIRECTORY_PATH EXT_PATH("apps_data/boilerplate")
#define BOILERPLATE_SETTINGS_SAVE_PATH CONFIG_FILE_DIRECTORY_PATH "/boilerplate.conf"
#define BOILERPLATE_SETTINGS_SAVE_PATH_TMP BOILERPLATE_SETTINGS_SAVE_PATH ".tmp"
#define BOILERPLATE_SETTINGS_HEADER "Boilerplate Config File"
#define BOILERPLATE_SETTINGS_KEY_HAPTIC "Haptic"
#define BOILERPLATE_SETTINGS_KEY_LED "Led"
#define BOILERPLATE_SETTINGS_KEY_SPEAKER "Speaker"
#define BOILERPLATE_SETTINGS_KEY_SAVE_SETTINGS "SaveSettings"

void boilerplate_save_settings(void* context);
void boilerplate_read_settings(void* context);