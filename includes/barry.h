#ifndef BARRY_H
#define BARRY_H

#include <stdbool.h>

#include <gui/gui.h>
#include "point.h"
#include "game_sprites.h"

#define GRAVITY_BOOST -0.3
#define GRAVITY_TICK 0.15

#define BARRY_HEIGHT 15
#define BARRY_WIDTH 11

typedef struct {
    float gravity;
    POINT point;
    bool isBoosting;
} BARRY;

void barry_tick(BARRY* const barry);
void draw_barry(const BARRY* barry, Canvas* const canvas, const GameSprites* sprites);

#endif // BARRY_H