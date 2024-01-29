#include "office.h"
#include "../fnaf.h"

static void draw_doors(Canvas* canvas, Fnaf* fnaf) {
    switch(fnaf->office->left_door_state) {
    case -1:
        if(fnaf->office->left_door_counter < 4) {
            fnaf->office->left_door_y -= 16;
            fnaf->office->left_door_counter += 1;
        } else {
            fnaf->office->left_door_counter = 0;
            fnaf->office->left_door_state = 0;
        }
        break;
    case 1:
        if(fnaf->office->left_door_counter < 4) {
            fnaf->office->left_door_y += 16;
            fnaf->office->left_door_counter += 1;
        } else {
            fnaf->office->left_door_counter = 0;
            fnaf->office->left_door_state = 0;
        }
        break;
    default:
        // it's not doing anything
        break;
    }
    switch(fnaf->office->right_door_state) {
    case -1:
        if(fnaf->office->right_door_counter < 4) {
            fnaf->office->right_door_y -= 16;
            fnaf->office->right_door_counter += 1;
        } else {
            fnaf->office->right_door_counter = 0;
            fnaf->office->right_door_state = 0;
        }
        break;
    case 1:
        if(fnaf->office->right_door_counter < 4) {
            fnaf->office->right_door_y += 16;
            fnaf->office->right_door_counter += 1;
        } else {
            fnaf->office->right_door_counter = 0;
            fnaf->office->right_door_state = 0;
        }
        break;
    default:
        // it's not doing anything
        break;
    }

    if(fnaf->electricity->left_door || fnaf->office->left_door_state) {
        canvas_set_color(canvas, 0);
        canvas_draw_icon(
            canvas,
            fnaf->office->camera_x + 4,
            fnaf->office->left_door_y - 4,
            &I_left_door_background_22x64);
        canvas_set_color(canvas, 1);
        canvas_draw_line(canvas, fnaf->office->camera_x + 24, 6, fnaf->office->camera_x + 25, 6);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 4, 0, &I_left_door_top_22x14);
        canvas_draw_icon(
            canvas, fnaf->office->camera_x + 4, fnaf->office->left_door_y, &I_left_door_22x53);
        canvas_set_color(canvas, 0);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 3, 0, &I_left_door_cover_24x14);
    }
    if(fnaf->electricity->right_door || fnaf->office->right_door_state) {
        canvas_set_color(canvas, 0);
        canvas_draw_icon(
            canvas,
            fnaf->office->camera_x + 101,
            fnaf->office->right_door_y - 4,
            &I_right_door_background_23x64);
        canvas_set_color(canvas, 1);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 101, 0, &I_right_door_top_23x14);
        canvas_draw_icon(
            canvas, fnaf->office->camera_x + 101, fnaf->office->right_door_y, &I_right_door_23x53);
        canvas_set_color(canvas, 0);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 101, 0, &I_right_door_cover_23x14);
    }

    // Door layer should be between background and front ones

    // Thing for door animation
}

static void moving_animation(Fnaf* fnaf) {
    switch(fnaf->office->camera_moving_direction) {
    case left:
        if(fnaf->counter < 24) {
            fnaf->office->camera_x += 8;
            fnaf->counter += 8;
        } else {
            fnaf->office->camera_moving_direction = none;
            fnaf->counter = 0;
        }
        break;
    case right:
        if(fnaf->counter < 24) {
            fnaf->office->camera_x -= 8;
            fnaf->counter += 8;
        } else {
            fnaf->office->camera_moving_direction = none;
            fnaf->counter = 0;
        }
        break;
    default:
        // What
        break;
    }
}

static void power_out(Canvas* canvas, Fnaf* fnaf) {
    if(fnaf->electricity->power_left != 0) {
        FURI_LOG_D(TAG, "power_out called, power: %u", fnaf->electricity->power_left);
        furi_crash("");
    }
    if(fnaf->office->camera_moving_direction != none) moving_animation(fnaf);
    if(fnaf->office->left_door_state == 0) fnaf->office->left_door_y = -54;
    if(fnaf->office->right_door_state == 0) fnaf->office->right_door_y = -54;
    if(fnaf->office->is_light_on) {
        canvas_set_color(canvas, 1);
        canvas_draw_icon(canvas, fnaf->office->camera_x, 0, &I_office_128x64);
        if(fnaf->counter_secondary > furi_get_tick() % 7 + 1) {
            canvas_draw_icon(canvas, fnaf->office->camera_x + 11, 23, &I_power_out_flipper_8x7);
            if(fnaf->counter_secondary > furi_get_tick() % 15 + 1) {
                fnaf->counter_secondary = 0;
            }
        } else {
            fnaf->counter_secondary += 1;
        }
        draw_doors(canvas, fnaf);
        canvas_set_color(canvas, 0);
        canvas_draw_box(canvas, fnaf->office->camera_x + 4, 47, 22, 15);
        canvas_draw_box(canvas, fnaf->office->camera_x + 101, 28, 23, 31);
    } else {
        canvas_set_color(canvas, 1);
        canvas_draw_line(canvas, fnaf->office->camera_x + 26, 15, fnaf->office->camera_x + 26, 53);
        canvas_draw_line(
            canvas, fnaf->office->camera_x + 100, 15, fnaf->office->camera_x + 100, 53);
    }
    canvas_set_color(canvas, 2);
    canvas_draw_box(canvas, 0, 0, 128, 64);

    canvas_set_color(canvas, 0);
    canvas_draw_box(canvas, 0, 0, 23, 64);
    canvas_draw_box(canvas, 105, 0, 23, 64);

    canvas_set_color(canvas, 1);
    fnaf->counter_music_box += 1;
    if(fnaf->counter_music_box > 3) {
        canvas_draw_icon(canvas, 108, 3, &I_music_box_17x17);
        if(fnaf->counter_music_box > 6) {
            fnaf->counter_music_box = 0;
        }
    }
}

static void draw_sides(Canvas* canvas, Fnaf* fnaf) {
    if(!fnaf->electricity->left_light) {
        canvas_set_color(canvas, 1);
        canvas_draw_icon(canvas, fnaf->office->camera_x, 0, &I_left_door_dark_28x64);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 29, 15, &I_left_window_dark_13x25);
    } else if(fnaf->dolphins->location[Blipper] == left_entrance) {
        canvas_set_color(canvas, 1);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 4, 21, &I_blipper_office_36x39);
    }
    if(!fnaf->electricity->right_light) {
        canvas_set_color(canvas, 1);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 100, 0, &I_right_door_dark_28x64);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 85, 15, &I_right_window_dark_13x25);
    } else if(fnaf->dolphins->location[Chipper] == right_entrance) {
        canvas_set_color(canvas, 1);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 87, 24, &I_chipper_window_9x12);
    }

    draw_doors(canvas, fnaf);
}

void office_draw(Canvas* canvas, void* ctx) {
    Fnaf* fnaf = ctx;
    if(fnaf->electricity->power_left == 0) {
        power_out(canvas, fnaf);
        return;
    }
    char time[7];
    if(fnaf->hour != 0)
        snprintf(time, 7, "%u AM", fnaf->hour);
    else
        snprintf(time, 7, "12 AM");
    char power[7];
    snprintf(power, 7, "%u%%", fnaf->electricity->power_left / 10);

    if(fnaf->office->camera_moving_direction == none) {
        signed char position[3] = {24, 0, -24};
        fnaf->office->camera_x = position[fnaf->office->location + 1];
    }
    if(fnaf->office->camera_moving_direction != none) moving_animation(fnaf);
    canvas_set_color(canvas, 1);
    canvas_draw_icon(canvas, fnaf->office->camera_x, 0, &I_office_128x64);
    // fan animation:
    if(fnaf->counter_secondary < 3) {
        canvas_set_color(canvas, 0);
        canvas_draw_box(canvas, fnaf->office->camera_x + 67, 26, 11, 11);
        canvas_set_color(canvas, 1);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 67, 26, &I_fan_11x11);
        fnaf->counter_secondary += 1;
    } else {
        fnaf->counter_secondary += 1;
        if(fnaf->counter_secondary > 6) fnaf->counter_secondary = 0;
    }

    draw_sides(canvas, fnaf);

    if(fnaf->dolphins->location[Fopper] == 4 && !fnaf->electricity->left_door) {
        if(fnaf->dolphins->fopper_counter < 4) {
            if(fnaf->dolphins->fopper_counter == 0) {
                fnaf->dolphins->fopper_counter = -8;
            }
            fnaf->dolphins->fopper_counter += 3;
        } else {
            SWITCH_VIEW(game_over);
        }
        canvas_set_color(canvas, 0);
        canvas_draw_box(
            canvas, fnaf->office->camera_x + fnaf->dolphins->fopper_counter + 4, 60, 2, 2);
        canvas_set_color(canvas, 2);
        canvas_draw_icon(
            canvas,
            fnaf->office->camera_x + fnaf->dolphins->fopper_counter,
            21,
            &I_fopper_doorway_12x43);
        canvas_set_color(canvas, 0);
        canvas_draw_box(canvas, fnaf->office->camera_x, 0, 3, 64);
        canvas_set_color(canvas, 1);
        canvas_draw_line(canvas, fnaf->office->camera_x + 3, 18, fnaf->office->camera_x + 3, 64);
    }

    // UI:
    canvas_set_color(canvas, 0);
    canvas_draw_box(canvas, 0, 0, 23, 64);
    canvas_draw_box(canvas, 105, 0, 23, 64);
    canvas_set_color(canvas, 1);
    canvas_draw_str_aligned(
        canvas, fnaf->office->camera_x + 64, 10, AlignCenter, AlignBottom, time);
    canvas_draw_str_aligned(canvas, 125, 62, AlignRight, AlignBottom, power);
    uint8_t x = 17;
    uint8_t power_usage = power_draw(fnaf);
    if(power_usage < 5) {
        for(uint8_t i = 0; i < power_usage; i++) {
            canvas_draw_box(canvas, x, 55, 4, 7);
            x -= 5;
        }
    }

    // make a timer (one-time) for left and right sound
    // and draw if the correct one is running
    if(furi_timer_is_running(fnaf->office->left_door_sound_timer))
        canvas_draw_icon(canvas, 3, 3, &I_speaker_17x17);
    if(furi_timer_is_running(fnaf->office->right_door_sound_timer))
        canvas_draw_icon(canvas, 108, 3, &I_speaker_17x17);
    if(furi_timer_is_running(fnaf->office->flipper_laugh_timer))
        canvas_draw_icon(canvas, 108, 23, &I_flipper_17x17);
}

static void close_door(Fnaf* fnaf, uint8_t door) {
    FURI_LOG_D(TAG, "Closing door %u", door);
    if(fnaf->electricity->power_left == 0) return;
    switch(door) {
    case 0: // left door
        if(fnaf->electricity->left_door) break;
        fnaf->office->left_door_state = 1;
        fnaf->office->left_door_y = -54;
        break;
    case 1: // right door
        if(fnaf->electricity->right_door) break;
        fnaf->office->right_door_state = 1;
        fnaf->office->right_door_y = -54;
        break;
    }
}

static void open_door(Fnaf* fnaf, uint8_t door) {
    FURI_LOG_D(TAG, "Opening door %u", door);
    switch(door) {
    case 0: // left door
        if(!fnaf->electricity->left_door) return;
        // if (!fnaf->office->left_door_state) break;
        fnaf->office->left_door_state = -1;
        fnaf->office->left_door_y = 10;
        break;
    case 1: // right door
        if(!fnaf->electricity->right_door) return;
        // if (!fnaf->office->right_door_state) break;
        fnaf->office->right_door_state = -1;
        fnaf->office->right_door_y = 10;
        break;
    }
}

static void door_input(Fnaf* fnaf) {
    switch(fnaf->office->location) {
    case -1:
        // Doesn't work if Blipper is inside
        if(fnaf->dolphins->location[Blipper] == office_location) break;

        if(!fnaf->office->left_door_state) {
            if(fnaf->electricity->left_door) {
                open_door(fnaf, 0);
            } else {
                close_door(fnaf, 0);
            }
            fnaf->electricity->left_door = !fnaf->electricity->left_door;
            fnaf->office->left_door_counter = 0;
        }
        break;
    case 0:
        break;
    case 1:
        // Doesn't work if Chipper is inside
        if(fnaf->dolphins->location[Chipper] == office_location) break;

        if(!fnaf->office->right_door_state) {
            if(fnaf->electricity->right_door) {
                open_door(fnaf, 1);
            } else {
                close_door(fnaf, 1);
            }
            fnaf->electricity->right_door = !fnaf->electricity->right_door;
            fnaf->office->right_door_counter = 0;
        }
        break;
    }
}

void office_input(void* ctx) {
    Fnaf* fnaf = ctx;
    if(fnaf->event.type == InputTypePress) {
        switch(fnaf->event.key) {
        case InputKeyLeft:
            if(fnaf->office->location != -1 && fnaf->office->camera_moving_direction == none) {
                fnaf->office->location -= 1;
                fnaf->office->camera_moving_direction = left;
            }
            break;
        case InputKeyRight:
            if(fnaf->office->location != 1 && fnaf->office->camera_moving_direction == none) {
                fnaf->office->location += 1;
                fnaf->office->camera_moving_direction = right;
            }
            break;
        case InputKeyUp:
            if(fnaf->electricity->power_left > 0) door_input(fnaf);
            break;
        case InputKeyDown:
            if(fnaf->electricity->power_left > 0) switch(fnaf->office->location) {
                case -1:
                    // Doesn't work if Blipper is inside
                    if(fnaf->dolphins->location[Blipper] == office_location) break;

                    fnaf->electricity->left_light = !fnaf->electricity->left_light;
                    fnaf->electricity->right_light = false;
                    break;
                case 0:
                    break;
                case 1:
                    // Doesn't work if Chipper is inside
                    if(fnaf->dolphins->location[Chipper] == office_location) break;

                    fnaf->electricity->right_light = !fnaf->electricity->right_light;
                    fnaf->electricity->left_light = false;
                    break;
                }
            break;
        case InputKeyOk:
            if(fnaf->electricity->power_left == 0) break;
            fnaf->office->camera_moving_direction = none;
            fnaf->electricity->left_light = false;
            fnaf->electricity->right_light = false;
            fnaf->electricity->monitor = true;
            if(furi_timer_is_running(fnaf->dolphins->fopper_inactivity))
                furi_timer_stop(fnaf->dolphins->fopper_inactivity);
            SWITCH_VIEW(cameras_view);

            if(fnaf->dolphins->flipper_move_state == 2 &&
               fnaf->dolphins->location[Flipper] == cam4B && fnaf->cameras->cursor != cam4B) {
                flipper_move(fnaf);
            }
            break;
        case InputKeyBack:
            break;
        default:
            break;
        }
    }
}

void set_difficulty(Fnaf* fnaf) {
    uint8_t difficulties[7][4] = {
        {0, 0, 0, 0},
        {0, 3, 1, 1},
        {1, 0, 5, 2},
        {1, 2, 4, 6},
        {3, 5, 7, 9},
        {4, 10, 12, 16},
        {0, 0, 0, 0},
    };
    // Flipper has AI of random 1 or 2 for the 4th night
    difficulties[3][Flipper] = rand() % 2 + 1;
    // CUSTOM NIGHT INTERFACE WHEN
    if(!fnaf->custom_night) {
        SET_DIFFICULTY(difficulties[fnaf->progress]);
    }
}

void reset_animatronic_positions(void* ctx) {
    Fnaf* fnaf = ctx;
    fnaf->dolphins->location[Blipper] = cam1A;
    fnaf->dolphins->location[Chipper] = cam1A;
    fnaf->dolphins->location[Flipper] = cam1A;
    fnaf->dolphins->location[Fopper] = 0;
}

void night_start(void* ctx) {
    Fnaf* fnaf = ctx;
    FURI_LOG_D(TAG, "night_start");
    FURI_LOG_D(TAG, "progress = %u", fnaf->progress);
    if(fnaf->progress > 5) {
        fnaf->progress = 5;
        SWITCH_VIEW(main_menu);
    } else {
        FURI_LOG_D(TAG, "Night started thing");
        dolphin_deed(DolphinDeedPluginGameStart);
        reset_animatronic_positions(fnaf);
        set_difficulty(fnaf);
        fnaf->cameras->cursor = cam1A;
        fnaf->hour = 0;
        fnaf->counter = 0;
        fnaf->counter_secondary = 0;
        fnaf->counter_music_box = 1;
        fnaf->dolphins->fopper_times_blocked = 0;
        fnaf->dolphins->fopper_counter = 0;
        fnaf->dolphins->flipper_move_state = 0;
        fnaf->dolphins->flipper_might_move_counter = 0;
        fnaf->office->camera_x = 0;
        fnaf->office->location = 0;
        fnaf->electricity->left_door = false;
        fnaf->electricity->right_door = false;
        fnaf->electricity->left_light = false;
        fnaf->electricity->right_light = false;
        fnaf->electricity->monitor = false;
        fnaf->electricity->power_left = 999;
        fnaf->office->camera_moving_direction = none;
        fnaf->office->left_door_state = 0;
        fnaf->office->right_door_state = 0;
        fnaf->office->left_door_counter = 0;
        fnaf->office->right_door_counter = 0;
        fnaf->office->left_door_y = -54;
        fnaf->office->right_door_y = -54;
        fnaf->office->is_light_on = true;
        SWITCH_VIEW(night_number);
        FURI_LOG_D(TAG, "Night started thing end");
    }
}

void timer_callback_blipper(void* ctx) {
    Fnaf* fnaf = ctx;
    switch(fnaf->dolphins->location[Blipper]) {
    case office_location:
        // Death
        FURI_LOG_D(TAG, "Blipper got you");
        stop_all_timers(fnaf);
        stop_hourly_timer(fnaf);
        save_progress(fnaf);
        SWITCH_VIEW(jumpscare);
        break;
    case left_entrance:
        if(rand() % 20 + 1 > fnaf->dolphins->AI[Blipper]) return;
        // If left door is closed reset do dining area
        if(fnaf->electricity->left_door) {
            FURI_LOG_D(TAG, "Blipper is back to Dining Area!");
            fnaf->dolphins->location[Blipper] = cam1B;
            return;
        }
        // If moves go to office
        fnaf->dolphins->location[Blipper] = office_location;
        FURI_LOG_D(TAG, "Blipper is in the office!");
        // Restart timer for office wait time
        if(furi_timer_is_running(fnaf->dolphins->timer[Blipper])) {
            furi_timer_stop(fnaf->dolphins->timer[Blipper]);
        }
        furi_timer_start(fnaf->dolphins->timer[Blipper], blipper_chipper_wait_time);
        // turn off the light and open left door just in case
        fnaf->electricity->left_light = false;
        if(fnaf->electricity->left_door) open_door(fnaf, 0);
        break;
    default:
        if(rand() % 20 + 1 > fnaf->dolphins->AI[Blipper]) return;
        Locations move[2][9] = {
            {cam5, cam5, 0xF, cam3, cam3, left_entrance, 0xF, 0xF, cam1B},
            {cam1B, cam2A, 0xF, cam2B, left_entrance, cam2A, 0xF, 0xF, cam2A}};
        uint8_t location =
            move[fnaf->dolphins->move_rand_value][fnaf->dolphins->location[Blipper]];
        FURI_LOG_D(
            TAG, "Blipper moved from %u to %u", fnaf->dolphins->location[Blipper], location);
        fnaf->dolphins->location[Blipper] = location;
        if(location == cam2B || location == left_entrance)
            furi_timer_start(fnaf->office->left_door_sound_timer, sound_time);
        break;
    }
}

void timer_callback_chipper(void* ctx) {
    Fnaf* fnaf = ctx;
    switch(fnaf->dolphins->location[Chipper]) {
    case office_location:
        // Death
        FURI_LOG_D(TAG, "Chipper got you");
        stop_all_timers(fnaf);
        stop_hourly_timer(fnaf);
        save_progress(fnaf);
        SWITCH_VIEW(jumpscare);
        break;
    case right_entrance:
        if(rand() % 20 + 1 > fnaf->dolphins->AI[Chipper]) return;
        // If left door is closed step back to east hall
        if(fnaf->electricity->right_door) {
            FURI_LOG_D(TAG, "Chipper is back to east hall!");
            fnaf->dolphins->location[Chipper] = cam4A;
            return;
        }
        // If moves go to office
        fnaf->dolphins->location[Chipper] = office_location;
        FURI_LOG_D(TAG, "Chipper is in the office!");
        // Restart timer for office wait time
        if(furi_timer_is_running(fnaf->dolphins->timer[Chipper])) {
            furi_timer_stop(fnaf->dolphins->timer[Chipper]);
        }
        furi_timer_start(fnaf->dolphins->timer[Chipper], blipper_chipper_wait_time);
        // turn off the light and open right door just in case
        fnaf->electricity->right_light = false;
        if(fnaf->electricity->right_door) open_door(fnaf, 1);
        break;
    default:
        if(rand() % 20 + 1 > fnaf->dolphins->AI[Chipper]) return;
        Locations move[2][11] = {
            {cam1B, cam7, 0xF, 0xF, 0xF, 0xF, cam1B, cam4A, 0xF, cam7, cam6},
            {cam1B, cam6, 0xF, 0xF, 0xF, 0xF, cam4B, right_entrance, 0xF, cam4A, cam4A}};
        uint8_t location =
            move[fnaf->dolphins->move_rand_value][fnaf->dolphins->location[Chipper]];
        FURI_LOG_D(
            TAG, "Chipper moved from %u to %u", fnaf->dolphins->location[Chipper], location);
        fnaf->dolphins->location[Chipper] = location;
        if(location == cam4B || location == right_entrance)
            furi_timer_start(fnaf->office->right_door_sound_timer, sound_time);
        break;
    }
}

void timer_callback_flipper(void* ctx) {
    Fnaf* fnaf = ctx;
    if(fnaf->dolphins->location[Flipper] == office_location) {
        if(fnaf->current_view == cameras_view) return;
        if(rand() % 4 == 0) {
            save_progress(fnaf);
            stop_all_timers(fnaf);
            stop_hourly_timer(fnaf);
            SWITCH_VIEW(jumpscare);
        }
        return;
    }
    if(fnaf->current_view == cameras_view) {
        if(fnaf->dolphins->location[Flipper] != cam4B) return;
        if(fnaf->cameras->cursor == cam4B && fnaf->dolphins->location[Flipper] == cam4B) return;
    }
    if(rand() % 20 + 1 > fnaf->dolphins->AI[Flipper]) return;
    if(fnaf->dolphins->flipper_move_state != 0) return;
    // Start move timer
    FURI_LOG_D(TAG, "Flipper might move! State = 1");
    fnaf->dolphins->flipper_move_state = 1;
    fnaf->dolphins->flipper_might_move_counter = 0;
    furi_timer_start(fnaf->dolphins->flipper_might_move_timer, flipper_might_move_time);
}

void flipper_might_move_callback(void* ctx) {
    Fnaf* fnaf = ctx;
    if(fnaf->dolphins->flipper_move_state == 1) {
        fnaf->dolphins->flipper_might_move_counter += 10;
    }
    if(fnaf->dolphins->flipper_might_move_counter > 1000 - (100 * fnaf->dolphins->AI[Flipper])) {
        if(furi_timer_is_running(fnaf->dolphins->flipper_might_move_timer))
            furi_timer_stop(fnaf->dolphins->flipper_might_move_timer);
        FURI_LOG_D(TAG, "Flipper is ready to move!");
        fnaf->dolphins->flipper_move_state = 2;
        fnaf->dolphins->flipper_might_move_counter = 0;
        switch(fnaf->current_view) {
        case office_view:
            /* Flipper can't move if at cam4B unless
                we're looking at camera (not cam4B) */
            if(fnaf->dolphins->location[Flipper] != cam4B) {
                flipper_move(fnaf);
            }
            break;
        case cameras_view:
            /* Flipper can't move if player's
                watching cam4B */
            if(fnaf->dolphins->location[Flipper] == cam4B && fnaf->cameras->cursor != cam4B) {
                flipper_move(fnaf);
            }
            break;
        }
    }
}

void flipper_move(void* ctx) {
    Fnaf* fnaf = ctx;
    // A check
    if(fnaf->dolphins->flipper_move_state != 2) {
        FURI_LOG_D(
            TAG,
            "Something happened?? flipper_move but state = %u",
            fnaf->dolphins->flipper_move_state);
        return;
    }
    // Move location
    switch(fnaf->dolphins->location[Flipper]) {
    case cam1A:
        fnaf->dolphins->location[Flipper] = cam1B;
        break;
    case cam1B:
        fnaf->dolphins->location[Flipper] = cam7;
        break;
    case cam7:
        fnaf->dolphins->location[Flipper] = cam6;
        break;
    case cam6:
        fnaf->dolphins->location[Flipper] = cam4A;
        break;
    case cam4A:
        fnaf->dolphins->location[Flipper] = cam4B;
        break;
    case cam4B:
        if(fnaf->electricity->right_door)
            fnaf->dolphins->location[Flipper] = cam4A;
        else
            fnaf->dolphins->location[Flipper] = office_location;
        break;
    }

    if(fnaf->dolphins->location[Flipper] == office_location) {
        if(furi_timer_is_running(fnaf->dolphins->timer[Flipper]))
            furi_timer_stop(fnaf->dolphins->timer[Flipper]);
        furi_timer_start(fnaf->dolphins->timer[Flipper], flipper_office_time);
    } else {
        furi_timer_start(fnaf->office->flipper_laugh_timer, sound_time);
    }

    FURI_LOG_D(TAG, "Flipper moved to %u", fnaf->dolphins->location[Flipper]);
    // Reset things
    fnaf->dolphins->flipper_might_move_counter = 0;
    fnaf->dolphins->flipper_move_state = 0;
    furi_timer_stop(fnaf->dolphins->flipper_might_move_timer);
}

void timer_callback_fopper(void* ctx) {
    Fnaf* fnaf = ctx;
    if(!furi_timer_is_running(fnaf->dolphins->fopper_inactivity) &&
       fnaf->current_view != cameras_view && fnaf->dolphins->location[Fopper] < 3) {
        if(rand() % 20 + 1 > fnaf->dolphins->AI[Fopper]) return;
        fnaf->dolphins->location[Fopper] += 1;
        FURI_LOG_D(TAG, "Fopper state is %u", fnaf->dolphins->location[Fopper]);
        if(fnaf->dolphins->location[Fopper] == 3) {
            // If Fopper is now waiting at cam2A
            // Restart his timer but with wait time instead
            if(furi_timer_is_running(fnaf->dolphins->timer[Fopper])) {
                furi_timer_stop(fnaf->dolphins->timer[Fopper]);
            }
            furi_timer_start(fnaf->dolphins->timer[Fopper], fopper_wait_time);
            FURI_LOG_D(TAG, "Fopper's waiting...");
            return;
        }
    }
    if(fnaf->dolphins->location[Fopper] == 3) {
        FURI_LOG_D(TAG, "Fopper finished running!");
        fnaf->dolphins->location[Fopper] = 4;
    }
    if(fnaf->dolphins->location[Fopper] == 4) {
        if(fnaf->electricity->left_door) {
            // Start timer for noise icon
            furi_timer_start(fnaf->office->left_door_sound_timer, sound_time);
            FURI_LOG_D(TAG, "Fopper blocked");
            // Reduce electricity by 1 + 5 * (times blocked)
            fnaf->electricity->power_left -= 10 + fnaf->dolphins->fopper_times_blocked * 5;
            // Reset Fopperm increment attack counter
            fnaf->dolphins->location[Fopper] = 0;
            fnaf->dolphins->fopper_times_blocked += 1;
            // Restart timer
            if(furi_timer_is_running(fnaf->dolphins->timer[Fopper])) {
                furi_timer_stop(fnaf->dolphins->timer[Fopper]);
            }
            furi_timer_start(fnaf->dolphins->timer[Fopper], fopper_time);
        } else {
            if(fnaf->current_view == cameras_view) SWITCH_VIEW(office_view);
            FURI_LOG_D(TAG, "Fopper got you");
            stop_all_timers(fnaf);
            stop_hourly_timer(fnaf);
            save_progress(fnaf);
        }
    }
}

void move_rand_callback(void* ctx) {
    Fnaf* fnaf = ctx;
    fnaf->dolphins->move_rand_value = rand() % 2;
}

void power_timer_callback(void* ctx) {
    Fnaf* fnaf = ctx;
    fnaf->electricity->power_left -= power_draw(fnaf);
    if(fnaf->electricity->power_left > 999 || fnaf->electricity->power_left == 0) {
        fnaf->electricity->power_left = 0;
        stop_all_timers(fnaf);
        if(fnaf->current_view != office_view) SWITCH_VIEW(office_view);
        if(fnaf->electricity->left_door) open_door(fnaf, 0);
        if(fnaf->electricity->right_door) open_door(fnaf, 1);
        fnaf->counter_secondary = 0;
        fnaf->office->is_light_on = true;
        furi_timer_start(fnaf->office->power_out_timer, power_out_time_period);
        furi_timer_start(fnaf->office->power_out_max_timer, power_out_max_time);
        return;
    }
    switch(fnaf->progress) {
    case 1:
        if(fnaf->electricity->counter == 5) {
            fnaf->electricity->power_left -= 1;
            fnaf->electricity->counter = 0;
        } else {
            fnaf->electricity->counter += 1;
        }
        break;
    case 2:
        if(fnaf->electricity->counter == 4) {
            fnaf->electricity->power_left -= 1;
            fnaf->electricity->counter = 0;
        } else {
            fnaf->electricity->counter += 1;
        }
        break;
    case 3:
        if(fnaf->electricity->counter == 3) {
            fnaf->electricity->power_left -= 1;
            fnaf->electricity->counter = 0;
        } else {
            fnaf->electricity->counter += 1;
        }
        break;
    case 4:
    case 5:
    case 6:
        if(fnaf->electricity->counter == 2) {
            fnaf->electricity->power_left -= 1;
            fnaf->electricity->counter = 0;
        } else {
            fnaf->electricity->counter += 1;
        }
        break;
    }

    fnaf->electricity->counter += 1;
}

void hourly_timer_callback(void* ctx) {
    Fnaf* fnaf = ctx;
    if(fnaf->current_view == main_menu) {
        stop_all_timers(fnaf);
        stop_hourly_timer(fnaf);
        FURI_LOG_D(TAG, "Error kekx in hourly callback");
        furi_delay_ms(10);
        return;
    }
    FURI_LOG_D(TAG, "Hour was %u", fnaf->hour);
    fnaf->hour += 1;
    switch(fnaf->hour) {
    case 1:
        if(furi_timer_is_running(fnaf->hourly_timer)) {
            FURI_LOG_D(TAG, "Hourly timer stopped (90s)");
            furi_timer_stop(fnaf->hourly_timer);
        }
        furi_timer_start(fnaf->hourly_timer, hour_time);
        FURI_LOG_D(TAG, "Hourly timer started (89s)");
        break;
    case 2:
        fnaf->dolphins->AI[Blipper] += 1;
        break;
    case 3:
    case 4:
        fnaf->dolphins->AI[Blipper] += 1;
        fnaf->dolphins->AI[Chipper] += 1;
        fnaf->dolphins->AI[Fopper] += 1;
        break;
    case 6:
        fnaf->progress += 1;
        if(fnaf->progress > 5) fnaf->progress = 5;
        save_progress(fnaf);
        dolphin_deed(DolphinDeedPluginGameWin);
        SWITCH_VIEW(night_complete);
        stop_all_timers(fnaf);
        stop_hourly_timer(fnaf);
    }
    FURI_LOG_D(TAG, "Hour is %u", fnaf->hour);
}

void power_out_callback(void* ctx) {
    Fnaf* fnaf = ctx;
    FURI_LOG_D(TAG, "WORKING");
    if(rand() % 5 + 1 == 1) {
        FURI_LOG_D(TAG, "DEF WORKING");
        // I'm sleepy and tired so a lot of comments
        if(fnaf->office->is_light_on) {
            // stop max timer
            if(furi_timer_is_running(fnaf->office->power_out_max_timer)) {
                FURI_LOG_D(TAG, "Power out timer took less than 20 seconds");
                furi_timer_stop(fnaf->office->power_out_max_timer);
            }
            // start max timer again
            furi_timer_start(fnaf->office->power_out_max_timer, power_out_max_time);
            // set is_light_on to false
            fnaf->office->is_light_on = false;
        } else {
            // stop all timers
            stop_all_timers(fnaf);
            // save progress
            save_progress(fnaf);
            // jumpscare
            SWITCH_VIEW(jumpscare);
        }
    }
}

void power_out_max_callback(void* ctx) {
    Fnaf* fnaf = ctx;

    if(fnaf->office->is_light_on) {
        // light off
        fnaf->office->is_light_on = false;
        // stop that timer and start it again
        if(furi_timer_is_running(fnaf->office->power_out_timer)) {
            FURI_LOG_D(TAG, "Power out timer took more than 20 seconds");
            furi_timer_stop(fnaf->office->power_out_timer);
        }
        furi_timer_start(fnaf->office->power_out_timer, power_out_time_period);
        furi_timer_start(fnaf->office->power_out_max_timer, power_out_max_time);
    } else {
        FURI_LOG_D(TAG, "Power out timer 2 took more than 20 seconds");
        // Save progress
        save_progress(fnaf);
        // Stop all timers
        stop_all_timers(fnaf);
        // Jumpscare
        SWITCH_VIEW(jumpscare);
    }
}
