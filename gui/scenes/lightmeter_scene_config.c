#include "../../lightmeter.h"

enum LightMeterSubmenuIndex {
    LightMeterSubmenuIndexISO,
    LightMeterSubmenuIndexND,
};

static void lightmeter_scene_config_submenu_callback(void* context, uint32_t index) {
    furi_assert(context);
    LightMeterApp* lightmeter = context;
    view_dispatcher_send_custom_event(lightmeter->view_dispatcher, index);
}

void lightmeter_scene_config_on_enter(void* context) {
    
    LightMeterApp* lightmeter = context;
    // virtual_button_scene_send_view_update_model(app);

    Submenu* submenu = lightmeter->submenu;

    submenu_add_item(
        submenu,
        "ISO",
        LightMeterSubmenuIndexISO,
        lightmeter_scene_config_submenu_callback,
        lightmeter);
    submenu_add_item(
        submenu,
        "ND filter",
        LightMeterSubmenuIndexND,
        lightmeter_scene_config_submenu_callback,
        lightmeter);
    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(lightmeter->scene_manager, LightMeterAppSceneConfig));

    view_dispatcher_switch_to_view(lightmeter->view_dispatcher, LightMeterAppViewSubmenu);

    // view_dispatcher_switch_to_view(lightmeter->view_dispatcher, LightMeterAppViewConfigView);
}

bool lightmeter_scene_config_on_event(void* context, SceneManagerEvent event) {
    LightMeterApp* lightmeter = context;
    bool consumed = false;

    UNUSED(lightmeter);

    if(event.type == SceneManagerEventTypeTick) {
        // lightmeter_scene_send_view_update_model(app);
        consumed = true;
    }
    return consumed;
}

void lightmeter_scene_config_on_exit(void* context) {
    LightMeterApp* lightmeter = context;
    submenu_reset(lightmeter->submenu);
}
