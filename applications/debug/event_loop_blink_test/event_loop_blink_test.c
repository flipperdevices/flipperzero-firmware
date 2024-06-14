#include <furi.h>
#include <furi_hal_resources.h>
#include <input/input.h>

#define TIMER_COUNT (6U)

typedef struct {
    FuriEventLoop* event_loop;
    FuriMessageQueue* input_queue;
    FuriEventLoopTimer* timers[TIMER_COUNT];
} EventLoopBlinkTestApp;

static const GpioPin* blink_gpio_pins[] = {
    &gpio_ext_pa7,
    &gpio_ext_pa6,
    &gpio_ext_pa4,
    &gpio_ext_pb3,
    &gpio_ext_pb2,
    &gpio_ext_pc3,
};

static_assert(COUNT_OF(blink_gpio_pins) == TIMER_COUNT);

static const uint32_t timer_timeouts[] = {
    320,
    750,
    1000,
    500,
    150,
    210,
};

static_assert(COUNT_OF(timer_timeouts) == TIMER_COUNT);

static void blink_gpio_init(void) {
    for(size_t i = 0; i < TIMER_COUNT; ++i) {
        furi_hal_gpio_init_simple(blink_gpio_pins[i], GpioModeOutputPushPull);
    }

    furi_hal_gpio_init_simple(&gpio_ext_pc0, GpioModeOutputPushPull);
}

static void blink_gpio_deinit(void) {
    for(size_t i = 0; i < TIMER_COUNT; ++i) {
        furi_hal_gpio_init_simple(blink_gpio_pins[i], GpioModeAnalog);
    }

    furi_hal_gpio_init_simple(&gpio_ext_pc0, GpioModeAnalog);
}

static void input_callback(const void* value, void* context) {
    EventLoopBlinkTestApp* app = context;
    furi_message_queue_put(app->input_queue, value, 0);
}

static bool input_queue_callback(FuriMessageQueue* queue, void* context) {
    EventLoopBlinkTestApp* app = context;

    InputEvent event;
    FuriStatus status = furi_message_queue_get(queue, &event, FuriWaitForever);
    furi_assert(status == FuriStatusOk);

    if(event.type == InputTypeShort && event.key == InputKeyBack) {
        furi_event_loop_stop(app->event_loop);
    }

    return true;
}

static void blink_timer_callback(uint32_t elapsed, void* context) {
    UNUSED(elapsed);
    const GpioPin* gpio = blink_gpio_pins[(size_t)context];
    furi_hal_gpio_write(gpio, !furi_hal_gpio_read(gpio));
}

static void event_loop_tick_callback(void* context) {
    UNUSED(context);
    furi_hal_gpio_write(&gpio_ext_pc0, !furi_hal_gpio_read(&gpio_ext_pc0));
}

int32_t event_loop_blink_test_app(void* arg) {
    UNUSED(arg);

    blink_gpio_init();

    EventLoopBlinkTestApp app;

    app.event_loop = furi_event_loop_alloc();
    app.input_queue = furi_message_queue_alloc(3, sizeof(InputEvent));

    for(size_t i = 0; i < TIMER_COUNT; ++i) {
        app.timers[i] = furi_event_loop_timer_alloc(
            blink_timer_callback, FuriEventLoopTimerTypePeriodic, (void*)i);
        furi_event_loop_timer_start(app.event_loop, app.timers[i], timer_timeouts[i]);
    }

    FuriPubSub* input = furi_record_open(RECORD_INPUT_EVENTS);
    FuriPubSubSubscription* input_subscription =
        furi_pubsub_subscribe(input, input_callback, &app);

    furi_event_loop_tick_set(app.event_loop, 500, event_loop_tick_callback, &app);
    furi_event_loop_message_queue_subscribe(
        app.event_loop, app.input_queue, FuriEventLoopEventIn, input_queue_callback, &app);

    furi_event_loop_run(app.event_loop);

    furi_event_loop_message_queue_unsubscribe(app.event_loop, app.input_queue);

    furi_pubsub_unsubscribe(input, input_subscription);
    furi_record_close(RECORD_INPUT_EVENTS);

    for(size_t i = 0; i < TIMER_COUNT; ++i) {
        furi_event_loop_timer_stop(app.event_loop, app.timers[i]);
        furi_event_loop_timer_free(app.timers[i]);
    }

    furi_message_queue_free(app.input_queue);
    furi_event_loop_free(app.event_loop);

    blink_gpio_deinit();
    return 0;
}
