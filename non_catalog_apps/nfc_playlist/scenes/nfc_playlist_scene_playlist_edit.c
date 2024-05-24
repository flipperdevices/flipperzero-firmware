#include "../nfc_playlist.h"

typedef enum {
    NfcPlaylistMenuSelection_CreatePlaylist,
    NfcPlaylistMenuSelection_DeletePlaylist,
    NfcPlaylistMenuSelection_RenamePlaylist,
    NfcPlaylistMenuSelection_AddNfcItem,
    NfcPlaylistMenuSelection_RemoveNfcItem,
    NfcPlaylistMenuSelection_ViewPlaylistContent
} NfcPlaylistFileEditMenuSelection;

void nfc_playlist_playlist_edit_menu_callback(void* context, uint32_t index) {
    NfcPlaylist* nfc_playlist = context;
    scene_manager_handle_custom_event(nfc_playlist->scene_manager, index);
}

void nfc_playlist_playlist_edit_scene_on_enter(void* context) {
    NfcPlaylist* nfc_playlist = context;

    submenu_set_header(nfc_playlist->submenu, "Edit Playlist");

    submenu_add_item(
        nfc_playlist->submenu,
        "Create Playlist",
        NfcPlaylistMenuSelection_CreatePlaylist,
        nfc_playlist_playlist_edit_menu_callback,
        nfc_playlist);

    submenu_add_lockable_item(
        nfc_playlist->submenu,
        "Delete Playlist",
        NfcPlaylistMenuSelection_DeletePlaylist,
        nfc_playlist_playlist_edit_menu_callback,
        nfc_playlist,
        furi_string_empty(nfc_playlist->settings.playlist_path),
        "No\nplaylist\nselected");

    submenu_add_lockable_item(
        nfc_playlist->submenu,
        "Rename Playlist",
        NfcPlaylistMenuSelection_RenamePlaylist,
        nfc_playlist_playlist_edit_menu_callback,
        nfc_playlist,
        furi_string_empty(nfc_playlist->settings.playlist_path),
        "No\nplaylist\nselected");

    submenu_add_lockable_item(
        nfc_playlist->submenu,
        "Add NFC Item",
        NfcPlaylistMenuSelection_AddNfcItem,
        nfc_playlist_playlist_edit_menu_callback,
        nfc_playlist,
        furi_string_empty(nfc_playlist->settings.playlist_path),
        "No\nplaylist\nselected");

    submenu_add_lockable_item(
        nfc_playlist->submenu,
        "Remove NFC Item",
        NfcPlaylistMenuSelection_RemoveNfcItem,
        nfc_playlist_playlist_edit_menu_callback,
        nfc_playlist,
        furi_string_empty(nfc_playlist->settings.playlist_path),
        "No\nplaylist\nselected");

    submenu_add_lockable_item(
        nfc_playlist->submenu,
        "View Playlist Content",
        NfcPlaylistMenuSelection_ViewPlaylistContent,
        nfc_playlist_playlist_edit_menu_callback,
        nfc_playlist,
        furi_string_empty(nfc_playlist->settings.playlist_path),
        "No\nplaylist\nselected");

    view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Submenu);
}

bool nfc_playlist_playlist_edit_scene_on_event(void* context, SceneManagerEvent event) {
    NfcPlaylist* nfc_playlist = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case NfcPlaylistMenuSelection_CreatePlaylist:
            scene_manager_next_scene(
                nfc_playlist->scene_manager, NfcPlaylistScene_NameNewPlaylist);
            consumed = true;
            break;
        case NfcPlaylistMenuSelection_DeletePlaylist:
            scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_ConfirmDelete);
            consumed = true;
            break;
        case NfcPlaylistMenuSelection_RenamePlaylist:
            scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_PlaylistRename);
            consumed = true;
            break;
        case NfcPlaylistMenuSelection_AddNfcItem:
            scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_NfcSelect);
            consumed = true;
            break;
        case NfcPlaylistMenuSelection_RemoveNfcItem:
            scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_NfcRemove);
            consumed = true;
            break;
        case NfcPlaylistMenuSelection_ViewPlaylistContent:
            scene_manager_next_scene(
                nfc_playlist->scene_manager, NfcPlaylistScene_ViewPlaylistContent);
            consumed = true;
            break;
        default:
            break;
        }
    }
    return consumed;
}

void nfc_playlist_playlist_edit_scene_on_exit(void* context) {
    NfcPlaylist* nfc_playlist = context;
    submenu_reset(nfc_playlist->submenu);
}