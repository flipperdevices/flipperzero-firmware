#include "nfc_playlist.h"
#include "scences/file_exists_error.h"

void nfc_playlist_file_exists_error_scene_on_enter(void* context) {
   NfcPlaylist* nfc_playlist = context;
   popup_reset(nfc_playlist->popup);
   popup_set_header(nfc_playlist->popup, "File Already Exists", 64, 10, AlignCenter, AlignTop);
   popup_set_text(nfc_playlist->popup, "The file already exists, Press back to try again", 64, 50, AlignCenter, AlignTop);
   popup_set_context(nfc_playlist->popup, nfc_playlist);
   view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_FileExistsError);
}

bool nfc_playlist_file_exists_error_scene_on_event(void* context, SceneManagerEvent event) {
   UNUSED(event);
   UNUSED(context);
   return false;
}

void nfc_playlist_file_exists_error_scene_on_exit(void* context) {
   NfcPlaylist* nfc_playlist = context;
   popup_reset(nfc_playlist->popup);
   scene_manager_previous_scene(nfc_playlist->scene_manager);
}