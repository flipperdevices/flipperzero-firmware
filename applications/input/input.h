#include "flipper.h"

/// List of buttons
typedef enum {
    InputsUp = 0,
    InputsDown,
    InputsRight,
    InputsLeft,
    InputsOk,
    InputsBack,
    InputsSize
} Inputs;

/// used to pass button press/release evens
typedef struct {
    Inputs input; /// what button
    bool state; /// true = press, false = release
} InputEvent;

/// Current state of buttons
typedef struct {
    bool up;
    bool down;
    bool right;
    bool left;
    bool ok;
    bool back;
} InputState;

typedef struct {
    PubSub events;
    PubSub raw_events;
    MutexValue state;
} Input;

/// Get input struct
inline Input* open_input(const char* name) {
    return furi_open(name);
}

/// read current state of all buttons
inline bool read_state(MutexValue state, InputState* value, uint32_t timeout) {
    return read_mutex(state, (void*)value, timeout);
}

/// subscribe on button press/release events
inline bool subscribe_events(PubSub events, void(*cb)(InputEvent*, void*), void* ctx) {
    return subscribe_pubsub(events, void(*)(InputEvent*, void*)(cb), ctx);
}

/* example usage

// function used to handle keyboard events
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
