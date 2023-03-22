#include "avr_isp_view_chip_detect.h"
#include "../avr_isp_app_i.h"
#include <avr_isp_icons.h>
#include <gui/elements.h>

#include "../helpers/avr_isp_rw.h"

struct AvrIspChipDetectView {
    View* view;
    AvrIspRW* avr_isp_rw;
    AvrIspChipDetectViewCallback callback;
    void* context;
};

typedef struct {
    uint16_t idx;
    const char* name_chip;
    uint32_t flash_size;
    AvrIspChipDetectViewStatus status;
} AvrIspChipDetectViewModel;

void avr_isp_chip_detect_view_set_callback(
    AvrIspChipDetectView* instance,
    AvrIspChipDetectViewCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void avr_isp_chip_detect_set_status(
    AvrIspChipDetectView* instance,
    AvrIspChipDetectViewStatus status) {
    furi_assert(instance);

    with_view_model(
        instance->view, AvrIspChipDetectViewModel * model, { model->status = status; }, true);
}

void avr_isp_chip_detect_view_draw(Canvas* canvas, AvrIspChipDetectViewModel* model) {
    canvas_clear(canvas);
    char str_buf[64] = {0};
    canvas_set_font(canvas, FontPrimary);

    switch(model->status) {
    case AvrIspChipDetectViewStatusDetected:
        canvas_draw_str_aligned(canvas, 64, 5, AlignCenter, AlignCenter, "AVR chip detected!");
        canvas_draw_icon(canvas, 29, 14, &I_chip_long_70x22);
        canvas_set_font(canvas, FontSecondary);
        snprintf(str_buf, sizeof(str_buf), "%ld Kb", model->flash_size / 1024);
        canvas_draw_str_aligned(canvas, 64, 25, AlignCenter, AlignCenter, str_buf);
        canvas_draw_str_aligned(canvas, 64, 45, AlignCenter, AlignCenter, model->name_chip);
        elements_button_right(canvas, "Next");
        break;
    case AvrIspChipDetectViewStatusErrorOccured:
        canvas_draw_str_aligned(
            canvas, 64, 5, AlignCenter, AlignCenter, "Error occured, try again!");
        canvas_draw_icon(canvas, 29, 14, &I_chip_error_70x22);
        break;
    case AvrIspChipDetectViewStatusErrorVerification:
        canvas_draw_str_aligned(
            canvas, 64, 5, AlignCenter, AlignCenter, "Data verification failed");
        canvas_draw_icon(canvas, 29, 14, &I_chip_error_70x22);
        break;

    default:
        //AvrIspChipDetectViewStatusNoDetect
        canvas_draw_str_aligned(canvas, 64, 5, AlignCenter, AlignCenter, "AVR chip not found!");
        canvas_draw_icon(canvas, 29, 12, &I_chif_not_found_83x37);

        break;
    }
    canvas_set_font(canvas, FontSecondary);
    elements_button_left(canvas, "Retry");
}

bool avr_isp_chip_detect_view_input(InputEvent* event, void* context) {
    furi_assert(context);
    AvrIspChipDetectView* instance = context;
    if(event->key == InputKeyBack || event->type != InputTypeShort) {
        return false;
    } else if(event->key == InputKeyRight && event->type == InputTypeShort) {
        with_view_model(
            instance->view,
            AvrIspChipDetectViewModel * model,
            {
                if(model->status == AvrIspChipDetectViewStatusDetected) {
                    if(instance->callback)
                        instance->callback(AvrIspCustomEventSceneChipDetectOk, instance->context);
                }
            },
            false);

    } else if(event->key == InputKeyLeft && event->type == InputTypeShort) {
        with_view_model(
            instance->view,
            AvrIspChipDetectViewModel * model,
            {
                if(model->status != AvrIspChipDetectViewStatusDetecting) {
                    model->status = AvrIspChipDetectViewStatusDetecting;
                    avr_isp_rw_detect_chip(instance->avr_isp_rw);
                }
            },
            false);
    }

    return true;
}

static void avr_isp_chip_detect_detect_chip_callback(
    void* context,
    const char* name,
    bool detect_chip,
    uint32_t flash_size) {
    furi_assert(context);
    AvrIspChipDetectView* instance = context;
    with_view_model(
        instance->view,
        AvrIspChipDetectViewModel * model,
        {
            model->name_chip = name;
            model->flash_size = flash_size;
            if(detect_chip) {
                model->status = AvrIspChipDetectViewStatusDetected;
            } else {
                model->status = AvrIspChipDetectViewStatusNoDetect;
            }
        },
        true);
}
void avr_isp_chip_detect_view_enter(void* context) {
    furi_assert(context);
    AvrIspChipDetectView* instance = context;

    //Start avr_isp_rw
    instance->avr_isp_rw = avr_isp_rw_alloc(instance->context);

    avr_isp_rw_set_callback(
        instance->avr_isp_rw, avr_isp_chip_detect_detect_chip_callback, instance);

    with_view_model(
        instance->view,
        AvrIspChipDetectViewModel * model,
        {
            if(model->status == AvrIspChipDetectViewStatusNoDetect ||
               model->status == AvrIspChipDetectViewStatusDetected) {
                avr_isp_rw_detect_chip(instance->avr_isp_rw);
            }
        },
        false);
}

void avr_isp_chip_detect_view_exit(void* context) {
    furi_assert(context);
    AvrIspChipDetectView* instance = context;

    avr_isp_rw_set_callback(instance->avr_isp_rw, NULL, NULL);
    avr_isp_rw_free(instance->avr_isp_rw);
}

AvrIspChipDetectView* avr_isp_chip_detect_view_alloc() {
    AvrIspChipDetectView* instance = malloc(sizeof(AvrIspChipDetectView));

    // View allocation and configuration
    instance->view = view_alloc();

    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(AvrIspChipDetectViewModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)avr_isp_chip_detect_view_draw);
    view_set_input_callback(instance->view, avr_isp_chip_detect_view_input);
    view_set_enter_callback(instance->view, avr_isp_chip_detect_view_enter);
    view_set_exit_callback(instance->view, avr_isp_chip_detect_view_exit);

    with_view_model(
        instance->view,
        AvrIspChipDetectViewModel * model,
        { model->status = AvrIspChipDetectViewStatusNoDetect; },
        false);
    return instance;
}

void avr_isp_chip_detect_view_free(AvrIspChipDetectView* instance) {
    furi_assert(instance);

    view_free(instance->view);
    free(instance);
}

View* avr_isp_chip_detect_view_get_view(AvrIspChipDetectView* instance) {
    furi_assert(instance);
    return instance->view;
}
