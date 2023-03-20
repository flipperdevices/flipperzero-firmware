#include "avr_isp_view_reader.h"
#include "../avr_isp_app_i.h"
#include <avr_isp_icons.h>
#include <gui/elements.h>

#include "../helpers/avr_isp_rw_worker.h"
//#include <math.h>

//#include <input/input.h>
//#include <gui/elements.h>

struct AvrIspWriterView {
    View* view;
    AvrIspRWWorker* worker;
    AvrIspWriterViewCallback callback;
    void* context;
};

typedef struct {
    uint16_t idx;
    IconAnimation* icon;
    const char* name_chip;
    bool detect_chip;
} AvrIspWriterViewModel;

void avr_asp_writer_view_set_callback(
    AvrIspWriterView* instance,
    AvrIspWriterViewCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void avr_asp_writer_view_draw(Canvas* canvas, AvrIspWriterViewModel* model) {
    UNUSED(model);
    canvas_clear(canvas);
    // canvas_set_color(canvas, ColorBlack);
    // canvas_set_font(canvas, FontSecondary);

    // canvas_draw_icon(canvas, 0, 0, &I_AvrIspProg);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 20, 10, "AVRISPWriter");
    // canvas_set_font(canvas, FontSecondary);
    canvas_set_font(canvas, FontPrimary);

    elements_button_left(canvas, "ReDetect");
    elements_button_center(canvas, "Dump");

    if(!model->detect_chip) {
        canvas_draw_icon_animation(canvas, 0, 0, model->icon);
        canvas_draw_str_aligned(canvas, 64, 26, AlignLeft, AlignCenter, "Detecting");
        canvas_draw_str_aligned(canvas, 64, 36, AlignLeft, AlignCenter, "AVR chip...");
    } else {
        canvas_draw_str_aligned(canvas, 20, 26, AlignLeft, AlignCenter, "AVR chip");
        canvas_draw_str_aligned(canvas, 20, 36, AlignLeft, AlignCenter, model->name_chip);
    }
}

bool avr_asp_writer_view_input(InputEvent* event, void* context) {
    furi_assert(context);
    AvrIspWriterView* instance = context;
    UNUSED(instance);
    if(event->key == InputKeyBack || event->type != InputTypeShort) {
        return false;
    } else if(event->key == InputKeyOk && event->type == InputTypePress) {
    } else if(event->key == InputKeyLeft && event->type == InputTypeShort) {
        FURI_LOG_D("s", "--Detecting--");
        avr_isp_rw_worker_detect_chip(instance->worker);
    }

    return true;
}

static void avr_asp_writer_detect_chip_callback(void* context, const char* name) {
    furi_assert(context);
    AvrIspWriterView* instance = context;
    with_view_model(
        instance->view,
        AvrIspWriterViewModel * model,
        {
            model->name_chip = name;
            model->detect_chip = true;
            icon_animation_stop(model->icon);
        },
        true);
}
void avr_asp_writer_view_enter(void* context) {
    furi_assert(context);
    AvrIspWriterView* instance = context;

    with_view_model(
        instance->view,
        AvrIspWriterViewModel * model,
        {
            icon_animation_start(model->icon);
            model->detect_chip = false;
        },
        false);

    //Start worker
    instance->worker = avr_isp_rw_worker_alloc(instance->context);

    avr_isp_rw_worker_set_callback(
        instance->worker, avr_asp_writer_detect_chip_callback, instance);

    avr_isp_rw_worker_detect_chip(instance->worker);

    //avr_isp_rw_worker_start(instance->worker);
}

void avr_asp_writer_view_exit(void* context) {
    furi_assert(context);
    AvrIspWriterView* instance = context;
    // //Stop worker
    // if(avr_isp_worker_is_running(instance->worker)) {
    //     avr_isp_worker_stop(instance->worker);
    // }
    avr_isp_rw_worker_free(instance->worker);

    with_view_model(
        instance->view,
        AvrIspWriterViewModel * model,
        { icon_animation_stop(model->icon); },
        false);
}

AvrIspWriterView* avr_asp_writer_view_alloc() {
    AvrIspWriterView* instance = malloc(sizeof(AvrIspWriterView));

    // View allocation and configuration
    instance->view = view_alloc();

    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(AvrIspWriterViewModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)avr_asp_writer_view_draw);
    view_set_input_callback(instance->view, avr_asp_writer_view_input);
    view_set_enter_callback(instance->view, avr_asp_writer_view_enter);
    view_set_exit_callback(instance->view, avr_asp_writer_view_exit);

    with_view_model(
        instance->view,
        AvrIspWriterViewModel * model,
        {
            model->icon = icon_animation_alloc(&A_ChipLooking_64x64);
            view_tie_icon_animation(instance->view, model->icon);
            model->detect_chip = false;
        },
        false);
    return instance;
}

void avr_asp_writer_view_free(AvrIspWriterView* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        AvrIspWriterViewModel * model,
        { icon_animation_free(model->icon); },
        false);
    view_free(instance->view);
    free(instance);
}

View* avr_asp_writer_view_get_view(AvrIspWriterView* instance) {
    furi_assert(instance);
    return instance->view;
}
