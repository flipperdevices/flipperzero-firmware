#pragma once

#include <gui/gui_i.h>
#include <gui/view.h>
#include <gui/canvas.h>
#include <gui/elements.h>
#include <furi.h>

typedef enum {
    DolphinFirstStartCompleted,
} DolphinFirstStartEvent;

typedef struct DolphinFirstStartView DolphinFirstStartView;

typedef void (*DolphinFirstStartViewCallback)(DolphinFirstStartEvent event, void* context);

struct DolphinFirstStartView {
    View* view;
    DolphinFirstStartViewCallback callback;
    void* context;
};

typedef struct {
    uint8_t page;
} DolphinFirstStartViewModel;

void dolphin_first_start_set_callback(
    DolphinFirstStartView* main_view,
    DolphinFirstStartViewCallback callback,
    void* context);

View* dolphin_first_start_get_view(DolphinFirstStartView* main_view);

DolphinFirstStartView* dolphin_first_start_view_alloc();
void dolphin_first_start_view_free(DolphinFirstStartView* main_view);
