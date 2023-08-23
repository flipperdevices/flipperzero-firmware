#include "can_lin_tools_view_can_hacker2.h"
#include "../can_lin_tools_app_i.h"
#include <can_lin_tools_icons.h>
#include "../helpers/can_hacker2_worker.h"

#include <input/input.h>
#include <gui/elements.h>

struct CanLinToolsCanHacker2View {
    View* view;
    CanChacker2Worker* worker;
    CanLinToolsCanHacker2ViewCallback callback;
    void* context;
};

typedef struct {
    CanChacker2WorkerStatus status;
} CanLinToolsCanHacker2Model;

void can_lin_tools_view_can_hacker2_set_callback(
    CanLinToolsCanHacker2View* instance,
    CanLinToolsCanHacker2ViewCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;
}

void can_lin_tools_view_can_hacker2_draw(Canvas* canvas, CanLinToolsCanHacker2Model* model) {
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);

    switch(model->status) {
    case CanChacker2WorkerStatusModuleDisconnect:
        canvas_draw_icon(canvas, 8, 2, &I_Attach_module_can_53x60);
        canvas_set_font(canvas, FontSecondary);
        elements_multiline_text(canvas, 80, 25, "Attach\nCAN\nModule");
        break;
    case CanChacker2WorkerStatusProcDisconnected:
        canvas_draw_icon(canvas, 0, 39, &I_waiting_for_can_software_127x24);
        elements_multiline_text(canvas, 13, 12, "Waiting for software");
        elements_multiline_text(canvas, 38, 22, "connection");
        break;
    case CanChacker2WorkerStatusProcConnected:
        canvas_draw_icon(canvas, 0, 40, &I_can_module_ready_127x24);
        elements_multiline_text(canvas, 13, 12, "Software connected!");
        canvas_set_font(canvas, FontSecondary);
        elements_multiline_text(canvas, 20, 22, "Module is ready to use");
        break;

    default:
        furi_crash("unknown status");
        break;
    }
}

bool can_lin_tools_view_can_hacker2_input(InputEvent* event, void* context) {
    furi_assert(context);
    CanLinToolsCanHacker2View* instance = context;
    UNUSED(instance);

    if(event->key == InputKeyBack) {
        return false;
    }

    return true;
}

static void can_lin_tools_view_can_hacker2_usb_connect_callback(
    void* context,
    CanChacker2WorkerStatus status) {
    furi_assert(context);
    CanLinToolsCanHacker2View* instance = context;

    switch(status) {
    case CanChacker2WorkerStatusModuleDisconnect:
        if(instance->callback)
            instance->callback(CanLinToolsCustomEventModuleDisconnected, instance->context);
        break;
    case CanChacker2WorkerStatusProcDisconnected:
        if(instance->callback)
            instance->callback(CanLinToolsCustomEventProcDisconnected, instance->context);
        break;
    case CanChacker2WorkerStatusProcConnected:
        if(instance->callback)
            instance->callback(CanLinToolsCustomEventProcConnected, instance->context);
        break;

    default:
        break;
    }

    with_view_model(
        instance->view, CanLinToolsCanHacker2Model * model, { model->status = status; }, true);
}

void can_lin_tools_view_can_hacker2_enter(void* context) {
    furi_assert(context);
    CanLinToolsCanHacker2View* instance = context;

    with_view_model(
        instance->view,
        CanLinToolsCanHacker2Model * model,
        { model->status = CanChacker2WorkerStatusModuleDisconnect; },
        true);

    //Start worker
    instance->worker = can_hacker2_worker_alloc(instance);

    can_hacker2_worker_set_callback(
        instance->worker, can_lin_tools_view_can_hacker2_usb_connect_callback, instance);

    can_hacker2_worker_start(instance->worker);
}

void can_lin_tools_view_can_hacker2_exit(void* context) {
    furi_assert(context);
    CanLinToolsCanHacker2View* instance = context;
    //Stop worker
    if(can_hacker2_worker_is_running(instance->worker)) {
        can_hacker2_worker_stop(instance->worker);
    }
    can_hacker2_worker_set_callback(instance->worker, NULL, NULL);
    can_hacker2_worker_free(instance->worker);
}

CanLinToolsCanHacker2View* can_lin_tools_view_can_hacker2_alloc() {
    CanLinToolsCanHacker2View* instance = malloc(sizeof(CanLinToolsCanHacker2View));

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
        { model->status = CanChacker2WorkerStatusModuleDisconnect; },
        true);

    return instance;
}

void can_lin_tools_view_can_hacker2_free(CanLinToolsCanHacker2View* instance) {
    furi_assert(instance);

    view_free(instance->view);
    free(instance);
}

View* can_lin_tools_view_can_hacker2_get_view(CanLinToolsCanHacker2View* instance) {
    furi_assert(instance);
    return instance->view;
}
