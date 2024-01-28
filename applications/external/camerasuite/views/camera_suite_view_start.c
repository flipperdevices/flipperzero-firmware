#include "../camera_suite.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>

void camera_suite_view_start_set_callback(
    CameraSuiteViewStart* instance,
    CameraSuiteViewStartCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void camera_suite_view_start_draw(Canvas* canvas, CameraSuiteViewStartModel* model) {
    UNUSED(model);
    canvas_clear(canvas);

    // Draw Camera Suite logo.
    canvas_draw_circle(canvas, 82, 28, 1);
    canvas_draw_circle(canvas, 85, 29, 11);
    canvas_draw_circle(canvas, 85, 29, 6);
    canvas_draw_circle(canvas, 85, 29, 9);
    canvas_draw_circle(canvas, 104, 17, 1);
    canvas_draw_line(canvas, 70, 14, 108, 14);
    canvas_draw_line(canvas, 70, 14, 74, 10);
    canvas_draw_line(canvas, 70, 15, 70, 37);
    canvas_draw_line(canvas, 70, 32, 74, 32);
    canvas_draw_line(canvas, 75, 9, 82, 9);
    canvas_draw_line(canvas, 78, 37, 70, 37);
    canvas_draw_line(canvas, 79, 20, 70, 20);
    canvas_draw_line(canvas, 81, 18, 85, 14);
    canvas_draw_line(canvas, 82, 14, 82, 8);
    canvas_draw_line(canvas, 82, 8, 86, 4);
    canvas_draw_line(canvas, 82, 8, 95, 8);
    canvas_draw_line(canvas, 83, 14, 85, 11);
    canvas_draw_line(canvas, 84, 15, 92, 15);
    canvas_draw_line(canvas, 86, 11, 92, 11);
    canvas_draw_line(canvas, 86, 23, 84, 24);
    canvas_draw_line(canvas, 86, 24, 82, 24);
    canvas_draw_line(canvas, 86, 25, 89, 27);
    canvas_draw_line(canvas, 86, 4, 98, 4);
    canvas_draw_line(canvas, 87, 24, 91, 28);
    canvas_draw_line(canvas, 87, 26, 89, 28);
    canvas_draw_line(canvas, 88, 26, 84, 25);
    canvas_draw_line(canvas, 88, 26, 86, 24);
    canvas_draw_line(canvas, 88, 28, 89, 31);
    canvas_draw_line(canvas, 89, 28, 89, 30);
    canvas_draw_line(canvas, 90, 28, 90, 31);
    canvas_draw_line(canvas, 90, 30, 89, 33);
    canvas_draw_line(canvas, 92, 11, 93, 14);
    canvas_draw_line(canvas, 93, 16, 97, 18);
    canvas_draw_line(canvas, 94, 8, 98, 4);
    canvas_draw_line(canvas, 95, 9, 95, 14);
    canvas_draw_line(canvas, 96, 32, 107, 32);
    canvas_draw_line(canvas, 98, 19, 100, 22);
    canvas_draw_line(canvas, 98, 5, 98, 9);
    canvas_draw_line(canvas, 98, 9, 111, 9);
    canvas_draw_line(canvas, 98, 9, 96, 14);
    canvas_draw_line(canvas, 99, 20, 108, 20);
    canvas_draw_line(canvas, 100, 23, 100, 27);
    canvas_draw_line(canvas, 100, 28, 93, 36);
    canvas_draw_line(canvas, 102, 23, 104, 23);
    canvas_draw_line(canvas, 104, 23, 106, 28);
    canvas_draw_line(canvas, 104, 24, 102, 28);
    canvas_draw_line(canvas, 107, 14, 112, 9);
    canvas_draw_line(canvas, 107, 28, 108, 27);
    canvas_draw_line(canvas, 107, 37, 92, 37);
    canvas_draw_line(canvas, 108, 15, 108, 37);
    canvas_draw_line(canvas, 108, 20, 112, 16);
    canvas_draw_line(canvas, 108, 32, 112, 28);
    canvas_draw_line(canvas, 108, 37, 112, 33);
    canvas_draw_line(canvas, 112, 10, 112, 33);

    // Draw "Start" button.
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 46, 57, "Start");
    canvas_draw_circle(canvas, 75, 53, 2);
    canvas_draw_dot(canvas, 72, 50);
    canvas_draw_dot(canvas, 72, 56);
    canvas_draw_dot(canvas, 78, 50);
    canvas_draw_dot(canvas, 78, 56);
    canvas_draw_line(canvas, 43, 47, 43, 59);
    canvas_draw_line(canvas, 44, 46, 81, 46);
    canvas_draw_line(canvas, 44, 60, 81, 60);
    canvas_draw_line(canvas, 71, 51, 71, 55);
    canvas_draw_line(canvas, 73, 49, 77, 49);
    canvas_draw_line(canvas, 73, 57, 77, 57);
    canvas_draw_line(canvas, 74, 52, 76, 52);
    canvas_draw_line(canvas, 74, 53, 76, 53);
    canvas_draw_line(canvas, 74, 54, 77, 54);
    canvas_draw_line(canvas, 79, 51, 79, 55);
    canvas_draw_line(canvas, 82, 47, 82, 59);

    // Draw "Camera Suite" text.
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 16, 23, "Camera");
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 23, 35, "Suite");
}

static void camera_suite_view_start_model_init(CameraSuiteViewStartModel* const model) {
    model->some_value = 1;
}

bool camera_suite_view_start_input(InputEvent* event, void* context) {
    furi_assert(context);
    CameraSuiteViewStart* instance = context;
    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            // Exit application.
            with_view_model(
                instance->view,
                CameraSuiteViewStartModel * model,
                {
                    UNUSED(model);
                    instance->callback(CameraSuiteCustomEventStartBack, instance->context);
                },
                true);
            break;
        case InputKeyOk:
            // Start the application.
            with_view_model(
                instance->view,
                CameraSuiteViewStartModel * model,
                {
                    UNUSED(model);
                    instance->callback(CameraSuiteCustomEventStartOk, instance->context);
                },
                true);
            break;
        case InputKeyMAX:
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyUp:
        case InputKeyDown:
            // Do nothing.
            break;
        }
    }
    return true;
}

void camera_suite_view_start_exit(void* context) {
    furi_assert(context);
}

void camera_suite_view_start_enter(void* context) {
    furi_assert(context);
    CameraSuiteViewStart* instance = (CameraSuiteViewStart*)context;
    with_view_model(
        instance->view,
        CameraSuiteViewStartModel * model,
        { camera_suite_view_start_model_init(model); },
        true);
}

CameraSuiteViewStart* camera_suite_view_start_alloc() {
    // Allocate memory for the instance
    CameraSuiteViewStart* instance = malloc(sizeof(CameraSuiteViewStart));

    // Allocate the view object
    instance->view = view_alloc();

    // Allocate model
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(CameraSuiteViewStartModel));

    // Set context for the view (furi_assert crashes in events without this)
    view_set_context(instance->view, instance);

    // Set draw callback
    view_set_draw_callback(instance->view, (ViewDrawCallback)camera_suite_view_start_draw);

    // Set input callback
    view_set_input_callback(instance->view, camera_suite_view_start_input);

    with_view_model(
        instance->view,
        CameraSuiteViewStartModel * model,
        { camera_suite_view_start_model_init(model); },
        true);

    return instance;
}

void camera_suite_view_start_free(CameraSuiteViewStart* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, CameraSuiteViewStartModel * model, { UNUSED(model); }, true);
    view_free(instance->view);
    free(instance);
}

View* camera_suite_view_start_get_view(CameraSuiteViewStart* instance) {
    furi_assert(instance);
    return instance->view;
}
