#include "mifare_fuzzer_i.h"

/// @brief mifare_fuzzer_custom_event_callback()
/// @param context
/// @param event
/// @return
static bool mifare_fuzzer_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    MifareFuzzerApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

/// @brief mifare_fuzzer_back_event_callback()
/// @param context
/// @return
static bool mifare_fuzzer_back_event_callback(void* context) {
    furi_assert(context);
    MifareFuzzerApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

/// @brief mifare_fuzzer_tick_event_callback()
/// @param context
static void mifare_fuzzer_tick_event_callback(void* context) {
    furi_assert(context);
    MifareFuzzerApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

/// @brief mifare_fuzzer_alloc()
/// @return
MifareFuzzerApp* mifare_fuzzer_alloc() {
    MifareFuzzerApp* app = malloc(sizeof(MifareFuzzerApp));

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&mifare_fuzzer_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, mifare_fuzzer_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, mifare_fuzzer_back_event_callback);

    // 1000 ticks are about 1 sec
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, mifare_fuzzer_tick_event_callback, MIFARE_FUZZER_TICK_PERIOD);

    // Open GUI record
    app->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // view: select card type
    app->submenu_card = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, MifareFuzzerViewSelectCard, submenu_get_view(app->submenu_card));

    // view: select attack type
    app->submenu_attack = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, MifareFuzzerViewSelectAttack, submenu_get_view(app->submenu_attack));

    // view: emulator
    app->emulator_view = mifare_fuzzer_emulator_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        MifareFuzzerViewEmulator,
        mifare_fuzzer_emulator_get_view(app->emulator_view));

    // worker
    app->worker = mifare_fuzzer_worker_alloc();

    // storage
    app->storage = furi_record_open(RECORD_STORAGE);
    if(!storage_simply_mkdir(app->storage, MIFARE_FUZZER_APP_FOLDER)) {
        FURI_LOG_E(TAG, "Could not create folder: %s", MIFARE_FUZZER_APP_FOLDER);
    }

    // dialog
    app->dialogs = furi_record_open(RECORD_DIALOGS);

    // furi strings
    app->uid_str = furi_string_alloc();
    app->file_path = furi_string_alloc();
    app->app_folder = furi_string_alloc_set(MIFARE_FUZZER_APP_FOLDER);

    return app;
}

/// @brief mifare_fuzzer_free()
/// @param app
void mifare_fuzzer_free(MifareFuzzerApp* app) {
    furi_assert(app);

    // Views
    //FURI_LOG_D(TAG, "mifare_fuzzer_free() :: Views");
    view_dispatcher_remove_view(app->view_dispatcher, MifareFuzzerViewSelectCard);
    view_dispatcher_remove_view(app->view_dispatcher, MifareFuzzerViewSelectAttack);
    view_dispatcher_remove_view(app->view_dispatcher, MifareFuzzerViewEmulator);

    // Submenus
    //FURI_LOG_D(TAG, "mifare_fuzzer_free() :: Submenus");
    submenu_free(app->submenu_card);
    submenu_free(app->submenu_attack);

    // View Dispatcher
    //FURI_LOG_D(TAG, "mifare_fuzzer_free() :: View Dispatcher");
    view_dispatcher_free(app->view_dispatcher);

    // Scene Manager
    //FURI_LOG_D(TAG, "mifare_fuzzer_free() :: Scene Manager");
    scene_manager_free(app->scene_manager);

    // GUI
    //FURI_LOG_D(TAG, "mifare_fuzzer_free() :: GUI");
    furi_record_close(RECORD_GUI);
    app->gui = NULL;

    // Worker
    //FURI_LOG_D(TAG, "mifare_fuzzer_free() :: Worker");
    mifare_fuzzer_worker_free(app->worker);

    // storage
    furi_record_close(RECORD_STORAGE);
    app->storage = NULL;

    // dialog
    furi_record_close(RECORD_DIALOGS);
    app->dialogs = NULL;

    // furi strings
    furi_string_free(app->uid_str);
    furi_string_free(app->file_path);
    furi_string_free(app->app_folder);

    // App
    //FURI_LOG_D(TAG, "mifare_fuzzer_free() :: App");
    free(app);
}

/// @brief mifare_fuzzer_app (ENTRYPOINT)
/// @param p
/// @return
int32_t mifare_fuzzer_app(void* p) {
    UNUSED(p);
    //FURI_LOG_D(TAG, "mifare_fuzzer_app()");

    MifareFuzzerApp* app = mifare_fuzzer_alloc();
    // init some defaults
    scene_manager_set_scene_state(app->scene_manager, MifareFuzzerSceneStart, 0);
    scene_manager_set_scene_state(app->scene_manager, MifareFuzzerSceneAttack, 0);
    // open scene
    scene_manager_next_scene(app->scene_manager, MifareFuzzerSceneStart);
    view_dispatcher_run(app->view_dispatcher);
    // free
    mifare_fuzzer_free(app);

    return 0;
}
