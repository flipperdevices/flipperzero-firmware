#include "main_view.h"
#include <furi.h>
#include <furi_hal.h>
#include <gui/elements.h>
#include "../../lightmeter.h"
#include "../../lightmeter_helper.h"
// #include <notification/notification.h>
// #include <notification/notification_messages.h>

#define WORKER_TAG "Main View"

static const int iso_numbers[] = {
    [ISO_100] = 100,
    [ISO_200] = 200,
    [ISO_400] = 400,
    [ISO_800] = 800,
};

// static const int nd_numbers[] = {
//     [ND_0] = 0,
//     [ND_3] = 3,
//     [ND_6] = 6,
//     [ND_9] = 9,
//     [ND_12] = 12,
//     [ND_15] = 15,
//     [ND_18] = 18,
//     [ND_21] = 21,
//     [ND_24] = 24,
//     [ND_27] = 27,
//     [ND_30] = 30,
//     [ND_33] = 33,
//     [ND_36] = 36,
//     [ND_39] = 39,
//     [ND_42] = 42,
//     [ND_45] = 45,
//     [ND_48] = 48,
// };

static const float aperture_numbers[] = {
    [AP_1] = 1.0,
    [AP_1_4] = 1.4,
    [AP_2] = 2.0,
    [AP_2_8] = 2.8,
    [AP_4] = 4.0,
    [AP_5_6] = 5.6,
    [AP_8] = 8,
    [AP_11] = 11,
    [AP_16] = 16,
    [AP_22] = 22,
    [AP_32] = 32,
    [AP_45] = 45,
    [AP_64] = 64,
    [AP_90] = 90,
    [AP_128] = 128,
};

// static const float time_numbers[] =
//     {
//         [TIME_30S] = 30.0,        [TIME_15S] = 15.0,        [TIME_8S] = 8.0,
//         [TIME_4S] = 4.0,          [TIME_2S] = 2.0,          [TIME_1S] = 1.0,
//         [TIME_2] = 1 / 2,       [TIME_4] = 1 / 4,       [TIME_8] = 1 / 8,
//         [TIME_15] = 1 / 15,     [TIME_30] = 1 / 30,     [TIME_60] = 1 / 60,
//         [TIME_125] = 1 / 125,   [TIME_250] = 1 / 250,   [TIME_500] = 1 / 500,
//         [TIME_1000] = 1 / 1000, [TIME_2000] = 1 / 2000, [TIME_4000] = 1 / 4000,
//         [TIME_8000] = 1 / 8000,
// };

struct MainView {
    View* view;
    LightMeterMainViewButtonCallback cb_left;
    void* cb_context;
};

typedef enum {
    FIXED_TIME,
    FIXED_APERTURE,

    /* Know menu Size*/
    MODES_SIZE
} MainViewMode;

typedef struct {
    bool right_pressed;
    bool connected;
    uint8_t recv[2];
    MainViewMode current_mode;
    int iso;
    int nd;
    int aperture;
    int time;
} MainViewModel;

int getLux() {
    uint8_t value = 0x20;
    uint8_t recv[2];
    furi_hal_i2c_acquire(I2C_BUS);
    uint8_t address = 0x23 << 1;
    furi_hal_i2c_trx(I2C_BUS, address, &value, sizeof(value), recv, sizeof(recv), I2C_TIMEOUT);
    furi_hal_i2c_release(I2C_BUS);
    return ((int)recv[0] << 8) | ((int)recv[1]);
}

void lightmeter_main_view_set_left_callback(
    MainView* lightmeter_main_view,
    LightMeterMainViewButtonCallback callback,
    void* context) {
    with_view_model(
        lightmeter_main_view->view,
        MainViewModel * model,
        {
            UNUSED(model);
            lightmeter_main_view->cb_left = callback;
            lightmeter_main_view->cb_context = context;
        },
        true);
}

static void main_view_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    MainViewModel* model = context;

    canvas_clear(canvas);

    char str[10];

    int lux = getLux();
    FURI_LOG_D(WORKER_TAG, "Get Lux: %d", lux);

    float EV = lux2ev((float)lux);
    FURI_LOG_D(WORKER_TAG, "Compute EV: %f", (double)EV);

    float A = aperture_numbers[model->aperture];
    FURI_LOG_D(WORKER_TAG, "Get Aperture: %.1f", (double)A);

    int iso = iso_numbers[model->iso];
    FURI_LOG_D(WORKER_TAG, "Get ISO: %d", iso);

    float A_fix = normalizeAperture(3.4);
    FURI_LOG_D(WORKER_TAG, "Fixed Aperture: %.1f", (double)A_fix);

    float T = 0;

    // uint8_t ndStop =

    // TODO ND correction
    float ISO_ND = iso;

    if(lux > 0) {
        if(model->current_mode == FIXED_APERTURE) {
            T = 100 * pow(A, 2) / (double)ISO_ND / pow(2, EV);
            FURI_LOG_D(WORKER_TAG, "Computed Time: 1/%.0f", 1 / (double)T);
            if(T < 1) {
                FURI_LOG_D(WORKER_TAG, "Normalized Time: 1/%.0f", 1 / (double)normalizeTime(T));
            } else {
                FURI_LOG_D(WORKER_TAG, "Normalized Time: %.0f", (double)normalizeTime(T));
            }
        }
    } else {
        T = 0;
        A = 0;
    }

    // compute values here

    // top row
    // draw line
    canvas_draw_line(canvas, 0, 10, 128, 10);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 0, 0, AlignLeft, AlignTop, "F");

    snprintf(str, sizeof(str), "ISO: %d", iso_numbers[model->iso]);
    canvas_draw_str_aligned(canvas, 20, 0, AlignLeft, AlignTop, str);

    snprintf(str, sizeof(str), "lx: %d", lux);
    canvas_draw_str_aligned(canvas, 80, 0, AlignLeft, AlignTop, str);

    // add f, T values
    if(model->current_mode == FIXED_APERTURE) {
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_icon(canvas, 15, 17, &I_f_10x14);
        if(model->aperture < AP_8) {
            snprintf(str, sizeof(str), "/%.1f", (double)aperture_numbers[model->aperture]);
        } else {
            snprintf(str, sizeof(str), "/%.0f", (double)aperture_numbers[model->aperture]);
        }
        canvas_draw_str_aligned(canvas, 27, 15, AlignLeft, AlignTop, str);
        canvas_draw_icon(canvas, 15, 34, &I_T_10x14);
        if(T < 1) {
            snprintf(str, sizeof(str), ":1/%.0f", 1 / (double)normalizeTime(T));
        } else {
            snprintf(str, sizeof(str), ":%.0f", (double)normalizeTime(T));
        }
        canvas_draw_str_aligned(canvas, 27, 34, AlignLeft, AlignTop, str);

    } else if(model->current_mode == FIXED_TIME) {
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_icon(canvas, 15, 17, &I_f_10x14);
        if(model->aperture < AP_8) {
            snprintf(str, sizeof(str), "/%.1f", (double)aperture_numbers[model->aperture]);
        } else {
            snprintf(str, sizeof(str), "/%.0f", (double)aperture_numbers[model->aperture]);
        }
        canvas_draw_str_aligned(canvas, 27, 15, AlignLeft, AlignTop, str);
        canvas_draw_icon(canvas, 15, 34, &I_T_10x14);
        canvas_draw_str_aligned(canvas, 27, 34, AlignLeft, AlignTop, ":1/50");
    }

    // create buttons (for the future)
    canvas_set_font(canvas, FontSecondary);
    elements_button_left(canvas, "Config");
    // elements_button_right(canvas, "Config");

    // draw EV number
    canvas_draw_line(canvas, 95, 15, 95, 33);
    canvas_draw_line(canvas, 96, 15, 96, 33);
    canvas_set_font(canvas, FontPrimary);
    snprintf(str, sizeof(str), "EV:\n%1.0f", (double)EV);
    elements_multiline_text_aligned(canvas, 100, 15, AlignLeft, AlignTop, str);

    switch(model->current_mode) {
    case FIXED_TIME:
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_str_aligned(canvas, 3, 36, AlignLeft, AlignTop, "*");
        break;

    case FIXED_APERTURE:
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_str_aligned(canvas, 3, 17, AlignLeft, AlignTop, "*");
        break;

    default:
        break;
    }

    // Right
    if(model->right_pressed) {
    }
}

static void main_view_process(MainView* main_view, InputEvent* event) {
    with_view_model(
        main_view->view,
        MainViewModel * model,
        {
            if(event->type == InputTypePress) {
                if(event->key == InputKeyUp) {
                    if(model->current_mode == FIXED_APERTURE) {
                        if(model->aperture < AP_NUM - 1) {
                            model->aperture++;
                            FURI_LOG_D(
                                WORKER_TAG,
                                "Set aperture: %f",
                                (double)aperture_numbers[model->aperture]);
                        }
                    }
                } else if(event->key == InputKeyDown) {
                    if(model->current_mode == FIXED_APERTURE) {
                        if(model->aperture > 0) model->aperture--;
                        FURI_LOG_D(
                            WORKER_TAG,
                            "Set aperture: %f",
                            (double)aperture_numbers[model->aperture]);
                    }
                } else if(event->key == InputKeyLeft) {
                } else if(event->key == InputKeyRight) {
                } else if(event->key == InputKeyOk) {
                    if(model->current_mode == FIXED_TIME) {
                        model->current_mode = FIXED_APERTURE;
                    } else if(model->current_mode == FIXED_APERTURE) {
                        model->current_mode = FIXED_TIME;
                    }
                } else if(event->key == InputKeyBack) {
                }
            } else if(event->type == InputTypeRelease) {
                if(event->key == InputKeyUp) {
                } else if(event->key == InputKeyDown) {
                } else if(event->key == InputKeyLeft) {
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = false;
                } else if(event->key == InputKeyOk) {
                } else if(event->key == InputKeyBack) {
                }
            } else if(event->type == InputTypeShort) {
                if(event->key == InputKeyBack) {
                }
            }
        },
        true);
}

static bool main_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    MainView* main_view = context;
    bool consumed = false;

    if(event->type == InputTypeShort && event->key == InputKeyLeft) {
        if(main_view->cb_left) {
            main_view->cb_left(main_view->cb_context);
        }
        consumed = true;
    } else if(event->type == InputTypeShort && event->key == InputKeyBack) {
    } else {
        main_view_process(main_view, event);
        consumed = true;
    }

    return consumed;
}

MainView* main_view_alloc() {
    MainView* main_view = malloc(sizeof(MainView));
    main_view->view = view_alloc();
    view_set_context(main_view->view, main_view);
    view_allocate_model(main_view->view, ViewModelTypeLocking, sizeof(MainViewModel));
    view_set_draw_callback(main_view->view, main_view_draw_callback);
    view_set_input_callback(main_view->view, main_view_input_callback);

    return main_view;
}

void main_view_free(MainView* main_view) {
    furi_assert(main_view);
    view_free(main_view->view);
    free(main_view);
}

View* main_view_get_view(MainView* main_view) {
    furi_assert(main_view);
    return main_view->view;
}

void main_view_set_data(MainView* main_view, bool connected) {
    furi_assert(main_view);
    with_view_model(
        main_view->view, MainViewModel * model, { model->connected = connected; }, true);
}

void main_view_set_iso(MainView* main_view, int iso) {
    furi_assert(main_view);
    with_view_model(
        main_view->view, MainViewModel * model, { model->iso = iso; }, true);
}

void main_view_set_nd(MainView* main_view, int nd) {
    furi_assert(main_view);
    with_view_model(
        main_view->view, MainViewModel * model, { model->nd = nd; }, true);
}
