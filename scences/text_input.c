#include "nfc_playlist.h"
#include "scences/text_input.h"

void nfc_playlist_text_input_menu_callback(void* context) {
   NfcPlaylist* nfc_playlist = context;
   scene_manager_previous_scene(nfc_playlist->scene_manager);
}

void nfc_playlist_text_input_scene_on_enter(void* context) {
   NfcPlaylist* nfc_playlist = context;

   text_input_set_header_text(nfc_playlist->text_input, "Enter new file name");
   text_input_set_minimum_length(nfc_playlist->text_input, 1);
   text_input_set_result_callback(nfc_playlist->text_input, nfc_playlist_text_input_menu_callback, nfc_playlist, nfc_playlist->text_input_data, 50, true);
   view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_TextInput);
}

bool nfc_playlist_text_input_scene_on_event(void* context, SceneManagerEvent event) {
   UNUSED(event);
   UNUSED(context);
   return false;
}

void nfc_playlist_text_input_scene_on_exit(void* context) {
   NfcPlaylist* nfc_playlist = context;
   text_input_reset(nfc_playlist->text_input);
}