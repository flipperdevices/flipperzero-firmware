#include "nfc_playlist.h"
#include "scences/main_menu.h"

typedef enum {
   NfcPlaylistEvent_ShowEmulatingPopup,
   NfcPlaylistEvent_ShowFileSelect,
   NfcPlaylistEvent_ShowFileEdit,
   NfcPlaylistEvent_ShowSettings
} NfcPlaylistMainMenuEvent;

typedef enum {
   NfcPlaylistMenuSelection_Start,
   NfcPlaylistMenuSelection_FileSelect,
   NfcPlaylistMenuSelection_FileEdit,
   NfcPlaylistMenuSelection_Settings
} NfcPlaylistMenuSelection;

void nfc_playlist_main_menu_menu_callback(void* context, uint32_t index) {
   NfcPlaylist* nfc_playlist = context;
   switch(index) {
      case NfcPlaylistMenuSelection_Start:
         scene_manager_handle_custom_event(nfc_playlist->scene_manager, NfcPlaylistEvent_ShowEmulatingPopup);
         break;
      case NfcPlaylistMenuSelection_FileSelect:
         scene_manager_handle_custom_event(nfc_playlist->scene_manager, NfcPlaylistEvent_ShowFileSelect);
         break;
      case NfcPlaylistMenuSelection_FileEdit:
         scene_manager_handle_custom_event(nfc_playlist->scene_manager, NfcPlaylistEvent_ShowFileEdit);
         break;
      case NfcPlaylistMenuSelection_Settings:
         scene_manager_handle_custom_event(nfc_playlist->scene_manager, NfcPlaylistEvent_ShowSettings);
         break;
      default:
         break;
   }
}

void nfc_playlist_main_menu_scene_on_enter(void* context) {
   NfcPlaylist* nfc_playlist = context;
   if (!nfc_playlist->settings.file_selected) {
      nfc_playlist->settings.file_selected = true;
      scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_FileSelect);
      return;
   }

   submenu_set_header(nfc_playlist->submenu, "NFC Playlist");

   submenu_add_lockable_item(
      nfc_playlist->submenu,
      "Start",
      NfcPlaylistMenuSelection_Start,
      nfc_playlist_main_menu_menu_callback,
      nfc_playlist,
      !nfc_playlist->settings.file_selected_check,
      "No\nplaylist\nselected");

   submenu_add_item(
      nfc_playlist->submenu,
      "Select playlist",
      NfcPlaylistMenuSelection_FileSelect,
      nfc_playlist_main_menu_menu_callback,
      nfc_playlist);

   submenu_add_item(
      nfc_playlist->submenu,
      "Edit playlist",
      NfcPlaylistMenuSelection_FileEdit,
      nfc_playlist_main_menu_menu_callback,
      nfc_playlist);

   submenu_add_item(
      nfc_playlist->submenu,
      "Settings",
      NfcPlaylistMenuSelection_Settings,
      nfc_playlist_main_menu_menu_callback,
      nfc_playlist);
   view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Menu);
}

bool nfc_playlist_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    NfcPlaylist* nfc_playlist = context;
    bool consumed = false;
    if (event.type == SceneManagerEventTypeCustom) {
      switch(event.event) {
         case NfcPlaylistEvent_ShowEmulatingPopup:
            scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_EmulatingPopup);
            consumed = true;
            break;
         case NfcPlaylistEvent_ShowFileSelect:
            scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_FileSelect);
            consumed = true;
            break;
         case NfcPlaylistEvent_ShowFileEdit:
            scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_FileEdit);
            consumed = true;
            break;
         case NfcPlaylistEvent_ShowSettings:
            scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_Settings);
            consumed = true;
            break;
         default:
            break;
      }
   }
   return consumed;
}

void nfc_playlist_main_menu_scene_on_exit(void* context) {
   NfcPlaylist* nfc_playlist = context;
   submenu_reset(nfc_playlist->submenu);
}