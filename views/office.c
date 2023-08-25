#include "office.h"
#include "../fnaf.h"

static void moving_animation(Fnaf* fnaf) {
    switch (fnaf->office->camera_moving_direction) {
    case left:
        if (fnaf->counter < 24) {
            fnaf->office->camera_x += 6;
            fnaf->counter += 6;
        } else {
            fnaf->office->camera_moving_direction = none;
            fnaf->counter = 0;
        }
        break;
    case right:
        if (fnaf->counter < 24) {
            fnaf->office->camera_x -= 6;
            fnaf->counter += 6;
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

static void draw_doors(Canvas* canvas, Fnaf* fnaf) {

    switch (fnaf->office->left_door_state) {
    case -1:
        if (fnaf->office->left_door_counter < 4) {
            fnaf->office->left_door_y -= 16;
            fnaf->office->left_door_counter += 1;
        } else {
            fnaf->office->left_door_counter = 0;
            fnaf->office->left_door_state = 0;
        }
        break;
    case 1:
        if (fnaf->office->left_door_counter < 4) {
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
    switch (fnaf->office->right_door_state) {
    case -1:
        if (fnaf->office->right_door_counter < 4) {
            fnaf->office->right_door_y -= 16;
            fnaf->office->right_door_counter += 1;
        } else {
            fnaf->office->right_door_counter = 0;
            fnaf->office->right_door_state = 0;
        }
        break;
    case 1:
        if (fnaf->office->right_door_counter < 4) {
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

    if (fnaf->electricity->left_door || fnaf->office->left_door_state) {
        canvas_set_color(canvas, 0);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 4, fnaf->office->left_door_y - 4, &I_left_door_background_22x65);
        canvas_set_color(canvas, 1);
        canvas_draw_line(canvas, fnaf->office->camera_x + 24, 6, fnaf->office->camera_x + 25, 6);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 4, 0, &I_left_door_top_22x14);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 4, fnaf->office->left_door_y, &I_left_door_22x53);
        canvas_set_color(canvas, 0);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 3, 0, &I_left_door_cover_24x14);
    }
    if (fnaf->electricity->right_door || fnaf->office->right_door_state) {
        canvas_set_color(canvas, 0);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 101, fnaf->office->right_door_y - 4, &I_right_door_background_23x65);
        canvas_set_color(canvas, 1);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 101, 0, &I_right_door_top_23x14);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 101, fnaf->office->right_door_y, &I_right_door_23x53);
        canvas_set_color(canvas, 0);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 101, 0, &I_right_door_cover_23x14);
    }

    // Door layer should be between background and front ones

    // Thing for door animation
}

static void draw_sides(Canvas* canvas, Fnaf* fnaf) {
    if (!fnaf->electricity->left_light) {
        canvas_set_color(canvas, 1);
        canvas_draw_icon(canvas, fnaf->office->camera_x, 0, &I_left_door_dark_28x64);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 29, 15, &I_left_window_dark_13x25);
    } else if (fnaf->animatronics->location[Bonnie] == left_entrance) {
        canvas_set_color(canvas, 1);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 4, 20, &I_bonnie_office_37x39);
    }
    if (!fnaf->electricity->right_light) {
        canvas_set_color(canvas, 1);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 100, 0, &I_right_door_dark_28x64);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 85, 15, &I_right_window_dark_13x25);
    } else if (fnaf->animatronics->location[Chica] == right_entrance) {
        canvas_set_color(canvas, 1);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 87, 20, &I_chica_window_9x16);
    }

    draw_doors(canvas, fnaf);

}

void office_draw(Canvas* canvas, void* ctx) {
    Fnaf* fnaf = ctx;
    char time[8];
    snprintf(time, 11, "0%u:00", fnaf->hour);
    char power[7];
    snprintf(power, 7, "%u%%", fnaf->electricity->power_left / 10);

    if (fnaf->office->camera_moving_direction == none) {
        signed char position[3] = { 24, 0, -24 };
        fnaf->office->camera_x = position[fnaf->office->location + 1];
    }
    if (fnaf->office->camera_moving_direction != none) moving_animation(fnaf);
    canvas_set_color(canvas, 1);
    canvas_draw_icon(canvas, fnaf->office->camera_x, 0, &I_office_128x64);
    // fan animation:
    if (fnaf->counter_secondary < 3) {
        canvas_set_color(canvas, 0);
        canvas_draw_box(canvas, fnaf->office->camera_x + 67, 26, 11, 11);
        canvas_set_color(canvas, 1);
        canvas_draw_icon(canvas, fnaf->office->camera_x + 67, 26, &I_fan_11x11);
        fnaf->counter_secondary += 1;
    } else {
        fnaf->counter_secondary += 1;
        if (fnaf->counter_secondary > 6) fnaf->counter_secondary = 0;
    }

    draw_sides(canvas, fnaf);

    // UI:
    canvas_set_color(canvas, 0);
    canvas_draw_box(canvas, 0, 0, 23, 64);
    canvas_draw_box(canvas, 105, 0, 23, 64);
    canvas_set_color(canvas, 1);
    canvas_draw_str(canvas, fnaf->office->camera_x + 52, 10, time);
    canvas_draw_str(canvas, 107, 62, power);
    uint8_t x = 17;
    if (fnaf->electricity->power_draw < 5) {
        for (uint8_t i = 0; i < fnaf->electricity->power_draw; i++) {
            canvas_draw_box(canvas, x, 55, 4, 7);
            x -= 5;
        }
    }
}

static void door_input(Fnaf* fnaf) {
    switch (fnaf->office->location) {
    case -1:
        if (!fnaf->office->left_door_state) {
            if (fnaf->electricity->left_door) {
                fnaf->office->left_door_state = -1;
                fnaf->office->left_door_y = 10;
            } else {
                fnaf->office->left_door_state = 1;
                fnaf->office->left_door_y = -54;
            }
            fnaf->electricity->left_door = !fnaf->electricity->left_door;
            fnaf->office->left_door_counter = 0;
        }
        break;
    case 0:
        break;
    case 1:
        if (!fnaf->office->right_door_state) {
            if (fnaf->electricity->right_door) {
                fnaf->office->right_door_state = -1;
                fnaf->office->right_door_y = 10;
            } else {
                fnaf->office->right_door_state = 1;
                fnaf->office->right_door_y = -54;
            }
            fnaf->electricity->right_door = !fnaf->electricity->right_door;
            fnaf->office->right_door_counter = 0;
        }
        break;
    }

}

void office_input(void* ctx) {
    Fnaf* fnaf = ctx;
    if (fnaf->event.type == InputTypePress) {
        switch (fnaf->event.key) {
        case InputKeyLeft:
            if (fnaf->office->location != -1 && fnaf->office->camera_moving_direction == none) {
                fnaf->office->location -= 1;
                fnaf->office->camera_moving_direction = left;
            }
            break;
        case InputKeyRight:
            if (fnaf->office->location != 1 && fnaf->office->camera_moving_direction == none) {
                fnaf->office->location += 1;
                fnaf->office->camera_moving_direction = right;
            }
            break;
        case InputKeyUp:
            door_input(fnaf);
            break;
        case InputKeyDown:
            switch (fnaf->office->location) {
            case -1:
                fnaf->electricity->left_light = !fnaf->electricity->left_light;
                break;
            case 0:
                break;
            case 1:
                fnaf->electricity->right_light = !fnaf->electricity->right_light;
                break;
            }
            break;
        case InputKeyOk:
            fnaf->office->camera_moving_direction = none;
            fnaf->electricity->left_light = false;
            fnaf->electricity->right_light = false;
            SWITCH_VIEW(cameras);
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
    // Freddy has AI of random 1 or 2 for the 4th night
    difficulties[3][Freddy] = furi_get_tick() % 2 + 1;
    // CUSTOM NIGHT INTERFACE WHEN
    SET_DIFFICULTY(difficulties[fnaf->progress]);
}

void night_start(void* ctx) {
    Fnaf* fnaf = ctx;
    FURI_LOG_D(TAG, "night_start");
    FURI_LOG_D(TAG, "progress = %u", fnaf->progress);
    if (fnaf->progress > 6) {
        fnaf->progress = 0;
        SWITCH_VIEW(main_menu);
    } else {
        FURI_LOG_D(TAG, "Night started thing");
        dolphin_deed(DolphinDeedPluginGameStart);
        reset_animatronic_positions(fnaf);
        set_difficulty(fnaf);
        fnaf->cameras->cursor = cam1A;
        fnaf->hour = 0;
        fnaf->counter = 0;
        fnaf->office->camera_x = 0;
        fnaf->office->location = 0;
        fnaf->electricity->left_door = false;
        fnaf->electricity->right_door = false;
        fnaf->electricity->left_light = false;
        fnaf->electricity->right_light = false;
        fnaf->electricity->monitor = false;
        fnaf->electricity->power_left = 999;
        fnaf->electricity->power_draw = 1;
        fnaf->office->camera_moving_direction = none;
        fnaf->office->left_door_state = 0;
        fnaf->office->right_door_state = 0;
        fnaf->office->left_door_counter = 0;
        fnaf->office->right_door_counter = 0;
        fnaf->office->left_door_y = -54;
        fnaf->office->right_door_y = -54;
        SWITCH_VIEW(night_number);
        FURI_LOG_D(TAG, "Night started thing 2");
        // What else?
        // Aninatronics actions/timers? Probs
    }
}

void reset_animatronic_positions(void* ctx) {
    Fnaf* fnaf = ctx;
    fnaf->animatronics->location[Bonnie] = cam1A;
    fnaf->animatronics->location[Chica] = cam1A;
    fnaf->animatronics->location[Freddy] = cam1A;
    fnaf->animatronics->location[Foxy] = 0;
}

void timer_callback_bonnie(void* ctx) {
    UNUSED(ctx);
}

void timer_callback_chica(void* ctx) {
    UNUSED(ctx);
}

void timer_callback_freddy(void* ctx) {
    UNUSED(ctx);
}

void timer_callback_foxy(void* ctx) {
    UNUSED(ctx);
}

void hourly_timer_callback(void* ctx) {
    Fnaf* fnaf = ctx;
    if (fnaf->current_view == main_menu) {
        furi_timer_stop(fnaf->hourly_timer);
        FURI_LOG_D(TAG, "Error kekx in hourly callback");
        furi_delay_ms(10);
        return;
    }
    FURI_LOG_D(TAG, "Hour was %u", fnaf->hour);
    fnaf->hour += 1;
    switch (fnaf->hour) {
    case 2:
        fnaf->animatronics->AI[Bonnie] += 1;
        break;
    case 3:
    case 4:
        fnaf->animatronics->AI[Bonnie] += 1;
        fnaf->animatronics->AI[Chica] += 1;
        fnaf->animatronics->AI[Foxy] += 1;
        break;
    case 6:
        fnaf->progress += 1;
        dolphin_deed(DolphinDeedPluginGameWin);
        SWITCH_VIEW(night_complete);
        furi_timer_stop(fnaf->hourly_timer);
    }
    FURI_LOG_D(TAG, "Hour is %u", fnaf->hour);
}
