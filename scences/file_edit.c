#include "nfc_playlist.h"
#include "scences/file_edit.h"

typedef enum {
   NfcPlaylistMenuSelection_DeletePlaylist
} NfcPlaylistMenuSelection;

void nfc_playlist_file_edit_menu_callback(void* context, uint32_t index) {
   NfcPlaylist* nfc_playlist = context;
   Storage* storage = furi_record_open(RECORD_STORAGE);
   switch(index) {
      case NfcPlaylistMenuSelection_DeletePlaylist: {
         storage_simply_remove(storage, furi_string_get_cstr(nfc_playlist->file_path));
         nfc_playlist->file_selected = false;
         nfc_playlist->file_selected_check = false;
         nfc_playlist->file_path = nfc_playlist->base_file_path;
         scene_manager_previous_scene(nfc_playlist->scene_manager);
         break;
      }
      default:
         break;
   }
   furi_record_close(RECORD_STORAGE);
}

void nfc_playlist_file_edit_scene_on_enter(void* context) {
   NfcPlaylist* nfc_playlist = context;

   submenu_set_header(nfc_playlist->submenu, "Edit Playlist");

   submenu_add_item(
      nfc_playlist->submenu,
      "Delete Playlist",
      NfcPlaylistMenuSelection_DeletePlaylist,
      nfc_playlist_file_edit_menu_callback,
      nfc_playlist);

   view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_FileEdit);
}

bool nfc_playlist_file_edit_scene_on_event(void* context, SceneManagerEvent event) {
   UNUSED(event);
   UNUSED(context);
   return false;
}

void nfc_playlist_file_edit_scene_on_exit(void* context) {
   NfcPlaylist* nfc_playlist = context;
   submenu_reset(nfc_playlist->submenu);
}