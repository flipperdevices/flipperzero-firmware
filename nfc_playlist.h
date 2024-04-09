#pragma once
#include <furi.h>
#include <furi_hal.h>
#include <string.h>
#include <gui/gui.h>
#include <assets_icons.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/popup.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/submenu.h>
#include <gui/modules/file_browser.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>
#include <gui/modules/widget_elements/widget_element.h>
#include <notification/notification_messages.h>
#include <nfc_playlist_worker.h>

typedef enum {
   NfcPlaylistView_Menu,
   NfcPlaylistView_Settings,
   NfcPlaylistView_Popup,
   NfcPlaylistView_PlaylistSelect,
   NfcPlaylistView_FileEdit,
   NfcPlaylistView_FileRename,
   NfcPlaylistView_ConfirmDelete
} NfcPlayScenesView;

typedef enum {
   NfcPlaylistScene_MainMenu,
   NfcPlaylistScene_Settings,
   NfcPlaylistScene_EmulatingPopup,
   NfcPlaylistScene_PlaylistSelect,
   NfcPlaylistScene_FileEdit,
   NfcPlaylistScene_FileRename,
   NfcPlaylistScene_ConfirmDelete,
   NfcPlaylistScene_count
} NfcPlaylistScene;

typedef struct {
   FuriString* file_path;
   bool playlist_selected;
   uint8_t emulate_timeout;
   uint8_t emulate_delay;
   bool emulate_led_indicator;
} NfcPlaylistSettings;

typedef struct {
   SceneManager* scene_manager;
   ViewDispatcher* view_dispatcher;
   VariableItemList* variable_item_list;
   FileBrowser* playlist_file_browser;
   FuriString* file_browser_output;
   TextInput* text_input;
   char* text_input_output;
   Submenu* submenu;
   Popup* popup;
   Widget* widget;
   NotificationApp* notification;
   FuriThread* thread;
   NfcPlaylistWorker* nfc_playlist_worker;
   NfcPlaylistSettings settings;
} NfcPlaylist;

static const int options_emulate_timeout[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
static const int default_emulate_timeout = 4;
static const int options_emulate_delay[] = {0, 1, 2, 3, 4, 5, 6};
static const int default_emulate_delay = 0;
static const bool default_emulate_led_indicator = true;

#define PLAYLIST_LOCATION "/ext/apps_data/nfc_playlist/"
#define PLAYLIST_DIR "/ext/apps_data/nfc_playlist"