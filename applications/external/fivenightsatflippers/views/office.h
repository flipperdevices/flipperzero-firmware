#pragma once

#include <gui/gui.h>
#include <dolphin/dolphin.h>
#include "cameras.h"

#define SET_DIFFICULTY(difficulties)                     \
    fnaf->dolphins->AI[Blipper] = difficulties[Blipper]; \
    fnaf->dolphins->AI[Chipper] = difficulties[Chipper]; \
    fnaf->dolphins->AI[Flipper] = difficulties[Flipper]; \
    fnaf->dolphins->AI[Fopper] = difficulties[Fopper]

typedef enum {
    left,
    none,
    right,
} CameraMovement;

typedef struct {
    int8_t camera_x; // coordinate for drawing
    signed char location; // left, center or right
    CameraMovement camera_moving_direction; // for office view
    signed char left_door_y; // for door animation
    signed char right_door_y; // for door animation
    uint8_t left_door_counter; // for door animation
    uint8_t right_door_counter; // for door animation
    signed char left_door_state; // -1 is up, 1 is down, 0 is static
    signed char right_door_state; // -1 is up, 1 is down, 0 is static
    FuriTimer* left_door_sound_timer; // for icon
    FuriTimer* right_door_sound_timer; // for icon
    FuriTimer* flipper_laugh_timer; // for icon
    FuriTimer* power_out_timer; // for when power goes out
    FuriTimer* power_out_max_timer; // for when power goes out
    bool is_light_on; // used when power goes out
} Office;

void office_draw(Canvas* canvas, void* ctx);

void office_input(void* ctx);

void night_start(void* ctx);

void reset_animatronic_positions(void* ctx);

void timer_callback_blipper(void* ctx);

void timer_callback_chipper(void* ctx);

void timer_callback_flipper(void* ctx);

void flipper_might_move_callback(void* ctx);

void flipper_move(void* ctx);

void timer_callback_fopper(void* ctx);

void move_rand_callback(void* ctx);

void power_timer_callback(void* ctx);

void hourly_timer_callback(void* ctx);

void power_out_callback(void* ctx);

void power_out_max_callback(void* ctx);
