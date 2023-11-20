/**
MIT License

Copyright (c) 2023 Vojtech Suk (https://github.com/sukvojte)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "view_flasher.h"

#include <gui/elements.h>
#include <dialogs/dialogs.h>
#include <gui/view.h>
#include <wch_swio_flasher_icons.h>
#include "../utils.h"

#define TAG "WCH_VieFl"

struct ViewFlasher {
    View* view;
    FuriTimer* timer;
    FuriMutex* mutex;
    uint32_t animation_enabled;
};

typedef struct {
    struct {
        ViewFlasher_View_Action selected_action;
    } menu;
    struct {
        ViewFlasherDoActionCallback cb;
        void* context;
    } do_action;
    char* display_text;
} ViewFlasherModel;
/*
static void draw_menu_item(
    Canvas* canvas,
    ViewFlasherModel* model,
    ViewFlasher_View_Action item_id,
    const char* text) {
    uint8_t box_y = (item_id * 16);
    uint8_t text_y = 4 + (item_id * 16);

    if(model->menu.selected_action == item_id) {
        canvas_set_color(canvas, ColorBlack);
        elements_slightly_rounded_box(canvas, 72, box_y, 55, 16);

        canvas_set_color(canvas, ColorWhite);
        elements_multiline_text_aligned(canvas, 100, text_y, AlignCenter, AlignTop, text);
    } else {
        canvas_set_color(canvas, ColorBlack);
        elements_multiline_text_aligned(canvas, 100, text_y, AlignCenter, AlignTop, text);
    }
}
*/
static void view_flasher_draw_callback(Canvas* canvas, void* _model) {
    ViewFlasherModel* model = _model;
    UNUSED(model);
    elements_button_right(canvas, "Ok");
    elements_progress_bar(canvas, 5, 40, 30, 0.3);

    /*
    //canvas_draw_icon(canvas, 0, 0, &I_flasher_base_128x64);
    canvas_set_color(canvas, ColorBlack);
    //elements_bold_rounded_frame(canvas, 0, 0, 70, 63);

    if(model->display_text != NULL) {
        elements_bold_rounded_frame(canvas, 0, 0, 73, 63);
        elements_text_box(canvas, 4, 4, 110, 55, AlignLeft, AlignTop, model->display_text, false);
    }

    //elements_progress_bar(canvas, 5, 40, 30, 0.3);

    draw_menu_item(canvas, model, ViewFlasher_Action_ChipInfo, "Chip info");
    draw_menu_item(canvas, model, ViewFlasher_Action_OpenFile, "Open file");
    draw_menu_item(canvas, model, ViewFlasher_Action_EraseChip, "Erase chip");
    draw_menu_item(canvas, model, ViewFlasher_Action_WriteChip, "Write chip");

    //canvas_set_font(canvas, FontPrimary);
    canvas_set_font(canvas, FontSecondary);

    */
}

void view_flasher_display_text(ViewFlasher* handle, const char* message) {
    char* str = malloc(strlen(message) + 1);
    strcpy(str, message);
    with_view_model(
        handle->view,
        ViewFlasherModel * model,
        {
            if(model->display_text != NULL) {
                free(model->display_text);
            }
            model->display_text = str;
        },
        true);
}

void view_flasher_display_clear_text(ViewFlasher* handle) {
    with_view_model(
        handle->view,
        ViewFlasherModel * model,
        {
            if(model->display_text != NULL) {
                free(model->display_text);
            }
            model->display_text = NULL;
        },
        true);
}

void view_flasher_register_action_callback(
    ViewFlasher* handle,
    ViewFlasherDoActionCallback cb,
    void* cb_context) {
    with_view_model(
        handle->view,
        ViewFlasherModel * model,
        {
            model->do_action.cb = cb;
            model->do_action.context = cb_context;
        },
        false);
}

static bool view_flasher_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    ViewFlasher* flasher = context;
    bool consumed = false;

    if(event->type == InputTypeShort || event->type == InputTypeRepeat) {
        with_view_model(
            flasher->view,
            ViewFlasherModel * model,
            {
                switch(event->key) {
                case InputKeyUp:
                    if(model->menu.selected_action > 0) {
                        model->menu.selected_action--;
                    }
                    consumed = true;
                    break;
                case InputKeyDown:
                    model->menu.selected_action++;
                    if(model->menu.selected_action >= ViewFlasher_ActionCount) {
                        model->menu.selected_action = ViewFlasher_ActionCount - 1;
                    }
                    consumed = true;
                    break;
                case InputKeyOk:
                    if(model->do_action.cb) {
                        model->do_action.cb(model->do_action.context, model->menu.selected_action);
                    }
                    consumed = true;
                    break;
                default:
                    break;
                }
            },
            true);
    }
    return consumed;
}

static void view_flasher_timer_callback(void* context) {
    ViewFlasher* instance = context;
    with_view_model(
        instance->view, ViewFlasherModel * model, { UNUSED(model); }, true);
}

static void view_flasher_enter(void* context) {
    ViewFlasher* instance = context;
    UNUSED(instance);
    //furi_timer_start(instance->timer, furi_kernel_get_tick_frequency() / 4);
}

static void view_flasher_exit(void* context) {
    ViewFlasher* instance = context;
    //view_flasher_display_clear_text(instance);
    UNUSED(instance);
    //furi_timer_stop(instance->timer);
}

ViewFlasher* view_flasher_alloc() {
    ViewFlasher* handle = malloc(sizeof(ViewFlasher));

    handle->view = view_alloc();
    view_allocate_model(handle->view, ViewModelTypeLocking, sizeof(ViewFlasherModel));

    with_view_model(
        handle->view,
        ViewFlasherModel * model,
        { model->menu.selected_action = ViewFlasher_Action_ChipInfo; },
        false);

    view_set_context(handle->view, handle);
    view_set_draw_callback(handle->view, view_flasher_draw_callback);
    view_set_input_callback(handle->view, view_flasher_input_callback);
    view_set_enter_callback(handle->view, view_flasher_enter);
    view_set_exit_callback(handle->view, view_flasher_exit);

    handle->timer = furi_timer_alloc(view_flasher_timer_callback, FuriTimerTypePeriodic, handle);

    handle->mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    return handle;
}
/*
void view_flasher_set_animation_status(ViewFlasher* handle, uint8_t enabled) {
    furi_mutex_acquire(handle->mutex, FuriWaitForever);
    handle->animation_enabled = enabled;

    with_view_model(
        handle->view,
        ViewFlasherModel * model,
        {
            if(enabled && !model->animation_visible) {
                model->animation_counter = 0;
                model->animation_visible = 1;
            }
        },
        false);
    furi_mutex_release(handle->mutex);
}*/

void view_flasher_free(ViewFlasher* handle) {
    furi_assert(handle);
    view_flasher_display_clear_text(handle);
    furi_timer_free(handle->timer);
    furi_mutex_free(handle->mutex);
    view_free(handle->view);
    free(handle);
}

View* view_flasher_get_view(ViewFlasher* flasher) {
    furi_assert(flasher);
    return flasher->view;
}
