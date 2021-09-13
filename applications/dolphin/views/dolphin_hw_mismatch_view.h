#pragma once

#include <gui/gui_i.h>
#include <gui/view.h>
#include <gui/canvas.h>
#include <gui/elements.h>
#include <furi.h>

typedef enum {
    DolphinHwMismatchEventExit,
} DolphinHwMismatchEvent;

typedef struct DolphinHwMismatchView DolphinHwMismatchView;

typedef void (*DolphinHwMismatchViewCallback)(DolphinHwMismatchEvent event, void* context);

struct DolphinHwMismatchView {
    View* view;
    DolphinHwMismatchViewCallback callback;
    void* context;
};

typedef struct {
    IconAnimation* animation;
    uint8_t scene_num;
    uint8_t hint_timeout;
    bool locked;
} DolphinHwMismatchViewModel;

void dolphin_hw_mismatch_set_callback(
    DolphinHwMismatchView* hw_mismatch_view,
    DolphinHwMismatchViewCallback callback,
    void* context);

View* dolphin_hw_mismatch_get_view(DolphinHwMismatchView* hw_mismatch_view);

DolphinHwMismatchView* dolphin_hw_mismatch_view_alloc();
void dolphin_hw_mismatch_view_free(DolphinHwMismatchView* hw_mismatch_view);
