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
    uint32_t line[SCREEN_SIZE_X];
    float coef;
    uint8_t data;
    uint8_t zoom;
    uint8_t newLinePosition;
    uint8_t version;
} mutexStruct;

static void draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);

    mutexStruct* mutexVal = ctx;
    mutexStruct mutexDraw;
    furi_mutex_acquire(mutexVal->mutex, FuriWaitForever);
    memcpy(&mutexDraw, mutexVal, sizeof(mutexStruct));
    furi_mutex_release(mutexVal->mutex);

    if(mutexDraw.version == 0) {
        char buffer[32];
        if(mutexDraw.data == 0)
            snprintf(buffer, sizeof(buffer), "%ld cps - %ld cpm", mutexDraw.cps, mutexDraw.cpm);
        else if(mutexDraw.data == 1)
            snprintf(
                buffer,
                sizeof(buffer),
                "%ld cps - %.2f uSv/h",
                mutexDraw.cps,
                ((double)mutexDraw.cpm * (double)CONVERSION_FACTOR));
        else if(mutexDraw.data == 2)
            snprintf(
                buffer,
                sizeof(buffer),
                "%ld cps - %.2f mSv/y",
                mutexDraw.cps,
                (((double)mutexDraw.cpm * (double)CONVERSION_FACTOR)) * (double)8.76);
        else if(mutexDraw.data == 3)
            snprintf(
                buffer,
                sizeof(buffer),
                "%ld cps - %.4f Rad/h",
                mutexDraw.cps,
                ((double)mutexDraw.cpm * (double)CONVERSION_FACTOR) / (double)10000);
        else if(mutexDraw.data == 4)
            snprintf(
                buffer,
                sizeof(buffer),
                "%ld cps - %.2f mR/h",
                mutexDraw.cps,
                ((double)mutexDraw.cpm * (double)CONVERSION_FACTOR) / (double)10);
        else
            snprintf(
                buffer,
                sizeof(buffer),
                "%ld cps - %.2f uR/h",
                mutexDraw.cps,
                ((double)mutexDraw.cpm * (double)CONVERSION_FACTOR) * (double)100);

        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignBottom, buffer);

        uint8_t linePosition = mutexDraw.newLinePosition;

        if(mutexDraw.zoom == 0) {
            for(int i = 0; i < SCREEN_SIZE_X; i += 8) {
                if(linePosition != 0)
                    linePosition--;
                else
                    linePosition = SCREEN_SIZE_X - 1;

                float Y = SCREEN_SIZE_Y - (mutexDraw.line[linePosition] * mutexDraw.coef);
                for(int j = 0; j < 8; j++)
                    canvas_draw_line(canvas, i + j, Y, i + j, SCREEN_SIZE_Y);
            }
        } else if(mutexDraw.zoom == 1) {
            for(int i = 0; i < SCREEN_SIZE_X; i += 4) {
                if(linePosition != 0)
                    linePosition--;
                else
                    linePosition = SCREEN_SIZE_X - 1;

                float Y = SCREEN_SIZE_Y - (mutexDraw.line[linePosition] * mutexDraw.coef);
                for(int j = 0; j < 4; j++)
                    canvas_draw_line(canvas, i + j, Y, i + j, SCREEN_SIZE_Y);
            }
        } else if(mutexDraw.zoom == 2) {
            for(int i = 0; i < SCREEN_SIZE_X; i += 2) {
                if(linePosition != 0)
                    linePosition--;
                else
                    linePosition = SCREEN_SIZE_X - 1;

                float Y = SCREEN_SIZE_Y - (mutexDraw.line[linePosition] * mutexDraw.coef);
                for(int j = 0; j < 2; j++)
                    canvas_draw_line(canvas, i + j, Y, i + j, SCREEN_SIZE_Y);
            }
        } else if(mutexDraw.zoom == 3) {
            for(int i = 0; i < SCREEN_SIZE_X; i++) {
                if(linePosition != 0)
                    linePosition--;
                else
                    linePosition = SCREEN_SIZE_X - 1;

                float Y = SCREEN_SIZE_Y - (mutexDraw.line[linePosition] * mutexDraw.coef);
                canvas_draw_line(canvas, i, Y, i, SCREEN_SIZE_Y);
            }
        }
    } else {
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignBottom, "Geiger Counter");
        canvas_draw_str_aligned(canvas, 64, 20, AlignCenter, AlignBottom, "Version 20230806");
        canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignBottom, "github.com/nmrr");
    }
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

int32_t flipper_geiger_app() {
    EventApp event;
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(EventApp));

    furi_hal_gpio_init(&gpio_ext_pa7, GpioModeInterruptFall, GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_pwm_start(FuriHalPwmOutputIdLptim2PA4, 5, 50);

    mutexStruct mutexVal;
    mutexVal.cps = 0;
    mutexVal.cpm = 0;
    for(int i = 0; i < SCREEN_SIZE_X; i++) mutexVal.line[i] = 0;
    mutexVal.coef = 1;
    mutexVal.data = 0;
    mutexVal.zoom = 2;
    mutexVal.newLinePosition = 0;
    mutexVal.version = 0;

    uint32_t counter = 0;

    mutexVal.mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!mutexVal.mutex) {
        furi_message_queue_free(event_queue);
        return 255;
    }

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, &mutexVal.mutex);
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
                if(event.input.key == InputKeyBack && event.input.type == InputTypeLong) {
                    break;
                } else if(event.input.key == InputKeyOk && event.input.type == InputTypeLong) {
                    counter = 0;
                    furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);

                    mutexVal.cps = 0;
                    mutexVal.cpm = 0;
                    for(uint8_t i = 0; i < SCREEN_SIZE_X; i++) mutexVal.line[i] = 0;
                    mutexVal.newLinePosition = 0;

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
                        mutexVal.data = 5;

                    screenRefresh = 1;
                    furi_mutex_release(mutexVal.mutex);
                } else if((event.input.key == InputKeyRight &&
                           event.input.type == InputTypeShort)) {
                    furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);

                    if(mutexVal.data != 5)
                        mutexVal.data++;
                    else
                        mutexVal.data = 0;

                    screenRefresh = 1;
                    furi_mutex_release(mutexVal.mutex);
                } else if((event.input.key == InputKeyUp && event.input.type == InputTypeShort)) {
                    furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);
                    if(mutexVal.zoom != 0) mutexVal.zoom--;

                    screenRefresh = 1;
                    furi_mutex_release(mutexVal.mutex);

                } else if((event.input.key == InputKeyDown &&
                           event.input.type == InputTypeShort)) {
                    furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);
                    if(mutexVal.zoom != 3) mutexVal.zoom++;

                    screenRefresh = 1;
                    furi_mutex_release(mutexVal.mutex);
                } else if((event.input.key == InputKeyDown && event.input.type == InputTypeLong)) {
                    furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);
                    if(mutexVal.version == 0)
                        mutexVal.version = 1;
                    else
                        mutexVal.version = 0;

                    screenRefresh = 1;
                    furi_mutex_release(mutexVal.mutex);
                }
            } else if(event.type == ClockEventTypeTick) {
                if(recordData == 1) {
                    furi_string_printf(dataString, "%lu,%lu\n", epoch++, counter);
                    stream_write_string(file_stream, dataString);
                }

                furi_mutex_acquire(mutexVal.mutex, FuriWaitForever);

                mutexVal.line[mutexVal.newLinePosition] = counter;
                mutexVal.cps = counter;
                counter = 0;

                mutexVal.cpm = mutexVal.line[mutexVal.newLinePosition];
                uint32_t max = mutexVal.line[mutexVal.newLinePosition];
                uint8_t linePosition = mutexVal.newLinePosition;

                for(int i = 1; i < SCREEN_SIZE_X; i++) {
                    if(linePosition != 0)
                        linePosition--;
                    else
                        linePosition = SCREEN_SIZE_X - 1;

                    if(i < 60) mutexVal.cpm += mutexVal.line[linePosition];
                    if(mutexVal.line[linePosition] > max) max = mutexVal.line[linePosition];
                }

                if(max > 0)
                    mutexVal.coef = ((float)(SCREEN_SIZE_Y - 15)) / ((float)max);
                else
                    mutexVal.coef = 1;

                if(mutexVal.newLinePosition != SCREEN_SIZE_X - 1)
                    mutexVal.newLinePosition++;
                else
                    mutexVal.newLinePosition = 0;

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