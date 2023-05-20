#ifndef COIN_H
#define COIN_H

#include <gui/gui.h>

#include "point.h"
#include "barry.h"

#define COINS_MAX 25

typedef struct {
    float gravity;
    POINT point;
} COIN;

void coin_tick(COIN* const coins, BARRY* const barry, int* const poins);
void spawn_random_coin(COIN* const coins);
bool coin_colides(COIN* const coin, BARRY* const barry);
void draw_coins(const COIN* coins, Canvas* const canvas);

#endif // COIN_H