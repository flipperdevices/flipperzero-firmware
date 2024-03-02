#include "nfc_playlist.h"
#include "scences/text_input.h"

void nfc_playlist_text_input_menu_callback(void* context) {
    NfcPlaylist* nfc_playlist = context;
    Storage* storage = furi_record_open(RECORD_STORAGE);

    char const* old_file_path = (char*)furi_string_get_cstr(nfc_playlist->settings.file_path);
    char const* old_file_name =
        strchr(old_file_path, '/') != NULL ? &strrchr(old_file_path, '/')[1] : old_file_path;

    int file_path_size = (strlen(old_file_path) - strlen(old_file_name) + 1);

    char* file_path = (char*)malloc(file_path_size);
    snprintf(file_path, file_path_size, "%s", old_file_path);

    int new_file_path_size =
        (strlen(nfc_playlist->playlist_name) + strlen(".txt") + file_path_size + 1);

    char* new_file_name = (char*)malloc(new_file_path_size);
    snprintf(
        new_file_name,
        new_file_path_size,
        "%s%s%s",
        file_path,
        nfc_playlist->playlist_name,
        ".txt");

    if(!storage_file_exists(storage, new_file_name)) {
        storage_common_rename(
            storage, furi_string_get_cstr(nfc_playlist->settings.file_path), new_file_name);
        nfc_playlist->settings.file_path = furi_string_alloc_set_str(new_file_name);
    }

    free(new_file_name);
    free(file_path);
    free(nfc_playlist->playlist_name);
    furi_record_close(RECORD_STORAGE);

    scene_manager_previous_scene(nfc_playlist->scene_manager);
}

void nfc_playlist_text_input_scene_on_enter(void* context) {
    NfcPlaylist* nfc_playlist = context;
    nfc_playlist->playlist_name = (char*)malloc(50);
    text_input_set_header_text(nfc_playlist->text_input, "Enter new file name");
    text_input_set_minimum_length(nfc_playlist->text_input, 1);
    text_input_set_result_callback(
        nfc_playlist->text_input,
        nfc_playlist_text_input_menu_callback,
        nfc_playlist,
        nfc_playlist->playlist_name,
        50,
        true);
    view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_TextInput);
}

bool nfc_playlist_text_input_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void nfc_playlist_text_input_scene_on_exit(void* context) {
    NfcPlaylist* nfc_playlist = context;
    text_input_reset(nfc_playlist->text_input);
}