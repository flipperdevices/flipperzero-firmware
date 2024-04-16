#include "../nfc_playlist.h"

void nfc_playlist_playlist_select_menu_callback(void* context) {
    NfcPlaylist* nfc_playlist = context;
    furi_string_swap(nfc_playlist->settings.file_path, nfc_playlist->file_browser_output);
    furi_string_reset(nfc_playlist->file_browser_output);
    scene_manager_previous_scene(nfc_playlist->scene_manager);
}

void nfc_playlist_playlist_select_scene_on_enter(void* context) {
    NfcPlaylist* nfc_playlist = context;
    file_browser_configure(
        nfc_playlist->file_browser, ".txt", PLAYLIST_LOCATION, true, true, &I_Nfc_10px, true);
    file_browser_set_callback(
        nfc_playlist->file_browser, nfc_playlist_playlist_select_menu_callback, nfc_playlist);
    FuriString* tmp_str = furi_string_alloc_set_str(PLAYLIST_LOCATION);
    file_browser_start(nfc_playlist->file_browser, tmp_str);
    furi_string_free(tmp_str);

    view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_FileBrowser);
}

bool nfc_playlist_playlist_select_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(event);
    UNUSED(context);
    return false;
}

void nfc_playlist_playlist_select_scene_on_exit(void* context) {
    NfcPlaylist* nfc_playlist = context;
    file_browser_stop(nfc_playlist->file_browser);
}