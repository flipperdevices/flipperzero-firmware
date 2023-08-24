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
    uint8_t camera_x; // coordinate for drawing
    signed char location; // left, center or right
    CameraMovement camera_moving_direction; // for office view
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
