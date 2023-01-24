#include "subghz_settings.h"
#include <loader/loader.h>
#include <lib/toolbox/value_index.h>
#include <locale/locale.h>

const char* const cc1101_text[] = {
    "Internal",
    "External",
};

static uint32_t subghz_settings_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static void cc1101_changed(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, cc1101_text[index]);
    furi_hal_subghz_set_internal_cc1101(index == 0);
    loader_update_menu();
}

SubghzSettings* subghz_settings_alloc() {
    SubghzSettings* app = malloc(sizeof(SubghzSettings));

    // Load settings
    app->gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    VariableItem* item;
    uint8_t value_index;
    app->var_item_list = variable_item_list_alloc();

    item = variable_item_list_add(
        app->var_item_list, "CC1101", COUNT_OF(cc1101_text), cc1101_changed, app);
    value_index = furi_hal_subghz_is_internal_cc1101() ? 0 : 1;
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, cc1101_text[value_index]);

    view_set_previous_callback(
        variable_item_list_get_view(app->var_item_list), subghz_settings_exit);
    view_dispatcher_add_view(
        app->view_dispatcher,
        SubghzSettingsViewVarItemList,
        variable_item_list_get_view(app->var_item_list));

    view_dispatcher_switch_to_view(app->view_dispatcher, SubghzSettingsViewVarItemList);

    return app;
}

void subghz_settings_free(SubghzSettings* app) {
    furi_assert(app);
    // Variable item list
    view_dispatcher_remove_view(app->view_dispatcher, SubghzSettingsViewVarItemList);
    variable_item_list_free(app->var_item_list);
    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    // Records
    furi_record_close(RECORD_GUI);
    free(app);
}

int32_t subghz_settings_app(void* p) {
    UNUSED(p);
    SubghzSettings* app = subghz_settings_alloc();
    view_dispatcher_run(app->view_dispatcher);
    subghz_settings_free(app);
    return 0;
}