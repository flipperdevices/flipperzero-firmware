#include <string.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include "financial_calc_icons.h"

//init some variables / constants

//pinpad variables for movement and stuff
int pinpad_x = 0;
int pinpad_y = 0;
int pinpad_values[4][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {-1, 0, -1}};
// pinpad constants for display
const int pinpad_x_base_offset = 84;
const int pinpad_y_base_offset = 4;
const int pinpad_x_offset = 15;
const int pinpad_y_offset = 12;

//TVM values
float tvm_n = 0;
float tvm_i = 0;
float tvm_pv = 0;
float tvm_pmt = 0;
float tvm_fv = 0;

// TVM cursor variables
int tvm_cursor_position = 0;
bool tvm_cpt_now = false;
// TVM cursor display Constants
const int tvm_cursor_x_base_offset = 1;
const int tvm_cursor_y_base_offset = 4;
const int tvm_cursor_y_offset = 12;
const int tvm_cpt_x_base_offset = 1;
const int tvm_cpt_y_base_offset = 9;
// Image of the CPT button. Too small for a regular font
const uint8_t CPT_IMAGE[] =
    {0x76, 0x07, 0x91, 0x02, 0x91, 0x02, 0x71, 0x02, 0x11, 0x02, 0x11, 0x02, 0x16, 0x02};
const uint8_t DECIMAL_IMAGE[] = {0x03, 0x03};

//the thing to draw to the screen
static void app_draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    // Clear the canvas at the start of a draw
    canvas_clear(canvas);
    canvas_set_bitmap_mode(canvas, true);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 6, 11, "N");
    canvas_draw_str(canvas, 6, 47, "PMT");
    canvas_draw_str(canvas, 6, 59, "FV");
    canvas_draw_str(canvas, 6, 35, "PV");
    canvas_draw_str(canvas, 6, 23, "I%");

    canvas_draw_line(canvas, 0, 13, 79, 13);
    canvas_draw_line(canvas, 0, 25, 79, 25);
    canvas_draw_line(canvas, 0, 37, 79, 37);
    canvas_draw_line(canvas, 0, 49, 79, 49);
    canvas_draw_line(canvas, 0, 61, 79, 61);

    canvas_draw_str(canvas, 26, 47, "=");
    canvas_draw_str(canvas, 26, 59, "=");
    canvas_draw_str(canvas, 26, 35, "=");
    canvas_draw_str(canvas, 26, 23, "=");
    canvas_draw_str(canvas, 26, 12, "=");

    canvas_draw_frame(canvas, 82, 2, 46, 61);
    canvas_draw_line(canvas, 97, 3, 97, 62);
    canvas_draw_line(canvas, 126, 14, 83, 14);

    canvas_draw_str(canvas, 89, 12, "1");
    canvas_draw_str(canvas, 103, 12, "2");
    canvas_draw_str(canvas, 118, 12, "3");
    canvas_draw_str(canvas, 88, 24, "4");
    canvas_draw_str(canvas, 103, 24, "5");
    canvas_draw_str(canvas, 118, 24, "6");
    canvas_draw_str(canvas, 88, 36, "7");
    canvas_draw_str(canvas, 103, 36, "8");
    canvas_draw_str(canvas, 118, 36, "9");
    canvas_draw_str(canvas, 88, 48, "-");
    canvas_draw_str(canvas, 103, 48, "0");
    canvas_draw_icon(canvas, 87, 52, &I_I_Pin_Arrow_Up);
    canvas_draw_icon(canvas, 117, 52, &I_I_Pin_Arrow_Down);
    canvas_draw_xbm(canvas, 105, 57, 2, 2, DECIMAL_IMAGE);
    canvas_draw_xbm(canvas, 114, 41, 11, 7, CPT_IMAGE);
    canvas_draw_line(canvas, 83, 50, 126, 50);
    canvas_draw_line(canvas, 79, 2, 0, 2);
    canvas_draw_line(canvas, 126, 26, 83, 26);
    canvas_draw_line(canvas, 127, 38, 83, 38);
    canvas_draw_line(canvas, 112, 3, 112, 61);

    //now draw the stuff that moves

    // tvm cursor
    canvas_draw_frame(
        canvas,
        tvm_cursor_x_base_offset,
        tvm_cursor_y_base_offset + (tvm_cursor_position * tvm_cursor_y_offset),
        4,
        4);
    // cpt cursor
    if(tvm_cpt_now) {
        canvas_draw_box(
            canvas,
            tvm_cpt_x_base_offset,
            tvm_cpt_y_base_offset + (tvm_cursor_position * tvm_cursor_y_offset),
            4,
            3);
    };

    // pinpad cursor
    canvas_draw_frame(
        canvas,
        pinpad_x_base_offset + (pinpad_x * pinpad_x_offset),
        pinpad_y_base_offset + (pinpad_y * pinpad_y_offset),
        12,
        9);
}

void addNumberToTVMString() {
    //TODO: handle this
}

void turnTVMnumberNegative() {
    //TODO: handle this
}
void addPeriodToTVMString() {
    //TODO: handle this
}
void computeTVM() {
}

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t fin_calc(void* p) {
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

    // the running loop
    bool running = true;
    while(running) {
        if(furi_message_queue_get(event_queue, &event, 100) == FuriStatusOk) {
            if((event.type == InputTypePress)) {
                // Input happens here
                switch(event.key) {
                case InputKeyUp:
                    if(pinpad_y != 0) {
                        pinpad_y--;
                    } else {
                        pinpad_y = 4;
                    }
                    break;

                case InputKeyDown:
                    if(pinpad_y != 4) {
                        pinpad_y++;
                    } else {
                        pinpad_y = 0;
                    }
                    break;

                case InputKeyLeft:
                    if(pinpad_x != 0) {
                        pinpad_x--;
                    } else {
                        pinpad_x = 2;
                    }
                    break;

                case InputKeyRight:
                    if(pinpad_x != 2) {
                        pinpad_x++;
                    } else {
                        pinpad_x = 0;
                    }
                    break;
                case InputKeyOk:
                    //if a number is inputted, handle that
                    if(pinpad_y <= 2 || (pinpad_x == 1 && pinpad_y == 3)) {
                        addNumberToTVMString();
                    }
                    // if - is pressed, turn the number negative
                    else if(pinpad_x == 0 && pinpad_y == 3) {
                        turnTVMnumberNegative();
                    }
                    // if up arrow pressed, move the tvm cursor up
                    else if(pinpad_x == 0 && pinpad_y == 4) {
                        if(tvm_cursor_position != 0) {
                            tvm_cursor_position--;
                        } else {
                            tvm_cursor_position = 4;
                        }
                    }
                    // if down arrow pressed, move tvm cursor down
                    else if(pinpad_x == 2 && pinpad_y == 4) {
                        if(tvm_cursor_position != 4) {
                            tvm_cursor_position++;
                        } else {
                            tvm_cursor_position = 0;
                        }
                    }
                    // if . pressed, handle that
                    else if(pinpad_x == 1 && pinpad_y == 4) {
                        addPeriodToTVMString();
                    }
                    // if compute pressed, handle that
                    else if(pinpad_x == 2 && pinpad_y == 3) {
                        computeTVM();
                    }
                    break;
                case InputKeyBack:

                    break;
                default:
                    break;
                }
            }
            if((event.type == InputTypeRepeat)) {
                switch(event.key) {
                case InputKeyBack:
                    running = false;
                    break;
                default:
                    break;
                }
            }

            view_port_update(view_port);
        }
    }

    //cleanup go brrrrr
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);

    furi_record_close(RECORD_GUI);

    return 0;
}