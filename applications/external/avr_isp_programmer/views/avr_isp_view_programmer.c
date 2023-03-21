#include "avr_isp_view_programmer.h"
#include "../avr_isp_app_i.h"
#include <avr_isp_icons.h>

#include "../helpers/avr_isp_worker.h"
//#include <math.h>

//#include <input/input.h>
#include <gui/elements.h>

struct AvrIspProgrammerView {
    View* view;
    AvrIspWorker* worker;
    AvrIspProgrammerViewCallback callback;
    void* context;
};

typedef struct {
    uint16_t idx;
    IconAnimation* icon;
    const char* name_chip;
    bool detect_chip;
} AvrIspProgrammerViewModel;

void avr_isp_programmer_view_set_callback(
    AvrIspProgrammerView* instance,
    AvrIspProgrammerViewCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void avr_isp_programmer_view_draw(Canvas* canvas, AvrIspProgrammerViewModel* model) {
    UNUSED(model);
    canvas_clear(canvas);
    // canvas_set_color(canvas, ColorBlack);
    // canvas_set_font(canvas, FontSecondary);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_icon(canvas, 20, 8, &I_Link_waiting_77x56);
    elements_multiline_text(canvas, 20, 10, "Waiting for software\nconnection");
    // canvas_set_font(canvas, FontPrimary);
    // canvas_draw_str(canvas, 63, 46, "AVRISPProg");
    // canvas_set_font(canvas, FontSecondary);
    
    // if(!model->detect_chip) {
    //     canvas_draw_icon_animation(canvas, 0, 0, model->icon);
    //     canvas_draw_str_aligned(canvas, 64, 26, AlignLeft, AlignCenter, "Detecting");
    //     canvas_draw_str_aligned(canvas, 64, 36, AlignLeft, AlignCenter, "AVR chip...");
    // } else {
    //     canvas_draw_str_aligned(canvas, 20, 26, AlignLeft, AlignCenter, "AVR chip");
    //     canvas_draw_str_aligned(canvas, 20, 36, AlignLeft, AlignCenter, model->name_chip);
    // }
}

bool avr_isp_programmer_view_input(InputEvent* event, void* context) {
    furi_assert(context);
    AvrIspProgrammerView* instance = context;
    UNUSED(instance);
    if(event->key == InputKeyBack || event->type != InputTypeShort) {
        return false;
    }

    return true;
}

static void avr_isp_programmer_detect_chip_callback(void* context, const char* name) {
    furi_assert(context);
    AvrIspProgrammerView* instance = context;
    with_view_model(
        instance->view,
        AvrIspProgrammerViewModel * model,
        {
            model->name_chip = name;
            model->detect_chip = true;
            icon_animation_stop(model->icon);
        },
        true);
}
void avr_isp_programmer_view_enter(void* context) {
    furi_assert(context);
    AvrIspProgrammerView* instance = context;

    with_view_model(
        instance->view,
        AvrIspProgrammerViewModel * model,
        {
            icon_animation_start(model->icon);
            model->detect_chip = false;
        },
        false);

    //Start worker
    instance->worker = avr_isp_worker_alloc(instance->context);

    avr_isp_worker_set_callback(
        instance->worker, avr_isp_programmer_detect_chip_callback, instance);

    avr_isp_worker_detect_chip(instance->worker);

    avr_isp_worker_start(instance->worker);
}

void avr_isp_programmer_view_exit(void* context) {
    furi_assert(context);
    AvrIspProgrammerView* instance = context;
    //Stop worker
    if(avr_isp_worker_is_running(instance->worker)) {
        avr_isp_worker_stop(instance->worker);
    }
    avr_isp_worker_free(instance->worker);

    with_view_model(
        instance->view,
        AvrIspProgrammerViewModel * model,
        { icon_animation_stop(model->icon); },
        false);
}

AvrIspProgrammerView* avr_isp_programmer_view_alloc() {
    AvrIspProgrammerView* instance = malloc(sizeof(AvrIspProgrammerView));

    // View allocation and configuration
    instance->view = view_alloc();

    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(AvrIspProgrammerViewModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)avr_isp_programmer_view_draw);
    view_set_input_callback(instance->view, avr_isp_programmer_view_input);
    view_set_enter_callback(instance->view, avr_isp_programmer_view_enter);
    view_set_exit_callback(instance->view, avr_isp_programmer_view_exit);

    with_view_model(
        instance->view,
        AvrIspProgrammerViewModel * model,
        {
            model->icon = icon_animation_alloc(&A_ChipLooking_64x64);
            view_tie_icon_animation(instance->view, model->icon);
            model->detect_chip = false;
        },
        false);
    return instance;
}

void avr_isp_programmer_view_free(AvrIspProgrammerView* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        AvrIspProgrammerViewModel * model,
        { icon_animation_free(model->icon); },
        false);
    view_free(instance->view);
    free(instance);
}

View* avr_isp_programmer_view_get_view(AvrIspProgrammerView* instance) {
    furi_assert(instance);
    return instance->view;
}
