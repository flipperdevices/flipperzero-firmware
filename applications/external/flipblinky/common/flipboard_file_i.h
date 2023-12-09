#pragma once

#include <storage/storage.h>
#include <flipper_format.h>

#include "action_model.h"
#include "flipboard_file.h"
#include "../app_config.h"

#define BUY_MSG "Buy your Flipboard at"
#define FLIPBOARD_URL "https://tindie.com/stores/MakeItHackin"

#define FLIPBOARD_KEY_NAME_SIZE 25
#define FLIPBOARD_APPS_DATA_FOLDER EXT_PATH("apps_data")
#define FLIPBOARD_SAVE_FOLDER      \
    FLIPBOARD_APPS_DATA_FOLDER "/" \
                               "flipboard"
#define FLIPBOARD_SAVE_EXTENSION ".txt"

#define FLIPBOARD_HEADER "Flipper Flipboard File"
#define FLIPBOARD_VERSION 2
