#pragma once 

#include <gui/gui.h>
#include <dolphin/dolphin.h>
#include "cameras.h"

#define SET_DIFFICULTY(difficulties)  \
        fnaf->animatronics->AI[Bonnie] = difficulties[Bonnie];\
        fnaf->animatronics->AI[Chica] = difficulties[Chica];\
        fnaf->animatronics->AI[Freddy] = difficulties[Freddy];\
        fnaf->animatronics->AI[Foxy] = difficulties[Foxy]

typedef enum {
    left,
    none,
    right,
}CameraMovement;

typedef struct {
    uint8_t camera_x;                       // coordinate for drawing
    signed char location;                   // left, center or right
    CameraMovement camera_moving_direction; // for office view
    signed char left_door_y;                // for door animation
    signed char right_door_y;               // for door animation
    uint8_t left_door_counter;              // for door animation
    uint8_t right_door_counter;             // for door animation
    signed char left_door_state;    // -1 is up, 1 is down, 0 is static
    signed char right_door_state;   // -1 is up, 1 is down, 0 is static
}Office;


void office_draw(Canvas* canvas, void* ctx);

void office_input(void* ctx);

void night_start(void* ctx);

void reset_animatronic_positions(void* ctx);

void timer_callback_bonnie(void* ctx);

void timer_callback_chica(void* ctx);

void timer_callback_freddy(void* ctx);

void timer_callback_foxy(void* ctx);

void hourly_timer_callback(void* ctx);
