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

#include "debugger_emulator.h"

#include <gui/elements.h>
#include <gui/view.h>
#include <wch_swio_flasher_icons.h>
#include "../utils.h"

struct DebuggerEmulator {
    View* view;
    FuriTimer* timer;
    FuriMutex* mutex;
    uint32_t animation_enabled;
};

typedef struct {
    uint16_t animation_counter;
    uint16_t animation_visible;
} DebuggerEmulatorModel;

static void draw_log0(Canvas* canvas, uint8_t x, uint8_t y) {
    canvas_draw_line(canvas, x, y + 1, x, y + 3);
    canvas_draw_line(canvas, x + 3, y + 1, x + 3, y + 3);
    canvas_draw_line(canvas, x + 1, y, x + 2, y);
    canvas_draw_line(canvas, x + 1, y + 4, x + 2, y + 4);
}

static void draw_log1(Canvas* canvas, uint8_t x, uint8_t y) {
    canvas_draw_line(canvas, x, y, x, y + 4);
}

#define MAX_ANIMATION_INCREMENT 4
#define MAX_ANIMATION_VIEWPOINT_X 84
#define MIN_ANIMATION_VIEWPOINT_X 47
#define BLANK_WINDOW_SIZE 26
#define ANIMATION_COUNTER_MAX_VALUE \
    (((MAX_ANIMATION_VIEWPOINT_X - MIN_ANIMATION_VIEWPOINT_X) + BLANK_WINDOW_SIZE) * 2)
#define ANIMATION_Y 22

static void drawBits(Canvas* canvas, uint8_t value, uint8_t x, uint8_t y) {
    for(uint8_t i = 0; i < 6; i++) {
        if(value & (1 << i)) {
            if(x >= MIN_ANIMATION_VIEWPOINT_X && x <= MAX_ANIMATION_VIEWPOINT_X) {
                draw_log1(canvas, x, y);
            }
            x += 2;
        } else {
            if(x >= MIN_ANIMATION_VIEWPOINT_X && (x + 3) <= MAX_ANIMATION_VIEWPOINT_X) {
                draw_log0(canvas, x, y);
            }
            x += 5;
        }
    }
}

static void debugger_emulator_draw_callback(Canvas* canvas, void* _model) {
    DebuggerEmulatorModel* model = _model;
    UNUSED(model);

    canvas_draw_icon(canvas, 0, 0, &I_debugger_base_128x64);
    //47x26 86x26

    uint32_t offset = model->animation_counter % ANIMATION_COUNTER_MAX_VALUE;
    //offset = MAX_OFFSET;

    if(offset >= (ANIMATION_COUNTER_MAX_VALUE / 2)) {
        offset = ANIMATION_COUNTER_MAX_VALUE - offset;
    }

    drawBits(canvas, 0xaa, MAX_ANIMATION_VIEWPOINT_X - offset, ANIMATION_Y);

    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(
        canvas, 126, 62, AlignRight, AlignBottom, "NCH Link042 Emulator");
    canvas_set_font(canvas, FontSecondary);

    elements_multiline_text_aligned(
        canvas, 126, 50, AlignRight, AlignBottom, "Press back to exit");
}

static bool debugger_emulator_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    DebuggerEmulator* debugger = context;
    UNUSED(debugger);
    UNUSED(event);
    bool consumed = false;

    return consumed;
}

static void view_display_test_timer_callback(void* context) {
    DebuggerEmulator* instance = context;
    with_view_model(
        instance->view,
        DebuggerEmulatorModel * model,
        {
            if(model->animation_visible) {
                model->animation_counter += MAX_ANIMATION_INCREMENT;
            }

            if(model->animation_counter >= ANIMATION_COUNTER_MAX_VALUE) {
                model->animation_counter = 0;
                if(!instance->animation_enabled) {
                    model->animation_visible = 0;
                }
            }
        },
        true);
}

static void view_display_test_enter(void* context) {
    DebuggerEmulator* instance = context;

    furi_timer_start(instance->timer, furi_kernel_get_tick_frequency() / 4);
}

static void view_display_test_exit(void* context) {
    DebuggerEmulator* instance = context;
    furi_timer_stop(instance->timer);
}

DebuggerEmulator* debugger_emulator_alloc() {
    DebuggerEmulator* handle = malloc(sizeof(DebuggerEmulator));

    handle->view = view_alloc();
    view_allocate_model(handle->view, ViewModelTypeLocking, sizeof(DebuggerEmulatorModel));

    with_view_model(
        handle->view,
        DebuggerEmulatorModel * model,
        {
            model->animation_counter = 0;
            model->animation_visible = 0;
        },
        false);

    view_set_context(handle->view, handle);
    view_set_draw_callback(handle->view, debugger_emulator_draw_callback);
    view_set_input_callback(handle->view, debugger_emulator_input_callback);
    view_set_enter_callback(handle->view, view_display_test_enter);
    view_set_exit_callback(handle->view, view_display_test_exit);

    handle->timer =
        furi_timer_alloc(view_display_test_timer_callback, FuriTimerTypePeriodic, handle);

    handle->mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    return handle;
}

void debugger_emulator_set_animation_status(DebuggerEmulator* handle, uint8_t enabled) {
    furi_mutex_acquire(handle->mutex, FuriWaitForever);
    handle->animation_enabled = enabled;

    with_view_model(
        handle->view,
        DebuggerEmulatorModel * model,
        {
            if(enabled && !model->animation_visible) {
                model->animation_counter = 0;
                model->animation_visible = 1;
            }
        },
        false);
    furi_mutex_release(handle->mutex);
}

void debugger_emulator_free(DebuggerEmulator* handle) {
    furi_assert(handle);
    furi_timer_free(handle->timer);
    furi_mutex_free(handle->mutex);
    view_free(handle->view);
    free(handle);
}

View* debugger_emulator_get_view(DebuggerEmulator* debugger) {
    furi_assert(debugger);
    return debugger->view;
}
