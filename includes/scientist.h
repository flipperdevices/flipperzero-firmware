#ifndef SCIENTIST_H
#define SCIENTIST_H

#include "point.h"
#include "game_sprites.h"
#include <gui/gui.h>

#define SCIENTIST_VELOCITY_MIN 2
#define SCIENTIST_VELOCITY_MAX 2

#define SCIENTISTS_MAX 6

typedef enum {
    ScientistStateAlive,
    ScientistStateDead,
} ScientistState;

typedef struct {
    float gravity;
    POINT point;
    int velocity_x;
    ScientistState state;
} SCIENTIST;

void scientist_tick(SCIENTIST* const scientist);
void spawn_random_scientist(SCIENTIST* const scientists);
void draw_scientists(const SCIENTIST* scientists, Canvas* const canvas, const GameSprites* sprites);

#endif // SCIENTIST_H