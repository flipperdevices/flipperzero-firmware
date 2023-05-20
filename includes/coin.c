#include <stdlib.h>
#include <stdbool.h>

#include <jetpack_joyride_icons.h>
#include <gui/gui.h>

#include "coin.h"
#include "barry.h"

void coin_tick(COIN* const coins, BARRY* const barry, int* const poins) {
    // Move coins towards the player
    for(int i = 0; i < COINS_MAX; i++) {
        if(coin_colides(&coins[i], barry)) {
            coins[i].point.x = 0; // Remove the coin
            (*poins)++;
        }
        if(coins[i].point.x > 0) {
            coins[i].point.x -= 1; // move left by 1 unit
            if(coins[i].point.x < -16) { // if the coin is out of screen
                coins[i].point.x = 0; // set coin x coordinate to 0 to mark it as "inactive"
            }
        }
    }
}

bool coin_colides(COIN* const coin, BARRY* const barry) {
    return !(
        barry->point.x > coin->point.x + 7 || // Barry is to the right of the coin
        barry->point.x + 11 < coin->point.x || // Barry is to the left of the coin
        barry->point.y > coin->point.y + 7 || // Barry is below the coin
        barry->point.y + 15 < coin->point.y); // Barry is above the coin
}

void spawn_random_coin(COIN* const coins) {
    // Check for an available slot for a new coin
    for(int i = 0; i < COINS_MAX; ++i) {
        if(coins[i].point.x <= 0) {
            coins[i].point.x = 127;
            coins[i].point.y = rand() % 64;
            break;
        }
    }
}

void draw_coins(const COIN* coins, Canvas* const canvas) {
    for(int i = 0; i < COINS_MAX; ++i) {
        if(coins[i].point.x > 0) {
            canvas_draw_icon(canvas, coins[i].point.x, coins[i].point.y, &I_coin);
        }
    }
}