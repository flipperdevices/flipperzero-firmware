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
#include <storage/storage.h>
#include <stream/buffered_file_stream.h>

#include <locale/locale.h>

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
    FuriMutex* mutex;
    uint32_t cps, cpm;
    uint32_t line[SCREEN_SIZE_X / 2];
    float coef;
    uint8_t data;
} mutexStruct;

static void draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);

    mutexStruct displayStruct;
    mutexStruct* geigerMutex = ctx;
    furi_mutex_acquire(geigerMutex->mutex, FuriWaitForever);
    memcpy(&displayStruct, geigerMutex, sizeof(mutexStruct));
    furi_mutex_release(geigerMutex->mutex);

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

    furi_hal_pwm_set_params(FuriHalPwmOutputIdLptim2PA4, randomNumber, 50);

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

int32_t flipper_geiger_app(void* p) {
    UNUSED(p);
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

    mutexVal.mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!mutexVal.mutex) {
        furi_message_queue_free(event_queue);
        return 255;
    }

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, &mutexVal);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    furi_hal_gpio_add_int_callback(&gpio_ext_pa7, gpiocallback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    FuriTimer* timer = furi_timer_alloc(clock_tick, FuriTimerTypePeriodic, event_queue);
    furi_timer_start(timer, 1000);

    // ENABLE 5V pin

    // Enable 5v power, multiple attempts to avoid issues with power chip protection false triggering
    uint8_t attempts = 0;
    while(!furi_hal_power_is_otg_enabled() && attempts++ < 5) {
        furi_hal_power_enable_otg();
        furi_delay_ms(10);
    }

    Storage* storage = furi_record_open(RECORD_STORAGE);
    Stream* file_stream = buffered_file_stream_alloc(storage);
    FuriString* dataString = furi_string_alloc();
    uint32_t epoch = 0;
    uint8_t recordData = 0;

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);

    while(1) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, FuriWaitForever);

        uint8_t screenRefresh = 0;

        if(event_status == FuriStatusOk) {
            if(event.type == EventTypeInput) {
                if(event.input.key == InputKeyBack) {
                    break;
                } else if(event.input.key == InputKeyOk && event.input.type == InputTypeShort) {
                    counter = 0;
                    furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);

                    mutexVal.cps = 0;
                    mutexVal.cpm = 0;
                    for(int i = 0; i < SCREEN_SIZE_X / 2; i++) mutexVal.line[i] = 0;

                    screenRefresh = 1;
                    furi_mutex_release(mutexVal.mutex);
                } else if(event.input.key == InputKeyUp && event.input.type == InputTypeLong) {
                    if(recordData == 0) {
                        notification_message(notification, &sequence_set_only_red_255);

                        FuriHalRtcDateTime datetime;
                        furi_hal_rtc_get_datetime(&datetime);

                        char path[64];
                        snprintf(
                            path,
                            sizeof(path),
                            EXT_PATH("/geiger-%.4d-%.2d-%.2d--%.2d-%.2d-%.2d.csv"),
                            datetime.year,
                            datetime.month,
                            datetime.day,
                            datetime.hour,
                            datetime.minute,
                            datetime.second);

                        buffered_file_stream_open(
                            file_stream, path, FSAM_WRITE, FSOM_CREATE_ALWAYS);
                        furi_string_printf(dataString, "epoch,cps\n");
                        stream_write_string(file_stream, dataString);
                        epoch = 0;
                        recordData = 1;
                    } else {
                        buffered_file_stream_close(file_stream);
                        notification_message(notification, &sequence_reset_red);
                        recordData = 0;
                    }
                } else if((event.input.key == InputKeyLeft &&
                           event.input.type == InputTypeShort)) {
                    furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);

                    if(mutexVal.data != 0)
                        mutexVal.data--;
                    else
                        mutexVal.data = 2;

                    screenRefresh = 1;
                    furi_mutex_release(mutexVal.mutex);
                } else if((event.input.key == InputKeyRight &&
                           event.input.type == InputTypeShort)) {
                    furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);

                    if(mutexVal.data != 2)
                        mutexVal.data++;
                    else
                        mutexVal.data = 0;

                    screenRefresh = 1;
                    furi_mutex_release(mutexVal.mutex);
                }
            } else if(event.type == ClockEventTypeTick) {
                furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);

                if(recordData == 1) {
                    furi_string_printf(dataString, "%lu,%lu\n", epoch++, counter);
                    stream_write_string(file_stream, dataString);
                }

                for(int i = 0; i < SCREEN_SIZE_X / 2 - 1; i++)
                    mutexVal.line[SCREEN_SIZE_X / 2 - 1 - i] =
                        mutexVal.line[SCREEN_SIZE_X / 2 - 2 - i];

                mutexVal.line[0] = counter;
                mutexVal.cps = counter;
                counter = 0;

                mutexVal.cpm = mutexVal.line[0];
                uint32_t max = mutexVal.line[0];
                for(int i = 1; i < SCREEN_SIZE_X / 2; i++) {
                    if(i < 60) mutexVal.cpm += mutexVal.line[i];
                    if(mutexVal.line[i] > max) max = mutexVal.line[i];
                }

                if(max > 0)
                    mutexVal.coef = ((float)(SCREEN_SIZE_Y - 15)) / ((float)max);
                else
                    mutexVal.coef = 1;

                screenRefresh = 1;
                furi_mutex_release(mutexVal.mutex);
            } else if(event.type == EventGPIO) {
                counter++;
            }
        }

        if(screenRefresh == 1) view_port_update(view_port);
    }

    if(recordData == 1) {
        buffered_file_stream_close(file_stream);
        notification_message(notification, &sequence_reset_red);
    }

    furi_string_free(dataString);
    furi_record_close(RECORD_NOTIFICATION);
    stream_free(file_stream);
    furi_record_close(RECORD_STORAGE);

    // Disable 5v power
    if(furi_hal_power_is_otg_enabled()) {
        furi_hal_power_disable_otg();
    }

    furi_hal_gpio_disable_int_callback(&gpio_ext_pa7);
    furi_hal_gpio_remove_int_callback(&gpio_ext_pa7);
    furi_hal_pwm_stop(FuriHalPwmOutputIdLptim2PA4);
    furi_hal_gpio_init(&gpio_ext_pa7, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    furi_message_queue_free(event_queue);
    furi_mutex_free(mutexVal.mutex);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_timer_free(timer);
    furi_record_close(RECORD_GUI);

    return 0;
}