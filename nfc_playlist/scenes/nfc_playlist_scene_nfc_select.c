#include "../nfc_playlist.h"

void nfc_playlist_nfc_select_menu_callback(void* context) {
    NfcPlaylist* nfc_playlist = context;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    Stream* stream = file_stream_alloc(storage);

    if(file_stream_open(
           stream,
           furi_string_get_cstr(nfc_playlist->settings.playlist_path),
           FSAM_READ_WRITE,
           FSOM_OPEN_EXISTING)) {
        FuriString* line = furi_string_alloc();
        FuriString* tmp_str = furi_string_alloc();
        while(stream_read_line(stream, line)) {
            furi_string_cat_printf(tmp_str, "%s", furi_string_get_cstr(line));
        }

        if(!furi_string_empty(tmp_str)) {
            furi_string_cat_printf(
                tmp_str, "\n%s", furi_string_get_cstr(nfc_playlist->file_browser_output));
        } else {
            furi_string_printf(
                tmp_str, "%s", furi_string_get_cstr(nfc_playlist->file_browser_output));
        }

        stream_clean(stream);
        furi_string_free(line);
        stream_write_string(stream, tmp_str);
        file_stream_close(stream);
        furi_string_free(tmp_str);
        nfc_playlist->settings.playlist_length++;
    }

    stream_free(stream);
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