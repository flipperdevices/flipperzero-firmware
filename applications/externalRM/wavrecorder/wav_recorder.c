#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
// #include <wav_recorder_icons.h>
#include "furi_hal_adc.h"

#define TAG "WavRecorder"

typedef enum {
    EventTypeTick,
    EventTypeInput,
} EventType;

typedef struct {
    EventType type;
    union {
        InputEvent input;
    };
} RecorderEvent;

typedef struct {
    FuriMessageQueue* event_queue;
    ViewPort* view_port;
    Gui* gui;
    FuriMutex* mutex;
    uint32_t sample_max;
    uint32_t sample_min;
} RecorderApp;

static void wav_recorder_draw(Canvas* canvas, void* context) {
    furi_assert(context);
    RecorderApp* app = context;
    furi_mutex_acquire(app->mutex, FuriWaitForever);

    char str[8];

    snprintf(str, 64, "%lu", app->sample_min);
    canvas_draw_str(canvas, 10, 10, "Min:");
    canvas_draw_str(canvas, 40, 10, str);

    snprintf(str, 64, "%lu", app->sample_max);
    canvas_draw_str(canvas, 10, 20, "Max:");
    canvas_draw_str(canvas, 40, 20, str);

    furi_mutex_release(app->mutex);
}

static void wav_recorder_input(InputEvent* event, void* context) {
    furi_assert(context);
    FuriMessageQueue* event_queue = context;
    RecorderEvent app_event = {.type = EventTypeInput, .input = *event};
    furi_message_queue_put(event_queue, &app_event, FuriWaitForever);
}

static void wav_recorder_tick(void* context) {
    furi_assert(context);
    FuriMessageQueue* event_queue = context;
    RecorderEvent app_event = {.type = EventTypeTick};
    furi_message_queue_put(event_queue, &app_event, 0);
}

static void wav_recorder_adc_init() {
    // PC3 is ADC1_IN4
    furi_hal_gpio_init(&gpio_ext_pc3, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    FURI_LOG_I(TAG, "Gpio Set OK");

    furi_hal_adc_init();
    FURI_LOG_I(TAG, "ADC Init OK");

    furi_hal_adc_set_vref(FuriHalVref2500);
    FURI_LOG_I(TAG, "Vref Set OK");

    furi_hal_adc_set_single_channel(FuriHalAdcChannel4);
    FURI_LOG_I(TAG, "ADC Set Channel OK");

    furi_hal_adc_enable();
    FURI_LOG_I(TAG, "ADC Enable OK");
}

static void wav_recorder_adc_deinit() {
    furi_hal_adc_disable();
    FURI_LOG_I(TAG, "ADC Disable OK");

    furi_hal_adc_deinit();
    FURI_LOG_I(TAG, "ADC Deinit OK");
}

static RecorderApp* wav_recorder_alloc() {
    RecorderApp* app = malloc(sizeof(RecorderApp));
    app->sample_max = 0;
    app->sample_min = 4096;

    app->mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    app->event_queue = furi_message_queue_alloc(8, sizeof(RecorderEvent));

    app->view_port = view_port_alloc();
    view_port_draw_callback_set(app->view_port, wav_recorder_draw, app);
    view_port_input_callback_set(app->view_port, wav_recorder_input, app->event_queue);

    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    return app;
}

static void wav_recorder_free(RecorderApp* app) {
    gui_remove_view_port(app->gui, app->view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(app->view_port);

    furi_message_queue_free(app->event_queue);

    furi_mutex_free(app->mutex);
    free(app);
}

int32_t wav_recorder_app(void* p) {
    UNUSED(p);
    FURI_LOG_I(TAG, "Hello world");
    FURI_LOG_I(TAG, "I'm wav_recorder!");

    wav_recorder_adc_init();

    RecorderApp* app = wav_recorder_alloc();

    FuriTimer* timer =
        furi_timer_alloc(wav_recorder_tick, FuriTimerTypePeriodic, app->event_queue);

    // TODO: FuriTimer max frequency is 1kHz. Use hardware timer.
    furi_timer_start(timer, 1);

    RecorderEvent event;
    for(bool running = true; running;) {
        furi_check(
            furi_message_queue_get(app->event_queue, &event, FuriWaitForever) == FuriStatusOk);
        if(event.type == EventTypeInput) {
            if(event.input.type == InputTypeShort && event.input.key == InputKeyBack) {
                running = false;
            }
        } else if(event.type == EventTypeTick) {
            furi_mutex_acquire(app->mutex, FuriWaitForever);

            uint32_t adc_value = furi_hal_adc_read_sw();
            // float adc_voltage = 2.5f * (float)adc_value / 4096.0f;
            app->sample_max = MAX(adc_value, app->sample_max);
            app->sample_min = MIN(adc_value, app->sample_min);

            furi_mutex_release(app->mutex);
            view_port_update(app->view_port);
        }
    }

    furi_timer_stop(timer);
    furi_timer_free(timer);

    wav_recorder_free(app);

    wav_recorder_adc_deinit();

    return 0;
}
