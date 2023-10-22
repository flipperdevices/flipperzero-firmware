#include <stdint.h>

#define MAX_ENEMY_PER_LEVEL 20 // Do we need this constant?

typedef struct {
    uint8_t index;
    bool isInvertedColor;
    uint8_t enemySpawDelay[MAX_ENEMY_PER_LEVEL];
    uint8_t enemy_spawn_y[MAX_ENEMY_PER_LEVEL];
    uint8_t enemySpawType[MAX_ENEMY_PER_LEVEL];
} Level;

static Level levels[2] = {
    {0,
     true,
     {2, 5, 7, 15, 18, 21, 26, 30, 34, 40},
     {32, 20, 10, 15, 50, 21, 26, 30, 34, 40},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {1,
     false,
     {2, 5, 7, 15, 18, 21, 26, 30, 34, 40},
     {32, 20, 10, 15, 50, 21, 26, 30, 34, 40},
     {0, 1, 0, 1, 0, 1, 0, 1, 0, 1}},
};
