#include "nfc_playlist.h"
#include "scences/text_input.h"

void nfc_playlist_text_input_menu_callback(void* context) {
   NfcPlaylist* nfc_playlist = context;
   Storage* storage = furi_record_open(RECORD_STORAGE);

   char path[28] = "/ext/apps_data/nfc_playlist/";
   
   int size = (strlen(nfc_playlist->playlist_name) + strlen(".txt") + strlen(path) + 1);

   char* new_file_name = (char*)malloc(size);
   snprintf(new_file_name, size, "%s%s%s", path, nfc_playlist->playlist_name, ".txt");
   storage_common_rename(storage, furi_string_get_cstr(nfc_playlist->file_path), new_file_name);
   nfc_playlist->file_path = furi_string_alloc_set_str(new_file_name);

   free(new_file_name);
   free(nfc_playlist->playlist_name);
   furi_record_close(RECORD_STORAGE);
   
   scene_manager_previous_scene(nfc_playlist->scene_manager);
}

void nfc_playlist_text_input_scene_on_enter(void* context) {
   NfcPlaylist* nfc_playlist = context;
   nfc_playlist->playlist_name = (char*)malloc(50);
   text_input_set_header_text(nfc_playlist->text_input, "Enter new file name");
   text_input_set_minimum_length(nfc_playlist->text_input, 1);
   text_input_set_result_callback(nfc_playlist->text_input, nfc_playlist_text_input_menu_callback, nfc_playlist, nfc_playlist->playlist_name, 50, true);
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