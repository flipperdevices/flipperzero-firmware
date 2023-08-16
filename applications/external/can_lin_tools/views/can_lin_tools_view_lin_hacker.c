#include "can_lin_tools_view_lin_hacker.h"
#include "../can_lin_tools_app_i.h"
#include <can_lin_tools_icons.h>
#include "../helpers/lin_hacker_worker.h"

#include <input/input.h>
#include <gui/elements.h>

struct CanLinToolsLinHackerView {
    View* view;
    LinChackerWorker* worker;
    CanLinToolsLinHackerViewCallback callback;
    void* context;
};

typedef struct {
    CanLinToolsLinHackerViewStatus status;
} CanLinToolsLinHackerModel;

void can_lin_tools_view_lin_hacker_draw(Canvas* canvas, CanLinToolsLinHackerModel* model) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    canvas_draw_str(canvas, 56, 36, "Lin Chacker");
    UNUSED(model);
}

bool can_lin_tools_view_lin_hacker_input(InputEvent* event, void* context) {
    furi_assert(context);
    CanLinToolsLinHackerView* instance = context;
    UNUSED(instance);

    if(event->key == InputKeyBack) {
        return false;
    }

    return true;
}

static void
    can_lin_tools_view_lin_hacker_usb_connect_callback(void* context, bool status_connect) {
    furi_assert(context);
    CanLinToolsLinHackerView* instance = context;

    with_view_model(
        instance->view,
        CanLinToolsLinHackerModel * model,
        {
            if(status_connect) {
                model->status = CanLinToolsLinHackerViewStatusUSBConnect;
            } else {
                model->status = CanLinToolsLinHackerViewStatusNoUSBConnect;
            }
        },
        true);
}

void can_lin_tools_view_lin_hacker_enter(void* context) {
    furi_assert(context);
    CanLinToolsLinHackerView* instance = context;

    with_view_model(
        instance->view,
        CanLinToolsLinHackerModel * model,
        { model->status = CanLinToolsLinHackerViewStatusNoUSBConnect; },
        true);

    //Start worker
    instance->worker = lin_hacker_worker_alloc(instance);

    lin_hacker_worker_set_callback(
        instance->worker, can_lin_tools_view_lin_hacker_usb_connect_callback, instance);

    lin_hacker_worker_start(instance->worker);
}

void can_lin_tools_view_lin_hacker_exit(void* context) {
    furi_assert(context);
    CanLinToolsLinHackerView* instance = context;
    //Stop worker
    if(lin_hacker_worker_is_running(instance->worker)) {
        lin_hacker_worker_stop(instance->worker);
    }
    lin_hacker_worker_set_callback(instance->worker, NULL, NULL);
    lin_hacker_worker_free(instance->worker);
}

CanLinToolsLinHackerView* can_lin_tools_view_lin_hacker_alloc() {
    CanLinToolsLinHackerView* instance = malloc(sizeof(CanLinToolsLinHackerView));

    // View allocation and configuration
    instance->view = view_alloc();

    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(CanLinToolsLinHackerModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)can_lin_tools_view_lin_hacker_draw);
    view_set_input_callback(instance->view, can_lin_tools_view_lin_hacker_input);
    view_set_enter_callback(instance->view, can_lin_tools_view_lin_hacker_enter);
    view_set_exit_callback(instance->view, can_lin_tools_view_lin_hacker_exit);

    with_view_model(
        instance->view,
        CanLinToolsLinHackerModel * model,
        { model->status = CanLinToolsLinHackerViewStatusNoUSBConnect; },
        true);
    return instance;
}

void can_lin_tools_view_lin_hacker_free(CanLinToolsLinHackerView* instance) {
    furi_assert(instance);

    view_free(instance->view);
    free(instance);
}

View* can_lin_tools_view_lin_hacker_get_view(CanLinToolsLinHackerView* instance) {
    furi_assert(instance);
    return instance->view;
}
