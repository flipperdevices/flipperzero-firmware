#include <furi.h>
#include "../dolphin_i.h"
#include <furi-hal.h>
#include <furi-hal-version.h>

#include "dolphin_hw_mismatch_view.h"

void dolphin_hw_mismatch_set_callback(
    DolphinHwMismatchView* main_view,
    DolphinHwMismatchViewCallback callback,
    void* context) {
    furi_assert(main_view);
    furi_assert(callback);
    main_view->callback = callback;
    main_view->context = context;
}

void dolphin_hw_mismatch_view_render(Canvas* canvas, void* model) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 15, "!!!! HW Mismatch !!!!");

    char buffer[64];
    canvas_set_font(canvas, FontSecondary);
    snprintf(buffer, 64, "HW target: F%d", furi_hal_version_get_hw_target());
    canvas_draw_str(canvas, 5, 27, buffer);
    canvas_draw_str(canvas, 5, 38, "FW target: " TARGET);
}

View* dolphin_hw_mismatch_get_view(DolphinHwMismatchView* hw_mismatch_view) {
    furi_assert(hw_mismatch_view);
    return hw_mismatch_view->view;
}

bool dolphin_hw_mismatch_view_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);

    DolphinHwMismatchView* hw_mismatch_view = context;

    if(event->type == InputTypeShort) {
        hw_mismatch_view->callback(DolphinHwMismatchEventExit, hw_mismatch_view->context);
    }

    return true;
}

DolphinHwMismatchView* dolphin_hw_mismatch_view_alloc() {
    DolphinHwMismatchView* hw_mismatch_view = furi_alloc(sizeof(DolphinHwMismatchView));
    hw_mismatch_view->view = view_alloc();
    view_allocate_model(
        hw_mismatch_view->view, ViewModelTypeLocking, sizeof(DolphinHwMismatchViewModel));
    view_set_context(hw_mismatch_view->view, hw_mismatch_view);
    view_set_draw_callback(
        hw_mismatch_view->view, (ViewDrawCallback)dolphin_hw_mismatch_view_render);
    view_set_input_callback(hw_mismatch_view->view, dolphin_hw_mismatch_view_input);

    return hw_mismatch_view;
}

void dolphin_hw_mismatch_view_free(DolphinHwMismatchView* hw_mismatch_view) {
    furi_assert(hw_mismatch_view);

    view_free(hw_mismatch_view->view);
    free(hw_mismatch_view);
}
