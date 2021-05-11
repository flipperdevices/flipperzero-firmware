#pragma once

#include <stdint.h>
#include <furi.h>
#include <gui/gui_i.h>
#include <gui/view_dispatcher.h>
#include <m-string.h>
#include <filesystem-api.h>
#include "archive_views.h"

#define EXIT_FLAG 0x00000001U

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef CLAMP
#define CLAMP(x, upper, lower) (MIN(upper, MAX(x, lower)))
#endif

typedef enum {
    EventTypeTick,
    EventTypeKey,
    EventTypeExit,
} EventType;

typedef struct {
    union {
        InputEvent input;
    } value;
    EventType type;
} AppEvent;

typedef enum {
    ArchiveTabFavorites,
    ArchiveTabIButton,
    ArchiveTabNFC,
    ArchiveTabSubOne,
    ArchiveTabLFRFID,
    ArchiveTabIrda,
    ArchiveTabTotal,
} ArchiveTabsEnum;

typedef struct {
    ArchiveTabsEnum id;
    const char* name;
    const char* path;
    const char* extension;
} ArchiveTab;

typedef struct {
    osMessageQueueId_t event_queue;
    FuriThread* app_thread;
    Gui* gui;
    ViewDispatcher* view_dispatcher;

    View* view_favorite_items;
    View* view_ibutton_keys;
    View* view_nfc_keys;
    View* view_subone_keys;
    View* view_lfrfid_keys;
    View* view_irda_keys;

    FS_Api* fs_api;
    ArchiveTab tab;

} ArchiveState;
