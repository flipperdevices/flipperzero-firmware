#include "flipper.h"

typedef enum {
    InputsUp = 0,
    InputsDown,
    InputsRight,
    InputsLeft,
    InputsOk,
    InputsBack,
    InputsCharging,
    InputsSize
} Inputs;

typedef struct {
    Inputs input;
    bool state;
} InputEvent;

typedef struct {
    bool up;
    bool down;
    bool right;
    bool left;
    bool ok;
    bool back;
    bool charging;
} InputState;

typedef struct {
    PubSub events;
    PubSub raw_events;
    MutexValue state;
} Input;

inline Input* open_input(const char* name) {
    return (Input*)furi_open(name);
}

inline InputState* get_input_state(MutexValue state, uint32_t timeout) {
    return get_mutex(state, timeout);
}

inline void release_input_state(MutexValue state, InputState* value) {
    relase_mutex(state, (void*)value);
}

inline bool subscribe_events(PubSub events, void(*cb)(InputEvent*, void*), void* ctx) {
    return subscribe_pubsub(events, void(*)(InputEvent*, void*)(cb), ctx);
}

/* example usage

void handle_keyboard(InputEvent event, void* _ctx) {
    if(event->state) {
        printf("you press %d", event->input);
    } else {
        printf("you release %d", event->input);
    }
}

void app_fn(void* p) {
    Input* input = open_input("/dev/kb");
    if(input == NULL) furiac_exit(NULL); // keyboard not available, critical error

    // async way
    subscribe_events(input->events, handle_keyboard, NULL);

    // blocking way
    while(1) {
        { InputState* state = get_input_state(input->state, MAX_TIMEOUT);
            if(state != NULL) {
                if(state->up) {
                    printf("up is pressed");
                    delay(1000);
                }
            }
        release_input_state(state); }

        delay(10);
    }
}
*/
