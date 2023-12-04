#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <dialogs/dialogs.h>
#include <storage/storage.h>
#include <notification/notification_messages.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/scene_manager.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/button_menu.h>
#include <gui/modules/dialog_ex.h>
#include "scenes/gba_cartridge_scene.h"
#include "views/gba_cartridge_startscreen.h"
#include "views/gba_cartridge_scene_1.h"
#include "uart.h"
#include "helpers/cJSON.h"
#include "malveke_notifications.h"

#define TAG "GBACartridge"

#define SUBGHZ_APP_EXTENSION ".sub"
#define SUBGHZ_APP_FOLDER ANY_PATH("subghz")

#define MALVEKE_APP_FOLDER_USER "apps_data/malveke"
#define MALVEKE_APP_FOLDER EXT_PATH(MALVEKE_APP_FOLDER_USER)
#define MALVEKE_APP_FOLDER_ROMS MALVEKE_APP_FOLDER "/roms"
#define MALVEKE_APP_FOLDER_RAMS MALVEKE_APP_FOLDER "/rams"
#define MALVEKE_APP_FOLDER_PHOTOS MALVEKE_APP_FOLDER "/photos"


#define UI_PADDING 3
#define UI_PROGRESS_COLS 8
#define UI_PROGRESS_ROWS 4

#define BUFFER_SIZE 64

typedef struct {
    Gui* gui;
    Storage* storage;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    VariableItemList* submenu;
    bool need_to_prompt_settings_init;
    SceneManager* scene_manager;
    VariableItemList* variable_item_list;
    GBACartridgeStartscreen* gba_cartridge_startscreen;
    GBACartridgeScene1* gba_cartridge_scene_1;
    
    DialogsApp* dialogs; // File Browser
    FuriString* file_path; // File Browser
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


    FuriThread* thread;
    bool is_writing_rom;
    bool is_writing_ram;

    int rom_banks;
    int ram_banks;

} GBACartridge;


typedef enum {
    GBACartridgeViewIdStartscreen,
    GBACartridgeViewIdMenu,
    GBACartridgeViewIdScene1,
} GBACartridgeViewId;




