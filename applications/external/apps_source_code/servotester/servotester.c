#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_clock.h>
#include <furi_hal_pwm.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <stdio.h>

#define DEFAULT_FREQ 50
#define DEFAULT_DUTY 1

uint16_t pWidth = 1500;
int8_t dir = 1;

enum Modes {
    Center,
    Manual,
    Auto,
};

const char* const modes_text[Auto + 1] = {
    "Center",
    "Manual",
    " Auto",
};

uint8_t mode = Manual;

typedef enum {
    EventTypeTick,
    EventTypeInput,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} ServoTesterEvent;

static void servotester_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    char temp_str[36];

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);

    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_draw_str(canvas, 35, 10, "Servo tester");
    canvas_draw_line(canvas, 14, 30, 114, 30);
    canvas_draw_line(canvas, 14, 30, 14, 20);
    canvas_draw_line(canvas, 114, 30, 114, 20);

    canvas_draw_frame(canvas, (pWidth - 1000) / 10.2 + 14, 20, 3, 10);

    snprintf(temp_str, sizeof(temp_str), "%i us", pWidth);

    canvas_draw_str(canvas, 50, 40, temp_str);

    canvas_draw_str(canvas, 50, 50, modes_text[mode]);
}

static void servotester_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    ServoTesterEvent event = {.type = EventTypeInput, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void servotester_timer_callback(FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    ServoTesterEvent event = {.type = EventTypeTick};
    furi_message_queue_put(event_queue, &event, 0);
}

void servotester_set_servo_pwm(uint32_t freq, uint32_t compare) {
    uint32_t freq_div = 64000000LU / freq;
    uint32_t prescaler = freq_div / 0x10000LU;
    uint32_t period = freq_div / (prescaler + 1);

    LL_TIM_SetPrescaler(TIM1, prescaler);
    LL_TIM_SetAutoReload(TIM1, period - 1);
    LL_TIM_OC_SetCompareCH1(TIM1, compare);
}

void servotester_update_pwm() {
    servotester_set_servo_pwm(DEFAULT_FREQ, pWidth * 3.2);
}

int32_t servotester_app(void* p) {
    UNUSED(p);

    ServoTesterEvent event;

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(ServoTesterEvent));

    ViewPort* view_port = view_port_alloc();

    // callbacks init
    view_port_draw_callback_set(view_port, servotester_draw_callback, NULL);
    view_port_input_callback_set(view_port, servotester_input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Timer for automatic mode
    FuriTimer* timer =
        furi_timer_alloc(servotester_timer_callback, FuriTimerTypePeriodic, event_queue);
    furi_timer_start(timer, 5);

    //GPIO init
    furi_hal_power_enable_otg(); // Turn 5V
    furi_hal_pwm_start(FuriHalPwmOutputIdTim1PA7, 50, 4); // Init Tim1
    servotester_set_servo_pwm(DEFAULT_FREQ, pWidth * 3.2); // set our PWM

    while(1) {
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);
        if(event.type == EventTypeInput) {
            if(event.input.key == InputKeyBack) {
                break;
            } else if(event.input.key == InputKeyOk) {
                if(event.input.type == InputTypeRelease) {
                    if(mode == Auto) {
                        mode = Center;
                    } else {
                        mode++;
                    }

                    if(mode == Center) {
                        pWidth = 1500;
                        servotester_update_pwm();
                    }
                }
            } else if(event.input.key == InputKeyLeft) {
                if(pWidth > 1000) pWidth--;
                servotester_update_pwm();
            } else if(event.input.key == InputKeyRight) {
                if(pWidth < 2000) pWidth++;
                servotester_update_pwm();
            } else if(event.input.key == InputKeyDown) {
                if(pWidth >= 1010) pWidth -= 10;
                servotester_update_pwm();
            } else if(event.input.key == InputKeyUp) {
                if(pWidth <= 1990) pWidth += 10;
                servotester_update_pwm();
            }
            view_port_update(view_port);
        } else if(event.type == EventTypeTick) {
            if(mode == Auto) {
                pWidth += dir;
                if(pWidth > 1990 || pWidth < 1010) {
                    dir = dir * -1;
                }
                servotester_update_pwm();
            }
            view_port_update(view_port);
        }
        view_port_update(view_port);
    }

    furi_hal_power_disable_otg();
    furi_hal_pwm_stop(FuriHalPwmOutputIdTim1PA7);

    furi_timer_free(timer);
    furi_message_queue_free(event_queue);

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);

    return 0;
}