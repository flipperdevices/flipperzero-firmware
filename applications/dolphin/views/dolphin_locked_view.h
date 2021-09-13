#pragma once

#include <gui/gui_i.h>
#include <gui/view.h>
#include <gui/canvas.h>
#include <gui/elements.h>
#include <furi.h>

typedef enum {
    DolphinLockedEventUnlock,
} DolphinLockedEvent;

typedef struct DolphinLockedView DolphinLockedView;

typedef void (*DolphinLockedViewCallback)(DolphinLockedEvent event, void* context);

struct DolphinLockedView {
    View* view;
    DolphinLockedViewCallback callback;
    void* context;
    uint8_t lock_count;
    uint32_t lock_lastpress;
};

typedef struct {
    IconAnimation* animation;
    uint8_t scene_num;
    uint8_t hint_timeout;

} DolphinLockedViewModel;

void dolphin_locked_set_callback(
    DolphinLockedView* locked_view,
    DolphinLockedViewCallback callback,
    void* context);

View* dolphin_locked_get_view(DolphinLockedView* locked_view);

void dolphin_locked_reset_counter(DolphinLockedView* locked_view);
DolphinLockedView* dolphin_locked_view_alloc();
void dolphin_locked_view_free(DolphinLockedView* locked_view);
