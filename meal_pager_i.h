#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/scene_manager.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/button_menu.h>
#include <gui/modules/dialog_ex.h>
#include "scenes/meal_pager_scene.h"
#include "views/meal_pager_startscreen.h"
#include "views/meal_pager_transmit.h"
#include "helpers/meal_pager_storage.h"
#include "helpers/subghz/subghz_types.h"
#include "helpers/subghz/subghz.h"

#define TAG "Meal_Pager"

#define SUBGHZ_APP_EXTENSION ".sub"
#define SUBGHZ_APP_FOLDER ANY_PATH("subghz")

typedef struct Meal_PagerTransmit Meal_PagerTransmit;
typedef struct SubGhz SubGhz;

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    SubGhzNotificationState state_notifications;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    SubGhz* subghz;
    SceneManager* scene_manager;
    VariableItemList* variable_item_list;
    Meal_PagerStartscreen* meal_pager_startscreen;
    Meal_PagerTransmit* meal_pager_transmit;
    DialogsApp* dialogs; // File Browser
    FuriString* file_path; // File Browser
    uint32_t haptic;
    uint32_t speaker;
    uint32_t led;
    uint32_t save_settings;
    uint32_t pager_type;
    uint32_t first_station;
    char* first_station_char;
    uint32_t last_station;
    char* last_station_char;
    uint32_t first_pager;
    char* first_pager_char;
    uint32_t last_pager;
    char* last_pager_char;
    uint32_t current_station;
    uint32_t current_pager;
    bool stop_transmit;
    uint32_t repeats;
    char* repeats_char;
} Meal_Pager;

typedef enum {
    Meal_PagerViewIdStartscreen,
    Meal_PagerViewIdMenu,
    Meal_PagerViewIdTransmit,
    Meal_PagerViewIdSettings,
} Meal_PagerViewId;

typedef enum {
    Meal_PagerPagerTypeT119,
    Meal_PagerPagerTypeTD157,
    Meal_PagerPagerTypeTD165,
    Meal_PagerPagerTypeTD174,
} Meal_PagerPagerType;

typedef enum {
    Meal_PagerHapticOff,
    Meal_PagerHapticOn,
} Meal_PagerHapticState;

typedef enum {
    Meal_PagerSpeakerOff,
    Meal_PagerSpeakerOn,
} Meal_PagerSpeakerState;

typedef enum {
    Meal_PagerLedOff,
    Meal_PagerLedOn,
} Meal_PagerLedState;

typedef enum {
    Meal_PagerSettingsOff,
    Meal_PagerSettingsOn,
} Meal_PagerSettingsStoreState;
