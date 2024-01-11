#ifndef MINESWEEPER_STORAGE_H
#define MINESWEEPER_STORAGE_H

#include <stdlib.h>
#include <string.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format.h>
#include "../minesweeper.h"

#define MINESWEEPER_SETTINGS_FILE_VERSION 1
#define CONFIG_FILE_DIRECTORY_PATH EXT_PATH("apps_data/f0_mine_sweeper")
#define MINESWEEPER_SETTINGS_SAVE_PATH CONFIG_FILE_DIRECTORY_PATH "/f0_mine_sweeper.conf"
#define MINESWEEPER_SETTINGS_SAVE_PATH_TMP MINESWEEPER_SETTINGS_SAVE_PATH ".tmp"
#define MINESWEEPER_SETTINGS_HEADER "Mine Sweeper Config File"

#define MINESWEEPER_SETTINGS_KEY_WIDTH "BoardWidth"
#define MINESWEEPER_SETTINGS_KEY_HEIGHT "BoardHeight"
#define MINESWEEPER_SETTINGS_KEY_DIFFICULTY "BoardDifficulty"

#define MINESWEEPER_SETTINGS_KEY_HAPTIC "Haptic"
#define MINESWEEPER_SETTINGS_KEY_LED "Led"
#define MINESWEEPER_SETTINGS_KEY_SPEAKER "Speaker"

void mine_sweeper_save_settings(void* context);
bool mine_sweeper_read_settings(void* context);

#endif