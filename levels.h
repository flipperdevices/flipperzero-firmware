#include <stdint.h>

typedef struct {
    uint8_t index;
    bool isInvertedColor;
    bool test;
} Level;

static Level levels[2] = {
    {0, true, false},
    {1, false, false},
};