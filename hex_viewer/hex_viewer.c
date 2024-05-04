#include "hex_viewer.h"

bool hex_viewer_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    HexViewer* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void hex_viewer_tick_event_callback(void* context) {
    furi_assert(context);
    HexViewer* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

//leave app if back button pressed
bool hex_viewer_navigation_event_callback(void* context) {
    furi_assert(context);
    HexViewer* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

HexViewer* hex_viewer_app_alloc() {
    HexViewer* app = malloc(sizeof(HexViewer));

    app->model = malloc(sizeof(HexViewerModel));
    memset(app->model, 0, sizeof(HexViewerModel));

    app->gui = furi_record_open(RECORD_GUI);
    app->storage = furi_record_open(RECORD_STORAGE);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    //Turn backlight on, believe me this makes testing your app easier
    notification_message(app->notification, &sequence_display_backlight_on);

    //Scene additions
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&hex_viewer_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, hex_viewer_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, hex_viewer_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, hex_viewer_custom_event_callback);

    // Set defaults, in case no config loaded
    app->haptic = 1;
    app->speaker = 1;
    app->led = 1;
    app->save_settings = 1;

    // Used for File Browser
    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->file_path = furi_string_alloc();

    // Load configs
    hex_viewer_read_settings(app);

    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, HexViewerViewIdMenu, submenu_get_view(app->submenu));

    app->hex_viewer_startscreen = hex_viewer_startscreen_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        HexViewerViewIdStartscreen,
        hex_viewer_startscreen_get_view(app->hex_viewer_startscreen));

    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, HexViewerViewIdScroll, text_input_get_view(app->text_input));

    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        HexViewerViewIdSettings,
        variable_item_list_get_view(app->variable_item_list));

    //End Scene Additions

    return app;
}

void hex_viewer_app_free(HexViewer* app) {
    furi_assert(app);

    if(app->model->stream) {
        buffered_file_stream_close(app->model->stream);
        stream_free(app->model->stream);
    }

    // Scene manager
    scene_manager_free(app->scene_manager);

    // View Dispatcher
    view_dispatcher_remove_view(app->view_dispatcher, HexViewerViewIdMenu);
    submenu_free(app->submenu);
    view_dispatcher_remove_view(app->view_dispatcher, HexViewerViewIdStartscreen);
    hex_viewer_startscreen_free(app->hex_viewer_startscreen);
    view_dispatcher_remove_view(app->view_dispatcher, HexViewerViewIdScroll);
    text_input_free(app->text_input);
    view_dispatcher_remove_view(app->view_dispatcher, HexViewerViewIdSettings);
    variable_item_list_free(app->variable_item_list);

    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_GUI);

    app->storage = NULL;
    app->gui = NULL;
    app->notification = NULL;

    // Close File Browser
    furi_record_close(RECORD_DIALOGS);
    furi_string_free(app->file_path);

    free(app->model);

    //Remove whatever is left
    free(app);
}

int32_t hex_viewer_app(void* p) {
    UNUSED(p);
    HexViewer* app = hex_viewer_app_alloc();

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    if(p && strlen(p) && hex_viewer_open_file(app, (const char*)p)) {
        hex_viewer_read_file(app);
        scene_manager_next_scene(app->scene_manager, HexViewerSceneStartscreen);
    } else {
        scene_manager_next_scene(app->scene_manager, HexViewerSceneStartscreen);
        scene_manager_next_scene(app->scene_manager, HexViewerSceneOpen);
    }

    furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);

    hex_viewer_save_settings(app);

    furi_hal_power_suppress_charge_exit();
    hex_viewer_app_free(app);

    return 0;
}
