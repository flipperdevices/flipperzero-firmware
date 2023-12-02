// CC0 1.0 Universal (CC0 1.0)
// Public Domain Dedication
// https://github.com/nmrr

#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <furi_hal_power.h>
#include <locale/locale.h>

#define SCREEN_SIZE_X 128
#define SCREEN_SIZE_Y 64

typedef enum {
    EventTypeInput,
    ClockEventTypeTick,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} EventApp;

typedef struct {
    FuriMutex* mutex;
    uint32_t timer;
    uint8_t minute;
    uint8_t hour;
} mutexStruct;

static void draw_callback(Canvas* canvas, void* ctx) {
    mutexStruct* mutexVal = ctx;
    mutexStruct mutexDraw;
    furi_mutex_acquire(mutexVal->mutex, FuriWaitForever);
    memcpy(&mutexDraw, mutexVal, sizeof(mutexStruct));
    furi_mutex_release(mutexVal->mutex);

    char buffer[16];
    canvas_set_font(canvas, FontBigNumbers);
    snprintf(
        buffer,
        sizeof(buffer),
        "%02u:%02u:%02lu",
        mutexDraw.hour,
        mutexDraw.minute,
        mutexDraw.timer / 64000000);
    canvas_draw_str_aligned(canvas, 5, SCREEN_SIZE_Y / 2 + 5, AlignLeft, AlignBottom, buffer);

    canvas_set_font(canvas, FontPrimary);
    snprintf(buffer, sizeof(buffer), "%03lu", (mutexDraw.timer % 64000000) / 64000);
    canvas_draw_str_aligned(
        canvas, SCREEN_SIZE_X - 5, SCREEN_SIZE_Y / 2, AlignRight, AlignBottom, buffer);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    EventApp event = {.type = EventTypeInput, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void clock_tick(void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* queue = ctx;
    EventApp event = {.type = ClockEventTypeTick};
    furi_message_queue_put(queue, &event, 0);
}

int32_t flipper_chronometer_app() {
    // 64 MHz
    furi_hal_bus_enable(FuriHalBusTIM2);
    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetClockDivision(TIM2, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetPrescaler(TIM2, 0);
    // return to 0 after 1 min
    LL_TIM_SetAutoReload(TIM2, 3839999999);
    LL_TIM_SetCounter(TIM2, 0);

    EventApp event;
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(EventApp));

    mutexStruct mutexVal;
    mutexVal.minute = 0;
    mutexVal.timer = 0;
    mutexVal.hour = 0;

    uint32_t previousTimer = 0;

    mutexVal.mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!mutexVal.mutex) {
        furi_message_queue_free(event_queue);
        return 255;
    }

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, &mutexVal.mutex);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    FuriTimer* timer = furi_timer_alloc(clock_tick, FuriTimerTypePeriodic, event_queue);

    uint8_t enableChrono = 0;

    while(1) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, FuriWaitForever);

        uint8_t screenRefresh = 0;

        if(event_status == FuriStatusOk) {
            if(event.type == EventTypeInput) {
                if(event.input.key == InputKeyBack && event.input.type == InputTypeLong) {
                    break;
                } else if(event.input.key == InputKeyOk && event.input.type == InputTypeShort) {
                    if(enableChrono == 1) {
                        LL_TIM_DisableCounter(TIM2);
                        furi_timer_stop(timer);
                        enableChrono = 0;
                    } else {
                        LL_TIM_EnableCounter(TIM2);
                        furi_timer_start(
                            timer,
                            43); // better to use prime number as timer for millisecond refresh effect
                        enableChrono = 1;
                    }

                    screenRefresh = 1;
                } else if(
                    enableChrono == 0 && event.input.key == InputKeyOk &&
                    event.input.type == InputTypeLong) {
                    LL_TIM_SetCounter(TIM2, 0);
                    furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);
                    mutexVal.minute = 0;
                    mutexVal.hour = 0;
                    mutexVal.timer = 0;
                    furi_mutex_release(mutexVal.mutex);

                    screenRefresh = 1;
                }
            } else if(event.type == ClockEventTypeTick) {
                screenRefresh = 1;
            }
        }

        if(screenRefresh == 1) {
            furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);
            previousTimer = mutexVal.timer;
            mutexVal.timer = TIM2->CNT;
            if(mutexVal.timer < previousTimer) {
                if(mutexVal.minute < 59)
                    mutexVal.minute++;
                else {
                    if(mutexVal.hour < 99) {
                        mutexVal.hour++;
                        mutexVal.minute = 0;
                    } else {
                        LL_TIM_DisableCounter(TIM2);
                        mutexVal.timer = 3839999999;
                        furi_timer_stop(timer);
                        enableChrono = 0;
                    }
                }
            }
            furi_mutex_release(mutexVal.mutex);

            view_port_update(view_port);
        }
    }

    LL_TIM_DisableCounter(TIM2);
    furi_hal_bus_disable(FuriHalBusTIM2);
    furi_message_queue_free(event_queue);
    furi_mutex_free(mutexVal.mutex);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_timer_free(timer);
    furi_record_close(RECORD_GUI);

    return 0;
}