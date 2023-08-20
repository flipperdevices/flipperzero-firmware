#include "power_settings_app.h"
#include "../about/about.h"

static bool power_settings_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    PowerSettingsApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool power_settings_back_event_callback(void* context) {
    furi_assert(context);
    PowerSettingsApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void power_settings_tick_event_callback(void* context) {
    furi_assert(context);
    PowerSettingsApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

PowerSettingsApp* power_settings_app_alloc(uint32_t first_scene, ViewDispatcherType type) {
    PowerSettingsApp* app = malloc(sizeof(PowerSettingsApp));

    app->about_battery = true;
    // app->about_battery = first_scene == PowerSettingsAppSceneBatteryInfo;

    // Records
    app->gui = furi_record_open(RECORD_GUI);
    app->power = furi_record_open(RECORD_POWER);

    //PubSub
    app->settings_events = power_get_settings_events_pubsub(app->power);

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&power_settings_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, power_settings_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, power_settings_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, power_settings_tick_event_callback, 2000);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, type);
    if(type == ViewDispatcherTypeDesktop) {
        gui_set_hide_statusbar(app->gui, true);
    }

    // Views
    app->battery_info = battery_info_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        PowerSettingsAppViewBatteryInfo,
        battery_info_get_view(app->battery_info));
    app->submenu = submenu_alloc();
    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, PowerSettingsAppViewSubmenu, submenu_get_view(app->submenu));
    app->dialog = dialog_ex_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, PowerSettingsAppViewDialog, dialog_ex_get_view(app->dialog));
    view_dispatcher_add_view(
        app->view_dispatcher,
        PowerSettingsAppViewVariableItemList,
        variable_item_list_get_view(app->variable_item_list));

    // Set first scene
    scene_manager_next_scene(app->scene_manager, first_scene);
    return app;
}

void power_settings_app_free(PowerSettingsApp* app) {
    furi_assert(app);
    // Views
    view_dispatcher_remove_view(app->view_dispatcher, PowerSettingsAppViewBatteryInfo);
    battery_info_free(app->battery_info);

    view_dispatcher_remove_view(app->view_dispatcher, PowerSettingsAppViewSubmenu);
    submenu_free(app->submenu);

    view_dispatcher_remove_view(app->view_dispatcher, PowerSettingsAppViewDialog);
    dialog_ex_free(app->dialog);

    view_dispatcher_remove_view(app->view_dispatcher, PowerSettingsAppViewVariableItemList);
    variable_item_list_free(app->variable_item_list);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Records
    furi_record_close(RECORD_POWER);
    furi_record_close(RECORD_GUI);

    free(app);
}

int32_t power_settings_app(void* p) {
    uint32_t first_scene = PowerSettingsAppSceneStart;
    ViewDispatcherType type = ViewDispatcherTypeFullscreen;
    if(p && strlen(p)) {
        if(!strcmp(p, "off")) {
            first_scene = PowerSettingsAppScenePowerOff;
            type = ViewDispatcherTypeDesktop;
        } else if(!strcmp(p, "about_battery")) {
            first_scene = PowerSettingsAppSceneBatteryInfo;
        }
    }
    PowerSettingsApp* app = power_settings_app_alloc(first_scene, type);
    while(true) {
        view_dispatcher_run(app->view_dispatcher);
        if(app->battery_info->exit_to_about) {
            app->battery_info->exit_to_about = false;
            if(about_settings_app("about_battery")) {
                scene_manager_next_scene(app->scene_manager, first_scene);
                continue;
            }
        }
        break;
    }
    if(type == ViewDispatcherTypeDesktop) {
        gui_set_hide_statusbar(app->gui, false);
    }
    power_settings_app_free(app);
    return 0;
}