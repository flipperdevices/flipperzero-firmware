#include "office.h"
#include "../fnaf.h"

static void moving_animation(Fnaf* fnaf) {
    switch (fnaf->camera_moving_direction) {
    case left:
        if (fnaf->counter < 24) {
            fnaf->office_camera_x += 6;
            fnaf->counter += 6;
        } else {
            fnaf->camera_moving_direction = none;
            fnaf->counter = 0;
        }
        break;
    case right:
        if (fnaf->counter < 24) {
            fnaf->office_camera_x -= 6;
            fnaf->counter += 6;
        } else {
            fnaf->camera_moving_direction = none;
            fnaf->counter = 0;
        }
        break;
    default:
        // What
        break;
    }
}

void office_draw(Canvas* canvas, Fnaf* fnaf) {
    canvas_set_color(canvas, 1);
    if (fnaf->camera_moving_direction == none) {
        signed char position[3] = { 24, 0, -24 };
        fnaf->office_camera_x = position[fnaf->office_location + 1];
    }
    canvas_draw_icon(canvas, fnaf->office_camera_x, 0, &I_office_128x64);
    if (fnaf->camera_moving_direction != none) moving_animation(fnaf);
    canvas_set_color(canvas, 0);
    canvas_draw_box(canvas, 0, 0, 24, 64);
    canvas_draw_box(canvas, 104, 0, 24, 64);
    canvas_set_color(canvas, 1);
    // 24*64 rectangles
}

void office_input(Fnaf* fnaf) {
    if (fnaf->event.type == InputTypePress) {
        switch (fnaf->event.key) {
        case InputKeyLeft:
            if (fnaf->office_location != -1 && fnaf->camera_moving_direction == none) {
                fnaf->office_location -= 1;
                fnaf->camera_moving_direction = left;
            }
            break;
        case InputKeyRight:
            if (fnaf->office_location != 1 && fnaf->camera_moving_direction == none) {
                fnaf->office_location += 1;
                fnaf->camera_moving_direction = right;
            }
            break;
        case InputKeyUp:
            break;
        case InputKeyDown:
            break;
        case InputKeyOk:
            fnaf->camera_moving_direction = none;
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
        {4, 10, 12, 6},
        {0, 0, 0, 0},
    };
    // CUSTOM NIGHT INTERFACE WHEN
    SET_DIFFICULTY(difficulties[fnaf->progress]);
}

void night_start(Fnaf* fnaf) {
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
        fnaf->electricity->power_left = 999;
        fnaf->camera_cursor = cam1A;
        fnaf->hour = 0;
        fnaf->counter = 0;
        fnaf->office_camera_x = 0;
        fnaf->office_location = 0;
        fnaf->camera_moving_direction = none;
        SWITCH_VIEW(night_number);
        FURI_LOG_D(TAG, "Night started thing 2");
        // What else?
        // Aninatronics actions/timers? Probs 
    }
}

void reset_animatronic_positions(Fnaf* fnaf) {
    fnaf->animatronics->location[Bonnie] = cam1A;
    fnaf->animatronics->location[Chica] = cam1A;
    fnaf->animatronics->location[Freddy] = cam1A;
    fnaf->animatronics->location[Foxy] = cam1C;
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
        return;
    }
    FURI_LOG_D(TAG, "Hour is %u", fnaf->hour);
    fnaf->hour += 1;
    if (fnaf->hour == 6) {
        fnaf->progress += 1;
        dolphin_deed(DolphinDeedPluginGameWin);
        SWITCH_VIEW(night_complete);
        furi_timer_stop(fnaf->hourly_timer);
    }
}
