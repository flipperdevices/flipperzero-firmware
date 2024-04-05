#pragma once
#include <furi.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/variable_item_list.h>

void nfc_playlist_settings_scene_on_enter(void* context);
bool nfc_playlist_settings_scene_on_event(void* context, SceneManagerEvent event);
void nfc_playlist_settings_scene_on_exit(void* context);

typedef enum {
   NfcPlaylistSettings_Timeout,
   NfcPlaylistSettings_Delay,
   NfcPlaylistSettings_LedIndicator,
   NfcPlaylistSettings_Reset
} NfcPlaylistSettingsMenuSelection;