#include "../nfc_playlist.h"

void nfc_playlist_confirm_delete_menu_callback(GuiButtonType result, InputType type, void* context) {
    NfcPlaylist* nfc_playlist = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(nfc_playlist->view_dispatcher, result);
    }
}

void nfc_playlist_confirm_delete_scene_on_enter(void* context) {
    NfcPlaylist* nfc_playlist = context;

    FuriString* temp_str = furi_string_alloc();
    char* file_path = (char*)furi_string_get_cstr(nfc_playlist->settings.playlist_path);
    furi_string_printf(
        temp_str,
        "\e#Delete %s?\e#",
        strchr(file_path, '/') != NULL ? &strrchr(file_path, '/')[1] : file_path);
    furi_string_replace(temp_str, ".txt", "");

    widget_add_text_box_element(
        nfc_playlist->widget,
        0,
        0,
        128,
        23,
        AlignCenter,
        AlignCenter,
        furi_string_get_cstr(temp_str),
        false);
    widget_add_button_element(
        nfc_playlist->widget,
        GuiButtonTypeLeft,
        "Cancel",
        nfc_playlist_confirm_delete_menu_callback,
        nfc_playlist);
    widget_add_button_element(
        nfc_playlist->widget,
        GuiButtonTypeRight,
        "Delete",
        nfc_playlist_confirm_delete_menu_callback,
        nfc_playlist);

    furi_string_free(temp_str);

    view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Widget);
}

bool nfc_playlist_confirm_delete_scene_on_event(void* context, SceneManagerEvent event) {
    NfcPlaylist* nfc_playlist = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case GuiButtonTypeRight:
            Storage* storage = furi_record_open(RECORD_STORAGE);
            storage_simply_remove(
                storage, furi_string_get_cstr(nfc_playlist->settings.playlist_path));
            nfc_playlist->settings.playlist_selected = false;
            furi_string_reset(nfc_playlist->settings.playlist_path);
            furi_record_close(RECORD_STORAGE);
            consumed = true;
            scene_manager_search_and_switch_to_previous_scene(
                nfc_playlist->scene_manager, NfcPlaylistScene_MainMenu);
            break;
        default:
            scene_manager_previous_scene(nfc_playlist->scene_manager);
            break;
        }
    }
    return consumed;
}

void nfc_playlist_confirm_delete_scene_on_exit(void* context) {
    NfcPlaylist* nfc_playlist = context;
    widget_reset(nfc_playlist->widget);
}