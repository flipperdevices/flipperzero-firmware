#include "nfc_playlist.h"
#include "scences/playlist_select.h"

void nfc_playlist_playlist_select_menu_callback(void* context) {
   NfcPlaylist* nfc_playlist = context;
   nfc_playlist->settings.playlist_selected_check = true;
   furi_string_move(nfc_playlist->settings.file_path, nfc_playlist->file_browser_output);
   furi_string_reset(nfc_playlist->file_browser_output);
   scene_manager_previous_scene(nfc_playlist->scene_manager);
}

void nfc_playlist_playlist_select_scene_on_enter(void* context) {
   NfcPlaylist* nfc_playlist = context;
   file_browser_configure(
      nfc_playlist->file_browser,
      ".txt",
      furi_string_get_cstr(nfc_playlist->settings.base_file_path),
      true,
      true,
      &I_sub1_10px,
      true);
   view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_PlaylistSelect);
   file_browser_set_callback(nfc_playlist->file_browser, nfc_playlist_playlist_select_menu_callback, nfc_playlist);
   file_browser_start(nfc_playlist->file_browser, nfc_playlist->settings.base_file_path);
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