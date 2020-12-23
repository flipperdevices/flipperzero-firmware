#pragma once

#include <input/input.h>
#include "canvas.h"

#define VIEW_NONE 0xFFFFFFFF

/* View Draw callback
 * @param canvas, pointer to canvas
 * @param view_model, pointer to context
 * @warning called from GUI thread
 */
typedef void (*ViewDrawCallback)(Canvas* canvas, void* view_model);

/* View Input callback
 * @param event, pointer to input event data
 * @param context, pointer to context
 * @return - bool, true if event handled, false if event ignored
 * @warning called from GUI thread
 */
typedef bool (*ViewInputCallback)(InputEvent* event, void* context);

/* View model types */
typedef enum {
    /* Model is not used.
     * Lock free.
     */
    ViewModelTypeNone,
    /* Model consist of atomic types and/or partial update is not critical for rendering.
     * Lock free.
     */
    ViewModelTypeLockFree,
    /* Model access is guarded with mutex.
     * Locking gui thread.
     */
    ViewModelTypeLocking,
} ViewModelType;

typedef struct View View;

/* Allocate and init View
 * @return pointer to View
 */
View* view_alloc();

/* Free View
 * @param pointer to View
 */
void view_free(View* view);

/* Set View Draw callback
 * @param view, pointer to View
 * @param callback, draw callback
 */
void view_set_draw_callback(View* view, ViewDrawCallback callback);

/* Set View Draw callback
 * @param view, pointer to View
 * @param callback, input callback
 */
void view_set_input_callback(View* view, ViewInputCallback callback);

/* Set View Draw callback
 * @param view, pointer to View
 * @param context, context for callbacks
 */
void view_set_context(View* view, void* context);

/* Set view model data.
 * @param view, pointer to View
 * @param model_type, View Model Type
 * @param model_size, modelsize
 * @warning after this operation you do not own data.
 */
void view_set_model(View* view, ViewModelType model_type, void* data);

/* Get view model data
 * @param view, pointer to View
 * @return pointer to model data
 * @warning Don't forget to commit model changes
 */
void* view_get_model(View* view);

/* Commit view model
 * @param view, pointer to View
 */
void view_commit_model(View* view);

/* 
 * With clause for view model
 * @param view, View instance pointer
 * @param function_body a (){} lambda declaration,
 * executed within you parent function context.
 */
#define with_view_model(view, function_body)        \
    {                                               \
        void* p = view_acquire_model(view);         \
        ({ void __fn__ function_body __fn__; })(p); \
        view_commit_model(view);                    \
    }
