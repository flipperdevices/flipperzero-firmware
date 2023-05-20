#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <gui/icon_animation.h>
#include <furi.h>

#include "barry.h"
#include "scientist.h"
#include "coin.h"
#include "particle.h"
#include "game_sprites.h"

typedef enum {
    GameStateLife,
    GameStateGameOver,
} State;

typedef struct {
    int points;
    int distance;
    BARRY barry;
    COIN coins[COINS_MAX];
    PARTICLE particles[PARTICLES_MAX];
    SCIENTIST scientists[SCIENTISTS_MAX];
    State state;
    GameSprites sprites;
    FuriMutex* mutex;
} GameState;

void game_state_tick(GameState* const game_state);

#endif // GAMESTATE_H