#include "can_lin_tools_view_can_hacker2.h"
#include "../can_lin_tools_app_i.h"
#include <can_lin_tools_icons.h>
#include "../helpers/can_hacker2_worker.h"

#include <input/input.h>
#include <gui/elements.h>

struct CanLinToolsCanHacker2 {
    View* view;
    CanChacker2Worker* worker;
    CanLinToolsCanHacker2ViewCallback callback;
    void* context;
};

typedef struct {
    CanLinToolsCanHacker2ViewStatus status;
} CanLinToolsCanHacker2Model;


void can_lin_tools_view_can_hacker2_draw(Canvas* canvas, CanLinToolsCanHacker2Model* model) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    canvas_draw_str(canvas, 56, 36, "Touch the reader");

    UNUSED(model);

}

bool can_lin_tools_view_can_hacker2_input(InputEvent* event, void* context) {
    furi_assert(context);
    CanLinToolsCanHacker2* instance = context;
    UNUSED(instance);

    if(event->key == InputKeyBack) {
        return false;
    }

    return true;
}

static void
    can_lin_tools_view_can_hacker2_usb_connect_callback(void* context, bool status_connect) {
    furi_assert(context);
    CanLinToolsCanHacker2* instance = context;

    with_view_model(
        instance->view,
        CanLinToolsCanHacker2Model * model,
        {
            if(status_connect) {
                model->status = CanLinToolsCanHacker2ViewStatusUSBConnect;
            } else {
                model->status = CanLinToolsCanHacker2ViewStatusNoUSBConnect;
            }
        },
        true);
}

void can_lin_tools_view_can_hacker2_enter(void* context) {
    furi_assert(context);
    CanLinToolsCanHacker2* instance = context;

    with_view_model(
        instance->view,
        CanLinToolsCanHacker2Model * model,
        { model->status = CanLinToolsCanHacker2ViewStatusNoUSBConnect; },
        true);

    //Start worker
    instance->worker = can_hacker2_worker_alloc(instance);

    can_hacker2_worker_set_callback(
        instance->worker, can_lin_tools_view_can_hacker2_usb_connect_callback, instance);

    can_hacker2_worker_start(instance->worker);
}

void can_lin_tools_view_can_hacker2_exit(void* context) {
    furi_assert(context);
    CanLinToolsCanHacker2* instance = context;
    //Stop worker
    if(can_hacker2_worker_is_running(instance->worker)) {
        can_hacker2_worker_stop(instance->worker);
    }
    can_hacker2_worker_set_callback(instance->worker, NULL, NULL);
    can_hacker2_worker_free(instance->worker);
}

CanLinToolsCanHacker2* can_lin_tools_view_can_hacker2_alloc() {
    CanLinToolsCanHacker2* instance = malloc(sizeof(CanLinToolsCanHacker2));

    // View allocation and configuration
    instance->view = view_alloc();

    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(CanLinToolsCanHacker2Model));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)can_lin_tools_view_can_hacker2_draw);
    view_set_input_callback(instance->view, can_lin_tools_view_can_hacker2_input);
    view_set_enter_callback(instance->view, can_lin_tools_view_can_hacker2_enter);
    view_set_exit_callback(instance->view, can_lin_tools_view_can_hacker2_exit);

    with_view_model(
        instance->view,
        CanLinToolsCanHacker2Model * model,
        { model->status = CanLinToolsCanHacker2ViewStatusNoUSBConnect; },
        true);

    return instance;
}

void can_lin_tools_view_can_hacker2_free(CanLinToolsCanHacker2* instance) {
    furi_assert(instance);

    view_free(instance->view);
    free(instance);
}

View* can_lin_tools_view_can_hacker2_get_view(CanLinToolsCanHacker2* instance) {
    furi_assert(instance);
    return instance->view;
}
