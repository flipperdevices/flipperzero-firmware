#ifndef PAPER_H
#define PAPER_H
#include <gui/gui.h>

#define PAPER_START_X 8
#define PAPER_START_Y 15

#define BASE_GRAVITY 4

#define PAPER_MIN_ROTATION -3
#define PAPER_MAX_ROTATION 3

#define ROTATION_MULTIPLIER 2

#define PLANE_UPDATE_MS_PER_FRAME 10

typedef struct Paper {
    const Icon* icon;
    int frame_ms;
    float x, y;
    int rotation;
} Paper;

void update_sprite_rotation(Paper* dest);

void rotate_left(Paper* dest);

void rotate_right(Paper* dest);

void paper_init(Paper* dest);

void update_position(Paper* dest, int delta_time_ms);

int get_gravity_from_rotation(int rotation);

#endif