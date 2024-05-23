#include "../nfc_playlist.h"

typedef enum {
    NfcPlaylistSettings_LineSelector,
    NfcPlaylistSettings_RemoveLine
} NfcPlaylistSettingsMenuSelection;

uint8_t selected_line;

void nfc_playlist_nfc_remove_menu_callback(void* context, uint32_t index) {
    NfcPlaylist* nfc_playlist = context;
    scene_manager_handle_custom_event(nfc_playlist->scene_manager, index);
}

void nfc_playlist_nfc_remove_options_change_callback(VariableItem* item) {
    NfcPlaylist* nfc_playlist = variable_item_get_context(item);

    uint8_t current_option =
        variable_item_list_get_selected_item_index(nfc_playlist->variable_item_list);
    uint8_t option_value_index = variable_item_get_current_value_index(item);
    FuriString* tmp_str = furi_string_alloc();
    switch(current_option) {
    case NfcPlaylistSettings_LineSelector:
        selected_line = option_value_index + 1;
        furi_string_printf(tmp_str, "%d", selected_line);
        variable_item_set_current_value_text(item, furi_string_get_cstr(tmp_str));
        break;
    default:
        break;
    }
    furi_string_free(tmp_str);
}

void nfc_playlist_nfc_remove_scene_on_enter(void* context) {
    NfcPlaylist* nfc_playlist = context;
    FuriString* tmp_str = furi_string_alloc();

    selected_line = nfc_playlist->settings.playlist_length;

    variable_item_list_set_header(nfc_playlist->variable_item_list, "Settings");

    VariableItem* Line_selector = variable_item_list_add(
        nfc_playlist->variable_item_list,
        "Select Line",
        nfc_playlist->settings.playlist_length,
        nfc_playlist_nfc_remove_options_change_callback,
        nfc_playlist);
    variable_item_set_current_value_index(
        Line_selector, nfc_playlist->settings.playlist_length - 1);
    furi_string_printf(tmp_str, "%d", selected_line);
    variable_item_set_current_value_text(Line_selector, furi_string_get_cstr(tmp_str));
    variable_item_set_locked(
        Line_selector,
        nfc_playlist->settings.playlist_length == 0 ? true : false,
        "Playlist\nis empty");

    variable_item_list_add(nfc_playlist->variable_item_list, "Remove Line", 0, NULL, NULL);
    variable_item_set_locked(
        variable_item_list_get(nfc_playlist->variable_item_list, NfcPlaylistSettings_RemoveLine),
        nfc_playlist->settings.playlist_length == 0 ? true : false,
        "Playlist\nis empty");

    variable_item_list_set_enter_callback(
        nfc_playlist->variable_item_list, nfc_playlist_nfc_remove_menu_callback, nfc_playlist);
    furi_string_free(tmp_str);

    view_dispatcher_switch_to_view(
        nfc_playlist->view_dispatcher, NfcPlaylistView_VariableItemList);
}

bool nfc_playlist_nfc_remove_scene_on_event(void* context, SceneManagerEvent event) {
    NfcPlaylist* nfc_playlist = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case NfcPlaylistSettings_RemoveLine:
            Storage* storage = furi_record_open(RECORD_STORAGE);
            Stream* stream = file_stream_alloc(storage);

            if(file_stream_open(
                   stream,
                   furi_string_get_cstr(nfc_playlist->settings.playlist_path),
                   FSAM_READ_WRITE,
                   FSOM_OPEN_EXISTING)) {
                FuriString* line = furi_string_alloc();
                FuriString* tmp_str = furi_string_alloc();
                uint8_t current_line = 0;
                while(stream_read_line(stream, line)) {
                    current_line++;
                    if(current_line != selected_line) {
                        furi_string_replace_all(line, "\n", "");
                        if(furi_string_empty(tmp_str)) {
                            furi_string_cat_printf(tmp_str, "%s", furi_string_get_cstr(line));
                        } else {
                            furi_string_cat_printf(tmp_str, "\n%s", furi_string_get_cstr(line));
                        }
                    }
                }

                stream_clean(stream);
                furi_string_free(line);
                stream_write_string(stream, tmp_str);
                furi_string_free(tmp_str);
                file_stream_close(stream);
                nfc_playlist->settings.playlist_length--;
                selected_line = nfc_playlist->settings.playlist_length;
            }
            stream_free(stream);
            furi_record_close(RECORD_STORAGE);

            if(selected_line == 0) {
                scene_manager_previous_scene(nfc_playlist->scene_manager);
            } else {
                FuriString* tmp_str = furi_string_alloc();

                VariableItem* Line_selector = variable_item_list_get(
                    nfc_playlist->variable_item_list, NfcPlaylistSettings_LineSelector);
                variable_item_set_values_count(
                    Line_selector, nfc_playlist->settings.playlist_length);
                variable_item_set_current_value_index(Line_selector, selected_line - 1);
                furi_string_printf(tmp_str, "%d", selected_line);
                variable_item_set_current_value_text(Line_selector, furi_string_get_cstr(tmp_str));

                furi_string_free(tmp_str);
            }

            consumed = true;
            break;
        default:
            break;
        }
    }
    return consumed;
}

void nfc_playlist_nfc_remove_scene_on_exit(void* context) {
    NfcPlaylist* nfc_playlist = context;
    variable_item_list_reset(nfc_playlist->variable_item_list);
}