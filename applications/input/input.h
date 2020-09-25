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
    return furi_open(name);
}

inline bool read_state(MutexValue state, InputState* value, uint32_t timeout) {
    return read_mutex(state, (void*)value, timeout);
}

inline bool subscribe_events(PubSub events, void(*cb)(InputEvent*, void*), void* ctx) {
    return subscribe_pubsub(events, void(*)(InputEvent*, void*)(cb), ctx);
}

/* example usage

void handle_keyboard(InputEvent* event, void* _ctx) {
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
    InputState state;
    while(1) {
        if(read_state(input->state, &state, MAX_TIMEOUT)) {
            if(state.up) {
                printf("up is pressed");
                delay(1000);
            }
        }

        delay(10);
    }
}

*/
