#pragma once

#include "view.h"
#include "view_dispatcher.h"
#include <flipper_v2.h>

typedef struct {
    void *data;
    osMutexId_t mutex;
} ViewModelLocking;

struct View {
    ViewDispatcher *dispatcher;
    ViewDrawCallback draw_callback;
    ViewInputCallback input_callback;
    ViewModelType model_type;
    void* model;
    void* context;
};

/* Set View dispatcher */
void view_set_dispatcher(View* view, ViewDispatcher* view_dispatcher);

/* Allocate */
void view_set_lock_free_model(View* view, void* data);

/* Set view model data.
 * @param view, pointer to View
 * @param model_size, modelsize
 * @warning after this operation you do not own data.
 */
void view_set_locking_model(View* view, void* data);

/* Draw Callback for View dispatcher */
void view_draw(View* view, Canvas* canvas);

/* Input Callback for View dispatcher */
bool view_input(View* view, InputEvent* event);
