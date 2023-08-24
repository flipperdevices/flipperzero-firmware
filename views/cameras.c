#include <gui/gui.h>
#include "../fnaf.h"
#include "cameras.h"

void draw_cameras(Canvas* canvas, Fnaf* fnaf) {

    uint8_t camera_coordinates[22] = { 26, 3,
                                    23, 13,
                                    17, 28,
                                    26, 47,
                                    26, 55,
                                    11, 44,
                                    45, 47,
                                    45, 55,
                                    3, 18,
                                    62, 42,
                                    64, 18,
    };

    char camera_names[11][7] = { "Cam 1A", "Cam 1B", "Cam 1C", "Cam 2A", "Cam 2B", "Cam 3", "Cam 4A", "Cam 4B", "Cam 5", "Cam 6", "Cam 7" };

    canvas_draw_icon(canvas, -1, 0, &I_cameras_78x64);
    canvas_draw_box(canvas, camX(fnaf->camera_cursor), camY(fnaf->camera_cursor), 7, 4);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 84, 10, camera_names[fnaf->camera_cursor]);
    canvas_set_font(canvas, FontSecondary);

    char time[11];
    snprintf(time, 11, "0%u:00 AM", fnaf->hour);
    canvas_draw_str(canvas, 85, 52, time);
    if (fnaf->camera_cursor != cam6) {
        uint8_t y = 20;
        if (fnaf->animatronics->location[Bonnie] == fnaf->camera_cursor) {
            canvas_draw_str(canvas, 85, y, "Bonnie");
            y += 9;
        }
        if (fnaf->animatronics->location[Chica] == fnaf->camera_cursor) {
            canvas_draw_str(canvas, 85, y, "Chica");
            y += 9;
        }
        if (fnaf->animatronics->location[Freddy] == fnaf->camera_cursor) {
            canvas_draw_str(canvas, 85, y, "Freddy");
            y += 9;
        }
    } else {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 85, 20, "Audio only");

        canvas_draw_icon(canvas, 94, 23, &I_speaker_22x19);

        if (fnaf->animatronics->location[Chica] == cam6) {
            if (fnaf->kitchen_counter > 7) {
                canvas_set_color(canvas, 0);
                canvas_draw_box(canvas, 106, 23, 10, 19);
            }
            fnaf->kitchen_counter += 1;
            if (fnaf->kitchen_counter > 14) fnaf->kitchen_counter = 0;
        } else {
            canvas_set_color(canvas, 0);
            canvas_draw_box(canvas, 106, 23, 10, 19);
        }
    }

    canvas_set_color(canvas, 1);

    char power[7];
    snprintf(power, 7, "%u%%", fnaf->electricity->power_left / 10);
    canvas_draw_str(canvas, 104, 62, power);

    uint8_t x = 98;
    if (fnaf->electricity->power_draw < 6) {
        for (uint8_t i = 0; i < fnaf->electricity->power_draw; i++) {
            canvas_draw_box(canvas, x, 55, 4, 7);
            x -= 5;
        }
    }
}

static void set_cursor(Fnaf* fnaf) {

    uint8_t cursors_x[11] = { 1, 1, 1, 1, 1, 0, 2, 2, 0, 3, 3 };
    uint8_t cursors_y[11] = { 0, 1, 2, 4, 5, 4, 4, 5, 1, 4, 0 };
    fnaf->camera_cursor_x = cursors_x[fnaf->camera_cursor];
    fnaf->camera_cursor_y = cursors_y[fnaf->camera_cursor];

}

void cameras_input(Fnaf* fnaf) {

    set_cursor(fnaf);

    uint8_t cameras_map[6][4] = { {cam5, cam1A, cam7, cam7},
                                    {cam5, cam1B, cam7, 127},
                                    {cam5, cam1C, cam7, 127},
                                    {cam1C, 127, 127, cam7},
                                    {cam3, cam2A, cam4A, cam6},
                                    {cam3, cam2B, cam4B, cam6}
    };

    if (fnaf->event.type == InputTypePress) {
        switch (fnaf->event.key) {
        case InputKeyLeft:
            if (fnaf->camera_cursor_x == 0) break;
            if (fnaf->camera_cursor == cam7) {
                fnaf->camera_cursor_x = 1;
                fnaf->camera_cursor_y = 1;
                break;
            }
            fnaf->camera_cursor_x -= 1;
            break;
        case InputKeyRight:
            if (fnaf->camera_cursor_x == 3) break;
            fnaf->camera_cursor_x += 1;
            break;
        case InputKeyUp:
            if (fnaf->camera_cursor_y == 0 && (fnaf->camera_cursor != cam5 && fnaf->camera_cursor != cam7)) break;
            if (fnaf->camera_cursor == cam2A) {
                fnaf->camera_cursor_x = 1;
                fnaf->camera_cursor_y = 2;
                break;
            } else if (fnaf->camera_cursor == cam4A) {
                fnaf->camera_cursor_x = 1;
                fnaf->camera_cursor_y = 2;
                break;
            } else if (fnaf->camera_cursor == cam7) {
                fnaf->camera_cursor_x = 1;
                fnaf->camera_cursor_y = 0;
                break;
            } else if (fnaf->camera_cursor == cam5) {
                fnaf->camera_cursor_x = 1;
                fnaf->camera_cursor_y = 0;
                break;
            }
            fnaf->camera_cursor_y -= 1;
            break;
        case InputKeyDown:
            if (fnaf->camera_cursor_y == 5) break;
            if (fnaf->camera_cursor == cam7) {
                fnaf->camera_cursor_x = 3;
                fnaf->camera_cursor_y = 4;
                break;
            } else if (fnaf->camera_cursor == cam5) {
                fnaf->camera_cursor_x = 1;
                fnaf->camera_cursor_y = 2;
                break;
            } else if (fnaf->camera_cursor == cam3) {
                fnaf->camera_cursor_x = 1;
                fnaf->camera_cursor_y = 5;
                break;
            } else if (fnaf->camera_cursor == cam6) {
                fnaf->camera_cursor_x = 2;
                fnaf->camera_cursor_y = 5;
                break;
            } else if (fnaf->camera_cursor == cam1C) {
                fnaf->camera_cursor_x = 0;
                fnaf->camera_cursor_y = 4;
                break;
            }
            fnaf->camera_cursor_y += 1;
            break;
        case InputKeyOk:
            SWITCH_VIEW(office);
            break;
        default:
            break;
        }
    }

    fnaf->camera_cursor = cameras_map[fnaf->camera_cursor_y][fnaf->camera_cursor_x];

    // Extra check just in case
    if (fnaf->camera_cursor_x < 0) fnaf->camera_cursor_x = 0;
    else if (fnaf->camera_cursor_x > 3) fnaf->camera_cursor_x = 3;
    if (fnaf->camera_cursor_y < 0) fnaf->camera_cursor_y = 0;
    else if (fnaf->camera_cursor_y > 5) fnaf->camera_cursor_y = 5;

}
