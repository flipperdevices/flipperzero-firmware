#include "input.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <furi.h>
#include <cli/cli.h>
#include <furi_hal_gpio.h>

#define INPUT_SRV_DEBOUNCE_TICKS 2
#define INPUT_SRV_DEBOUNCE_COUNT 10

#define INPUT_SRV_INPUT_LONG_PRESS_TICKS 150
#define INPUT_SRV_LONG_PRESS_COUNTS      3

#define GPIO_Read(input_pin) (furi_hal_gpio_read(input_pin.pin->gpio) ^ (input_pin.pin->inverted))

typedef struct {
    uint16_t repeat_count;
    uint16_t debounse_count;
    FuriEventLoopTimer* timer;
    FuriPubSub* event_pubsub;
    const InputPin* pin;
    volatile bool state;
    volatile bool state_last;
} InputSRVKeyState;

typedef struct {
    FuriEventLoop* event_loop;
    FuriPubSub* event_pubsub;
    FuriSemaphore* input_semaphore;
    InputSRVKeyState* key_state;
} InputSRV;

const char* input_get_key_name(InputKey key) {
    for(size_t i = 0; i < input_pins_count; i++) {
        if(input_pins[i].key == key) {
            return input_pins[i].name;
        }
    }
    return "Unknown";
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
        return "Unknown";
    }
}
static void input_timer_callback(void* context) {
    furi_assert(context);
    InputSRVKeyState* key_state = context;
    bool state = furi_hal_gpio_read(key_state->pin->gpio) ^ (key_state->pin->inverted);

    //debounce
    if(key_state->debounse_count) {
        if((key_state->state != state)) {
            key_state->debounse_count = 1;
            key_state->state = state;
        } else {
            if(++key_state->debounse_count > INPUT_SRV_DEBOUNCE_COUNT) {
                if(key_state->state_last != key_state->state) {
                    key_state->state_last = key_state->state;
                    key_state->debounse_count = 0;

                    InputEvent event;
                    event.sequence_source = INPUT_SEQUENCE_SOURCE_HARDWARE;
                    event.sequence_counter = 0;
                    event.key = key_state->pin->key;
                    if(key_state->state) {
                        event.type = InputTypePress;
                        furi_event_loop_timer_start(
                            key_state->timer, INPUT_SRV_INPUT_LONG_PRESS_TICKS);
                    } else {
                        if((key_state->repeat_count < INPUT_SRV_LONG_PRESS_COUNTS)) {
                            event.type = InputTypeShort;
                            furi_pubsub_publish(key_state->event_pubsub, &event);
#ifdef INPUT_DEBUG
                            FURI_LOG_I(
                                "Input",
                                "Key: %s %s",
                                input_get_key_name(event.key),
                                input_get_type_name(event.type));
#endif
                        }
                        event.type = InputTypeRelease;
                        furi_event_loop_timer_stop(key_state->timer);
                    }
                    furi_pubsub_publish(key_state->event_pubsub, &event);
#ifdef INPUT_DEBUG
                    FURI_LOG_I(
                        "Input",
                        "Key: %s %s",
                        input_get_key_name(event.key),
                        input_get_type_name(event.type));
#endif

                    key_state->repeat_count = 1;
                }
            }
        }
    } else {
        //Hold key
        if(key_state->repeat_count == INPUT_SRV_LONG_PRESS_COUNTS) {
            InputEvent event;
            event.sequence_source = INPUT_SEQUENCE_SOURCE_HARDWARE;
            event.sequence_counter = 0;
            event.key = key_state->pin->key;
            event.type = InputTypeLong;
            furi_pubsub_publish(key_state->event_pubsub, &event);
#ifdef INPUT_DEBUG
            FURI_LOG_I(
                "Input",
                "Key: %s %s",
                input_get_key_name(event.key),
                input_get_type_name(event.type));
#endif
        } else if(key_state->repeat_count > INPUT_SRV_LONG_PRESS_COUNTS) {
            InputEvent event;
            event.sequence_source = INPUT_SEQUENCE_SOURCE_HARDWARE;
            event.sequence_counter = key_state->repeat_count - INPUT_SRV_LONG_PRESS_COUNTS;
            event.key = key_state->pin->key;
            event.type = InputTypeRepeat;
            furi_pubsub_publish(key_state->event_pubsub, &event);
#ifdef INPUT_DEBUG
            FURI_LOG_I(
                "Input",
                "Key: %s %s %d",
                input_get_key_name(event.key),
                input_get_type_name(event.type),
                event.sequence_counter);
#endif
        }
        key_state->repeat_count++;
    }
}

static bool input_semaphore_callback(FuriEventLoopObject* object, void* context) {
    furi_assert(context);
    InputSRV* instance = context;
    furi_assert(object == instance->input_semaphore);

    furi_check(furi_semaphore_release(instance->input_semaphore) == FuriStatusOk);

    for(size_t i = 0; i < input_pins_count; i++) {
        if(instance->key_state[i].state != GPIO_Read(instance->key_state[i])) {
            if(!instance->key_state[i].state) {
                instance->key_state[i].repeat_count = 0;
            }
            instance->key_state[i].debounse_count = 1;
            furi_event_loop_timer_start(instance->key_state[i].timer, INPUT_SRV_DEBOUNCE_TICKS);
        }
    }
    return true;
}

void input_isr_key(void* context) {
    InputSRV* instance = context;
    furi_semaphore_acquire(instance->input_semaphore, 0);
}

int32_t input_srv(void* p) {
    UNUSED(p);
    InputSRV* instance = malloc(sizeof(InputSRV));
    instance->event_pubsub = furi_pubsub_alloc();
    furi_record_create(RECORD_INPUT_EVENTS, instance->event_pubsub);

    instance->input_semaphore = furi_semaphore_alloc(1, 0);
    instance->event_loop = furi_event_loop_alloc();

    instance->key_state = malloc(sizeof(InputSRVKeyState) * input_pins_count);
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
        FuriEventLoopEventOut,
        input_semaphore_callback,
        instance);

    // Start Input Service
    furi_event_loop_run(instance->event_loop);

    return 0;
}
