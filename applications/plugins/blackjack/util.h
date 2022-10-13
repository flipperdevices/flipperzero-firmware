#pragma once
#include "defines.h"

typedef struct{
    void (*callback)(GameState *game_state);
    void (*processing)(const GameState *game_state, Canvas *const canvas);
    void (*start)(GameState *game_state);
    void *next;
} List;

float lerp(float v0, float v1, float t);
void queue(GameState *game_state,
           void (*callback)(GameState *game_state),
           void (*start)(GameState *game_state),
           void (*processing)(const GameState *gameState, Canvas *const canvas));
bool run_queue(GameState *gameState);
void animateQueue(const GameState *gameState, Canvas *const canvas);
void queue_clear();