#pragma once

#include "nfc.h"
#include "nfc_types.h"
#include "nfc_views.h"
#include "nfc_worker.h"

#include <flipper_v2.h>

#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <assets_icons.h>

#include <menu/menu.h>
#include <menu/menu_item.h>

struct Nfc {
    osMessageQueueId_t message_queue;

    NfcWorker* worker;

    ValueMutex* menu_vm;
    MenuItem* menu;
    Icon* icon;

    View* view_read;
    ViewDispatcher* view_dispatcher;
};

Nfc* nfc_alloc();

void nfc_menu_read_callback(void* context);

void nfc_menu_field_on_callback(void* context);

void nfc_menu_field_off_callback(void* context);

void nfc_read(Nfc* nfc);

void nfc_task(void* p);