#ifndef GAME_SPRITES_H
#define GAME_SPRITES_H

#include "point.h"
#include <gui/icon_animation.h>

typedef struct {
    IconAnimation* barry;
    IconAnimation* scientist_left;
    IconAnimation* scientist_right;
    IconAnimation* missile;
    const Icon* bg[3];
    POINT bg_pos[3];
} GameSprites;

#endif // GAME_SPRITES_H