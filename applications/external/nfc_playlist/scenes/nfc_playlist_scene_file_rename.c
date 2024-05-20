#include "../nfc_playlist.h"

void nfc_playlist_file_rename_menu_callback(void* context) {
    NfcPlaylist* nfc_playlist = context;
    Storage* storage = furi_record_open(RECORD_STORAGE);

    char const* old_file_path = (char*)furi_string_get_cstr(nfc_playlist->settings.file_path);
    char const* old_file_name =
        strchr(old_file_path, '/') != NULL ? &strrchr(old_file_path, '/')[1] : old_file_path;

    FuriString* tmp_old_file_path = furi_string_alloc_set_str(old_file_path);
    furi_string_replace(tmp_old_file_path, old_file_name, "");

    FuriString* tmp_new_file_path = furi_string_alloc();
    furi_string_printf(
        tmp_new_file_path,
        "%s%s.txt",
        furi_string_get_cstr(tmp_old_file_path),
        nfc_playlist->text_input_output);

    if(!storage_file_exists(storage, furi_string_get_cstr(tmp_new_file_path))) {
        storage_common_rename(
            storage,
            furi_string_get_cstr(nfc_playlist->settings.file_path),
            furi_string_get_cstr(tmp_new_file_path));
        nfc_playlist->settings.file_path =
            furi_string_alloc_set_str(furi_string_get_cstr(tmp_new_file_path));
    }
    furi_record_close(RECORD_STORAGE);
    furi_string_free(tmp_new_file_path);
    furi_string_free(tmp_old_file_path);

    scene_manager_previous_scene(nfc_playlist->scene_manager);
}

void nfc_playlist_file_rename_scene_on_enter(void* context) {
    NfcPlaylist* nfc_playlist = context;

    char const* tmp_file_path = (char*)furi_string_get_cstr(nfc_playlist->settings.file_path);
    char const* tmp_file_name =
        strchr(tmp_file_path, '/') != NULL ? &strrchr(tmp_file_path, '/')[1] : tmp_file_path;

    FuriString* tmp_file_name_furi = furi_string_alloc_set_str(tmp_file_name);
    furi_string_replace(tmp_file_name_furi, ".txt", "");

   nfc_playlist->text_input_output = (char*)malloc(50 * sizeof(char));
   strcpy(nfc_playlist->text_input_output, furi_string_get_cstr(tmp_file_name_furi));
   
   furi_string_free(tmp_file_name_furi);

   text_input_set_header_text(nfc_playlist->text_input, "Enter new file name");
   text_input_set_minimum_length(nfc_playlist->text_input, 1);
   text_input_set_result_callback(nfc_playlist->text_input, nfc_playlist_file_rename_menu_callback, nfc_playlist, nfc_playlist->text_input_output, (50*sizeof(char)), false);

    text_input_set_header_text(nfc_playlist->text_input, "Enter new file name");
    text_input_set_minimum_length(nfc_playlist->text_input, 1);
    text_input_set_result_callback(
        nfc_playlist->text_input,
        nfc_playlist_file_rename_menu_callback,
        nfc_playlist,
        nfc_playlist->text_input_output,
        50,
        false);

    view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_TextInput);
}

bool nfc_playlist_file_rename_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void nfc_playlist_file_rename_scene_on_exit(void* context) {
    NfcPlaylist* nfc_playlist = context;
    text_input_reset(nfc_playlist->text_input);
    free(nfc_playlist->text_input_output);
}