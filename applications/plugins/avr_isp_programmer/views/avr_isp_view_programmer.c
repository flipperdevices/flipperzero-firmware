#include "avr_isp_view_programmer.h"
#include "../avr_isp_app_i.h"
#include <avr_isp_icons.h>

#include "../helpers/avr_isp_worker.h"
//#include <math.h>

//#include <input/input.h>
//#include <gui/elements.h>

struct AvrIspProgrammerView {
    View* view;
    AvrIspWorker* worker;
    AvrIspProgrammerViewCallback callback;
    void* context;
};

typedef struct {
    uint16_t idx;
} AvrIspProgrammerViewModel;

void avr_asp_programmer_view_set_callback(
    AvrIspProgrammerView* instance,
    AvrIspProgrammerViewCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void avr_asp_programmer_view_draw(Canvas* canvas, AvrIspProgrammerViewModel* model) {
    UNUSED(model);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);

    canvas_draw_icon(canvas, 0, 0, &I_AvrIspProg);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 63, 46, "AvrIspProg");
    canvas_set_font(canvas, FontSecondary);
}

bool avr_asp_programmer_view_input(InputEvent* event, void* context) {
    furi_assert(context);
    AvrIspProgrammerView* instance = context;
    UNUSED(instance);
    if(event->key == InputKeyBack || event->type != InputTypeShort) {
        return false;
    }

    return true;
}

void avr_asp_programmer_view_enter(void* context) {
    furi_assert(context);
    AvrIspProgrammerView* instance = context;
    //Start worker
    instance->worker = avr_isp_worker_alloc(instance->context);

    // avr_isp_worker_set_callback(
    //     instance->worker,
    //     (SubGhzFrequencyAnalyzerWorkerPairCallback)avr_isp_callback,
    //     instance);

    avr_isp_worker_start(instance->worker);
}

void avr_asp_programmer_view_exit(void* context) {
    furi_assert(context);
    AvrIspProgrammerView* instance = context;
    //Stop worker
    if(avr_isp_worker_is_running(instance->worker)) {
        avr_isp_worker_stop(instance->worker);
    }
    avr_isp_worker_free(instance->worker);
}

AvrIspProgrammerView* avr_asp_programmer_view_alloc() {
    AvrIspProgrammerView* instance = malloc(sizeof(AvrIspProgrammerView));

    // View allocation and configuration
    instance->view = view_alloc();

    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(AvrIspProgrammerViewModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)avr_asp_programmer_view_draw);
    view_set_input_callback(instance->view, avr_asp_programmer_view_input);
    view_set_enter_callback(instance->view, avr_asp_programmer_view_enter);
    view_set_exit_callback(instance->view, avr_asp_programmer_view_exit);

    // with_view_model(
    //     instance->view,
    //     AvrIspProgrammerViewModel * model,
    //     {
    //         //
    //     },
    //     true);
    return instance;
}

void avr_asp_programmer_view_free(AvrIspProgrammerView* instance) {
    furi_assert(instance);

    // with_view_model(
    //     avr_asp_programmer->view,
    //     AvrIspProgrammerViewModel * model,
    //     {
    //         //
    //     },
    //     false);
    view_free(instance->view);
    free(instance);
}

View* avr_asp_programmer_view_get_view(AvrIspProgrammerView* instance) {
    furi_assert(instance);
    return instance->view;
}
