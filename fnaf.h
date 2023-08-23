#pragma once

#include <stdio.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>

#define TAG "flipperzero_fnaf"

#include "flipperzero_fnaf_icons.h"

#define SWITCH_VIEW(view) switch_view(fnaf, view)

typedef struct {
    uint8_t AI[4];
    uint8_t location[4];
    uint8_t foxy_timer;
    uint8_t freddy_position;
    uint8_t freddy_timer;
}Animatronics;

typedef struct {
    uint8_t left_door;
    uint8_t right_door;
    uint8_t left_light;
    uint8_t right_light;
    uint8_t monitor;

    uint8_t power_left;
    uint8_t power_draw;
}Electricity;

typedef struct {
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    Gui* gui;
    InputEvent event;

    uint8_t time;
    Animatronics* animatronics;
    short int camera_cursor;
    short int camera_cursor_x;
    short int camera_cursor_y;
    Electricity* electricity;

    uint16_t counter;
    uint8_t kitchen_counter;
    short int menu_cursor;
    uint8_t current_view;
    uint8_t progress; // Last completed night

    FuriTimer* timer_bonnie;
    FuriTimer* timer_chica;
    FuriTimer* timer_freddy;
    FuriTimer* timer_foxy;
} Fnaf;

typedef enum {
    Bonnie,
    Chica,
    Freddy,
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
