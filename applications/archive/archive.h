#pragma once

#include <stdint.h>
#include <furi.h>
#include <gui/gui_i.h>
#include <gui/view_dispatcher.h>
#include <m-string.h>
#include <filesystem-api.h>
#include "archive_views.h"

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
    View* view_archive_main;

    FS_Api* fs_api;
    ArchiveTab tab;

} ArchiveState;
