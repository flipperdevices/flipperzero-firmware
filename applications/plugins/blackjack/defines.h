#pragma once

#include <furi.h>
#include <input/input.h>
#include <gui/elements.h>
#include "card.h"

#define ANIMATION_TIME furi_ms_to_ticks(1500)
#define ANIMATION_END_MARGIN furi_ms_to_ticks(200)
#define ROUND_PRICE 10

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} AppEvent;

typedef enum {
    GameStateGameOver,
    GameStateStart,
    GameStatePlay,
    GameStateDealer,
} PlayState;

typedef enum {
    DirectionUp,
    DirectionRight,
    DirectionDown,
    DirectionLeft,
    Select,
    None
} Direction;

typedef struct {
    Card player_cards[21];
    Card dealer_cards[21];
    uint8_t player_card_count;
    uint8_t dealer_card_count;

    Direction selectDirection;

    uint32_t player_score;
    uint8_t multiplier;
    uint32_t bet;
    uint8_t player_time;
    bool doubled;
    bool animating;
    bool started;
    uint8_t selectedMenu;
    Deck deck;
    PlayState state;
    unsigned int last_tick;
    unsigned int animationStart;
    bool dealer_animating;
    unsigned int delay_tick;
} GameState;

