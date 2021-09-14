#pragma once

#include <gui/gui_i.h>
#include <gui/view.h>
#include <gui/canvas.h>
#include <gui/elements.h>
#include <furi.h>

typedef enum {
    DolphinMainEventOpenMenu,
    DolphinMainEventOpenLockMenu,
    DolphinMainEventOpenDebug,
    DolphinMainEventUnlocked,
    DolphinMainEventOpenArchive,
} DolphinMainEvent;

typedef struct DolphinMainView DolphinMainView;

typedef void (*DolphinMainViewCallback)(DolphinMainEvent event, void* context);

struct DolphinMainView {
    View* view;
    DolphinMainViewCallback callback;
    void* context;
};

typedef struct {
    IconAnimation* animation;
    uint8_t scene_num;
    uint8_t hint_timeout;
    bool unlocked;
} DolphinMainViewModel;

void dolphin_main_set_callback(
    DolphinMainView* main_view,
    DolphinMainViewCallback callback,
    void* context);

View* dolphin_main_get_view(DolphinMainView* main_view);

DolphinMainView* dolphin_main_view_alloc();
void dolphin_main_view_free(DolphinMainView* main_view);
