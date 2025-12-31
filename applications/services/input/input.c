<<<<<<< HEAD
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
=======
#include "input_i.h"

// #define INPUT_DEBUG

#define GPIO_Read(input_pin) (furi_hal_gpio_read(input_pin.pin->gpio) ^ (input_pin.pin->inverted))

static Input* input = NULL;

void input_press_timer_callback(void* arg) {
    InputPinState* input_pin = arg;
    InputEvent event;
    event.sequence_source = INPUT_SEQUENCE_SOURCE_HARDWARE;
    event.sequence_counter = input_pin->counter;
    event.key = input_pin->pin->key;
    input_pin->press_counter++;
    if(input_pin->press_counter == INPUT_LONG_PRESS_COUNTS) {
        event.type = InputTypeLong;
        furi_pubsub_publish(input->event_pubsub, &event);
    } else if(input_pin->press_counter > INPUT_LONG_PRESS_COUNTS) {
        input_pin->press_counter--;
        event.type = InputTypeRepeat;
        furi_pubsub_publish(input->event_pubsub, &event);
    }
}

void input_isr(void* _ctx) {
    UNUSED(_ctx);
    furi_thread_flags_set(input->thread_id, INPUT_THREAD_FLAG_ISR);
}
>>>>>>> origin/upstream-pr-2141-doom/2991-e2e-runner

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

static void input_semaphore_callback(FuriEventLoopObject* object, void* context) {
    furi_assert(context);
    InputSrv* instance = context;
    furi_assert(object == instance->input_semaphore);

    furi_check(furi_semaphore_acquire(instance->input_semaphore, 0) == FuriStatusOk);

    if(!furi_event_loop_timer_is_running(instance->debounce_timer)) {
        furi_event_loop_timer_start(instance->debounce_timer, INPUT_SRV_DEBOUNCE_TIMER_TICKS);
    }
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
                input_key_sequence_run(&instance->key_sequence[i], InputTypeRelease, 0);
            }
        }
    }

    if(!is_changing) {
        furi_event_loop_timer_stop(instance->debounce_timer);
    }
}

static inline void
    input_send(FuriPubSub* pubsub, InputKey key, InputType type, uint32_t sequence_counter) {
    InputEvent event = {
        .sequence_source = INPUT_SEQUENCE_SOURCE_HARDWARE,
        .sequence_counter = sequence_counter,
        .key = key,
        .type = type,
    };

    furi_pubsub_publish(pubsub, &event);
    INPUT_LOG(
        "input_send: %s %s %x",
        input_get_key_name(event.key),
        input_get_type_name(event.type),
        event.sequence_counter);
}

static void input_key_sequence_run(
    InputSrvKeySequence* key_sequence,
    InputType type,
    uint32_t sequence_counter) {
    switch(type) {
    case InputTypePress:
        key_sequence->sequence_counter = sequence_counter;
        key_sequence->press_counter = 0;

        furi_assert(!furi_event_loop_timer_is_running(key_sequence->timer));
        furi_event_loop_timer_start(key_sequence->timer, INPUT_SRV_INPUT_LONG_PRESS_TICKS);

        input_send(
            key_sequence->event_pubsub,
            key_sequence->key,
            InputTypePress,
            key_sequence->sequence_counter);

        break;
    case InputTypeRelease:
        if(key_sequence->press_counter < INPUT_SRV_LONG_PRESS_COUNTS) {
            input_send(
                key_sequence->event_pubsub,
                key_sequence->key,
                InputTypeShort,
                key_sequence->sequence_counter);
        }

        furi_assert(furi_event_loop_timer_is_running(key_sequence->timer));
        furi_event_loop_timer_stop(key_sequence->timer);

        input_send(
            key_sequence->event_pubsub,
            key_sequence->key,
            InputTypeRelease,
            key_sequence->sequence_counter);

        break;
    default:
        furi_crash();
        break;
    }
}

static void input_sequence_timer_callback(void* context) {
    furi_assert(context);
    InputSrvKeySequence* key_sequence = context;

    key_sequence->press_counter++;

    if(key_sequence->press_counter == INPUT_SRV_LONG_PRESS_COUNTS) {
        input_send(
            key_sequence->event_pubsub,
            key_sequence->key,
            InputTypeLong,
            key_sequence->sequence_counter);
    } else if(key_sequence->press_counter > INPUT_SRV_LONG_PRESS_COUNTS) {
        input_send(
            key_sequence->event_pubsub,
            key_sequence->key,
            InputTypeRepeat,
            key_sequence->sequence_counter);
    }
}

int32_t input_srv(void* p) {
    UNUSED(p);
<<<<<<< HEAD
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
=======
    input = malloc(sizeof(Input));
    input->thread_id = furi_thread_get_current_id();
    input->event_pubsub = furi_pubsub_alloc();
    furi_record_create(RECORD_INPUT_EVENTS, input->event_pubsub);

#if INPUT_DEBUG
    furi_hal_gpio_init_simple(&gpio_ext_pa4, GpioModeOutputPushPull);
#endif

#ifdef SRV_CLI
    input->cli = furi_record_open(RECORD_CLI);
    cli_add_command(input->cli, "input", CliCommandFlagParallelSafe, input_cli, input);
#endif

    input->pin_states = malloc(input_pins_count * sizeof(InputPinState));
>>>>>>> origin/upstream-pr-2141-doom/2991-e2e-runner

    instance->key_state = malloc(sizeof(InputSrvKeyState) * input_pins_count);
    for(size_t i = 0; i < input_pins_count; i++) {
<<<<<<< HEAD
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
        instance->key_sequence[i].key = input_pins[i].key;
        instance->key_sequence[i].timer = furi_event_loop_timer_alloc(
            instance->event_loop,
            input_sequence_timer_callback,
            FuriEventLoopTimerTypePeriodic,
            &instance->key_sequence[i]);
        instance->key_sequence[i].event_pubsub = instance->event_pubsub;
=======
        furi_hal_gpio_add_int_callback(input_pins[i].gpio, input_isr, NULL);
        input->pin_states[i].pin = &input_pins[i];
        input->pin_states[i].state = GPIO_Read(input->pin_states[i]);
        input->pin_states[i].debounce = INPUT_DEBOUNCE_TICKS_HALF;
        input->pin_states[i].press_timer = furi_timer_alloc(
            input_press_timer_callback, FuriTimerTypePeriodic, &input->pin_states[i]);
        input->pin_states[i].press_counter = 0;
    }

    while(1) {
        bool is_changing = false;
        for(size_t i = 0; i < input_pins_count; i++) {
            bool state = GPIO_Read(input->pin_states[i]);
            if(state) {
                if(input->pin_states[i].debounce < INPUT_DEBOUNCE_TICKS)
                    input->pin_states[i].debounce += 1;
            } else {
                if(input->pin_states[i].debounce > 0) input->pin_states[i].debounce -= 1;
            }

            if(input->pin_states[i].debounce > 0 &&
               input->pin_states[i].debounce < INPUT_DEBOUNCE_TICKS) {
                is_changing = true;
            } else if(input->pin_states[i].state != state) {
                input->pin_states[i].state = state;

                // Common state info
                InputEvent event;
                event.sequence_source = INPUT_SEQUENCE_SOURCE_HARDWARE;
                event.key = input->pin_states[i].pin->key;

                // Short / Long / Repeat timer routine
                if(state) {
                    input->counter++;
                    input->pin_states[i].counter = input->counter;
                    event.sequence_counter = input->pin_states[i].counter;
                    furi_timer_start(input->pin_states[i].press_timer, INPUT_PRESS_TICKS);
                } else {
                    event.sequence_counter = input->pin_states[i].counter;
                    furi_timer_stop(input->pin_states[i].press_timer);
                    while(furi_timer_is_running(input->pin_states[i].press_timer))
                        furi_delay_tick(1);
                    if(input->pin_states[i].press_counter < INPUT_LONG_PRESS_COUNTS) {
                        event.type = InputTypeShort;
                        furi_pubsub_publish(input->event_pubsub, &event);
                    }
                    input->pin_states[i].press_counter = 0;
                }

                // Send Press/Release event
                event.type = input->pin_states[i].state ? InputTypePress : InputTypeRelease;
                furi_pubsub_publish(input->event_pubsub, &event);
            }
        }

        if(is_changing) {
#if INPUT_DEBUG
            furi_hal_gpio_write(&gpio_ext_pa4, 1);
#endif
            furi_delay_tick(1);
        } else {
#if INPUT_DEBUG
            furi_hal_gpio_write(&gpio_ext_pa4, 0);
#endif
            furi_thread_flags_wait(INPUT_THREAD_FLAG_ISR, FuriFlagWaitAny, FuriWaitForever);
        }
>>>>>>> origin/upstream-pr-2141-doom/2991-e2e-runner
    }

    // Start Input Service
    furi_event_loop_run(instance->event_loop);

    return 0;
}
