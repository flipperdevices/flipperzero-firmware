#include "can_lin_tools_view_can_hacker2.h"
#include "../can_lin_tools_app_i.h"
#include <can_lin_tools_icons.h>
#include "../helpers/carbus_analyzer_worker.h"

#include <input/input.h>
#include <gui/elements.h>

struct CanLinToolsCarBusAnalyzerView {
    View* view;
    CarBusAnalyzerWorker* worker;
    CanLinToolsCarBusAnalyzerViewCallback callback;
    void* context;
};

typedef struct {
    CarBusAnalyzerWorkerStatus status;
} CanLinToolsCarBusAnalyzerModel;

void can_lin_tools_view_carbus_analyzer_set_callback(
    CanLinToolsCarBusAnalyzerView* instance,
    CanLinToolsCarBusAnalyzerViewCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;
}

void can_lin_tools_view_carbus_analyzer_draw(Canvas* canvas, CanLinToolsCarBusAnalyzerModel* model) {
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);

    switch(model->status) {
    case CarBusAnalyzerWorkerStatusModuleDisconnect:
        canvas_draw_icon(canvas, 8, 2, &I_Attach_module_can_53x60);
        canvas_set_font(canvas, FontSecondary);
        elements_multiline_text(canvas, 80, 25, "Attach\nCAN\nModule");
        break;
    case CarBusAnalyzerWorkerStatusProcDisconnected:
        canvas_draw_icon(canvas, 0, 39, &I_waiting_for_can_software_127x24);
        elements_multiline_text(canvas, 13, 12, "Waiting for software");
        elements_multiline_text(canvas, 38, 22, "connection");
        break;
    case CarBusAnalyzerWorkerStatusProcConnected:
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

bool can_lin_tools_view_carbus_analyzer_input(InputEvent* event, void* context) {
    furi_assert(context);
    CanLinToolsCarBusAnalyzerView* instance = context;
    UNUSED(instance);

    if(event->key == InputKeyBack) {
        return false;
    }

    return true;
}

static void can_lin_tools_view_carbus_analyzer_usb_connect_callback(
    void* context,
    CarBusAnalyzerWorkerStatus status) {
    furi_assert(context);
    CanLinToolsCarBusAnalyzerView* instance = context;

    switch(status) {
    case CarBusAnalyzerWorkerStatusModuleDisconnect:
        if(instance->callback)
            instance->callback(CanLinToolsCustomEventModuleDisconnected, instance->context);
        break;
    case CarBusAnalyzerWorkerStatusProcDisconnected:
        if(instance->callback)
            instance->callback(CanLinToolsCustomEventProcDisconnected, instance->context);
        break;
    case CarBusAnalyzerWorkerStatusProcConnected:
        if(instance->callback)
            instance->callback(CanLinToolsCustomEventProcConnected, instance->context);
        break;

    default:
        break;
    }

    with_view_model(
        instance->view, CanLinToolsCarBusAnalyzerModel * model, { model->status = status; }, true);
}

void can_lin_tools_view_carbus_analyzer_enter(void* context) {
    furi_assert(context);
    CanLinToolsCarBusAnalyzerView* instance = context;

    with_view_model(
        instance->view,
        CanLinToolsCarBusAnalyzerModel * model,
        { model->status = CarBusAnalyzerWorkerStatusModuleDisconnect; },
        true);

    //Start worker
    instance->worker = carbus_analyzer_worker_alloc(instance);

    carbus_analyzer_worker_set_callback(
        instance->worker, can_lin_tools_view_carbus_analyzer_usb_connect_callback, instance);

    carbus_analyzer_worker_start(instance->worker);
}

void can_lin_tools_view_carbus_analyzer_exit(void* context) {
    furi_assert(context);
    CanLinToolsCarBusAnalyzerView* instance = context;
    //Stop worker
    if(carbus_analyzer_worker_is_running(instance->worker)) {
        carbus_analyzer_worker_stop(instance->worker);
    }
    carbus_analyzer_worker_set_callback(instance->worker, NULL, NULL);
    carbus_analyzer_worker_free(instance->worker);
}

CanLinToolsCarBusAnalyzerView* can_lin_tools_view_carbus_analyzer_alloc() {
    CanLinToolsCarBusAnalyzerView* instance = malloc(sizeof(CanLinToolsCarBusAnalyzerView));

    // View allocation and configuration
    instance->view = view_alloc();

    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(CanLinToolsCarBusAnalyzerModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)can_lin_tools_view_carbus_analyzer_draw);
    view_set_input_callback(instance->view, can_lin_tools_view_carbus_analyzer_input);
    view_set_enter_callback(instance->view, can_lin_tools_view_carbus_analyzer_enter);
    view_set_exit_callback(instance->view, can_lin_tools_view_carbus_analyzer_exit);

    with_view_model(
        instance->view,
        CanLinToolsCarBusAnalyzerModel * model,
        { model->status = CarBusAnalyzerWorkerStatusModuleDisconnect; },
        true);

    return instance;
}

void can_lin_tools_view_carbus_analyzer_free(CanLinToolsCarBusAnalyzerView* instance) {
    furi_assert(instance);

    view_free(instance->view);
    free(instance);
}

View* can_lin_tools_view_carbus_analyzer_get_view(CanLinToolsCarBusAnalyzerView* instance) {
    furi_assert(instance);
    return instance->view;
}
