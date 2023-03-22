#include "avr_isp_view_reader.h"
#include "../avr_isp_app_i.h"
//#include <avr_isp_icons.h>
#include <gui/elements.h>

#include "../helpers/avr_isp_worker_rw.h"
//#include <math.h>

//#include <input/input.h>
//#include <gui/elements.h>

struct AvrIspReaderView {
    View* view;
    AvrIspWorkerRW* avr_isp_worker_rw;
    const char* file_path;
    const char* file_name;
    AvrIspReaderViewCallback callback;
    void* context;
};

typedef struct {
    AvrIspReaderViewStatus status;
    float progress_flash;
    float progress_eeprom;
    // IconAnimation* icon;
    // const char* name_chip;
    // bool detect_chip;
} AvrIspReaderViewModel;

void avr_isp_reader_update_progress(AvrIspReaderView* instance) {
    with_view_model(
        instance->view,
        AvrIspReaderViewModel * model,
        {
            model->progress_flash =
                avr_isp_worker_rw_get_progress_flash(instance->avr_isp_worker_rw);
            model->progress_eeprom =
                avr_isp_worker_rw_get_progress_eeprom(instance->avr_isp_worker_rw);
        },
        true);
}

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
    canvas_set_font(canvas, FontSecondary);
    //canvas_draw_str(canvas, 20, 10, "Reding dump");
    canvas_set_font(canvas, FontSecondary);
    //canvas_set_font(canvas, FontPrimary);

    canvas_set_font(canvas, FontPrimary);
    switch(model->status) {
    case AvrIspReaderViewStatusIDLE:
        canvas_draw_str_aligned(canvas, 64, 5, AlignCenter, AlignCenter, "Reding dump");
        canvas_set_font(canvas, FontSecondary);
        elements_button_center(canvas, "Start");
        break;
    case AvrIspReaderViewStatusReading:
        canvas_draw_str_aligned(canvas, 64, 5, AlignCenter, AlignCenter, "Reding dump");
        //elements_button_left(canvas, "Cancel");
        break;
    case AvrIspReaderViewStatusVerification:
        canvas_draw_str_aligned(canvas, 64, 5, AlignCenter, AlignCenter, "Verifyng dump");
        canvas_set_font(canvas, FontSecondary);
        //elements_button_left(canvas, "Cancel");
        break;

    default:
        break;
    }

    char str_buf[64] = {0};

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 27, "Flash");
    snprintf(str_buf, sizeof(str_buf), "%d%%", (uint8_t)(model->progress_flash * 100));
    elements_progress_bar_with_text(canvas, 44, 17, 84, model->progress_flash, str_buf);
    canvas_draw_str(canvas, 0, 43, "EEPROM");
    snprintf(str_buf, sizeof(str_buf), "%d%%", (uint8_t)(model->progress_eeprom * 100));
    elements_progress_bar_with_text(canvas, 44, 34, 84, model->progress_eeprom, str_buf);
}

bool avr_isp_reader_view_input(InputEvent* event, void* context) {
    furi_assert(context);
    AvrIspReaderView* instance = context;
    UNUSED(instance);
    if(event->key == InputKeyBack || event->type != InputTypeShort) {
        return false;
    } else if(event->key == InputKeyOk && event->type == InputTypeShort) {
        with_view_model(
            instance->view,
            AvrIspReaderViewModel * model,
            {
                if(model->status == AvrIspReaderViewStatusIDLE) {
                    model->status = AvrIspReaderViewStatusReading;
                    avr_isp_worker_rw_get_dump_start(
                        instance->avr_isp_worker_rw, instance->file_path, instance->file_name);
                }
            },
            false);
    }
    // else if(event->key == InputKeyLeft && event->type == InputTypeShort) {
    //     // with_view_model(
    //     //     instance->view,
    //     //     AvrIspReaderViewModel * model,
    //     //     {
    //     //         icon_animation_start(model->icon);
    //     //         model->detect_chip = false;
    //     //     },
    //     //     false);
    //     // avr_isp_worker_rw_detect_chip(instance->avr_isp_worker_rw);
    // }

    return true;
}

// static void
//     avr_isp_reader_detect_chip_callback(void* context, const char* name, bool detect_chip) {
//     furi_assert(context);
//     AvrIspReaderView* instance = context;
//     with_view_model(
//         instance->view,
//         AvrIspReaderViewModel * model,
//         {
//             model->name_chip = name;
//             model->detect_chip = detect_chip;
//             if(detect_chip) icon_animation_stop(model->icon);
//         },
//         true);
// }

static void avr_isp_reader_callback_status(void* context, AvrIspWorkerRWStatus status) {
    furi_assert(context);
    AvrIspReaderView* instance = context;
    with_view_model(
        instance->view,
        AvrIspReaderViewModel * model,
        {
            switch(status) {
            case AvrIspWorkerRWStatusEndReading:
                model->status = AvrIspReaderViewStatusVerification;
                avr_isp_worker_rw_verification_start(
                    instance->avr_isp_worker_rw, instance->file_path, instance->file_name);
                break;
            case AvrIspWorkerRWStatusEndVerification:
                if(instance->callback)
                    instance->callback(AvrIspCustomEventSceneReadingOk, instance->context);
                break;
            case AvrIspWorkerRWStatusErrorVerification:
                if(instance->callback)
                    instance->callback(AvrIspCustomEventSceneErrorVerification, instance->context);
                break;

            default:
                //AvrIspWorkerRWStatusErrorReading;
                if(instance->callback)
                    instance->callback(AvrIspCustomEventSceneErrorReading, instance->context);
                break;
            }
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
            model->status = AvrIspReaderViewStatusIDLE;
            model->progress_flash = 0.0f;
            model->progress_eeprom = 0.0f;
        },
        true);

    //Start avr_isp_worker_rw
    instance->avr_isp_worker_rw = avr_isp_worker_rw_alloc(instance->context);

    avr_isp_worker_rw_set_callback_status(
        instance->avr_isp_worker_rw, avr_isp_reader_callback_status, instance);

    avr_isp_worker_rw_start(instance->avr_isp_worker_rw);
}

void avr_isp_reader_view_exit(void* context) {
    furi_assert(context);
    AvrIspReaderView* instance = context;
    UNUSED(instance);
    // //Stop avr_isp_worker_rw
    if(avr_isp_worker_rw_is_running(instance->avr_isp_worker_rw)) {
        avr_isp_worker_rw_stop(instance->avr_isp_worker_rw);
    }

    avr_isp_worker_rw_free(instance->avr_isp_worker_rw);

    // with_view_model(
    //     instance->view,
    //     AvrIspReaderViewModel * model,
    //     { icon_animation_stop(model->icon); },
    //     false);
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

    // with_view_model(
    //     instance->view,
    //     AvrIspReaderViewModel * model,
    //     {
    //         model->icon = icon_animation_alloc(&A_ChipLooking_64x64);
    //         view_tie_icon_animation(instance->view, model->icon);
    //         model->detect_chip = false;
    //     },
    //     false);
    return instance;
}

void avr_isp_reader_view_free(AvrIspReaderView* instance) {
    furi_assert(instance);

    // with_view_model(
    //     instance->view,
    //     AvrIspReaderViewModel * model,
    //     { icon_animation_free(model->icon); },
    //     false);
    view_free(instance->view);
    free(instance);
}

View* avr_isp_reader_view_get_view(AvrIspReaderView* instance) {
    furi_assert(instance);
    return instance->view;
}
