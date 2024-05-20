#include "../nfc_playlist.h"

void nfc_playlist_nfc_select_menu_callback(void* context) {
    NfcPlaylist* nfc_playlist = context;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);

    if(storage_file_open(
           file,
           furi_string_get_cstr(nfc_playlist->settings.file_path),
           FSAM_READ_WRITE,
           FSOM_OPEN_EXISTING)) {
        uint8_t buffer[PLAYLIST_VIEW_MAX_SIZE];
        uint16_t read_count = storage_file_read(file, buffer, PLAYLIST_VIEW_MAX_SIZE);
        FuriString* playlist_content = furi_string_alloc();

        for(uint16_t i = 0; i < read_count; i++) {
            furi_string_push_back(playlist_content, buffer[i]);
        }

        if(read_count > 0) {
            furi_string_printf(
                playlist_content, "\n%s", furi_string_get_cstr(nfc_playlist->file_browser_output));
        } else {
            furi_string_printf(
                playlist_content, "%s", furi_string_get_cstr(nfc_playlist->file_browser_output));
        }

        storage_file_write(
            file,
            furi_string_get_cstr(playlist_content),
            sizeof(char) * furi_string_utf8_length(playlist_content));

        nfc_playlist->settings.playlist_length++;

        furi_string_free(playlist_content);
        storage_file_close(file);
    }

    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
    furi_string_reset(nfc_playlist->file_browser_output);

    scene_manager_previous_scene(nfc_playlist->scene_manager);
}

void nfc_playlist_nfc_select_scene_on_enter(void* context) {
    NfcPlaylist* nfc_playlist = context;
    file_browser_configure(
        nfc_playlist->file_browser, ".nfc", "/ext/nfc/", true, true, &I_Nfc_10px, true);
    file_browser_set_callback(
        nfc_playlist->file_browser, nfc_playlist_nfc_select_menu_callback, nfc_playlist);
    FuriString* tmp_str = furi_string_alloc_set_str("/ext/nfc/");
    file_browser_start(nfc_playlist->file_browser, tmp_str);
    furi_string_free(tmp_str);

    view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_FileBrowser);
}

bool nfc_playlist_nfc_select_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(event);
    UNUSED(context);
    return false;
}

void nfc_playlist_nfc_select_scene_on_exit(void* context) {
    NfcPlaylist* nfc_playlist = context;
    file_browser_stop(nfc_playlist->file_browser);
}