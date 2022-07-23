#include "wifi_marauder_app_i.h"

#include <furi.h>
#include <furi_hal.h>

static bool wifi_marauder_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    WifiMarauderApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool wifi_marauder_app_back_event_callback(void* context) {
    furi_assert(context);
    WifiMarauderApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void wifi_marauder_app_tick_event_callback(void* context) {
    furi_assert(context);
    WifiMarauderApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

WifiMarauderApp* wifi_marauder_app_alloc() {
    WifiMarauderApp* app = malloc(sizeof(WifiMarauderApp));

    app->selected_wifi_channel = 1;

    app->gui = furi_record_open("gui");

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&wifi_marauder_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, wifi_marauder_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, wifi_marauder_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, wifi_marauder_app_tick_event_callback, 100);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->var_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        WifiMarauderAppViewVarItemList,
        variable_item_list_get_view(app->var_item_list));

    // TODO add other views

    scene_manager_next_scene(app->scene_manager, WifiMarauderSceneStart);

    return app;
}

void wifi_marauder_app_free(WifiMarauderApp* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, WifiMarauderAppViewVarItemList);
    // TODO add back
    // view_dispatcher_remove_view(app->view_dispatcher, WifiMarauderAppViewScan);
    // view_dispatcher_remove_view(app->view_dispatcher, WifiMarauderAppViewAttack);
    // view_dispatcher_remove_view(app->view_dispatcher, WifiMarauderAppViewSniff);
    // view_dispatcher_remove_view(app->view_dispatcher, WifiMarauderAppViewChannel);
    // view_dispatcher_remove_view(app->view_dispatcher, WifiMarauderAppViewUpdate);
    // view_dispatcher_remove_view(app->view_dispatcher, WifiMarauderAppViewReboot);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Close records
    furi_record_close("gui");

    free(app);
}

int32_t wifi_marauder_app(void* p) {
    UNUSED(p);
    WifiMarauderApp* wifi_marauder_app = wifi_marauder_app_alloc();

    view_dispatcher_run(wifi_marauder_app->view_dispatcher);

    wifi_marauder_app_free(wifi_marauder_app);

    return 0;
}
