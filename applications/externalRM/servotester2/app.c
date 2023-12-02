#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_pwm.h>
#include <input/input.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <stm32wbxx_ll_tim.h>
#include <stm32wbxx_ll_lptim.h>
#include <stm32wbxx_ll_rcc.h>

// ./fbt.cmd launch_app APPSRC=servotester

#define SCREEN_XRES 128
#define SCREEN_YRES 64
#define MIN_ANGLE 0
#define MAX_ANGLE 180
#define FREQ 50

typedef enum Mode {
    Manual = 1,
    Center = 2,
    Sweep = 3,
} Mode;

typedef struct App {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;

    int running;
    Mode mode;
    uint8_t angle;

    FuriHalPwmOutputId ch_prev;
    FuriHalPwmOutputId ch;
} App;

void input_callback(InputEvent* input_event, void* ctx) {
    App* app = ctx;
    furi_message_queue_put(app->event_queue, input_event, FuriWaitForever);
}

uint32_t angle_to_compare(uint8_t angle) {
    /*
        1%:   freq_div: 1280000, prescaler: 19, period: 64000, compare: 640
        3%:   freq_div: 1280000, prescaler: 19, period: 64000, compare: 1920
        5%:   freq_div: 1280000, prescaler: 19, period: 64000, compare: 3200
        7.5%: freq_div: 1280000, prescaler: 19, period: 64000, compare: 4800
        7%:   freq_div: 1280000, prescaler: 19, period: 64000, compare: 4480
        10%:  freq_div: 1280000, prescaler: 19, period: 64000, compare: 6400
        13%:  freq_div: 1280000, prescaler: 19, period: 64000, compare: 8320
        15%:  freq_div: 1280000, prescaler: 19, period: 64000, compare: 9600
        20%:  freq_div: 1280000, prescaler: 19, period: 64000, compare: 12800
    */

    uint32_t min_compare = 1920;
    uint32_t max_compare = 8320;

    if(angle == MIN_ANGLE) return min_compare;
    if(angle == MAX_ANGLE) return max_compare;

    return min_compare + floor(((float)angle / (float)MAX_ANGLE) * (max_compare - min_compare));
}

void render_callback(Canvas* const canvas, void* ctx) {
    App* app = ctx;

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 0, 0, SCREEN_XRES - 1, SCREEN_YRES - 1);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);

    uint8_t mode_base_x = 20;
    canvas_draw_str(canvas, mode_base_x, 10, "Mode");
    canvas_draw_str(canvas, mode_base_x, 30, "Manual");
    canvas_draw_str(canvas, mode_base_x, 45, "Center");
    canvas_draw_str(canvas, mode_base_x, 60, "Sweep");

    uint8_t selector_base_x = 5;
    uint8_t r = 4;
    uint8_t correction = r;
    if(app->mode == Manual) {
        canvas_draw_disc(canvas, selector_base_x, 30 - correction, r);
    } else {
        canvas_draw_circle(canvas, selector_base_x, 30 - correction, r);
    }

    if(app->mode == Center) {
        canvas_draw_disc(canvas, selector_base_x, 45 - correction, r);
    } else {
        canvas_draw_circle(canvas, selector_base_x, 45 - correction, r);
    }

    if(app->mode == Sweep) {
        canvas_draw_disc(canvas, selector_base_x, 60 - correction, r);
    } else {
        canvas_draw_circle(canvas, selector_base_x, 60 - correction, r);
    }

    uint8_t angle_base_x = 70;
    canvas_draw_str(canvas, angle_base_x, 10, "Angle");
    canvas_draw_circle(canvas, angle_base_x + 18, 21, 2);

    char angle_str[32];
    snprintf(angle_str, sizeof(angle_str), "%3u", app->angle);
    canvas_draw_str(canvas, angle_base_x, 30, angle_str);
}

App* app_alloc() {
    App* app = malloc(sizeof(App));

    app->gui = furi_record_open(RECORD_GUI);
    app->view_port = view_port_alloc();
    view_port_draw_callback_set(app->view_port, render_callback, app);
    view_port_input_callback_set(app->view_port, input_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    app->running = 1;
    app->mode = Manual;
    app->ch = FuriHalPwmOutputIdTim1PA7; // A7
    app->angle = 0;

    return app;
}

void app_free(App* app) {
    furi_assert(app);

    furi_hal_pwm_stop(app->ch);

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    furi_message_queue_free(app->event_queue);
    app->gui = NULL;

    free(app);
}

void update_state(App* app, InputKey key) {
    if(key == InputKeyUp) {
        if(app->mode == Manual) {
            app->angle = MIN_ANGLE;
            app->mode = Sweep;
        } else if(app->mode == Center) {
            app->mode = Manual;
        } else if(app->mode == Sweep) {
            app->mode = Center;
        }
    }

    if(key == InputKeyDown) {
        if(app->mode == Manual) {
            app->mode = Center;
        } else if(app->mode == Center) {
            app->angle = MIN_ANGLE;
            app->mode = Sweep;
        } else if(app->mode == Sweep) {
            app->mode = Manual;
        }
    }

    if(app->mode == Manual) {
        if(key == InputKeyRight) {
            if(app->angle < MAX_ANGLE) {
                app->angle = app->angle + 10;
            }
        }

        if(key == InputKeyLeft) {
            if(app->angle > MIN_ANGLE) {
                app->angle = app->angle - 10;
            }
        }
    }
}

void custom_pwm_set_params(uint32_t freq, uint32_t compare) {
    furi_assert(freq > 0);
    uint32_t freq_div = 64000000LU / freq;

    uint32_t prescaler = freq_div / 0x10000LU;
    uint32_t period = freq_div / (prescaler + 1);
    // uint32_t compare = period * duty / 100;

    LL_TIM_SetPrescaler(TIM1, prescaler);
    LL_TIM_SetAutoReload(TIM1, period - 1);
    LL_TIM_OC_SetCompareCH1(TIM1, compare);
}

void tick(void* ctx) {
    App* app = ctx;

    if(!app->running) return;
    if(app->mode != Sweep) return;

    if(app->angle == MAX_ANGLE) {
        app->angle = MIN_ANGLE;
    } else if(app->angle == MIN_ANGLE) {
        app->angle = MAX_ANGLE;
    }

    custom_pwm_set_params(FREQ, angle_to_compare(app->angle));
}

int32_t servotester_app_entry(void* p) {
    UNUSED(p);

    App* app = app_alloc();

    furi_hal_pwm_start(app->ch, FREQ, angle_to_compare(app->angle));
    custom_pwm_set_params(FREQ, angle_to_compare(app->angle));

    FuriTimer* timer = furi_timer_alloc(tick, FuriTimerTypePeriodic, app);
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 1);

    InputEvent input;
    while(app->running) {
        FuriStatus qstat = furi_message_queue_get(app->event_queue, &input, 100);
        if(qstat == FuriStatusOk) {
            if(input.key == InputKeyBack) {
                app->running = 0;
                // TODO: handle InputTypeLong
            } else if(input.type == InputTypePress) {
                update_state(app, input.key);

                if(!app->running) {
                    break;
                }

                switch(app->mode) {
                case Manual:
                    custom_pwm_set_params(FREQ, angle_to_compare(app->angle));
                    break;
                case Center:
                    app->angle = 90;
                    custom_pwm_set_params(FREQ, angle_to_compare(app->angle));
                    break;
                case Sweep:
                    // handled in ticks
                    break;
                }
            }
        }
        view_port_update(app->view_port);
    }

    furi_timer_free(timer);
    app_free(app);

    return 0;
}