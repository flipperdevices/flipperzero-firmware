#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/gui_i.h>
#include <gui/modules/file_select.h>
#include <gui/view_dispatcher.h>
#include <gui/elements.h>
#include <m-string.h>
#include <sys/param.h>
#include <input/input.h>

#include <sd-card-api.h>
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
    ArchiveTabAll,
    ArchiveTabIButton,
    // ArchiveTabNFC,
    // ArchiveTabSubOne,
    // ArchiveTabLFRFID,
    ArchiveTabTotal,
} ArchiveTabsEnum;

typedef struct {
    osMessageQueueId_t event_queue;
    FuriThread* app_thread;
    Gui* gui;

    ViewDispatcher* view_dispatcher;

    View* view_favorite_items;
    View* view_all_items;
    View* view_ibutton_keys;

    FS_Api* fs_api;

    uint8_t current_tab;
    char string_buff[255];

    const char* path;
    const char* extension;

    bool init_completed;

    FileSelectCallback callback;
    void* context;

    char* buffer;
    uint8_t buffer_size;

} ArchiveState;
