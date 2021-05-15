#pragma once

#include <stdint.h>
#include <furi.h>
#include <gui/gui_i.h>
#include <gui/view_dispatcher.h>
#include <m-string.h>
#include <filesystem-api.h>
#include "archive_views.h"

#define MAX_DEPTH_LEVEL 32

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
    ArchiveTabBrowser,
    ArchiveTabTotal,
} ArchiveTabsEnum;

typedef struct {
    ArchiveTabsEnum id;
    //const char* name;
    //const char* path;
    const char* extension;

    //char* prev_path[MAX_DEPTH_LEVEL];

    string_t name;
    string_t ext_filter;
    string_t path[MAX_DEPTH_LEVEL];

    uint8_t level;

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
