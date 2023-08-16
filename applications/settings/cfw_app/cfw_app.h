#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <dialogs/dialogs.h>
#include <gui/modules/dialog_ex.h>
#include <applications.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/byte_input.h>
#include <gui/modules/popup.h>
#include <lib/toolbox/path.h>
#include <lib/toolbox/value_index.h>
#include <storage/storage.h>
#include <toolbox/stream/file_stream.h>
#include "scenes/cfw_app_scene.h"
#include "dolphin/helpers/dolphin_state.h"
#include "dolphin/dolphin.h"
#include "dolphin/dolphin_i.h"
#include <lib/flipper_format/flipper_format.h>
#include <lib/subghz/subghz_setting.h>
#include <flipper_application/flipper_application.h>
#include <loader/loader.h>
#include <loader/loader_mainmenu.h>
#include <notification/notification_app.h>
#include <power/power_service/power.h>
#include <rgb_backlight.h>
#include <m-array.h>
#include "namespoof.h"
#include "cfw_icons.h"
#include <cfw.h>
#include <applications.h>
#include <desktop/desktop_settings.h>
#include "helpers/passport_settings.h"

#define MENU_ICON_MAX_SIZE 14
#define CFW_SUBGHZ_FREQ_BUFFER_SIZE 6

ARRAY_DEF(CharList, char*)

typedef struct {
    Gui* gui;
    DialogsApp* dialogs;
    NotificationApp* notification;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    VariableItemList* var_item_list;
    Submenu* submenu;
    TextInput* text_input;
    ByteInput* byte_input;
    Popup* popup;
    DialogEx* dialog_ex;

    DesktopSettings desktop;
    PassportSettings passport;

    CharList_t mainmenu_app_names;
    CharList_t mainmenu_app_paths;
    uint8_t mainmenu_app_index;

    uint8_t start_point_index;

    bool subghz_use_defaults;
    FrequencyList_t subghz_static_freqs;
    uint8_t subghz_static_index;
    FrequencyList_t subghz_hopper_freqs;
    uint8_t subghz_hopper_index;
    char subghz_freq_buffer[CFW_SUBGHZ_FREQ_BUFFER_SIZE];
    bool subghz_extend;
    bool subghz_bypass;
    RgbColor lcd_color;
    char device_name[FURI_HAL_VERSION_ARRAY_NAME_LENGTH];
    FuriString* version_tag;

    bool save_mainmenu_apps;
    bool save_subghz_frequencies;
    bool save_subghz;
    bool save_name;
    bool save_backlight;
    bool save_settings;
    bool require_reboot;
} CfwApp;

typedef enum {
    CfwAppViewVarItemList,
    CfwAppViewSubmenu,
    CfwAppViewTextInput,
    CfwAppViewByteInput,
    CfwAppViewPopup,
    CfwAppViewDialogEx,
} CfwAppView;

bool cfw_app_apply(CfwApp* app);
