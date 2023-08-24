#pragma once

#include <stdio.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>

#define TAG "flipperzero_fnaf"

#include "flipperzero_fnaf_icons.h"

#define bonnie_time 4970
#define chica_time 4980
#define freddy_time 3020
#define foxy_time 5010
#define hour_time 1000 // for tests, actually 90000

#define SWITCH_VIEW(view) switch_view(fnaf, view)

typedef struct {
    uint8_t AI[4];
    uint8_t location[4];

    FuriTimer* timer[4];
}Animatronics;

typedef struct {
    bool left_door;
    bool right_door;
    bool left_light;
    bool right_light;
    bool monitor;

    uint16_t power_left;
    uint8_t power_draw;
}Electricity;

typedef enum {
    left,
    none,
    right,
}CameraMovement;

typedef struct {
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    Gui* gui;
    InputEvent event;

    Animatronics* animatronics;
    signed char camera_cursor;
    signed char camera_cursor_x;
    signed char camera_cursor_y;
    uint8_t hour;
    Electricity* electricity;
    uint8_t office_camera_x; // coordinate for drawing

    uint16_t counter; // general purpose
    signed char office_location; // left, center or right
    CameraMovement camera_moving_direction;
    uint8_t kitchen_counter; // for speaker animation on cam6
    signed char menu_cursor; // for the main menu
    uint8_t current_view;
    uint8_t progress; // Last completed nights
    FuriTimer* hourly_timer;
} Fnaf;

typedef enum {
    Freddy,
    Bonnie,
    Chica,
    Foxy,
}Names;

typedef enum {
    main_menu,
    night_number,
    office,
    cameras,
    night_complete,
    screamer,
    game_over,
}Views;

typedef struct {
    uint8_t Bonnie;
    uint8_t Chika;
    uint8_t Freddy;
    uint8_t Foxy;
}NightDifficulty;

void switch_view(Fnaf* fnaf, Views view);

// Array for difficulties. Night 7 is custom remember
// NightDifficulty night_difficulty = {};
