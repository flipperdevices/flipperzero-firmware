#ifndef GAME_SPRITES_H
#define GAME_SPRITES_H

#include "point.h"
#include <gui/icon_animation.h>

typedef struct {
    IconAnimation* barry;
    const Icon* barry_infill;
    const Icon* scientist_left;
    const Icon* scientist_left_infill;
    const Icon* scientist_right;
    const Icon* scientist_right_infill;
    IconAnimation* missile;
    const Icon* missile_infill;
    const Icon* bg[3];
    POINT bg_pos[3];
} GameSprites;

#endif // GAME_SPRITES_H