#include "input.h"

#include <stdbool.h>
#include <stdint.h>
#include <furi.h>
#include <cli/cli.h>
#include <furi_hal_gpio.h>

#define TAG "Input"

#define INPUT_SRV_DEBOUNCE_TICKS 10
#define INPUT_SRV_DEBOUNCE_COUNT 2

#define INPUT_SRV_INPUT_LONG_PRESS_TICKS 150
#define INPUT_SRV_LONG_PRESS_COUNTS      3

#define GPIO_Read(input_pin) (furi_hal_gpio_read(input_pin.pin->gpio) ^ (input_pin.pin->inverted))

#ifdef INPUT_DEBUG
#define INPUT_LOG(...) FURI_LOG_D(TAG, __VA_ARGS__)
#else
#define INPUT_LOG(...)
#endif

typedef struct {
    uint16_t repeat_count;
    uint16_t debounce_count;
    FuriEventLoopTimer* timer;
    FuriPubSub* event_pubsub;
    const InputPin* pin;
    uint32_t sequence_counter;
    bool state;
    bool state_last;
} InputSrvKeyState;

typedef struct {
    FuriEventLoop* event_loop;
    FuriPubSub* event_pubsub;
    FuriSemaphore* input_semaphore;
    InputSrvKeyState* key_state;
    uint32_t sequence_counter;
} InputSrv;

const char* input_get_key_name(InputKey key) {
    for(size_t i = 0; i < input_pins_count; i++) {
        if(input_pins[i].key == key) {
            return input_pins[i].name;
        }
    }
    furi_crash();
}

const char* input_get_type_name(InputType type) {
    switch(type) {
    case InputTypePress:
        return "Press";
    case InputTypeRelease:
        return "Release";
    case InputTypeShort:
        return "Short";
    case InputTypeLong:
        return "Long";
    case InputTypeRepeat:
        return "Repeat";
    default:
        furi_crash();
    }
}

static inline void input_send(FuriPubSub* pubsub, InputEvent* event) {
    furi_pubsub_publish(pubsub, event);
    INPUT_LOG(
        "input_send: %s %s %x",
        input_get_key_name(event->key),
        input_get_type_name(event->type),
        event->sequence_counter);
}

static void input_timer_callback(void* context) {
    furi_assert(context);
    InputSrvKeyState* key_state = context;
    bool state = furi_hal_gpio_read(key_state->pin->gpio) ^ (key_state->pin->inverted);

    // De-bounce
    if(key_state->debounce_count) {
        if((key_state->state != state)) {
            key_state->debounce_count = 1;
            key_state->state = state;
        } else {
            if(++key_state->debounce_count > INPUT_SRV_DEBOUNCE_COUNT) {
                if(key_state->state_last != key_state->state) {
                    key_state->state_last = key_state->state;
                    key_state->debounce_count = 0;

                    InputEvent event;
                    event.sequence_source = INPUT_SEQUENCE_SOURCE_HARDWARE;
                    event.sequence_counter = key_state->sequence_counter;
                    event.key = key_state->pin->key;
                    if(key_state->state) {
                        event.type = InputTypePress;
                        furi_event_loop_timer_start(
                            key_state->timer, INPUT_SRV_INPUT_LONG_PRESS_TICKS);
                    } else {
                        if((key_state->repeat_count < INPUT_SRV_LONG_PRESS_COUNTS)) {
                            event.type = InputTypeShort;
                            input_send(key_state->event_pubsub, &event);
                        }
                        event.type = InputTypeRelease;
                        furi_event_loop_timer_stop(key_state->timer);
                    }
                    input_send(key_state->event_pubsub, &event);
                    key_state->repeat_count = 1;
                }
            }
        }
    } else {
        // Hold key
        if(key_state->repeat_count == INPUT_SRV_LONG_PRESS_COUNTS) {
            InputEvent event;
            event.sequence_source = INPUT_SEQUENCE_SOURCE_HARDWARE;
            event.sequence_counter = key_state->sequence_counter;
            event.key = key_state->pin->key;
            event.type = InputTypeLong;
            input_send(key_state->event_pubsub, &event);
        } else if(key_state->repeat_count > INPUT_SRV_LONG_PRESS_COUNTS) {
            InputEvent event;
            event.sequence_source = INPUT_SEQUENCE_SOURCE_HARDWARE;
            event.sequence_counter = key_state->sequence_counter;
            event.key = key_state->pin->key;
            event.type = InputTypeRepeat;
            input_send(key_state->event_pubsub, &event);
        }
        key_state->repeat_count++;
    }
}

static bool input_semaphore_callback(FuriEventLoopObject* object, void* context) {
    furi_assert(context);
    InputSrv* instance = context;
    furi_assert(object == instance->input_semaphore);

    furi_check(furi_semaphore_acquire(instance->input_semaphore, 0) == FuriStatusOk);

    for(size_t i = 0; i < input_pins_count; i++) {
        if(instance->key_state[i].state != GPIO_Read(instance->key_state[i])) {
            if(!instance->key_state[i].state) {
                instance->key_state[i].repeat_count = 0;
            }
            instance->key_state[i].debounce_count = 1;
            instance->key_state[i].sequence_counter = instance->sequence_counter++;
            furi_event_loop_timer_start(instance->key_state[i].timer, INPUT_SRV_DEBOUNCE_TICKS);
        }
    }
    return true;
}

void input_isr_key(void* context) {
    InputSrv* instance = context;
    furi_semaphore_release(instance->input_semaphore);
}

int32_t input_srv(void* p) {
    UNUSED(p);
    InputSrv* instance = malloc(sizeof(InputSrv));
    instance->event_pubsub = furi_pubsub_alloc();
    furi_record_create(RECORD_INPUT_EVENTS, instance->event_pubsub);

    instance->input_semaphore = furi_semaphore_alloc(1, 0);
    instance->event_loop = furi_event_loop_alloc();

    instance->key_state = malloc(sizeof(InputSrvKeyState) * input_pins_count);
    for(size_t i = 0; i < input_pins_count; i++) {
        furi_hal_gpio_add_int_callback(input_pins[i].gpio, input_isr_key, instance);
        instance->key_state[i].pin = &input_pins[i];
        instance->key_state[i].state = GPIO_Read(instance->key_state[i]);
        instance->key_state[i].state_last = instance->key_state[i].state;
        instance->key_state[i].repeat_count = 0;
        instance->key_state[i].timer = furi_event_loop_timer_alloc(
            instance->event_loop,
            input_timer_callback,
            FuriEventLoopTimerTypePeriodic,
            &instance->key_state[i]);
        instance->key_state[i].event_pubsub = instance->event_pubsub;
    }

    furi_event_loop_subscribe_semaphore(
        instance->event_loop,
        instance->input_semaphore,
        FuriEventLoopEventIn,
        input_semaphore_callback,
        instance);

    // Start Input Service
    furi_event_loop_run(instance->event_loop);

    return 0;
}
