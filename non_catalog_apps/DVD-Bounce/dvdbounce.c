#include <string.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include "dvd_bounce_icons.h"

//init some variables
int x = 0;
int y = 0;
int mode = 0;
bool bounce_up = false;
bool bounce_right = true;
char mode_str[12];

//the thing to draw to the screen
static void app_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);

    //draws the ball to positions x and y
    canvas_draw_icon(canvas, x, y, &I_Ok_btn_pressed_13x13);

    //displays the current mode
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 8, "Mode:");
    //converts mode int to string
    itoa(mode, mode_str, 10);
    canvas_draw_str(canvas, 28, 8, mode_str);
    switch(mode) {
    case 1:
        canvas_draw_str(canvas, 2, 16, "Left/Right");
        break;
    case 2:
        canvas_draw_str(canvas, 2, 16, "Up/Down");
        break;
    default:
        canvas_draw_str(canvas, 2, 16, "Normal");
        break;
    }
}

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t bounce_moment(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Configure viewport
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, app_draw_callback, view_port);
    view_port_input_callback_set(view_port, app_input_callback, event_queue);

    // Register viewport in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    InputEvent event;

    bool running = true;
    while(running) {
        if(furi_message_queue_get(event_queue, &event, 100) == FuriStatusOk) {
            if((event.type == InputTypePress) || (event.type == InputTypeRepeat)) {
                //arrows move the ball by 10 in their respective directions
                switch(event.key) {
                case InputKeyUp:
                    y += -10;
                    break;

                case InputKeyDown:
                    y += 10;
                    break;

                case InputKeyLeft:
                    x += -10;
                    break;

                case InputKeyRight:
                    x += 10;
                    break;
                //sets the ball to the middle of the screen and sets the current mode
                case InputKeyOk:
                    x = 51;
                    y = 19;
                    if(mode == 2) {
                        mode = 0;
                    } else {
                        mode += 1;
                    }
                    break;
                //exits the program if back is pressed
                default:
                    running = false;
                    break;
                }
            }
        }

        //bunch of conditionals determining how the ball should move
        if(x <= 0) {
            bounce_up = false;
        }
        if(x >= 115) {
            bounce_up = true;
        }
        if(y <= 0) {
            bounce_right = true;
        }
        if(y >= 51) {
            bounce_right = false;
        }

        if((bounce_up) && (mode != 2)) {
            x += -1;
        }
        if((!bounce_up) && (mode != 2)) {
            x += 1;
        }
        if((bounce_right) && (mode != 1)) {
            y += 1;
        }
        if((!bounce_right) && (mode != 1)) {
            y += -1;
        }

        view_port_update(view_port);
    }
    //cleanup go brrrrr
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_GUI);

    return 0;
}
