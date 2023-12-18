#include "nfc_playlist.h"
#include "scences/main_menu.h"

typedef enum {
    NfcPlaylistEvent_ShowEmulatingPopup
} NfcPlaylistMainMenuEvent;

typedef enum {
    NfcPlaylistSettings_Timeout,
    NfcPlaylistSettings_Delay,
    NfcPlaylistMenuSelection_Start
} NfcPlaylistMenuSelection;

static void nfc_playlist_menu_callback(void* context, uint32_t index) {
    NfcPlaylist* app = context;
    switch(index) {
        case NfcPlaylistMenuSelection_Start:
            scene_manager_handle_custom_event(app->scene_manager, NfcPlaylistEvent_ShowEmulatingPopup);
            break;
        default:
            break;
   }
}

static void nfc_playlist_settings_change_callback(VariableItem* item) {
    NfcPlaylist* app = variable_item_get_context(item);

    uint8_t current_option = variable_item_list_get_selected_item_index(app->variable_item_list);
    uint8_t option_value_index = variable_item_get_current_value_index(item);

    switch(current_option) {
        case NfcPlaylistSettings_Timeout: ;
            app->emulate_timeout = option_value_index;
            char emulate_timeout_text[10];
            snprintf(emulate_timeout_text, 10, "%ds", (options_emulate_timeout[option_value_index]/1000));
            variable_item_set_current_value_text(item, (char*)emulate_timeout_text);
            break;
        case NfcPlaylistSettings_Delay: ;
            app->emulate_delay = option_value_index;
            char emulate_delay_text[10];
            snprintf(emulate_delay_text, 10, "%ds", (options_emulate_delay[option_value_index]/1000));
            variable_item_set_current_value_text(item, (char*)emulate_delay_text);
            break;
        default:
            break;
   }
}

void nfc_playlist_main_menu_scene_on_enter(void* context) {
    NfcPlaylist* app = context;
    variable_item_list_set_header(app->variable_item_list, "NFC Playlist");
    VariableItem* emulation_timeout_settings = variable_item_list_add(
        app->variable_item_list,
        "Timeout",
        10,
        nfc_playlist_settings_change_callback,
        app);
    variable_item_set_current_value_index(emulation_timeout_settings, app->emulate_timeout);
    char emulation_timeout_settings_text[10];
    snprintf(emulation_timeout_settings_text, 10, "%ds", (options_emulate_timeout[app->emulate_timeout]/1000));
    variable_item_set_current_value_text(emulation_timeout_settings, (char*)emulation_timeout_settings_text);
    VariableItem* emulation_delay_settings = variable_item_list_add(
        app->variable_item_list,
        "Delay",
        6,
        nfc_playlist_settings_change_callback,
        app);
    variable_item_set_current_value_index(emulation_delay_settings, app->emulate_delay);
    char emulation_delay_settings_text[10];
    snprintf(emulation_delay_settings_text, 10, "%ds", (options_emulate_delay[app->emulate_delay]/1000));
    variable_item_set_current_value_text(emulation_delay_settings, (char*)emulation_delay_settings_text);
    variable_item_list_add(app->variable_item_list, "Start", 0, NULL, NULL);
    variable_item_list_set_enter_callback(app->variable_item_list, nfc_playlist_menu_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, NfcPlaylistView_Menu);
}

bool nfc_playlist_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    NfcPlaylist* app = context;
    bool consumed = false;
    switch(event.type) {
        case SceneManagerEventTypeCustom:
            switch(event.event) {
                case NfcPlaylistEvent_ShowEmulatingPopup:
                    scene_manager_next_scene(app->scene_manager, NfcPlaylistScene_EmulatingPopup);
                    consumed = true;
                    break;
                default:
                    break;
            }
            break;
        default:
            consumed = false;
            break;
   }
   return consumed;
}

void nfc_playlist_main_menu_scene_on_exit(void* context) {
   NfcPlaylist* app = context;
   variable_item_list_reset(app->variable_item_list);
}