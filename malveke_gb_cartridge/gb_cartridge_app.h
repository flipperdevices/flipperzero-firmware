#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <dialogs/dialogs.h>
#include <expansion/expansion.h>
#include <notification/notification_messages.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/scene_manager.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/button_menu.h>
#include <gui/modules/dialog_ex.h>
#include <stdlib.h>
#include <string.h>
#include <storage/storage.h>
#include <flipper_format/flipper_format_i.h>

#include "scenes/gb_cartridge_scene.h"
#include "views/gb_cartridge_startscreen.h"
#include "views/gb_cartridge_scene_1.h"
#include "views/gb_cartridge_scene_2.h"
#include "views/gb_cartridge_scene_3.h"
#include "views/gb_cartridge_scene_4.h"
#include "views/gb_cartridge_scene_5.h"
#include "uart.h"
#include "helpers/cJSON.h"
#include "malveke_notifications.h"

#define TAG "GBCartridge"

#define MALVEKE_APP_FOLDER_USER "apps_data/malveke"
#define MALVEKE_APP_FOLDER EXT_PATH(MALVEKE_APP_FOLDER_USER)
#define MALVEKE_APP_FOLDER_ROMS MALVEKE_APP_FOLDER "/roms"
#define MALVEKE_APP_FOLDER_RAMS MALVEKE_APP_FOLDER "/rams"
#define MALVEKE_APP_FOLDER_PHOTOS MALVEKE_APP_FOLDER "/photos"

#define UI_PADDING 3
#define UI_PROGRESS_COLS 8
#define UI_PROGRESS_ROWS 4

#define BUFFER_SIZE 64

#ifndef MAX
#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b)) ? (a) : (b)
#endif

typedef struct {
    Gui* gui;
    Storage* storage;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    VariableItemList* submenu;
    bool need_to_prompt_settings_init;
    SceneManager* scene_manager;
    VariableItemList* variable_item_list;
    GBCartridgeStartscreen* gb_cartridge_startscreen;
    GBCartridgeScene1* gb_cartridge_scene_1;
    GBCartridgeScene2* gb_cartridge_scene_2;
    GBCartridgeScene3* gb_cartridge_scene_3;
    GBCartridgeScene4* gb_cartridge_scene_4;
    GBCartridgeScene5* gb_cartridge_scene_5;

    DialogsApp* dialogs; // File Browser
    FuriString* file_path; // File Browser
    uint32_t haptic;
    uint32_t speaker;
    uint32_t led;
    uint32_t save_settings;
    ButtonMenu* button_menu; // Button Menu

    Uart* uart;
    Uart* lp_uart;

    char* cart_title;
    char* cart_dump_rom_filename;
    char* cart_dump_rom_extension;

    char* cart_dump_ram_filename;
    char* cart_dump_ram_extension;

    File* cart_rom;
    File* cart_ram;
    File* cart_log;

    uint32_t gameboy_rom_option_selected_index;
    char* gameboy_rom_option_selected_text;

    FuriThread* thread;
    bool is_writing_rom;
    bool is_writing_ram;

    int rom_banks;
    int ram_banks;

} GBCartridge;

typedef enum {
    GBCartridgeViewIdStartscreen,
    GBCartridgeViewIdMenu,
    GBCartridgeViewIdScene1,
    GBCartridgeViewIdScene2,
    GBCartridgeViewIdScene3,
    GBCartridgeViewIdScene4,
    GBCartridgeViewIdScene5,
    GBCartridgeViewIdSettings,
} GBCartridgeViewId;

typedef enum {
    GBCartridgeHapticOff,
    GBCartridgeHapticOn,
} GBCartridgeHapticState;

typedef enum {
    GBCartridgeSpeakerOff,
    GBCartridgeSpeakerOn,
} GBCartridgeSpeakerState;

typedef enum {
    GBCartridgeLedOff,
    GBCartridgeLedOn,
} GBCartridgeLedState;

typedef enum {
    GBCartridgeSettingsOff,
    GBCartridgeSettingsOn,
} GBCartridgeSettingsStoreState;

typedef enum {
    GBCartridgeRomOptionGB,
    GBCartridgeRomOptionGBC,
} GBCartridgeRomOptionState;
