#include "input.h"

#include <stdbool.h>
#include <stdint.h>
#include <furi.h>
#include <cli/cli.h>
#include <furi_hal_gpio.h>

#define TAG "Input"

#define INPUT_SRV_DEBOUNCE_TIMER_TICKS 1 //ms

#define INPUT_SRV_INPUT_LONG_PRESS_TICKS 150 //ms
#define INPUT_SRV_LONG_PRESS_COUNTS      2

#define GPIO_Read(input_pin) (furi_hal_gpio_read(input_pin.pin->gpio) ^ (input_pin.pin->inverted))

#ifdef INPUT_DEBUG
#define INPUT_LOG(...) FURI_LOG_D(TAG, __VA_ARGS__)
#else
#define INPUT_LOG(...)
#endif
typedef struct {
    FuriEventLoopTimer* timer;
    FuriPubSub* event_pubsub;
    uint32_t sequence_counter;
    uint32_t press_counter;
    InputType type;
    InputKey key;
} InputSrvKeySequence;

typedef struct {
    const InputPin* pin;
    uint16_t debounce_count;
    bool state;
} InputSrvKeyState;

typedef struct {
    FuriEventLoop* event_loop;
    FuriPubSub* event_pubsub;
    FuriSemaphore* input_semaphore;
    FuriEventLoopTimer* debounce_timer;
    InputSrvKeyState* key_state;
    InputSrvKeySequence* key_sequence;
    uint32_t sequence_counter;
} InputSrv;

static void input_key_sequence_run(
    InputSrvKeySequence* key_sequence,
    InputType type,
    uint32_t sequence_counter);

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

static void input_isr_key(void* context) {
    InputSrv* instance = context;
    furi_semaphore_release(instance->input_semaphore);
}

static bool input_semaphore_callback(FuriEventLoopObject* object, void* context) {
    furi_assert(context);
    InputSrv* instance = context;
    furi_assert(object == instance->input_semaphore);

    furi_check(furi_semaphore_acquire(instance->input_semaphore, 0) == FuriStatusOk);

    if(!furi_event_loop_timer_is_running(instance->debounce_timer)) {
        furi_event_loop_timer_start(instance->debounce_timer, INPUT_SRV_DEBOUNCE_TIMER_TICKS);
    }
    return true;
}

static void input_debounce_timer_callback(void* context) {
    furi_assert(context);
    InputSrv* instance = context;
    bool is_changing = false;
    for(size_t i = 0; i < input_pins_count; i++) {
        bool state = GPIO_Read(instance->key_state[i]);

        if(state) {
            if(instance->key_state[i].debounce_count < INPUT_DEBOUNCE_TICKS) {
                instance->key_state[i].debounce_count++;
                is_changing = true;
            }
        } else if(instance->key_state[i].debounce_count > 0) {
            instance->key_state[i].debounce_count--;
            is_changing = true;
        }

        if(!is_changing && instance->key_state[i].state != state) {
            instance->key_state[i].state = state;

            if(state) {
                input_key_sequence_run(
                    &instance->key_sequence[i], InputTypePress, ++instance->sequence_counter);
            } else {
                input_key_sequence_run(
                    &instance->key_sequence[i], InputTypeRelease, instance->sequence_counter);
            }
        }
    }

    if(!is_changing) {
        furi_event_loop_timer_stop(instance->debounce_timer);
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

static void input_key_sequence_run(
    InputSrvKeySequence* key_sequence,
    InputType type,
    uint32_t sequence_counter) {
    InputEvent event;

    switch(type) {
    case InputTypePress:
        key_sequence->sequence_counter = sequence_counter;
        key_sequence->press_counter = 0;
        key_sequence->type = InputTypePress;

        event.sequence_source = INPUT_SEQUENCE_SOURCE_HARDWARE;
        event.sequence_counter = key_sequence->sequence_counter;
        event.key = key_sequence->key;
        event.type = InputTypePress;
        input_send(key_sequence->event_pubsub, &event);

        furi_check(!furi_event_loop_timer_is_running(key_sequence->timer));
        furi_event_loop_timer_start(key_sequence->timer, INPUT_SRV_INPUT_LONG_PRESS_TICKS);

        key_sequence->type = InputTypeRepeat;
        break;
    case InputTypeRelease:
        if(key_sequence->press_counter < INPUT_SRV_LONG_PRESS_COUNTS) {
            event.sequence_source = INPUT_SEQUENCE_SOURCE_HARDWARE;
            event.sequence_counter = key_sequence->sequence_counter;
            event.key = key_sequence->key;
            event.type = InputTypeShort;
            input_send(key_sequence->event_pubsub, &event);
        }
        event.sequence_source = INPUT_SEQUENCE_SOURCE_HARDWARE;
        event.sequence_counter = key_sequence->sequence_counter;
        event.key = key_sequence->key;
        event.type = InputTypeRelease;
        input_send(key_sequence->event_pubsub, &event);

        furi_event_loop_timer_stop(key_sequence->timer);

        key_sequence->type = InputTypeRelease;
        break;

    default:
        break;
    }
}
static void input_sequence_timer_callback(void* context) {
    furi_assert(context);
    InputSrvKeySequence* key_sequence = context;
    InputEvent event;

    if(key_sequence->press_counter == INPUT_SRV_LONG_PRESS_COUNTS) {
        event.sequence_source = INPUT_SEQUENCE_SOURCE_HARDWARE;
        event.sequence_counter = key_sequence->sequence_counter;
        event.key = key_sequence->key;
        event.type = InputTypeLong;
        input_send(key_sequence->event_pubsub, &event);
    } else if(key_sequence->press_counter > INPUT_SRV_LONG_PRESS_COUNTS) {
        event.sequence_source = INPUT_SEQUENCE_SOURCE_HARDWARE;
        event.sequence_counter = key_sequence->sequence_counter;
        event.key = key_sequence->key;
        event.type = InputTypeRepeat;
        input_send(key_sequence->event_pubsub, &event);
    }

    key_sequence->press_counter++;
}

int32_t input_srv(void* p) {
    UNUSED(p);
    InputSrv* instance = malloc(sizeof(InputSrv));
    instance->event_pubsub = furi_pubsub_alloc();
    furi_record_create(RECORD_INPUT_EVENTS, instance->event_pubsub);

    instance->input_semaphore = furi_semaphore_alloc(1, 0);
    instance->event_loop = furi_event_loop_alloc();
    instance->debounce_timer = furi_event_loop_timer_alloc(
        instance->event_loop,
        input_debounce_timer_callback,
        FuriEventLoopTimerTypePeriodic,
        instance);

    instance->key_state = malloc(sizeof(InputSrvKeyState) * input_pins_count);
    for(size_t i = 0; i < input_pins_count; i++) {
        furi_hal_gpio_add_int_callback(input_pins[i].gpio, input_isr_key, instance);
        instance->key_state[i].pin = &input_pins[i];
        instance->key_state[i].state = GPIO_Read(instance->key_state[i]);
        instance->sequence_counter = 0;
    }

    furi_event_loop_subscribe_semaphore(
        instance->event_loop,
        instance->input_semaphore,
        FuriEventLoopEventIn,
        input_semaphore_callback,
        instance);

    instance->key_sequence = malloc(sizeof(InputSrvKeySequence) * input_pins_count);
    for(size_t i = 0; i < input_pins_count; i++) {
        instance->key_sequence[i].sequence_counter = 0;
        instance->key_sequence[i].press_counter = 0;
        instance->key_sequence[i].type = InputTypeRelease;
        instance->key_sequence[i].key = input_pins[i].key;
        instance->key_sequence[i].timer = furi_event_loop_timer_alloc(
            instance->event_loop,
            input_sequence_timer_callback,
            FuriEventLoopTimerTypePeriodic,
            &instance->key_sequence[i]);
        instance->key_sequence[i].event_pubsub = instance->event_pubsub;
    }

    // Start Input Service
    furi_event_loop_run(instance->event_loop);

    return 0;
}
