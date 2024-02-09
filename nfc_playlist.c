#include "nfc_playlist.h"
#include "nfc_playlist_i.h"

static void (*const nfc_playlist_scene_on_enter_handlers[])(void*) = {
   nfc_playlist_main_menu_scene_on_enter,
   nfc_playlist_settings_scene_on_enter,
   nfc_playlist_emulation_scene_on_enter,
   nfc_playlist_file_select_scene_on_enter,
   nfc_playlist_file_edit_scene_on_enter,
   nfc_playlist_text_input_scene_on_enter
};

static bool (*const nfc_playlist_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
   nfc_playlist_main_menu_scene_on_event,
   nfc_playlist_settings_scene_on_event,
   nfc_playlist_emulation_scene_on_event,
   nfc_playlist_file_select_scene_on_event,
   nfc_playlist_file_edit_scene_on_event,
   nfc_playlist_text_input_scene_on_event
};

static void (*const nfc_playlist_scene_on_exit_handlers[])(void*) = {
   nfc_playlist_main_menu_scene_on_exit,
   nfc_playlist_settings_scene_on_exit,
   nfc_playlist_emulation_scene_on_exit,
   nfc_playlist_file_select_scene_on_exit,
   nfc_playlist_file_edit_scene_on_exit,
   nfc_playlist_text_input_scene_on_exit
};

static const SceneManagerHandlers nfc_playlist_scene_manager_handlers = {
   .on_enter_handlers = nfc_playlist_scene_on_enter_handlers,
   .on_event_handlers = nfc_playlist_scene_on_event_handlers,
   .on_exit_handlers = nfc_playlist_scene_on_exit_handlers,
   .scene_num = NfcPlaylistScene_count
};

static bool nfc_playlist_custom_callback(void* context, uint32_t custom_event) {
   furi_assert(context);
   NfcPlaylist* nfc_playlist = context;
   return scene_manager_handle_custom_event(nfc_playlist->scene_manager, custom_event);
}

static bool nfc_playlist_back_event_callback(void* context) {
   furi_assert(context);
   NfcPlaylist* nfc_playlist = context;
   return scene_manager_handle_back_event(nfc_playlist->scene_manager);
}

static NfcPlaylist* nfc_playlist_alloc() {
   NfcPlaylist* nfc_playlist = malloc(sizeof(NfcPlaylist));
   furi_assert(nfc_playlist);
   nfc_playlist->scene_manager = scene_manager_alloc(&nfc_playlist_scene_manager_handlers, nfc_playlist);
   nfc_playlist->view_dispatcher = view_dispatcher_alloc();
   view_dispatcher_enable_queue(nfc_playlist->view_dispatcher);
   nfc_playlist->variable_item_list = variable_item_list_alloc();
   nfc_playlist->submenu = submenu_alloc();

   nfc_playlist->settings.base_file_path = furi_string_alloc_set_str("/ext/apps_data/nfc_playlist/");
   nfc_playlist->settings.file_path = nfc_playlist->settings.base_file_path;
   nfc_playlist->settings.file_selected = false;
   nfc_playlist->settings.file_selected_check = false;
   nfc_playlist->settings.emulate_timeout = default_emulate_timeout;
   nfc_playlist->settings.emulate_delay = default_emulate_delay;
   nfc_playlist->settings.emulate_led_indicator = default_emulate_led_indicator;

   nfc_playlist->notification = furi_record_open(RECORD_NOTIFICATION);
   nfc_playlist->file_browser = file_browser_alloc(nfc_playlist->settings.file_path);
   nfc_playlist->text_input = text_input_alloc();
   nfc_playlist->popup = popup_alloc();

   view_dispatcher_set_event_callback_context(nfc_playlist->view_dispatcher, nfc_playlist);
   view_dispatcher_set_custom_event_callback(nfc_playlist->view_dispatcher, nfc_playlist_custom_callback);
   view_dispatcher_set_navigation_event_callback(nfc_playlist->view_dispatcher, nfc_playlist_back_event_callback);
   view_dispatcher_add_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Menu, submenu_get_view(nfc_playlist->submenu));
   view_dispatcher_add_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Settings, variable_item_list_get_view(nfc_playlist->variable_item_list));
   view_dispatcher_add_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Popup, popup_get_view(nfc_playlist->popup));
   view_dispatcher_add_view(nfc_playlist->view_dispatcher, NfcPlaylistView_FileSelect, file_browser_get_view(nfc_playlist->file_browser));
   view_dispatcher_add_view(nfc_playlist->view_dispatcher, NfcPlaylistView_FileEdit, submenu_get_view(nfc_playlist->submenu));
   view_dispatcher_add_view(nfc_playlist->view_dispatcher, NfcPlaylistView_TextInput, text_input_get_view(nfc_playlist->text_input));
   return nfc_playlist;
}

static void nfc_playlist_free(NfcPlaylist* nfc_playlist) {
   furi_assert(nfc_playlist);
   scene_manager_free(nfc_playlist->scene_manager);
   view_dispatcher_remove_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Menu);
   view_dispatcher_remove_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Settings);
   view_dispatcher_remove_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Popup);
   view_dispatcher_remove_view(nfc_playlist->view_dispatcher, NfcPlaylistView_FileSelect);
   view_dispatcher_remove_view(nfc_playlist->view_dispatcher, NfcPlaylistView_FileEdit);
   view_dispatcher_remove_view(nfc_playlist->view_dispatcher, NfcPlaylistView_TextInput);
   view_dispatcher_free(nfc_playlist->view_dispatcher);
   variable_item_list_free(nfc_playlist->variable_item_list);
   submenu_free(nfc_playlist->submenu);
   file_browser_free(nfc_playlist->file_browser);
   text_input_free(nfc_playlist->text_input);
   popup_free(nfc_playlist->popup);
   furi_record_close(RECORD_NOTIFICATION);
   furi_string_free(nfc_playlist->settings.base_file_path);
   furi_string_free(nfc_playlist->settings.file_path);
   free(nfc_playlist->playlist_name);
   free(nfc_playlist);
}

int32_t nfc_playlist_main(void* p) {
   UNUSED(p);

   NfcPlaylist* nfc_playlist = nfc_playlist_alloc();

   Gui* gui = furi_record_open(RECORD_GUI);
   view_dispatcher_attach_to_gui(nfc_playlist->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
   scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_MainMenu);
   view_dispatcher_run(nfc_playlist->view_dispatcher);

   furi_record_close(RECORD_GUI);
   nfc_playlist_free(nfc_playlist);

   return 0;
}