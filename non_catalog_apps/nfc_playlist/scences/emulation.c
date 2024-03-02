#include "nfc_playlist.h"
#include "scences/emulation.h"

NfcPlaylistEmulationState EmulationState = NfcPlaylistEmulationState_Stopped;

void nfc_playlist_emulation_scene_on_enter(void* context) {
   NfcPlaylist* nfc_playlist = context;
   nfc_playlist_emulation_setup(nfc_playlist);
   nfc_playlist_emulation_start(nfc_playlist);
}

bool nfc_playlist_emulation_scene_on_event(void* context, SceneManagerEvent event) {
   UNUSED(context);
   if (event.event == 0 && EmulationState == NfcPlaylistEmulationState_Emulating) {
      EmulationState = NfcPlaylistEmulationState_Canceled;
      return true;
   }
   return false;
}

void nfc_playlist_emulation_scene_on_exit(void* context) {
   NfcPlaylist* nfc_playlist = context;
   EmulationState = NfcPlaylistEmulationState_Stopped;
   nfc_playlist_emulation_stop(nfc_playlist);
   nfc_playlist_emulation_free(nfc_playlist);
   popup_reset(nfc_playlist->popup);
}

void nfc_playlist_emulation_setup(void* context) {
   NfcPlaylist* nfc_playlist = context;
   nfc_playlist->thread = furi_thread_alloc_ex("NfcPlaylistEmulationWorker", 8192, nfc_playlist_emulation_task, nfc_playlist);
   nfc_playlist->nfc_playlist_worker = nfc_playlist_worker_alloc();
}

void nfc_playlist_emulation_free(NfcPlaylist* nfc_playlist) {
   furi_assert(nfc_playlist);
   furi_thread_free(nfc_playlist->thread);
   nfc_playlist_worker_free(nfc_playlist->nfc_playlist_worker);
   nfc_playlist->thread = NULL;
   nfc_playlist->nfc_playlist_worker = NULL;
}

void nfc_playlist_emulation_start(NfcPlaylist* nfc_playlist) {
   furi_assert(nfc_playlist);
   furi_thread_start(nfc_playlist->thread);
}

void nfc_playlist_emulation_stop(NfcPlaylist* nfc_playlist) {
   furi_assert(nfc_playlist);
   furi_thread_join(nfc_playlist->thread);
}

int32_t nfc_playlist_emulation_task(void* context) {
   NfcPlaylist* nfc_playlist = context;

   Storage* storage = furi_record_open(RECORD_STORAGE);
   Stream* stream = file_stream_alloc(storage);
   FuriString* line = furi_string_alloc();

   popup_reset(nfc_playlist->popup);
   popup_set_context(nfc_playlist->popup, nfc_playlist);
   view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Popup);

   if (file_stream_open(stream, furi_string_get_cstr(nfc_playlist->settings.file_path), FSAM_READ, FSOM_OPEN_EXISTING) && nfc_playlist->settings.file_selected_check) {
      EmulationState = NfcPlaylistEmulationState_Emulating;
      int file_position = 0;
      while(stream_read_line(stream, line) && EmulationState == NfcPlaylistEmulationState_Emulating) {

         char* file_path = (char*)furi_string_get_cstr(line);

         if (strlen(file_path) <= 1) {continue;}

         if (nfc_playlist->settings.emulate_delay > 0 && file_position != 0) {
            popup_set_header(nfc_playlist->popup, "Delaying", 64, 10, AlignCenter, AlignTop);
            start_blink(nfc_playlist, NfcPlaylistLedState_Error);
            int time_counter_delay_ms = (options_emulate_delay[nfc_playlist->settings.emulate_delay]*1000);
            do {
               char display_text[10];
               snprintf(display_text, 10, "%ds", (time_counter_delay_ms/1000));
               popup_set_text(nfc_playlist->popup, display_text, 64, 50, AlignCenter, AlignTop);
               furi_delay_ms(50);
               time_counter_delay_ms -= 50;
            } while(time_counter_delay_ms > 0 && EmulationState == NfcPlaylistEmulationState_Emulating);
         } else if (nfc_playlist->settings.emulate_delay > 0) {
            file_position++;
         }

         if (EmulationState != NfcPlaylistEmulationState_Emulating) {break;}

         char* file_name = strchr(file_path, '/') != NULL ? &strrchr(file_path, '/')[1] : file_path;
         char const* file_ext = &strrchr(file_path, '.')[1];
         int time_counter_ms = (options_emulate_timeout[nfc_playlist->settings.emulate_timeout]*1000);

         if (storage_file_exists(storage, file_path) == false) {
            int popup_header_text_size = strlen(file_name) + 18;
            char popup_header_text[popup_header_text_size];
            snprintf(popup_header_text, popup_header_text_size, "%s\n%s", "ERROR not found:", file_name);
            popup_set_header(nfc_playlist->popup, popup_header_text, 64, 10, AlignCenter, AlignTop);
            start_blink(nfc_playlist, NfcPlaylistLedState_Error);
            do {
               char popup_text[10];
               snprintf(popup_text, 10, "%ds", (time_counter_ms/1000));
               popup_set_text(nfc_playlist->popup, popup_text, 64, 50, AlignCenter, AlignTop);
               furi_delay_ms(50);
               time_counter_ms -= 50;
            } while(time_counter_ms > 0 && EmulationState == NfcPlaylistEmulationState_Emulating);
         } else if (strcasestr(file_ext, "nfc") == NULL) {
            int popup_header_text_size = strlen(file_name) + 21;
            char popup_header_text[popup_header_text_size];
            snprintf(popup_header_text, popup_header_text_size, "%s\n%s", "ERROR invalid file:", file_name);
            popup_set_header(nfc_playlist->popup, popup_header_text, 64, 10, AlignCenter, AlignTop);
            start_blink(nfc_playlist, NfcPlaylistLedState_Error);
            do {
               char popup_text[10];
               snprintf(popup_text, 10, "%ds", (time_counter_ms/1000));
               popup_set_text(nfc_playlist->popup, popup_text, 64, 50, AlignCenter, AlignTop);
               furi_delay_ms(50);
               time_counter_ms -= 50;
            } while(time_counter_ms > 0 && EmulationState == NfcPlaylistEmulationState_Emulating);
         } else {
            int popup_header_text_size = strlen(file_name) + 12;
            char popup_header_text[popup_header_text_size];
            snprintf(popup_header_text, popup_header_text_size, "%s\n%s", "Emulating:", file_name);
            popup_set_header(nfc_playlist->popup, popup_header_text, 64, 10, AlignCenter, AlignTop);
            nfc_playlist_worker_set_nfc_data(nfc_playlist->nfc_playlist_worker, file_path);
            nfc_playlist_worker_start(nfc_playlist->nfc_playlist_worker);
            start_blink(nfc_playlist, NfcPlaylistLedState_Normal);
            do {
               char popup_text[10];
               snprintf(popup_text, 10, "%ds", (time_counter_ms/1000));
               popup_set_text(nfc_playlist->popup, popup_text, 64, 50, AlignCenter, AlignTop);
               furi_delay_ms(50);
               time_counter_ms -= 50;
            } while(nfc_playlist_worker_is_emulating(nfc_playlist->nfc_playlist_worker) && time_counter_ms > 0 && EmulationState == NfcPlaylistEmulationState_Emulating);
            nfc_playlist_worker_stop(nfc_playlist->nfc_playlist_worker);
            nfc_playlist_worker_clear_nfc_data(nfc_playlist->nfc_playlist_worker);
         }
         free(file_path);
      }
      popup_reset(nfc_playlist->popup);
      popup_set_header(nfc_playlist->popup, EmulationState == NfcPlaylistEmulationState_Canceled ? "Emulation stopped" : "Emulation finished", 64, 10, AlignCenter, AlignTop);
      popup_set_text(nfc_playlist->popup, "Press back", 64, 50, AlignCenter, AlignTop);
      stop_blink(nfc_playlist);
      EmulationState = NfcPlaylistEmulationState_Stopped;
   } else {
      popup_set_header(nfc_playlist->popup, "Failed to open playlist", 64, 10, AlignCenter, AlignTop);
      popup_set_text(nfc_playlist->popup, "Press back", 64, 50, AlignCenter, AlignTop);
   }

   furi_string_free(line);
   file_stream_close(stream);
   furi_record_close(RECORD_STORAGE);
   stream_free(stream);

   return 0;
}