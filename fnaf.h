#pragma once

#include <stdio.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <storage/storage.h>

#define TAG "flipperzero_fnaf"

#include "flipperzero_fnaf_icons.h"
#include "views/cameras.h"
#include "views/office.h"

#define blipper_time 4970
#define chipper_time 4980
#define flipper_time 3020
#define flipper_might_move_time 167
#define flipper_office_time 1000
#define fopper_time 5010
#define fopper_wait_time 25000
#define fopper_run_time 2000 // not sure
#define blipper_chipper_wait_time 30000
#define first_hour_time 90000
#define hour_time 89000
#define sound_time 1500
#define power_time 1000
#define power_out_time_period 5000
#define power_out_max_time 20000
#define move_rand_time 1000

#define SWITCH_VIEW(view) switch_view(fnaf, view)

typedef struct {
    uint8_t AI[4];
    uint8_t location[4];
    FuriTimer* timer[4];

    FuriTimer* fopper_inactivity;
    uint8_t fopper_times_blocked;
    signed char fopper_counter;
    uint8_t flipper_move_state;
    uint16_t flipper_might_move_counter;
    FuriTimer* flipper_might_move_timer;

    FuriTimer* move_rand_timer;
    uint8_t move_rand_value;
}Dolphins;

typedef struct {
    bool left_door;
    bool right_door;
    bool left_light;
    bool right_light;
    bool monitor;
    uint16_t power_left;
    uint8_t counter; // for extra drain on nights 2-7
    FuriTimer* timer;
}Electricity;

typedef struct {
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    Gui* gui;
    InputEvent event;
    Storage* storage;
    File* save_data;

    Dolphins* dolphins;
    Cameras* cameras;
    Office* office;
    Electricity* electricity;

    uint8_t hour; // current time
    uint16_t counter; // general purpose
    uint8_t counter_secondary; // for speaker animation on cam6 and fan
    uint8_t counter_music_box; // for music box 
    signed char menu_cursor; // for the main menu
    uint8_t custom_menu_cursor; // for custom night menu
    uint8_t current_view;
    uint8_t progress; // Last completed nights
    bool custom_night;
    FuriTimer* hourly_timer;
} Fnaf;

typedef enum {
    Flipper,
    Blipper,
    Chipper,
    Fopper,
}Names;

typedef enum {
    main_menu,
    custom_night,
    night_number,
    office_view,
    cameras,
    night_complete,
    jumpscare,
    game_over,
}Views;

void stop_hourly_timer(Fnaf* fnaf);

void stop_all_timers(Fnaf* fnaf);

void switch_view(Fnaf* fnaf, Views view);

uint8_t power_draw(Fnaf* fnaf);

void save_progress(Fnaf* fnaf);

void load_progress(Fnaf* fnaf);
