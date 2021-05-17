#pragma once

#include "archive.h"
#include <stdint.h>
#include <furi.h>
#include <gui/gui_i.h>
#include <gui/view_dispatcher.h>
#include <m-string.h>
#include <m-array.h>
#include <filesystem-api.h>
#include "archive_views.h"
#include "applications.h"

#define MAX_DEPTH 16
#define MAX_NAME_LEN 255

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

static const char* known_ext[] = {
    [ArchiveFileTypeIButton] = ".ibtn",
    [ArchiveFileTypeNFC] = ".nfc",
    [ArchiveFileTypeSubOne] = ".sub1",
    [ArchiveFileTypeLFRFID] = ".rfid",
    [ArchiveFileTypeIrda] = ".irda",
};

static const char* tab_default_paths[] = {
    [ArchiveTabFavorites] = "favorites",
    [ArchiveTabIButton] = "ibutton",
    [ArchiveTabNFC] = "nfc",
    [ArchiveTabSubOne] = "subone",
    [ArchiveTabLFRFID] = "lfrfid",
    [ArchiveTabIrda] = "irda",
    [ArchiveTabBrowser] = "/",
};

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

typedef struct {
    ArchiveTabsEnum id;

    string_t name;
    string_t ext_filter;
    string_t path[MAX_DEPTH];

    uint8_t depth;
    bool menu;

} ArchiveTab;

struct ArchiveApp {
    osMessageQueueId_t event_queue;
    FuriThread* app_thread;
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    View* view_archive_main;

    FS_Api* fs_api;
    ArchiveTab tab;
};
