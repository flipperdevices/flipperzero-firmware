#include "avr_isp_view_reader.h"
#include "../avr_isp_app_i.h"
#include <avr_isp_icons.h>
#include <gui/elements.h>

#include "../helpers/avr_isp_rw.h"
//#include <math.h>

//#include <input/input.h>
//#include <gui/elements.h>

struct AvrIspReaderView {
    View* view;
    AvrIspRW* avr_isp_rw;
    const char* file_path;
    const char* file_name;
    AvrIspReaderViewCallback callback;
    void* context;
};

typedef struct {
    uint16_t idx;
    IconAnimation* icon;
    const char* name_chip;
    bool detect_chip;
} AvrIspReaderViewModel;

void avr_isp_reader_view_set_callback(
    AvrIspReaderView* instance,
    AvrIspReaderViewCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void avr_isp_reader_set_file_path(
    AvrIspReaderView* instance,
    const char* file_path,
    const char* file_name) {
    furi_assert(instance);
    instance->file_path = file_path;
    instance->file_name = file_name;
}

void avr_isp_reader_view_draw(Canvas* canvas, AvrIspReaderViewModel* model) {
    UNUSED(model);
    canvas_clear(canvas);
    // canvas_set_color(canvas, ColorBlack);
    // canvas_set_font(canvas, FontSecondary);

    // canvas_draw_icon(canvas, 0, 0, &I_AvrIspProg);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 20, 10, "AVRISPReader");
    // canvas_set_font(canvas, FontSecondary);
    canvas_set_font(canvas, FontPrimary);

    if(!model->detect_chip) {
        canvas_draw_icon_animation(canvas, 0, 0, model->icon);
        canvas_draw_str_aligned(canvas, 64, 26, AlignLeft, AlignCenter, "Detecting");
        canvas_draw_str_aligned(canvas, 64, 36, AlignLeft, AlignCenter, "AVR chip...");
    } else {
        canvas_draw_str_aligned(canvas, 20, 26, AlignLeft, AlignCenter, "AVR chip");
        canvas_draw_str_aligned(canvas, 20, 36, AlignLeft, AlignCenter, model->name_chip);
    }
    elements_button_left(canvas, "ReDetect");
    elements_button_center(canvas, "Dump");
}

bool avr_isp_reader_view_input(InputEvent* event, void* context) {
    furi_assert(context);
    AvrIspReaderView* instance = context;
    UNUSED(instance);
    if(event->key == InputKeyBack || event->type != InputTypeShort) {
        return false;
    } else if(event->key == InputKeyOk && event->type == InputTypeShort) {
        FURI_LOG_D("d", "Read dump chip 1");
        avr_isp_rw_get_dump(instance->avr_isp_rw, instance->file_path, instance->file_name);
    } else if(event->key == InputKeyLeft && event->type == InputTypeShort) {
        with_view_model(
            instance->view,
            AvrIspReaderViewModel * model,
            {
                icon_animation_start(model->icon);
                model->detect_chip = false;
            },
            false);
        avr_isp_rw_detect_chip(instance->avr_isp_rw);
    }

    return true;
}

static void
    avr_isp_reader_detect_chip_callback(void* context, const char* name, bool detect_chip) {
    furi_assert(context);
    AvrIspReaderView* instance = context;
    with_view_model(
        instance->view,
        AvrIspReaderViewModel * model,
        {
            model->name_chip = name;
            model->detect_chip = detect_chip;
            if(detect_chip) icon_animation_stop(model->icon);
        },
        true);
}
void avr_isp_reader_view_enter(void* context) {
    furi_assert(context);
    AvrIspReaderView* instance = context;

    with_view_model(
        instance->view,
        AvrIspReaderViewModel * model,
        {
            icon_animation_start(model->icon);
            model->detect_chip = false;
        },
        false);

    //Start avr_isp_rw
    instance->avr_isp_rw = avr_isp_rw_alloc(instance->context);

    avr_isp_rw_set_callback(instance->avr_isp_rw, avr_isp_reader_detect_chip_callback, instance);

    avr_isp_rw_detect_chip(instance->avr_isp_rw);

    //avr_isp_rw_start(instance->avr_isp_rw);
}

void avr_isp_reader_view_exit(void* context) {
    furi_assert(context);
    AvrIspReaderView* instance = context;
    // //Stop avr_isp_rw
    // if(avr_isp_avr_isp_rw_is_running(instance->avr_isp_rw)) {
    //     avr_isp_avr_isp_rw_stop(instance->avr_isp_rw);
    // }
    avr_isp_rw_free(instance->avr_isp_rw);

    with_view_model(
        instance->view,
        AvrIspReaderViewModel * model,
        { icon_animation_stop(model->icon); },
        false);
}

AvrIspReaderView* avr_isp_reader_view_alloc() {
    AvrIspReaderView* instance = malloc(sizeof(AvrIspReaderView));

    // View allocation and configuration
    instance->view = view_alloc();

    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(AvrIspReaderViewModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)avr_isp_reader_view_draw);
    view_set_input_callback(instance->view, avr_isp_reader_view_input);
    view_set_enter_callback(instance->view, avr_isp_reader_view_enter);
    view_set_exit_callback(instance->view, avr_isp_reader_view_exit);

    with_view_model(
        instance->view,
        AvrIspReaderViewModel * model,
        {
            model->icon = icon_animation_alloc(&A_ChipLooking_64x64);
            view_tie_icon_animation(instance->view, model->icon);
            model->detect_chip = false;
        },
        false);
    return instance;
}

void avr_isp_reader_view_free(AvrIspReaderView* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        AvrIspReaderViewModel * model,
        { icon_animation_free(model->icon); },
        false);
    view_free(instance->view);
    free(instance);
}

View* avr_isp_reader_view_get_view(AvrIspReaderView* instance) {
    furi_assert(instance);
    return instance->view;
}
