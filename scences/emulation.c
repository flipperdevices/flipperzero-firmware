#include "nfc_playlist.h"
#include "scences/emulation.h"

void nfc_playlist_emulation_scene_on_enter(void* context) {
    NfcPlaylist* nfc_playlist = context;

    // open/alloc resources
    Storage* storage = furi_record_open(RECORD_STORAGE);
    Stream* stream = file_stream_alloc(storage);
    FuriString* line = furi_string_alloc();
    NfcPlaylistWorker* nfc_worker = nfc_playlist_worker_alloc();
    // Read file
    if(file_stream_open(stream, APP_DATA_PATH("playlist.txt"), FSAM_READ, FSOM_OPEN_EXISTING)) {
        popup_reset(nfc_playlist->popup);
        popup_set_context(nfc_playlist->popup, nfc_playlist);
        popup_set_header(nfc_playlist->popup, "Emulating:", 64, 10, AlignCenter, AlignTop);
        view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Popup);

        int file_position = 0;
        // read the file line by line and print the text
        while(stream_read_line(stream, line)) {
            if (options_emulate_delay[nfc_playlist->emulate_delay] > 0) {
                if (file_position > 0) {
                    start_error_blink(nfc_playlist);
                    int time_counter_delay_ms = (options_emulate_delay[nfc_playlist->emulate_delay] * 1000);
                    do {
                        char display_text[30];
                        snprintf(display_text, 30, "%s\n\n%ds", "Delaying...", (time_counter_delay_ms/1000));
                        popup_set_text(nfc_playlist->popup, display_text, 64, 25, AlignCenter, AlignTop);
                        furi_delay_ms(500);
                        time_counter_delay_ms -= 500;
                    } while(time_counter_delay_ms > 0);
                } else {
                    file_position++;
                }
            }

            char* file_path = (char*)furi_string_get_cstr(line);
            char* file_name = &strrchr(file_path, '/')[1];
            int time_counter_ms = (options_emulate_timeout[nfc_playlist->emulate_timeout] * 1000);

            if (storage_file_exists(storage, file_path) == false) {
                start_error_blink(nfc_playlist);
                char const* popup_text_unformatted = strcat(file_name, "\nnot found");
                int popup_text_size = (strlen(popup_text_unformatted) + 4);
                char popup_text[popup_text_size];

                do {
                    snprintf(popup_text, popup_text_size, "%s\n%ds", file_name, (time_counter_ms/1000));
                    popup_set_text(nfc_playlist->popup, popup_text, 64, 25, AlignCenter, AlignTop);
                    furi_delay_ms(500);
                    time_counter_ms -= 500;
                } while(time_counter_ms > 0);
            } else {
                start_normal_blink(nfc_playlist);
                nfc_playlist_worker_set_nfc_data(nfc_worker, file_path);
                nfc_playlist_worker_start(nfc_worker);

                int popup_text_size = (strlen(file_name) + 4);
                char popup_text[popup_text_size];

                do {
                    snprintf(popup_text, popup_text_size, "%s\n%ds", file_name, (time_counter_ms/1000));
                    popup_set_text(nfc_playlist->popup, popup_text, 64, 25, AlignCenter, AlignTop);
                    furi_delay_ms(500);
                    time_counter_ms -= 500;
                } while(nfc_playlist_worker_is_emulating(nfc_worker) && time_counter_ms > 0);
                nfc_playlist_worker_stop(nfc_worker);
            }
        }
        popup_reset(nfc_playlist->popup);
        scene_manager_previous_scene(nfc_playlist->scene_manager);
        stop_blink(nfc_playlist);
    } else {
        popup_reset(nfc_playlist->popup);
        popup_set_context(nfc_playlist->popup, nfc_playlist);
        popup_set_header(nfc_playlist->popup, "Error:", 64, 10, AlignCenter, AlignTop);
        popup_set_text(nfc_playlist->popup, "Failed to open file\n/ext/apps_data/nfc_playlist/playlist.txt", 64, 25, AlignCenter, AlignTop);
        view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Popup);
    }
    // Free/close resources
    furi_string_free(line);
    file_stream_close(stream);
    stream_free(stream);
    nfc_playlist_worker_free(nfc_worker);
    // Close storage
    furi_record_close(RECORD_STORAGE);
}

bool nfc_playlist_emulation_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void nfc_playlist_emulation_scene_on_exit(void* context) {
   NfcPlaylist* nfc_playlist = context;
   popup_reset(nfc_playlist->popup);
}