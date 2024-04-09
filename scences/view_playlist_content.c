#include "nfc_playlist.h"
#include "scences/view_playlist_content.h"

void nfc_playlist_view_playlist_content_scene_on_enter(void* context) {
   NfcPlaylist* nfc_playlist = context;

   Storage* storage = furi_record_open(RECORD_STORAGE);
   File* file = storage_file_alloc(storage);
   uint8_t buffer[8000];
   FuriString* playlist_content = furi_string_alloc();

   storage_file_open(file, furi_string_get_cstr(nfc_playlist->settings.file_path), FSAM_READ, FSOM_OPEN_EXISTING);
   uint16_t read_count = storage_file_read(file, buffer, 8000);

   for(uint16_t i = 0; i < read_count; i++) {
      furi_string_push_back(playlist_content, buffer[i]);
   }

   widget_add_text_scroll_element(nfc_playlist->widget, 4, 4, 124, 60, furi_string_get_cstr(playlist_content));
   widget_add_frame_element(nfc_playlist->widget, 0, 0, 128, 64, 0);

   furi_string_free(playlist_content);
   storage_file_close(file);
   storage_file_free(file);
   furi_record_close(RECORD_STORAGE);

   view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_ViewPlaylistContent);
}

bool nfc_playlist_view_playlist_content_scene_on_event(void* context, SceneManagerEvent event) {
   UNUSED(context);
   UNUSED(event);
   return false;
}

void nfc_playlist_view_playlist_content_scene_on_exit(void* context) {
   NfcPlaylist* nfc_playlist = context;
   widget_reset(nfc_playlist->widget);
}