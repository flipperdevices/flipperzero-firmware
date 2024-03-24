#include <input/input.h>
#pragma once
typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

typedef struct {
    FuriMutex* mutex;
    char arrowDirections[8]; // Store directions of up to 8 arrows
    bool arrowFilled[8]; // Track whether each arrow is filled
    int numArrows; // Number of arrows currently active
    int nextArrowToFill; // Index of the next arrow to fill
    int score;
    int round;
    int timer;
    int32_t record_score;
    int32_t record_round;
    bool isGameStarted;
    bool isGameOver;
} PluginState;

typedef enum {
    ArrowDirectionUp,
    ArrowDirectionRight,
    ArrowDirectionLeft,
    ArrowDirectionDown
} ArrowDirection;
