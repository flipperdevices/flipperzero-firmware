#include "nfc_playlist.h"

static bool nfc_playlist_custom_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    NfcPlaylist* nfc_playlist = context;
    return scene_manager_handle_custom_event(nfc_playlist->scene_manager, custom_event);
}

static bool nfc_playlist_back_event_callback(void* context) {
    furi_assert(context);
    NfcPlaylist* nfc_playlist = context;
    return scene_manager_handle_back_event(nfc_playlist->scene_manager);
}

static NfcPlaylist* nfc_playlist_alloc() {
    NfcPlaylist* nfc_playlist = malloc(sizeof(NfcPlaylist));
    furi_assert(nfc_playlist);
    nfc_playlist->scene_manager = scene_manager_alloc(&nfc_playlist_scene_handlers, nfc_playlist);
    nfc_playlist->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(nfc_playlist->view_dispatcher);
    nfc_playlist->variable_item_list = variable_item_list_alloc();
    nfc_playlist->submenu = submenu_alloc();
    nfc_playlist->widget = widget_alloc();

    nfc_playlist->settings.playlist_path = furi_string_alloc();
    nfc_playlist->file_browser_output = furi_string_alloc();
    nfc_playlist->settings.playlist_selected = false;
    nfc_playlist->settings.emulate_timeout = default_emulate_timeout;
    nfc_playlist->settings.emulate_delay = default_emulate_delay;
    nfc_playlist->settings.emulate_led_indicator = default_emulate_led_indicator;
    nfc_playlist->settings.skip_error = default_skip_error;

    nfc_playlist->notification = furi_record_open(RECORD_NOTIFICATION);
    nfc_playlist->file_browser = file_browser_alloc(nfc_playlist->file_browser_output);

    nfc_playlist->text_input = text_input_alloc();
    nfc_playlist->popup = popup_alloc();

    view_dispatcher_set_event_callback_context(nfc_playlist->view_dispatcher, nfc_playlist);
    view_dispatcher_set_custom_event_callback(
        nfc_playlist->view_dispatcher, nfc_playlist_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        nfc_playlist->view_dispatcher, nfc_playlist_back_event_callback);

    view_dispatcher_add_view(
        nfc_playlist->view_dispatcher,
        NfcPlaylistView_Submenu,
        submenu_get_view(nfc_playlist->submenu));
    view_dispatcher_add_view(
        nfc_playlist->view_dispatcher, NfcPlaylistView_Popup, popup_get_view(nfc_playlist->popup));
    view_dispatcher_add_view(
        nfc_playlist->view_dispatcher,
        NfcPlaylistView_Widget,
        widget_get_view(nfc_playlist->widget));
    view_dispatcher_add_view(
        nfc_playlist->view_dispatcher,
        NfcPlaylistView_VariableItemList,
        variable_item_list_get_view(nfc_playlist->variable_item_list));
    view_dispatcher_add_view(
        nfc_playlist->view_dispatcher,
        NfcPlaylistView_FileBrowser,
        file_browser_get_view(nfc_playlist->file_browser));
    view_dispatcher_add_view(
        nfc_playlist->view_dispatcher,
        NfcPlaylistView_TextInput,
        text_input_get_view(nfc_playlist->text_input));

    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_simply_mkdir(storage, PLAYLIST_DIR);

    furi_record_close(RECORD_STORAGE);

    return nfc_playlist;
}

static void nfc_playlist_free(NfcPlaylist* nfc_playlist) {
    furi_assert(nfc_playlist);

    view_dispatcher_remove_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Submenu);
    view_dispatcher_remove_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Popup);
    view_dispatcher_remove_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Widget);
    view_dispatcher_remove_view(nfc_playlist->view_dispatcher, NfcPlaylistView_VariableItemList);
    view_dispatcher_remove_view(nfc_playlist->view_dispatcher, NfcPlaylistView_FileBrowser);
    view_dispatcher_remove_view(nfc_playlist->view_dispatcher, NfcPlaylistView_TextInput);

    scene_manager_free(nfc_playlist->scene_manager);
    view_dispatcher_free(nfc_playlist->view_dispatcher);
    furi_record_close(RECORD_NOTIFICATION);

    variable_item_list_free(nfc_playlist->variable_item_list);
    submenu_free(nfc_playlist->submenu);
    widget_free(nfc_playlist->widget);
    file_browser_free(nfc_playlist->file_browser);
    text_input_free(nfc_playlist->text_input);
    popup_free(nfc_playlist->popup);

    furi_string_free(nfc_playlist->settings.playlist_path);
    furi_string_free(nfc_playlist->file_browser_output);
    free(nfc_playlist);
}

void nfc_playlist_set_log_level() {
#ifdef FURI_DEBUG
    furi_log_set_level(FuriLogLevelTrace);
#else
    furi_log_set_level(FuriLogLevelInfo);
#endif
}

int32_t nfc_playlist_main(void* p) {
    UNUSED(p);

    NfcPlaylist* nfc_playlist = nfc_playlist_alloc();

    nfc_playlist_set_log_level();

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        nfc_playlist->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_MainMenu);
    view_dispatcher_run(nfc_playlist->view_dispatcher);

    furi_record_close(RECORD_GUI);
    nfc_playlist_free(nfc_playlist);

    return 0;
}

NotificationMessage blink_message_normal = {
    .type = NotificationMessageTypeLedBlinkStart,
    .data.led_blink.color = LightBlue | LightGreen,
    .data.led_blink.on_time = 10,
    .data.led_blink.period = 100};
const NotificationSequence blink_sequence_normal = {
    &blink_message_normal,
    &message_do_not_reset,
    NULL};

NotificationMessage blink_message_error = {
    .type = NotificationMessageTypeLedBlinkStart,
    .data.led_blink.color = LightRed,
    .data.led_blink.on_time = 10,
    .data.led_blink.period = 100};

const NotificationSequence blink_sequence_error = {
    &blink_message_error,
    &message_do_not_reset,
    NULL};

void start_blink(NfcPlaylist* nfc_playlist, int state) {
    if(nfc_playlist->settings.emulate_led_indicator) {
        if(state == NfcPlaylistLedState_Normal) {
            notification_message_block(nfc_playlist->notification, &blink_sequence_normal);
        } else if(state == NfcPlaylistLedState_Error) {
            notification_message_block(nfc_playlist->notification, &blink_sequence_error);
        }
    }
}

void stop_blink(NfcPlaylist* nfc_playlist) {
    if(nfc_playlist->settings.emulate_led_indicator) {
        notification_message_block(nfc_playlist->notification, &sequence_blink_stop);
    }
}