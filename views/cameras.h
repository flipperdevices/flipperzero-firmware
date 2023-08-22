#pragma once

#include <gui/gui.h>
#include "../fnaf.h"

#define camX(camera) camera_coordinates[camera*2]
#define camY(camera) camera_coordinates[camera*2 + 1]

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
}Cameras;

void draw_cameras(Canvas* canvas, Fnaf* fnaf);

void cameras_switching(Fnaf* fnaf);
