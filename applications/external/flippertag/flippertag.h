#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <infrared_worker.h>
#include <input/input.h>
#include <stdlib.h>
#include <flippertag_icons.h>
#include <dialogs/dialogs.h>
#include <notification/notification_messages.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/scene_manager.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/button_menu.h>
#include <gui/modules/dialog_ex.h>
#include "scenes/flippertag_scene.h"
#include "views/flippertag_startscreen.h"
#include "views/flippertag_scene_1.h"
#include "helpers/flippertag_storage.h"
#include "helpers/flippertag_haptic.h"
#include "helpers/flippertag_led.h"
#include "helpers/flippertag_speaker.h"

#define TAG "FlipperTag"

#define SUBGHZ_APP_EXTENSION ".sub"
#define SUBGHZ_APP_FOLDER ANY_PATH("subghz")

#define DEFAULT_HEALTH 25
#define DEFAULT_AMMO 10
#define DEFAULT_SHIELD 10

typedef enum { FlipperTagSolo, FlipperTagTeam1, FlipperTagTeam2 } FlipperTagTeam;

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    SceneManager* scene_manager;
    VariableItemList* variable_item_list;
    InfraredWorker* rx;
    InfraredWorker* tx;

    FlipperTagStartscreen* flippertag_startscreen;
    FlipperTagScene1* flippertag_scene_1;
    DialogsApp* dialogs; // File Browser
    FuriString* file_path; // File Browser
    uint32_t haptic;
    uint32_t speaker;
    uint32_t led;
    uint32_t save_settings;
    ButtonMenu* button_menu; // Button Menu
    FlipperTagTeam team;
    uint32_t health;
    uint32_t ammo;
    uint32_t* current_signal;
    bool is_transmitting;
    bool is_shielded;
    uint32_t last_shield_time;
    uint32_t shield_status;
} FlipperTag;

typedef enum {
    FlipperTagViewIdStartscreen,
    FlipperTagViewIdScene1,
} FlipperTagViewId;

typedef enum {
    FlipperTagHapticOff,
    FlipperTagHapticOn,
} FlipperTagHapticState;

typedef enum {
    FlipperTagSpeakerOff,
    FlipperTagSpeakerOn,
} FlipperTagSpeakerState;

typedef enum {
    FlipperTagLedOff,
    FlipperTagLedOn,
} FlipperTagLedState;

typedef enum {
    FlipperTagSettingsOff,
    FlipperTagSettingsOn,
} FlipperTagSettingStoreState;
