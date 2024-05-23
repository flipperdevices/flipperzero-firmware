#include "../nfc_playlist.h"

void nfc_playlist_name_new_playlist_menu_callback(void* context) {
    NfcPlaylist* nfc_playlist = context;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FuriString* file_name = furi_string_alloc();

    furi_string_printf(
        file_name, "/ext/apps_data/nfc_playlist/%s.txt", nfc_playlist->text_input_output);

    File* file = storage_file_alloc(storage);
    if(storage_file_open(file, furi_string_get_cstr(file_name), FSAM_READ_WRITE, FSOM_CREATE_NEW)) {
        storage_file_close(file);
        furi_string_swap(nfc_playlist->settings.playlist_path, file_name);
    }

    nfc_playlist->settings.playlist_length = 0;

    storage_file_free(file);
    furi_string_free(file_name);
    furi_record_close(RECORD_STORAGE);
    scene_manager_previous_scene(nfc_playlist->scene_manager);
}

void nfc_playlist_name_new_playlist_scene_on_enter(void* context) {
    NfcPlaylist* nfc_playlist = context;

    nfc_playlist->text_input_output = (char*)malloc(PLAYLIST_NAME_LEN);
    text_input_set_header_text(nfc_playlist->text_input, "Enter file name");
    text_input_set_minimum_length(nfc_playlist->text_input, 1);
    text_input_set_result_callback(
        nfc_playlist->text_input,
        nfc_playlist_name_new_playlist_menu_callback,
        nfc_playlist,
        nfc_playlist->text_input_output,
        PLAYLIST_NAME_LEN,
        true);

    view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_TextInput);
}

bool nfc_playlist_name_new_playlist_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void nfc_playlist_name_new_playlist_scene_on_exit(void* context) {
    NfcPlaylist* nfc_playlist = context;
    free(nfc_playlist->text_input_output);
    text_input_reset(nfc_playlist->text_input);
}