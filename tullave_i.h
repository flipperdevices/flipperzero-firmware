#pragma once

#include "tullave.h"
#include "lib/tullave/tullave_worker.h"

#include "lib/tullave/protocols/tullave_apdu.h"

#include <furi.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>

#include <notification/notification_messages.h>

#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include <gui/modules/widget.h>

#include "scenes/tullave_scene.h"

// Requires to have fap_libs=["assets"] in the application.fam to read F0 asset icons.
#include <assets_icons.h>

typedef enum { TuLlaveViewMenu, TuLlaveViewWidget, TuLlaveViewPopup } TuLlaveView;

struct TuLlave {
    TuLlaveWorker* worker;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;
    Gui* gui;
    SceneManager* scene_manager;
    Widget* widget;
    Submenu* submenu;
    Popup* popup;
};

static const NotificationSequence seq_tullave_found = {
    &message_display_backlight_on,
    &message_blue_255,
    &message_vibro_on,
    &message_note_c5,
    &message_delay_100,
    &message_note_cs5,
    &message_delay_100,
    &message_note_d5,
    &message_delay_100,
    &message_note_ds5,
    &message_delay_250,
    &message_sound_off,
    NULL,
};

static const NotificationSequence seq_search_tullave_blink = {
    &message_blink_start_10,
    &message_blink_set_color_blue,
    &message_do_not_reset,
    NULL,
};

TuLlave* tullave_alloc();

void tullave_free(TuLlave* t_llave);

int32_t tullave_co_main(void* p);

void tullave_blink_start(TuLlave* t_llave);

void tullave_blink_stop(TuLlave* t_llave);