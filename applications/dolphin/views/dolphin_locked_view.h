#pragma once

#include <gui/gui_i.h>
#include <gui/view.h>
#include <gui/canvas.h>
#include <gui/elements.h>
#include <furi.h>

#define UNLOCK_RST_TIMEOUT 200
#define UNLOCK_CNT 2 // 3 actually

typedef enum {
    DolphinLockedEventUnlock,
    DolphinLockedEventUpdate,
} DolphinLockedEvent;

typedef struct DolphinLockedView DolphinLockedView;

typedef void (*DolphinLockedViewCallback)(DolphinLockedEvent event, void* context);

struct DolphinLockedView {
    View* view;
    DolphinLockedViewCallback callback;
    osTimerId_t timer;

    void* context;
    uint8_t lock_count;
    uint32_t lock_lastpress;
};

typedef struct {
    IconAnimation* animation;
    uint8_t scene_num;
    int8_t door_left_x;
    int8_t door_right_x;
    uint8_t hint_timeout;

} DolphinLockedViewModel;

void dolphin_locked_set_callback(
    DolphinLockedView* locked_view,
    DolphinLockedViewCallback callback,
    void* context);

void dolphin_locked_update_hint_timeout(DolphinLockedView* locked_view);
void dolphin_locked_reset_counter(DolphinLockedView* locked_view);
void dolphin_locked_reset_door_pos(DolphinLockedView* locked_view);
void dolphin_locked_trigger_redraw(DolphinLockedView* locked_view);

View* dolphin_locked_get_view(DolphinLockedView* locked_view);
DolphinLockedView* dolphin_locked_view_alloc();
void dolphin_locked_view_free(DolphinLockedView* locked_view);
void dolphin_main_view_unlocked(DolphinMainView* main_view);
void dolphin_main_view_reset_hint(DolphinMainView* main_view);