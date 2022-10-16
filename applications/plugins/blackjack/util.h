#pragma once
#include "defines.h"
#define CONFIG_FILE_HEADER "Blackjack config file"
#define CONFIG_FILE_VERSION 1

typedef struct{
    void (*callback)(GameState *game_state);
    void (*processing)(const GameState *game_state, Canvas *const canvas,uint32_t duration,uint32_t margin);
    void (*start)(GameState *game_state);
    void *next;
    uint32_t duration;
    uint32_t margin;
} QueueItem;

struct Vector{
    float x;
    float y;
};

float lerp(float v0, float v1, float t);
void queue(GameState *game_state,
           void (*callback)(GameState *game_state),
           void (*start)(GameState *game_state),
           void (*processing)(const GameState *gameState, Canvas *const canvas, uint32_t duration,uint32_t margin),
           uint32_t duration,uint32_t margin
           );
bool run_queue(GameState *gameState);
void animateQueue(const GameState *gameState, Canvas *const canvas);
void queue_clear();
Vector lerp_2d(Vector start, Vector end, float t);
Vector quadratic_2d(Vector start, Vector control, Vector end, float t);

void save_settings(Settings settings);
Settings load_settings();