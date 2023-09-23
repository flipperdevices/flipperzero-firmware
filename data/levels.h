#include <stdint.h>

#define MAX_ENEMY_PER_LEVEL 20

typedef struct {
    uint8_t index;
    bool isInvertedColor;
    uint8_t enemySpawDelay[MAX_ENEMY_PER_LEVEL];
    uint8_t enemySpawType[MAX_ENEMY_PER_LEVEL];
} Level;

static Level levels[2] = {
    {0, true, {3, 10, 18}, {0, 1, 0}},
    {1, false, {3, 6}, {1, 0}},
};
