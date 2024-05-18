#include "gba_cartridge_app.h"

bool gba_cartridge_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    GBACartridge* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void gba_cartridge_app_tick_event_callback(void* context) {
    furi_assert(context);
    GBACartridge* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

//leave app if back button pressed
bool gba_cartridge_app_navigation_event_callback(void* context) {
    furi_assert(context);
    GBACartridge* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

GBACartridge* gba_cartridge_app_app_alloc() {
    GBACartridge* app = malloc(sizeof(GBACartridge));
    app->gui = furi_record_open(RECORD_GUI);
    app->storage = furi_record_open(RECORD_STORAGE);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    //Turn backlight on, believe me this makes testing your app easier
    notification_message(app->notification, &sequence_display_backlight_on);

    //Scene additions
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&gba_cartridge_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, gba_cartridge_app_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, gba_cartridge_app_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, gba_cartridge_app_custom_event_callback);
    app->submenu = variable_item_list_alloc();

    // Used for File Browser
    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->file_path = furi_string_alloc();

    app->uart = usart_init(app);
    app->lp_uart = lp_uart_init(app);

    view_dispatcher_add_view(
        app->view_dispatcher, GBACartridgeViewIdMenu, variable_item_list_get_view(app->submenu));
    app->gba_cartridge_startscreen = gba_cartridge_startscreen_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        GBACartridgeViewIdStartscreen,
        gba_cartridge_startscreen_get_view(app->gba_cartridge_startscreen));
    app->gba_cartridge_scene_1 = gba_cartridge_scene_1_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        GBACartridgeViewIdScene1,
        gba_cartridge_scene_1_get_view(app->gba_cartridge_scene_1));

    //  Enable 5v
    // furi_hal_power_enable_otg();
    // furi_delay_ms(1);
    return app;
}

void gba_cartridge_app_app_free(GBACartridge* app) {
    furi_assert(app);
    // Views
    view_dispatcher_remove_view(app->view_dispatcher, GBACartridgeViewIdMenu);
    view_dispatcher_remove_view(app->view_dispatcher, GBACartridgeViewIdScene1);
    view_dispatcher_remove_view(app->view_dispatcher, GBACartridgeViewIdStartscreen);
    variable_item_list_free(app->submenu);

    // View Dispatcher
    view_dispatcher_free(app->view_dispatcher);
    // Scene manager
    scene_manager_free(app->scene_manager);

    //
    uart_free(app->uart);
    uart_free(app->lp_uart);

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);

    app->gui = NULL;
    app->notification = NULL;
    app->storage = NULL;

    furi_string_free(app->file_path);

    //Remove whatever is left
    free(app);
}

int32_t gba_cartridge_app(void* p) {
    UNUSED(p);
    // Disable expansion protocol to avoid interference with UART Handle
    Expansion* expansion = furi_record_open(RECORD_EXPANSION);
    expansion_disable(expansion);

    GBACartridge* app = gba_cartridge_app_app_alloc();

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(
        app->scene_manager, GBACartridgeSceneStartscreen); //Start with start screen

    furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);

    furi_hal_power_suppress_charge_exit();
    gba_cartridge_app_app_free(app);

    // Return previous state of expansion
    expansion_enable(expansion);
    furi_record_close(RECORD_EXPANSION);

    return 0;
}
