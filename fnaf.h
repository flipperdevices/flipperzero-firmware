#pragma once

#include <stdio.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>

#define TAG "flipperzero_fnaf"

#include "flipperzero_fnaf_icons.h"

typedef struct {
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    Gui* gui;
    InputEvent event;

    uint8_t counter;
    short int menu_cursor;
    uint8_t current_view;
    uint8_t progress; // Last completed night
} Fnaf;

typedef enum {
    main_menu,
    office,
    cameras,
    screamer,
    game_over,
}Views;


typedef struct {
    uint8_t Bonnie;
    uint8_t Chika;
    uint8_t Freddy;
    uint8_t Foxy;
}NightDifficulty;

// Array for difficulties. Night 7 is custom remember
// NightDifficulty night_difficulty = {};

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

typedef struct {
    uint8_t x;
    uint8_t y;
}Coordinates;

// Coordinates cameras[] = { {26, 3}, {23, 13} };
