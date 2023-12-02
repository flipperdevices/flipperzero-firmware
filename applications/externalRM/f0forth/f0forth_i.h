#pragma once

#include <furi.h>
#include <gui/modules/text_box.h>
#include <gui/view_dispatcher.h>

typedef struct {
    ViewDispatcher* view_dispatcher;
    TextBox* text_box;
    FuriString* text_box_store;
    FuriMutex* cli_running_mutex;
    ViewPortInputCallback orig_input_cb;
    bool exit_for_real;
} F0ForthState;

typedef enum {
    F0ForthView_TextBox,
} F0ForthView;
