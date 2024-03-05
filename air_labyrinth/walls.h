#pragma once
#include <furi.h>

typedef struct {
    bool horizontal;
    int x;
    int y;
    int length;
} Wall;

#define WALL(h, y, x, l)   \
    (Wall) {               \
        h, x * 2, y * 2, l \
    }

Wall walls[] = {
    WALL(true, 12, 0, 3),   WALL(false, 3, 3, 17),  WALL(false, 23, 3, 6),
    WALL(true, 3, 4, 57),   WALL(true, 28, 4, 56),  WALL(false, 4, 7, 5),
    WALL(false, 12, 7, 13), WALL(true, 8, 8, 34),   WALL(true, 12, 8, 42),
    WALL(true, 24, 8, 8),   WALL(true, 16, 11, 8),  WALL(false, 17, 11, 4),
    WALL(true, 20, 12, 22), WALL(false, 6, 17, 2),  WALL(true, 24, 19, 15),
    WALL(true, 16, 22, 16), WALL(false, 4, 24, 1),  WALL(false, 21, 28, 2),
    WALL(false, 6, 33, 2),  WALL(false, 13, 34, 3), WALL(false, 17, 37, 11),
    WALL(true, 16, 41, 14), WALL(false, 20, 41, 5), WALL(true, 20, 45, 12),
    WALL(true, 24, 45, 12), WALL(false, 4, 46, 2),  WALL(false, 9, 46, 3),
    WALL(false, 6, 50, 3),  WALL(true, 12, 53, 7),  WALL(true, 8, 54, 6),
    WALL(false, 4, 60, 19), WALL(false, 26, 60, 6),
};
