#ifndef MISSILE_H
#define MISSILE_H

#include <gui/gui.h>
#include "game_sprites.h"

#include "states.h"
#include "point.h"
#include "barry.h"

#define MISSILES_MAX 2

typedef struct {
    float gravity;
    POINT point;
} MISSILE;

void missile_tick(MISSILE* const missiles, BARRY* const barry, State* const state);
void spawn_random_missile(MISSILE* const MISSILEs);
bool missile_colides(MISSILE* const MISSILE, BARRY* const barry);
void draw_missiles(const MISSILE* missiles, Canvas* const canvas, const GameSprites* sprites);

#endif // MISSILE_H