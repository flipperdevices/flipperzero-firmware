#pragma once

#include <gui/gui.h>

#define camX(camera) camera_coordinates[camera * 2]
#define camY(camera) camera_coordinates[camera * 2 + 1]

typedef enum {
    cam1A,
    cam1B,
    cam1C,
    cam2A,
    cam2B,
    cam3,
    cam4A,
    cam4B,
    cam5,
    cam6,
    cam7,
    left_entrance,
    right_entrance,
    office_location,

    error = 0xF,
} Locations;

typedef struct {
    Locations cursor; // current camera
    signed char cursor_x; // for camera navigation
    signed char cursor_y; // for camera navigation too
} Cameras;

void draw_cameras(Canvas* canvas, void* ctx);

void cameras_input(void* ctx);

void noise_callback(void* ctx);
