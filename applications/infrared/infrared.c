#include "infrared_i.h"

static const NotificationSequence* infrared_notification_sequences[] = {
    &sequence_success,
    &sequence_set_only_green_255,
    &sequence_reset_green,
    &sequence_blink_cyan_10,
    &sequence_blink_magenta_10};

static void infrared_make_app_folder(Infrared* infrared) {
    if(!storage_simply_mkdir(infrared->storage, INFRARED_APP_FOLDER)) {
        dialog_message_show_storage_error(infrared->dialogs, "Cannot create\napp folder");
    }
}

static bool infrared_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    Infrared* infrared = context;
    return scene_manager_handle_custom_event(infrared->scene_manager, event);
}

static bool infrared_back_event_callback(void* context) {
    furi_assert(context);
    Infrared* infrared = context;
    return scene_manager_handle_back_event(infrared->scene_manager);
}

static void infrared_tick_event_callback(void* context) {
    furi_assert(context);
    Infrared* infrared = context;
    scene_manager_handle_tick_event(infrared->scene_manager);
}

static Infrared* infrared_alloc() {
    Infrared* infrared = malloc(sizeof(Infrared));

    string_init(infrared->file_path);

    infrared->scene_manager = scene_manager_alloc(&infrared_scene_handlers, infrared);
    infrared->view_dispatcher = view_dispatcher_alloc();

    infrared->gui = furi_record_open("gui");

    ViewDispatcher* view_dispatcher = infrared->view_dispatcher;
    view_dispatcher_attach_to_gui(view_dispatcher, infrared->gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_enable_queue(view_dispatcher);
    view_dispatcher_set_event_callback_context(view_dispatcher, infrared);
    view_dispatcher_set_custom_event_callback(view_dispatcher, infrared_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(view_dispatcher, infrared_back_event_callback);
    view_dispatcher_set_tick_event_callback(view_dispatcher, infrared_tick_event_callback, 100);

    infrared->storage = furi_record_open("storage");
    infrared->dialogs = furi_record_open("dialogs");
    infrared->notifications = furi_record_open("notification");

    infrared->worker = infrared_worker_alloc();

    infrared->submenu = submenu_alloc();
    view_dispatcher_add_view(
        view_dispatcher, InfraredViewSubmenu, submenu_get_view(infrared->submenu));

    infrared->text_input = text_input_alloc();
    view_dispatcher_add_view(
        view_dispatcher, InfraredViewTextInput, text_input_get_view(infrared->text_input));

    infrared->dialog_ex = dialog_ex_alloc();
    view_dispatcher_add_view(
        view_dispatcher, InfraredViewDialogEx, dialog_ex_get_view(infrared->dialog_ex));

    infrared->button_menu = button_menu_alloc();
    view_dispatcher_add_view(
        view_dispatcher, InfraredViewButtonMenu, button_menu_get_view(infrared->button_menu));

    infrared->popup = popup_alloc();
    view_dispatcher_add_view(view_dispatcher, InfraredViewPopup, popup_get_view(infrared->popup));

    infrared->view_stack = view_stack_alloc();
    view_set_orientation(view_stack_get_view(infrared->view_stack), ViewOrientationVertical);
    view_dispatcher_add_view(
        view_dispatcher, InfraredViewStack, view_stack_get_view(infrared->view_stack));

    infrared->button_panel = button_panel_alloc();
    infrared->loading = loading_alloc();
    infrared->progress = infrared_progress_view_alloc();

    view_stack_add_view(infrared->view_stack, button_panel_get_view(infrared->button_panel));

    return infrared;
}

static void infrared_free(Infrared* infrared) {
    furi_assert(infrared);
    ViewDispatcher* view_dispatcher = infrared->view_dispatcher;

    view_dispatcher_remove_view(view_dispatcher, InfraredViewSubmenu);
    submenu_free(infrared->submenu);

    view_dispatcher_remove_view(view_dispatcher, InfraredViewTextInput);
    text_input_free(infrared->text_input);

    view_dispatcher_remove_view(view_dispatcher, InfraredViewDialogEx);
    dialog_ex_free(infrared->dialog_ex);

    view_dispatcher_remove_view(view_dispatcher, InfraredViewButtonMenu);
    button_menu_free(infrared->button_menu);

    view_dispatcher_remove_view(view_dispatcher, InfraredViewPopup);
    popup_free(infrared->popup);

    view_dispatcher_remove_view(view_dispatcher, InfraredViewStack);
    view_stack_remove_view(infrared->view_stack, button_panel_get_view(infrared->button_panel));
    view_stack_free(infrared->view_stack);

    button_panel_free(infrared->button_panel);
    loading_free(infrared->loading);
    infrared_progress_view_free(infrared->progress);

    view_dispatcher_free(view_dispatcher);
    scene_manager_free(infrared->scene_manager);

    furi_record_close("gui");
    infrared->gui = NULL;

    furi_record_close("notification");
    infrared->notifications = NULL;

    furi_record_close("dialogs");
    infrared->dialogs = NULL;

    furi_record_close("gui");
    infrared->gui = NULL;

    string_clear(infrared->file_path);

    free(infrared);
}

void infrared_play_notification_message(Infrared* infrared, uint32_t message) {
    furi_assert(message < sizeof(infrared_notification_sequences) / sizeof(NotificationSequence*));
    notification_message(infrared->notifications, infrared_notification_sequences[message]);
}

int32_t infrared_app(void* p) {
    Infrared* infrared = infrared_alloc();

    infrared_make_app_folder(infrared);

    bool is_remote_loaded = false;

    if(p) {
        string_set_str(infrared->file_path, (const char*)p);
        //         is_remote_loaded = //Load remote function
        //         if(!is_remote_loaded) {
        //             dialog_message_show_storage_error(infrared->dialogs, "Failed to load\nselected remote");
        //             return -1;
        //         }
    }

    if(is_remote_loaded) {
        scene_manager_next_scene(infrared->scene_manager, InfraredSceneRemote);
    } else {
        scene_manager_next_scene(infrared->scene_manager, InfraredSceneStart);
    }

    view_dispatcher_run(infrared->view_dispatcher);

    infrared_free(infrared);
    return 0;
}
