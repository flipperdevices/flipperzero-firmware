#include "genie_config.h"

#include <furi.h>
#include <furi_hal.h>
#include "genie_app.h"
#include "genie_submenu.h"

#include <dialogs/dialogs.h>
#include <storage/storage.h>
#include "toolbox/path.h"
#include "genie_record_v3_icons.h"

#define TAG "GenieConfig"

struct GenieConfig {
    VariableItemList* variable_item_list;
    GenieApp* app;
    VariableItem* file_item;
    FuriThread* thread;
    DialogsApp* dialogs;
    FuriString* tmp;
};

#define GENIE_SAVE_FOLDER \
    EXT_PATH("apps_data") \
    "/"                   \
    "genie"

bool select_genie_file(GenieConfig* config) {
    GenieApp* app = config->app;
    const char* previous_path = genie_app_get_file_path(app);
    if(previous_path && previous_path[0] != '\0') {
        furi_string_set(config->tmp, previous_path);
    } else {
        furi_string_set(config->tmp, GENIE_SAVE_FOLDER);
    }

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, ".gne", &I_genie_10x10);
    browser_options.base_path = GENIE_SAVE_FOLDER;
    browser_options.skip_assets = false;
    browser_options.item_loader_callback = NULL;
    browser_options.item_loader_context = NULL;
    bool file_selected =
        dialog_file_browser_show(config->dialogs, config->tmp, config->tmp, &browser_options);

    if(file_selected) {
        FURI_LOG_D("TAG", "Selected file: %s", furi_string_get_cstr(config->tmp));
        genie_app_update_file_path(app, furi_string_get_cstr(config->tmp));
    } else {
        FURI_LOG_D("TAG", "No file selected.");
    }

    return file_selected;
}

static uint32_t setting_frequency_values[] = {315000000, 390000000};
static char* setting_frequency_names[] = {"315 MHz", "390 MHz"};
static void genie_setting_frequency_change(VariableItem* item) {
    GenieApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, setting_frequency_names[index]);
    genie_app_set_frequency(app, setting_frequency_values[index]);
}

static void genie_config_update_file_item(GenieConfig* genie_config) {
    FuriString* current_name = furi_string_alloc();
    const char* current_path = genie_app_get_file_path(genie_config->app);
    if(current_path && current_path[0] != '\0') {
        path_extract_filename_no_ext(current_path, current_name);
    } else {
        furi_string_set(current_name, "Select");
    }
    variable_item_set_current_value_text(
        genie_config->file_item, furi_string_get_cstr(current_name));
    furi_string_free(current_name);
}

static void genie_config_ok_button(void* context, uint32_t index) {
    FURI_LOG_D(TAG, "OK button clicked. index = %ld", index);

    if(index != 1) {
        return;
    }

    GenieConfig* genie_config = (GenieConfig*)context;
    furi_thread_flags_set(furi_thread_get_id(genie_config->thread), 1);
}

static int32_t genie_config_thread(void* context) {
    GenieConfig* genie_config = (GenieConfig*)context;
    furi_thread_flags_clear(3);
    uint32_t flags = 0;
    do {
        flags = furi_thread_flags_wait(3, FuriFlagWaitAny, FuriWaitForever);
        if((flags & 1) == 1) {
            if(select_genie_file(genie_config)) {
                genie_config_update_file_item(genie_config);
            }
        }
    } while((flags & 2) != 2);

    return 0;
}

GenieConfig* genie_config_alloc(GenieApp* app) {
    GenieConfig* genie_config = (GenieConfig*)malloc(sizeof(GenieConfig));
    genie_config->app = app;
    genie_config->variable_item_list = variable_item_list_alloc();
    genie_config->tmp = furi_string_alloc();
    genie_config->dialogs = furi_record_open(RECORD_DIALOGS);
    genie_config->thread = furi_thread_alloc_ex("config", 1024, genie_config_thread, genie_config);
    furi_thread_start(genie_config->thread);

    VariableItemList* variable_item_list = genie_config->variable_item_list;
    variable_item_list_reset(variable_item_list);
    variable_item_list_set_enter_callback(
        variable_item_list, genie_config_ok_button, genie_config);
    view_set_previous_callback(
        variable_item_list_get_view(variable_item_list), genie_navigation_submenu_callback);

    VariableItem* item = variable_item_list_add(
        variable_item_list,
        "Frequency",
        COUNT_OF(setting_frequency_names),
        genie_setting_frequency_change,
        app);
    uint32_t current_freq = genie_app_get_frequency(app);
    uint8_t default_freq_index = 0;
    for(size_t i = 0; i < COUNT_OF(setting_frequency_values); i++) {
        if(setting_frequency_values[i] == current_freq) {
            default_freq_index = i;
            break;
        }
    }
    variable_item_set_current_value_index(item, default_freq_index);
    variable_item_set_current_value_text(item, setting_frequency_names[default_freq_index]);
    genie_app_set_frequency(app, setting_frequency_values[default_freq_index]);

    genie_config->file_item =
        variable_item_list_add(variable_item_list, "Genie File", 1, NULL, app);
    genie_config_update_file_item(genie_config);

    return genie_config;
}

void genie_config_free(GenieConfig* genie_config) {
    furi_assert(genie_config);
    furi_assert(genie_config->variable_item_list);
    furi_thread_flags_set(furi_thread_get_id(genie_config->thread), 2);
    furi_thread_join(genie_config->thread);
    furi_thread_free(genie_config->thread);
    variable_item_list_free(genie_config->variable_item_list);
    furi_string_free(genie_config->tmp);
    genie_config->tmp = NULL;
    genie_config->variable_item_list = NULL;
    genie_config->app = NULL;
    free(genie_config);
    furi_record_close(RECORD_DIALOGS);
}

View* genie_config_get_view(GenieConfig* genie_config) {
    furi_assert(genie_config);
    furi_assert(genie_config->variable_item_list);
    return variable_item_list_get_view(genie_config->variable_item_list);
}