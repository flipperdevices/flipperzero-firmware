#pragma once

#include <gui/gui_i.h>
#include <gui/view.h>
#include <gui/canvas.h>
#include <gui/elements.h>
#include <furi.h>
#include <storage/storage.h>

typedef enum {
    DolphinDebugEventWrongDeed,
    DolphinDebugEventDeed,
    DolphinDebugEventSaveState,
    DolphinDebugEventExit,
} DolphinDebugEvent;

typedef struct DolphinDebugView DolphinDebugView;

typedef void (*DolphinDebugViewCallback)(DolphinDebugEvent event, void* context);

// Debug info
typedef enum {
    DolphinViewStatsFw,
    DolphinViewStatsBoot,
    DolphinViewStatsMeta,
    DolphinViewStatsTotalCount,
} DolphinViewStatsScreens;

struct DolphinDebugView {
    View* view;
    DolphinDebugViewCallback callback;
    void* context;
};

typedef struct {
    uint32_t icounter;
    uint32_t butthurt;
    DolphinViewStatsScreens screen;
} DolphinDebugViewModel;

void dolphin_debug_set_callback(
    DolphinDebugView* debug_view,
    DolphinDebugViewCallback callback,
    void* context);

View* dolphin_debug_get_view(DolphinDebugView* debug_view);

DolphinDebugView* dolphin_debug_view_alloc();
void dolphin_debug_view_free(DolphinDebugView* debug_view);
void dolphin_debug_get_dolphin_data(DolphinDebugView* debug_view, DolphinState* state);
void dolphin_debug_reset_screen_idx(DolphinDebugView* debug_view);