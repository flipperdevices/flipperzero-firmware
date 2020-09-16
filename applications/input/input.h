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
