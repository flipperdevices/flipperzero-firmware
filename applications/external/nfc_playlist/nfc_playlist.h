#pragma once
#include <furi.h>
#include <furi_hal.h>
#include <string.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/popup.h>
#include <gui/modules/variable_item_list.h>
#include <notification/notification_messages.h>
#include <nfc_playlist_worker.h>

typedef enum { NfcPlaylistView_Menu, NfcPlaylistView_Popup } NfcPlayScenesView;

typedef enum {
    NfcPlaylistScene_MainMenu,
    NfcPlaylistScene_EmulatingPopup,
    NfcPlaylistScene_count
} NfcPlaylistScene;

typedef struct {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    VariableItemList* variable_item_list;
    Popup* popup;
    NotificationApp* notification;
    FuriThread* thread;
    NfcPlaylistWorker* nfc_playlist_worker;
    uint8_t emulate_timeout;
    uint8_t emulate_delay;
    bool emulate_led_indicator;
} NfcPlaylist;

static const int options_emulate_timeout[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
static const int default_emulate_timeout = 4;
static const int options_emulate_delay[] = {0, 1, 2, 3, 4, 5, 6};
static const int default_emulate_delay = 0;
static const bool default_emulate_led_indicator = true;