#include "../scope_app_i.h"
#include "../helpers/scope_types.h"

static const double time_list[] = {1.0, 0.1, 0.01, 0.001, 0.0005};

void scope_scene_setup_widget_callback(GuiButtonType result, InputType type, void* context) {
    ScopeApp* app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

static void timeperiod_cb(VariableItem* item) {
    UNUSED(item);
    ScopeApp* app = variable_item_get_context(item);
    furi_assert(app);
    uint8_t index = variable_item_get_current_value_index(item);
    char tmp[8];
    snprintf(tmp, 7, "%3.4f", time_list[index]);
    variable_item_set_current_value_text(item, tmp);
    app->time = time_list[index];
}

void scope_scene_setup_on_enter(void* context) {
    ScopeApp* app = context;
    VariableItemList* var_item_list = app->variable_item_list;
    VariableItem* item;
    item = variable_item_list_add(
        var_item_list, "Time period", COUNT_OF(time_list), timeperiod_cb, app);

    char tmp[8];
    snprintf(tmp, 7, "%3.4f", app->time);

    for(uint32_t i = 0; i < COUNT_OF(time_list); i++) {
        if(time_list[i] == app->time) {
            variable_item_set_current_value_index(item, i);
            variable_item_set_current_value_text(item, tmp);
            break;
        }
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, ScopeViewVariableItemList);
}

bool scope_scene_setup_on_event(void* context, SceneManagerEvent event) {
    ScopeApp* app = context;
    bool consumed = false;
    UNUSED(app);
    UNUSED(event);
    return consumed;
}

void scope_scene_setup_on_exit(void* context) {
    ScopeApp* app = context;
    variable_item_list_reset(app->variable_item_list);
    // Clear views
    widget_reset(app->widget);
}
