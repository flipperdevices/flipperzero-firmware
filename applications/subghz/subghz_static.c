#include "subghz_static.h"
#include "subghz_i.h"

#include <math.h>
#include <furi.h>
#include <api-hal.h>
#include <input/input.h>

struct SubghzTestBasic {
    View* view;
};

typedef enum {
    SubghzStaticStatusRx,
    SubghzStaticStatusTx,
} SubghzStaticStatus;

typedef struct {
    uint8_t frequency;
    uint32_t real_frequency;
    ApiHalSubGhzPath path;
    float rssi;
    SubghzStaticStatus status;
} SubghzStaticModel;

void subghz_static_draw(Canvas* canvas, SubghzStaticModel* model) {
    char buffer[64];

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 12, "CC1101 Basic Test");

    canvas_set_font(canvas, FontSecondary);
    // Frequency
    snprintf(
        buffer,
        sizeof(buffer),
        "Freq: %03ld.%03ld.%03ld Hz",
        model->real_frequency / 1000000 % 1000,
        model->real_frequency / 1000 % 1000,
        model->real_frequency % 1000);
    canvas_draw_str(canvas, 2, 24, buffer);
    // Path
    char* path_name = "Unknown";
    if(model->path == ApiHalSubGhzPathIsolate) {
        path_name = "isolate";
    } else if(model->path == ApiHalSubGhzPath1) {
        path_name = "433MHz";
    } else if(model->path == ApiHalSubGhzPath2) {
        path_name = "315MHz";
    } else if(model->path == ApiHalSubGhzPath3) {
        path_name = "868MHz";
    }
    snprintf(buffer, sizeof(buffer), "Path: %d - %s", model->path, path_name);
    canvas_draw_str(canvas, 2, 36, buffer);
    if(model->status == SubghzStaticStatusRx) {
        snprintf(
            buffer,
            sizeof(buffer),
            "RSSI: %ld.%ld dBm",
            (int32_t)(model->rssi),
            (int32_t)fabs(model->rssi * 10) % 10);
        canvas_draw_str(canvas, 2, 48, buffer);
    } else {
        canvas_draw_str(canvas, 2, 48, "TX");
    }
}

bool subghz_static_input(InputEvent* event, void* context) {
    furi_assert(context);
    SubghzTestBasic* subghz_static = context;

    if(event->key == InputKeyBack) {
        return false;
    }

    with_view_model(
        subghz_static->view, (SubghzStaticModel * model) {
            api_hal_subghz_idle();

            if(event->type == InputTypeShort) {
                if(event->key == InputKeyLeft) {
                    if(model->frequency > 0) model->frequency--;
                } else if(event->key == InputKeyRight) {
                    if(model->frequency < subghz_frequencies_count - 1) model->frequency++;
                } else if(event->key == InputKeyDown) {
                    if(model->path > 0) model->path--;
                } else if(event->key == InputKeyUp) {
                    if(model->path < ApiHalSubGhzPath3) model->path++;
                }

                model->real_frequency =
                    api_hal_subghz_set_frequency(subghz_frequencies[model->frequency]);
                api_hal_subghz_set_path(model->path);
            }

            if(event->key == InputKeyOk) {
                if(event->type == InputTypePress) {
                    model->status = SubghzStaticStatusTx;
                } else if(event->type == InputTypeRelease) {
                    model->status = SubghzStaticStatusRx;
                }
            }

            if(model->status == SubghzStaticStatusRx) {
                gpio_init(&cc1101_g0_gpio, GpioModeInput);
                api_hal_subghz_rx();
            } else {
                gpio_init(&cc1101_g0_gpio, GpioModeOutputPushPull);
                gpio_write(&cc1101_g0_gpio, false);
                api_hal_subghz_tx();
            }

            return true;
        });

    return true;
}

void subghz_static_enter(void* context) {
    furi_assert(context);
    SubghzTestBasic* subghz_static = context;

    api_hal_subghz_reset();
    api_hal_subghz_load_preset(ApiHalSubGhzPresetOokAsync);

    gpio_init(&cc1101_g0_gpio, GpioModeInput);

    with_view_model(
        subghz_static->view, (SubghzStaticModel * model) {
            model->frequency = 4; // 433
            model->real_frequency =
                api_hal_subghz_set_frequency(subghz_frequencies[model->frequency]);
            model->path = ApiHalSubGhzPathIsolate; // isolate
            model->rssi = 0.0f;
            model->status = SubghzStaticStatusRx;
            return true;
        });

    api_hal_subghz_rx();
}

void subghz_static_exit(void* context) {
    furi_assert(context);
    SubghzTestBasic* subghz_static = context;

    // Reinitialize IC to default state
    api_hal_subghz_init();
}

uint32_t subghz_static_back(void* context) {
    return SubGhzViewMenu;
}

SubghzTestBasic* subghz_static_alloc() {
    SubghzTestBasic* subghz_static = furi_alloc(sizeof(SubghzTestBasic));

    // View allocation and configuration
    subghz_static->view = view_alloc();
    view_allocate_model(
        subghz_static->view, ViewModelTypeLockFree, sizeof(SubghzStaticModel));
    view_set_context(subghz_static->view, subghz_static);
    view_set_draw_callback(subghz_static->view, (ViewDrawCallback)subghz_static_draw);
    view_set_input_callback(subghz_static->view, subghz_static_input);
    view_set_enter_callback(subghz_static->view, subghz_static_enter);
    view_set_exit_callback(subghz_static->view, subghz_static_exit);
    view_set_previous_callback(subghz_static->view, subghz_static_back);

    return subghz_static;
}

void subghz_static_free(SubghzTestBasic* subghz_static) {
    furi_assert(subghz_static);
    view_free(subghz_static->view);
    free(subghz_static);
}

View* subghz_static_get_view(SubghzTestBasic* subghz_static) {
    furi_assert(subghz_static);
    return subghz_static->view;
}
