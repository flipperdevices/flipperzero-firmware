#include <gui/gui.h>
#include "../fnaf.h"
#include "cameras.h"

void draw_cameras(Canvas* canvas, void* ctx) {
    Fnaf* fnaf = ctx;
    uint8_t camera_coordinates[22] = {
        26, 3, 23, 13, 17, 28, 26, 47, 26, 55, 11, 44, 45, 47, 45, 55, 3, 18, 62, 42, 64, 18,
    };

    char camera_names[11][7] = {
        "Cam 1A",
        "Cam 1B",
        "Cam 1C",
        "Cam 2A",
        "Cam 2B",
        "Cam 3",
        "Cam 4A",
        "Cam 4B",
        "Cam 5",
        "Cam 6",
        "Cam 7"};

    canvas_draw_icon(canvas, -1, 0, &I_cameras_78x64);
    canvas_draw_box(canvas, camX(fnaf->cameras->cursor), camY(fnaf->cameras->cursor), 7, 4);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 84, 10, camera_names[fnaf->cameras->cursor]);
    canvas_set_font(canvas, FontSecondary);

    char time[7];
    if(fnaf->hour != 0)
        snprintf(time, 7, "%u AM", fnaf->hour);
    else
        snprintf(time, 7, "12 AM");
    canvas_draw_str_aligned(canvas, 125, 52, AlignRight, AlignBottom, time);
    if(fnaf->cameras->cursor != cam6 && fnaf->cameras->cursor != cam1C) {
        uint8_t y = 20;
        if(fnaf->dolphins->location[Blipper] == fnaf->cameras->cursor) {
            canvas_draw_str(canvas, 85, y, "Blipper");
            y += 9;
        }
        if(fnaf->dolphins->location[Chipper] == fnaf->cameras->cursor) {
            canvas_draw_str(canvas, 85, y, "Chipper");
            y += 9;
        }
        if(fnaf->dolphins->location[Flipper] == fnaf->cameras->cursor) {
            canvas_draw_str(canvas, 85, y, "Flipper");
            y += 9;
        }
        if((fnaf->dolphins->location[Fopper] == 3) && fnaf->cameras->cursor == cam2A) {
            FURI_LOG_D(TAG, "Fopper runs");
            canvas_draw_str(canvas, 85, y, "Fopper");
            if(furi_timer_is_running(fnaf->dolphins->timer[Fopper])) {
                furi_timer_stop(fnaf->dolphins->timer[Fopper]);
            }
            furi_timer_start(fnaf->dolphins->timer[Fopper], fopper_run_time);
            y += 9;
        }
    } else if(fnaf->cameras->cursor == cam6) {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 85, 20, "Audio only");

        canvas_draw_icon(canvas, 87, 23, &I_speaker_22x19);

        if(fnaf->dolphins->location[Chipper] == cam6) {
            if(fnaf->counter_secondary > 5) {
                canvas_set_color(canvas, 0);
                canvas_draw_box(canvas, 99, 23, 10, 19);
                if(fnaf->counter_secondary > 10) fnaf->counter_secondary = 0;
            }
            fnaf->counter_secondary += 1;
        } else {
            canvas_set_color(canvas, 0);
            canvas_draw_box(canvas, 99, 23, 10, 19);
        }
        if(fnaf->dolphins->location[Flipper] == cam6) {
            if(fnaf->counter_music_box < 6) {
                canvas_set_color(canvas, 1);
                canvas_draw_icon(canvas, 110, 24, &I_music_box_17x17);
            }
            if(fnaf->counter_music_box > 11) fnaf->counter_music_box = 1;
            fnaf->counter_music_box += 1;
        }
    } else if(fnaf->cameras->cursor == cam1C) {
        switch(fnaf->dolphins->location[Fopper]) {
        case 0:
            canvas_set_font(canvas, FontSecondary);
            canvas_draw_str(canvas, 85, 20, "Curtains");
            canvas_draw_str(canvas, 85, 29, "are closed");
            break;
        case 1:
            canvas_set_font(canvas, FontSecondary);
            canvas_draw_str(canvas, 85, 20, "Curtains");
            canvas_draw_str(canvas, 85, 29, "are open");
            canvas_draw_str(canvas, 85, 38, "a bit");
            break;
        case 2:
            canvas_set_font(canvas, FontSecondary);
            canvas_draw_str(canvas, 85, 20, "Curtains");
            canvas_draw_str(canvas, 85, 29, "are open");
            canvas_draw_str(canvas, 85, 38, "fully");
            break;
        case 3:
        case 4:
            canvas_set_font(canvas, FontSecondary);
            canvas_draw_str(canvas, 85, 20, "No one");
            canvas_draw_str(canvas, 85, 29, "is here");
            break;
        }
    }

    canvas_set_color(canvas, 1);

    char power[7];
    snprintf(power, 7, "%u%%", fnaf->electricity->power_left / 10);
    canvas_draw_str_aligned(canvas, 125, 62, AlignRight, AlignBottom, power);

    uint8_t x = 98;
    uint8_t power_usage = power_draw(fnaf);
    if(power_usage < 5) {
        for(uint8_t i = 0; i < power_usage; i++) {
            canvas_draw_box(canvas, x, 55, 4, 7);
            x -= 5;
        }
    }
}

static void set_cursor(Fnaf* fnaf) {
    uint8_t cursors_x[11] = {1, 1, 1, 1, 1, 0, 2, 2, 0, 3, 3};
    uint8_t cursors_y[11] = {0, 1, 2, 4, 5, 4, 4, 5, 1, 4, 0};
    fnaf->cameras->cursor_x = cursors_x[fnaf->cameras->cursor];
    fnaf->cameras->cursor_y = cursors_y[fnaf->cameras->cursor];
}

static void switch_to_office(Fnaf* fnaf) {
    SWITCH_VIEW(office_view);
    fnaf->electricity->monitor = false;
    if(furi_timer_is_running(fnaf->dolphins->fopper_inactivity))
        furi_timer_stop(fnaf->dolphins->fopper_inactivity);
    if(fnaf->dolphins->location[Fopper] < 3) {
        uint32_t time = (rand() % 951 + 50) * 1000 / 60;
        furi_timer_start(fnaf->dolphins->fopper_inactivity, time);
        FURI_LOG_D(TAG, "Fopper inactivity timer is set to %lu ms", time);
    }
    if(fnaf->dolphins->location[Blipper] == office_location) {
        FURI_LOG_D(TAG, "Blipper got you");
        stop_all_timers(fnaf);
        stop_hourly_timer(fnaf);
        save_progress(fnaf);
        SWITCH_VIEW(jumpscare);
    }
    if(fnaf->dolphins->location[Chipper] == office_location) {
        FURI_LOG_D(TAG, "Chipper got you");
        stop_all_timers(fnaf);
        stop_hourly_timer(fnaf);
        save_progress(fnaf);
        SWITCH_VIEW(jumpscare);
    }
    if(fnaf->dolphins->flipper_move_state == 2) {
        if(fnaf->dolphins->location[Flipper] != cam4B) {
            flipper_move(fnaf);
        } else if(fnaf->cameras->cursor != cam4B) {
            flipper_move(fnaf);
        }
    }
}

void cameras_input(void* ctx) {
    Fnaf* fnaf = ctx;

    set_cursor(fnaf);

    uint8_t cameras_map[6][4] = {
        {cam5, cam1A, cam7, cam7},
        {cam5, cam1B, cam7, 127},
        {cam5, cam1C, cam7, 127},
        {cam1C, 127, 127, cam7},
        {cam3, cam2A, cam4A, cam6},
        {cam3, cam2B, cam4B, cam6}};

    if(fnaf->event.type == InputTypePress) {
        switch(fnaf->event.key) {
        case InputKeyLeft:
            if(fnaf->cameras->cursor_x == 0) break;
            if(fnaf->cameras->cursor == cam7) {
                fnaf->cameras->cursor_x = 1;
                fnaf->cameras->cursor_y = 1;
                break;
            }
            fnaf->cameras->cursor_x -= 1;
            break;
        case InputKeyRight:
            if(fnaf->cameras->cursor_x == 3) break;
            fnaf->cameras->cursor_x += 1;
            break;
        case InputKeyUp:
            if(fnaf->cameras->cursor_y == 0 &&
               (fnaf->cameras->cursor != cam5 && fnaf->cameras->cursor != cam7))
                break;
            if(fnaf->cameras->cursor == cam2A) {
                fnaf->cameras->cursor_x = 1;
                fnaf->cameras->cursor_y = 2;
                break;
            } else if(fnaf->cameras->cursor == cam4A) {
                fnaf->cameras->cursor_x = 1;
                fnaf->cameras->cursor_y = 2;
                break;
            } else if(fnaf->cameras->cursor == cam7) {
                fnaf->cameras->cursor_x = 1;
                fnaf->cameras->cursor_y = 0;
                break;
            } else if(fnaf->cameras->cursor == cam5) {
                fnaf->cameras->cursor_x = 1;
                fnaf->cameras->cursor_y = 0;
                break;
            }
            fnaf->cameras->cursor_y -= 1;
            break;
        case InputKeyDown:
            if(fnaf->cameras->cursor_y == 5) break;
            if(fnaf->cameras->cursor == cam7) {
                fnaf->cameras->cursor_x = 3;
                fnaf->cameras->cursor_y = 4;
                break;
            } else if(fnaf->cameras->cursor == cam5) {
                fnaf->cameras->cursor_x = 1;
                fnaf->cameras->cursor_y = 2;
                break;
            } else if(fnaf->cameras->cursor == cam3) {
                fnaf->cameras->cursor_x = 1;
                fnaf->cameras->cursor_y = 5;
                break;
            } else if(fnaf->cameras->cursor == cam6) {
                fnaf->cameras->cursor_x = 2;
                fnaf->cameras->cursor_y = 5;
                break;
            } else if(fnaf->cameras->cursor == cam1C) {
                fnaf->cameras->cursor_x = 0;
                fnaf->cameras->cursor_y = 4;
                break;
            }
            fnaf->cameras->cursor_y += 1;
            break;
        case InputKeyOk:
        case InputKeyBack:
            switch_to_office(fnaf);
            break;
        default:
            break;
        }
    }

    fnaf->cameras->cursor = cameras_map[fnaf->cameras->cursor_y][fnaf->cameras->cursor_x];

    // Extra check just in case
    if(fnaf->cameras->cursor_x < 0)
        fnaf->cameras->cursor_x = 0;
    else if(fnaf->cameras->cursor_x > 3)
        fnaf->cameras->cursor_x = 3;
    if(fnaf->cameras->cursor_y < 0)
        fnaf->cameras->cursor_y = 0;
    else if(fnaf->cameras->cursor_y > 5)
        fnaf->cameras->cursor_y = 5;
}
