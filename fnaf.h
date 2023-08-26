#pragma once

#include <stdio.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>

#define TAG "flipperzero_fnaf"

#include "flipperzero_fnaf_icons.h"
#include "views/cameras.h"
#include "views/office.h"

#define bonnie_time 4970
#define chica_time 4980
#define freddy_time 3020
#define foxy_time 5010
#define hour_time 9000  // for tests, actually 90000
#define noise_time 4500 // something around this idk
#define power_time 1000

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
    uint8_t counter;
    FuriTimer* timer;
}Electricity;

typedef struct {
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    Gui* gui;
    InputEvent event;

    Animatronics* animatronics;
    uint8_t hour;
    Cameras* cameras;
    Office* office;
    Electricity* electricity;

    uint16_t counter; // general purpose
    uint8_t counter_secondary; // for speaker animation on cam6 and fan
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
    custom_night,
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

uint8_t power_draw(Fnaf* fnaf);
