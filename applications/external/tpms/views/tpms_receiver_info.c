#include "tpms_receiver.h"
#include "../tpms_app_i.h"
#include "tpms_icons.h"
#include "../protocols/tpms_generic.h"
#include <input/input.h>
#include <gui/elements.h>
#include <float_tools.h>

struct TPMSReceiverInfo {
    View* view;
    FuriTimer* timer;
};

typedef struct {
    uint32_t curr_ts;
    FuriString* protocol_name;
    TPMSBlockGeneric* generic;
} TPMSReceiverInfoModel;

void tpms_view_receiver_info_update(TPMSReceiverInfo* tpms_receiver_info, FlipperFormat* fff) {
    furi_assert(tpms_receiver_info);
    furi_assert(fff);

    with_view_model(
        tpms_receiver_info->view,
        TPMSReceiverInfoModel * model,
        {
            flipper_format_rewind(fff);
            flipper_format_read_string(fff, "Protocol", model->protocol_name);

            tpms_block_generic_deserialize(model->generic, fff);

            FuriHalRtcDateTime curr_dt;
            furi_hal_rtc_get_datetime(&curr_dt);
            model->curr_ts = furi_hal_rtc_datetime_to_timestamp(&curr_dt);
        },
        true);
}

void tpms_view_receiver_info_draw(Canvas* canvas, TPMSReceiverInfoModel* model) {
    char buffer[64];
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);

    snprintf(
        buffer,
        sizeof(buffer),
        "%s %db",
        furi_string_get_cstr(model->protocol_name),
        model->generic->data_count_bit);
    canvas_draw_str(canvas, 0, 8, buffer);

    snprintf(buffer, sizeof(buffer), "ID: 0x%lX", model->generic->id);
    canvas_draw_str(canvas, 0, 20, buffer);

    if(model->generic->battery_low != TPMS_NO_BATT) {
        snprintf(
            buffer, sizeof(buffer), "Batt: %s", (!model->generic->battery_low ? "ok" : "low"));
        canvas_draw_str_aligned(canvas, 126, 17, AlignRight, AlignCenter, buffer);
    }

    // snprintf(buffer, sizeof(buffer), "Data: 0x%llX", model->generic->data);
    // canvas_draw_str(canvas, 0, 32, buffer);

    elements_bold_rounded_frame(canvas, 0, 38, 127, 25);
    canvas_set_font(canvas, FontPrimary);

    // Temperature
    canvas_draw_icon(canvas, 6, 43, &I_Therm_7x16);

    uint8_t temp_x1 = 0;
    uint8_t temp_x2 = 0;
    if(furi_hal_rtc_get_locale_units() == FuriHalRtcLocaleUnitsMetric) {
        snprintf(buffer, sizeof(buffer), "%2.0f C", (double)model->generic->temperature);
        if(model->generic->temperature < -9.0f) {
            temp_x1 = 42;
            temp_x2 = 33;
        } else {
            temp_x1 = 40;
            temp_x2 = 30;
        }
    } else {
        snprintf(
            buffer,
            sizeof(buffer),
            "%3.0f F",
            (double)locale_celsius_to_fahrenheit(model->generic->temperature));
        if((model->generic->temperature < -27.77f) || (model->generic->temperature > 37.77f)) {
            temp_x1 = 43;
            temp_x2 = 35;
        } else {
            temp_x1 = 41;
            temp_x2 = 33;
        }
    }

    canvas_draw_str_aligned(canvas, temp_x1, 47, AlignRight, AlignTop, buffer);
    canvas_draw_circle(canvas, temp_x2, 46, 1);

    // Pressure
    canvas_draw_icon(canvas, 46, 43, &I_Press_7x16);
    snprintf(buffer, sizeof(buffer), "%2.1fbar", (double)model->generic->pressure);
    canvas_draw_str(canvas, 56, 55, buffer);

    if((int)model->generic->timestamp > 0 && model->curr_ts) {
        int ts_diff = (int)model->curr_ts - (int)model->generic->timestamp;

        canvas_draw_icon(canvas, 92, 46, &I_Timer_11x11);

        if(ts_diff > 60) {
            int tmp_sec = ts_diff;
            int cnt_min = 1;
            for(int i = 1; tmp_sec > 60; i++) {
                tmp_sec = tmp_sec - 60;
                cnt_min = i;
            }

            if(model->curr_ts % 2 == 0) {
                canvas_draw_str_aligned(canvas, 106, 51, AlignLeft, AlignCenter, "Old");
            } else {
                if(cnt_min >= 59) {
                    canvas_draw_str_aligned(canvas, 106, 51, AlignLeft, AlignCenter, "Old");
                } else {
                    snprintf(buffer, sizeof(buffer), "%dm", cnt_min);
                    canvas_draw_str_aligned(canvas, 115, 51, AlignCenter, AlignCenter, buffer);
                }
            }

        } else {
            snprintf(buffer, sizeof(buffer), "%d", ts_diff);
            canvas_draw_str_aligned(canvas, 112, 51, AlignCenter, AlignCenter, buffer);
        }
    }
}

bool tpms_view_receiver_info_input(InputEvent* event, void* context) {
    furi_assert(context);
    //TPMSReceiverInfo* tpms_receiver_info = context;

    if(event->key == InputKeyBack) {
        return false;
    }

    return true;
}

static void tpms_view_receiver_info_enter(void* context) {
    furi_assert(context);
    TPMSReceiverInfo* tpms_receiver_info = context;

    furi_timer_start(tpms_receiver_info->timer, 1000);
}

static void tpms_view_receiver_info_exit(void* context) {
    furi_assert(context);
    TPMSReceiverInfo* tpms_receiver_info = context;

    furi_timer_stop(tpms_receiver_info->timer);

    with_view_model(
        tpms_receiver_info->view,
        TPMSReceiverInfoModel * model,
        { furi_string_reset(model->protocol_name); },
        false);
}

static void tpms_view_receiver_info_timer(void* context) {
    TPMSReceiverInfo* tpms_receiver_info = context;
    // Force redraw
    with_view_model(
        tpms_receiver_info->view,
        TPMSReceiverInfoModel * model,
        {
            FuriHalRtcDateTime curr_dt;
            furi_hal_rtc_get_datetime(&curr_dt);
            model->curr_ts = furi_hal_rtc_datetime_to_timestamp(&curr_dt);
        },
        true);
}

TPMSReceiverInfo* tpms_view_receiver_info_alloc() {
    TPMSReceiverInfo* tpms_receiver_info = malloc(sizeof(TPMSReceiverInfo));

    // View allocation and configuration
    tpms_receiver_info->view = view_alloc();

    view_allocate_model(
        tpms_receiver_info->view, ViewModelTypeLocking, sizeof(TPMSReceiverInfoModel));
    view_set_context(tpms_receiver_info->view, tpms_receiver_info);
    view_set_draw_callback(
        tpms_receiver_info->view, (ViewDrawCallback)tpms_view_receiver_info_draw);
    view_set_input_callback(tpms_receiver_info->view, tpms_view_receiver_info_input);
    view_set_enter_callback(tpms_receiver_info->view, tpms_view_receiver_info_enter);
    view_set_exit_callback(tpms_receiver_info->view, tpms_view_receiver_info_exit);

    with_view_model(
        tpms_receiver_info->view,
        TPMSReceiverInfoModel * model,
        {
            model->generic = malloc(sizeof(TPMSBlockGeneric));
            model->protocol_name = furi_string_alloc();
        },
        true);

    tpms_receiver_info->timer =
        furi_timer_alloc(tpms_view_receiver_info_timer, FuriTimerTypePeriodic, tpms_receiver_info);

    return tpms_receiver_info;
}

void tpms_view_receiver_info_free(TPMSReceiverInfo* tpms_receiver_info) {
    furi_assert(tpms_receiver_info);

    furi_timer_free(tpms_receiver_info->timer);

    with_view_model(
        tpms_receiver_info->view,
        TPMSReceiverInfoModel * model,
        {
            furi_string_free(model->protocol_name);
            free(model->generic);
        },
        false);

    view_free(tpms_receiver_info->view);
    free(tpms_receiver_info);
}

View* tpms_view_receiver_info_get_view(TPMSReceiverInfo* tpms_receiver_info) {
    furi_assert(tpms_receiver_info);
    return tpms_receiver_info->view;
}
