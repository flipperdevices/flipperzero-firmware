#include "paper.h"
#include "paper_plane_icons.h"

void update_sprite_rotation(Paper* dest) {
    switch(dest->rotation) {
    case -3:
        dest->icon = &I_PaperLeft3;
        break;
    case -2:
        dest->icon = &I_PaperLeft2;
        break;
    case -1:
        dest->icon = &I_PaperLeft1;
        break;
    case 0:
        dest->icon = &I_PaperDown;
        break;
    case 1:
        dest->icon = &I_PaperRight1;
        break;
    case 2:
        dest->icon = &I_PaperRight2;
        break;
    case 3:
        dest->icon = &I_PaperRight3;
        break;

    default:
        break;
    }
}

void rotate_left(Paper* dest) {
    if(dest->rotation > PAPER_MIN_ROTATION) dest->rotation--;

    update_sprite_rotation(dest);
}

void rotate_right(Paper* dest) {
    if(dest->rotation < PAPER_MAX_ROTATION) dest->rotation++;

    update_sprite_rotation(dest);
}

void paper_init(Paper* dest) {
    dest->x = PAPER_START_X;
    dest->rotation = 0;
    dest->y = 0;

    update_sprite_rotation(dest);
}

int get_gravity_from_rotation(int rotation) {
    return -abs(rotation) + BASE_GRAVITY;
}

void update_position(Paper* dest, int delta_time_ms) {
    dest->x += (float)dest->rotation * ROTATION_MULTIPLIER * delta_time_ms / 1000;
    dest->y += (float)get_gravity_from_rotation(dest->rotation) * delta_time_ms / 1000;
}