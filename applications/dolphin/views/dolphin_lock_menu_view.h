#pragma once

#include <gui/gui_i.h>
#include <gui/view.h>
#include <gui/canvas.h>
#include <gui/elements.h>
#include <furi.h>

typedef enum {
    DolphinLockMenuEventLock,
    DolphinLockMenuEventUnlock,
    DolphinLockMenuEventExit,
} DolphinLockMenuEvent;

typedef struct DolphinLockMenuView DolphinLockMenuView;

typedef void (*DolphinLockMenuViewCallback)(DolphinLockMenuEvent event, void* context);

struct DolphinLockMenuView {
    View* view;
    DolphinLockMenuViewCallback callback;
    void* context;
};

typedef struct {
    uint8_t idx;
    uint8_t hint_timeout;
    bool locked;
} DolphinLockMenuViewModel;

void dolphin_lock_menu_set_callback(
    DolphinLockMenuView* lock_menu,
    DolphinLockMenuViewCallback callback,
    void* context);

View* dolphin_lock_menu_get_view(DolphinLockMenuView* lock_menu);
void dolphin_lock_menu_reset_idx(DolphinLockMenuView* lock_menu);
DolphinLockMenuView* dolphin_lock_menu_alloc();
void dolphin_lock_menu_free(DolphinLockMenuView* lock_menu);
