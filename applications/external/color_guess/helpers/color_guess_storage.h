#include <stdlib.h>
#include <string.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format_i.h>
#include "../color_guess.h"

#define COLOR_GUESS_SETTINGS_FILE_VERSION 1
#define CONFIG_FILE_DIRECTORY_PATH EXT_PATH("apps_data/color_guess")
#define COLOR_GUESS_SETTINGS_SAVE_PATH CONFIG_FILE_DIRECTORY_PATH "/color_guess.conf"
#define COLOR_GUESS_SETTINGS_SAVE_PATH_TMP COLOR_GUESS_SETTINGS_SAVE_PATH ".tmp"
#define COLOR_GUESS_SETTINGS_HEADER "Color Guess Config File"
#define COLOR_GUESS_SETTINGS_KEY_HAPTIC "Haptic"
#define COLOR_GUESS_SETTINGS_KEY_LED "Led"
#define COLOR_GUESS_SETTINGS_KEY_SPEAKER "Speaker"
#define COLOR_GUESS_SETTINGS_KEY_SAVE_SETTINGS "SaveSettings"

void color_guess_save_settings(void* context);
void color_guess_read_settings(void* context);