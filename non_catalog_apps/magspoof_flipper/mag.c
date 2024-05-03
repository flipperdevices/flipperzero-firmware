#include "mag_i.h"

#define TAG "Mag"

static bool mag_debug_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    Mag* mag = context;
    return scene_manager_handle_custom_event(mag->scene_manager, event);
}

static bool mag_debug_back_event_callback(void* context) {
    furi_assert(context);
    Mag* mag = context;
    return scene_manager_handle_back_event(mag->scene_manager);
}

static Mag* mag_alloc() {
    Mag* mag = malloc(sizeof(Mag));

    mag->storage = furi_record_open(RECORD_STORAGE);
    mag->dialogs = furi_record_open(RECORD_DIALOGS);

    mag->file_name = furi_string_alloc();
    mag->file_path = furi_string_alloc_set(MAG_APP_FOLDER);
    mag->args = furi_string_alloc();

    mag->view_dispatcher = view_dispatcher_alloc();
    mag->scene_manager = scene_manager_alloc(&mag_scene_handlers, mag);
    view_dispatcher_enable_queue(mag->view_dispatcher);
    view_dispatcher_set_event_callback_context(mag->view_dispatcher, mag);
    view_dispatcher_set_custom_event_callback(
        mag->view_dispatcher, mag_debug_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        mag->view_dispatcher, mag_debug_back_event_callback);

    mag->mag_dev = mag_device_alloc();
    mag_state_load(&mag->state);

    // Open GUI record
    mag->gui = furi_record_open(RECORD_GUI);

    // Open Notification record
    mag->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Submenu
    mag->submenu = submenu_alloc();
    view_dispatcher_add_view(mag->view_dispatcher, MagViewSubmenu, submenu_get_view(mag->submenu));

    // Popup
    mag->popup = popup_alloc();
    view_dispatcher_add_view(mag->view_dispatcher, MagViewPopup, popup_get_view(mag->popup));

    // Loading
    mag->loading = loading_alloc();
    view_dispatcher_add_view(mag->view_dispatcher, MagViewLoading, loading_get_view(mag->loading));

    // Widget
    mag->widget = widget_alloc();
    view_dispatcher_add_view(mag->view_dispatcher, MagViewWidget, widget_get_view(mag->widget));

    // Variable Item List
    mag->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        mag->view_dispatcher,
        MagViewVariableItemList,
        variable_item_list_get_view(mag->variable_item_list));

    // Text Input
    mag->text_input = text_input_alloc();
    view_dispatcher_add_view(
        mag->view_dispatcher, MagViewTextInput, text_input_get_view(mag->text_input));

    // Custom Mag Text Input
    mag->mag_text_input = mag_text_input_alloc();
    view_dispatcher_add_view(
        mag->view_dispatcher, MagViewTextMagInput, mag_text_input_get_view(mag->mag_text_input));

    // Disable expansion protocol to avoid interference with UART Handle
    mag->expansion = furi_record_open(RECORD_EXPANSION);
    expansion_disable(mag->expansion);

    // Move UART here? conditional upon setting?

    return mag;
}

static void mag_free(Mag* mag) {
    furi_assert(mag);

    furi_string_free(mag->file_name);
    furi_string_free(mag->file_path);
    furi_string_free(mag->args);

    // Mag device
    mag_device_free(mag->mag_dev);
    mag->mag_dev = NULL;

    // Submenu
    view_dispatcher_remove_view(mag->view_dispatcher, MagViewSubmenu);
    submenu_free(mag->submenu);

    // Popup
    view_dispatcher_remove_view(mag->view_dispatcher, MagViewPopup);
    popup_free(mag->popup);

    // Loading
    view_dispatcher_remove_view(mag->view_dispatcher, MagViewLoading);
    loading_free(mag->loading);

    // Widget
    view_dispatcher_remove_view(mag->view_dispatcher, MagViewWidget);
    widget_free(mag->widget);

    // Variable Item List
    view_dispatcher_remove_view(mag->view_dispatcher, MagViewVariableItemList);
    variable_item_list_free(mag->variable_item_list);

    // TextInput
    view_dispatcher_remove_view(mag->view_dispatcher, MagViewTextInput);
    text_input_free(mag->text_input);

    view_dispatcher_remove_view(mag->view_dispatcher, MagViewTextMagInput);
    mag_text_input_free(mag->mag_text_input);

    // View Dispatcher
    view_dispatcher_free(mag->view_dispatcher);

    // Scene Manager
    scene_manager_free(mag->scene_manager);

    // GUI
    furi_record_close(RECORD_GUI);
    mag->gui = NULL;

    // Notifications
    furi_record_close(RECORD_NOTIFICATION);
    mag->notifications = NULL;

    // Return previous state of expansion
    expansion_enable(mag->expansion);
    furi_record_close(RECORD_EXPANSION);

    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);

    free(mag);
}

// entry point for app
int32_t mag_app(void* p) {
    const char* args = p;

    Mag* mag = mag_alloc();

    if(args && strlen(args)) {
        furi_string_set(mag->args, args);
    }

    mag_make_app_folder(mag);
    mag_migrate_and_copy_files(mag);

    // Enable 5v power, multiple attempts to avoid issues with power chip protection false triggering
    uint8_t attempts = 0;
    bool otg_was_enabled = furi_hal_power_is_otg_enabled();
    while(!furi_hal_power_is_otg_enabled() && attempts++ < 5) {
        furi_hal_power_enable_otg();
        furi_delay_ms(10);
    }

    view_dispatcher_attach_to_gui(mag->view_dispatcher, mag->gui, ViewDispatcherTypeFullscreen);

    if(furi_string_empty(mag->args)) {
        scene_manager_next_scene(mag->scene_manager, MagSceneStart);
    } else {
        mag_device_load_data(mag->mag_dev, mag->args, true);
        MagTrackState auto_track = mag_device_autoselect_track_state(mag->mag_dev);
        if(auto_track) {
            mag->state.track = auto_track;
        }
        scene_manager_next_scene(mag->scene_manager, MagSceneEmulate);
    }

    view_dispatcher_run(mag->view_dispatcher);

    // Disable 5v power
    if(furi_hal_power_is_otg_enabled() && !otg_was_enabled) {
        furi_hal_power_disable_otg();
    }

    mag_free(mag);

    return 0;
}

void mag_make_app_folder(Mag* mag) {
    furi_assert(mag);

    if(!storage_simply_mkdir(mag->storage, MAG_APP_FOLDER)) {
        dialog_message_show_storage_error(mag->dialogs, "Cannot create\napp folder");
    }
}

void mag_migrate_and_copy_files(Mag* mag) {
    furi_assert(mag);
    Storage* storage = mag->storage;

    storage_common_migrate(storage, EXT_PATH("magspoof"), STORAGE_APP_DATA_PATH_PREFIX);
    storage_common_migrate(storage, EXT_PATH("mag"), STORAGE_APP_DATA_PATH_PREFIX);

    if(!storage_common_exists(storage, APP_DATA_PATH(MAG_EXAMPLE_FILE_1))) {
        storage_common_copy(
            storage, APP_ASSETS_PATH(MAG_EXAMPLE_FILE_1), APP_DATA_PATH(MAG_EXAMPLE_FILE_1));
    }

    if(!storage_common_exists(storage, APP_DATA_PATH(MAG_EXAMPLE_FILE_2))) {
        storage_common_copy(
            storage, APP_ASSETS_PATH(MAG_EXAMPLE_FILE_2), APP_DATA_PATH(MAG_EXAMPLE_FILE_2));
    }

    if(!storage_common_exists(storage, APP_DATA_PATH(MAG_EXAMPLE_FILE_3))) {
        storage_common_copy(
            storage, APP_ASSETS_PATH(MAG_EXAMPLE_FILE_3), APP_DATA_PATH(MAG_EXAMPLE_FILE_3));
    }
}

void mag_text_store_set(Mag* mag, const char* text, ...) {
    furi_assert(mag);
    va_list args;
    va_start(args, text);

    vsnprintf(mag->text_store, MAG_TEXT_STORE_SIZE, text, args);

    va_end(args);
}

void mag_text_store_clear(Mag* mag) {
    furi_assert(mag);
    memset(mag->text_store, 0, sizeof(mag->text_store));
}

void mag_popup_timeout_callback(void* context) {
    Mag* mag = context;
    view_dispatcher_send_custom_event(mag->view_dispatcher, MagEventPopupClosed);
}

void mag_widget_callback(GuiButtonType result, InputType type, void* context) {
    Mag* mag = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(mag->view_dispatcher, result);
    }
}

void mag_text_input_callback(void* context) {
    Mag* mag = context;
    view_dispatcher_send_custom_event(mag->view_dispatcher, MagEventNext);
}

void mag_show_loading_popup(void* context, bool show) {
    Mag* mag = context;

    if(show) {
        // Raise timer priority so that animations can play
        furi_timer_set_thread_priority(FuriTimerThreadPriorityElevated);
        view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewLoading);
    } else {
        // Restore default timer priority
        furi_timer_set_thread_priority(FuriTimerThreadPriorityNormal);
    }
}
