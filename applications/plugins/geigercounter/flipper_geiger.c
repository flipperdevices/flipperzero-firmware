// CC0 1.0 Universal (CC0 1.0)
// Public Domain Dedication
// https://github.com/nmrr

#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <furi_hal_random.h>
#include <furi_hal_pwm.h>
#include <furi_hal_power.h>

#define SCREEN_SIZE_X 128
#define SCREEN_SIZE_Y 64

// FOR J305 GEIGER TUBE
#define CONVERSION_FACTOR 0.0081

typedef enum {
    EventTypeInput,
    ClockEventTypeTick,
    EventGPIO,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} EventApp;

typedef struct {
    uint32_t cps, cpm;
    uint32_t line[SCREEN_SIZE_X / 2];
    float coef;
    uint8_t data;
} mutexStruct;

static void draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    mutexStruct displayStruct;
    mutexStruct* geigerMutex = (mutexStruct*)acquire_mutex_block((ValueMutex*)ctx);
    memcpy(&displayStruct, geigerMutex, sizeof(mutexStruct));
    release_mutex((ValueMutex*)ctx, geigerMutex);

    char buffer[32];
    if(displayStruct.data == 0)
        snprintf(
            buffer, sizeof(buffer), "%ld cps - %ld cpm", displayStruct.cps, displayStruct.cpm);
    else if(displayStruct.data == 1)
        snprintf(
            buffer,
            sizeof(buffer),
            "%ld cps - %.2f uSv/h",
            displayStruct.cps,
            ((double)displayStruct.cpm * (double)CONVERSION_FACTOR));
    else
        snprintf(
            buffer,
            sizeof(buffer),
            "%ld cps - %.2f mSv/y",
            displayStruct.cps,
            (((double)displayStruct.cpm * (double)CONVERSION_FACTOR)) * (double)8.76);

    for(int i = 0; i < SCREEN_SIZE_X; i += 2) {
        float Y = SCREEN_SIZE_Y - (displayStruct.line[i / 2] * displayStruct.coef);

        canvas_draw_line(canvas, i, Y, i, SCREEN_SIZE_Y);
        canvas_draw_line(canvas, i + 1, Y, i + 1, SCREEN_SIZE_Y);
    }

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignBottom, buffer);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    EventApp event = {.type = EventTypeInput, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void clock_tick(void* ctx) {
    furi_assert(ctx);

    uint32_t randomNumber = furi_hal_random_get();
    randomNumber &= 0xFFF;
    if(randomNumber == 0) randomNumber = 1;

    furi_hal_pwm_start(FuriHalPwmOutputIdLptim2PA4, randomNumber, 50);

    FuriMessageQueue* queue = ctx;
    EventApp event = {.type = ClockEventTypeTick};
    furi_message_queue_put(queue, &event, 0);
}

static void gpiocallback(void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* queue = ctx;
    EventApp event = {.type = EventGPIO};
    furi_message_queue_put(queue, &event, 0);
}

int32_t flipper_geiger_app() {
    EventApp event;
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(EventApp));

    furi_hal_gpio_init(&gpio_ext_pa7, GpioModeInterruptFall, GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_pwm_start(FuriHalPwmOutputIdLptim2PA4, 5, 50);

    mutexStruct mutexVal;
    mutexVal.cps = 0;
    mutexVal.cpm = 0;
    for(int i = 0; i < SCREEN_SIZE_X / 2; i++) mutexVal.line[i] = 0;
    mutexVal.coef = 1;
    mutexVal.data = 0;

    uint32_t counter = 0;

    ValueMutex state_mutex;
    init_mutex(&state_mutex, &mutexVal, sizeof(mutexVal));

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, &state_mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    furi_hal_gpio_add_int_callback(&gpio_ext_pa7, gpiocallback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    FuriTimer* timer = furi_timer_alloc(clock_tick, FuriTimerTypePeriodic, event_queue);
    furi_timer_start(timer, 1000);

    // ENABLE 5V pin
    furi_hal_power_enable_otg();

    while(1) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, FuriWaitForever);

        uint8_t screenRefresh = 0;

        if(event_status == FuriStatusOk) {
            if(event.type == EventTypeInput) {
                if(event.input.key == InputKeyBack) {
                    break;
                } else if(event.input.key == InputKeyOk && event.input.type == InputTypeShort) {
                    counter = 0;
                    mutexStruct* geigerMutex = (mutexStruct*)acquire_mutex_block(&state_mutex);

                    geigerMutex->cps = 0;
                    geigerMutex->cpm = 0;
                    for(int i = 0; i < SCREEN_SIZE_X / 2; i++) geigerMutex->line[i] = 0;

                    screenRefresh = 1;
                    release_mutex(&state_mutex, geigerMutex);
                } else if((event.input.key == InputKeyLeft &&
                           event.input.type == InputTypeShort)) {
                    mutexStruct* geigerMutex = (mutexStruct*)acquire_mutex_block(&state_mutex);

                    if(geigerMutex->data != 0)
                        geigerMutex->data--;
                    else
                        geigerMutex->data = 2;

                    screenRefresh = 1;
                    release_mutex(&state_mutex, geigerMutex);
                } else if((event.input.key == InputKeyRight &&
                           event.input.type == InputTypeShort)) {
                    mutexStruct* geigerMutex = (mutexStruct*)acquire_mutex_block(&state_mutex);

                    if(geigerMutex->data != 2)
                        geigerMutex->data++;
                    else
                        geigerMutex->data = 0;

                    screenRefresh = 1;
                    release_mutex(&state_mutex, geigerMutex);
                }
            } else if(event.type == ClockEventTypeTick) {
                mutexStruct* geigerMutex = (mutexStruct*)acquire_mutex_block(&state_mutex);

                for(int i = 0; i < SCREEN_SIZE_X / 2 - 1; i++)
                    geigerMutex->line[SCREEN_SIZE_X / 2 - 1 - i] =
                        geigerMutex->line[SCREEN_SIZE_X / 2 - 2 - i];

                geigerMutex->line[0] = counter;
                geigerMutex->cps = counter;
                counter = 0;

                geigerMutex->cpm = geigerMutex->line[0];
                uint32_t max = geigerMutex->line[0];
                for(int i = 1; i < SCREEN_SIZE_X / 2; i++) {
                    if(i < 60) geigerMutex->cpm += geigerMutex->line[i];
                    if(geigerMutex->line[i] > max) max = geigerMutex->line[i];
                }

                if(max > 0)
                    geigerMutex->coef = ((float)(SCREEN_SIZE_Y - 15)) / ((float)max);
                else
                    geigerMutex->coef = 1;

                screenRefresh = 1;
                release_mutex(&state_mutex, geigerMutex);
            } else if(event.type == EventGPIO) {
                counter++;
            }
        }

        if(screenRefresh == 1) view_port_update(view_port);
    }

    furi_hal_power_disable_otg();

    furi_hal_gpio_disable_int_callback(&gpio_ext_pa7);
    furi_hal_gpio_remove_int_callback(&gpio_ext_pa7);
    furi_hal_pwm_stop(FuriHalPwmOutputIdLptim2PA4);

    furi_message_queue_free(event_queue);
    delete_mutex(&state_mutex);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_timer_free(timer);
    furi_record_close(RECORD_GUI);

    return 0;
}