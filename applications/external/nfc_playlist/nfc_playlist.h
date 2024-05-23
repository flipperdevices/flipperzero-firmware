#pragma once
#include <furi.h>
#include <furi_hal.h>

#include <string.h>

#include <nfc_playlist_icons.h>

#include <gui/gui.h>
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

#include <storage/storage.h>

#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>

#include "lib/worker/nfc_playlist_worker.h"

#include "scenes/nfc_playlist_scene.h"

typedef enum {
    NfcPlaylistView_Submenu,
    NfcPlaylistView_Popup,
    NfcPlaylistView_Widget,
    NfcPlaylistView_VariableItemList,
    NfcPlaylistView_FileBrowser,
    NfcPlaylistView_TextInput
} NfcPlaylistViews;

typedef struct {
    FuriString* playlist_path;
    uint8_t playlist_length;
    bool playlist_selected;
    uint8_t emulate_timeout;
    uint8_t emulate_delay;
    bool emulate_led_indicator;
    bool skip_error;
} NfcPlaylistSettings;

typedef struct {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Popup* popup;
    Widget* widget;
    VariableItemList* variable_item_list;
    FileBrowser* file_browser;
    FuriString* file_browser_output;
    TextInput* text_input;
    char* text_input_output;
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
static const bool default_skip_error = false;

#define PLAYLIST_LOCATION "/ext/apps_data/nfc_playlist/"
#define PLAYLIST_DIR "/ext/apps_data/nfc_playlist"
#define PLAYLIST_NAME_LEN 100

typedef enum NfcPlaylistLedState {
    NfcPlaylistLedState_Normal,
    NfcPlaylistLedState_Error
} NfcPlaylistLedState;

void start_blink(NfcPlaylist* nfc_playlist, int state);
void stop_blink(NfcPlaylist* nfc_playlist);