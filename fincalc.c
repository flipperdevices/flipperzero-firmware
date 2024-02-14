#include <string.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include "financial_calc_icons.h"
#include <tvmfunctions.h>

//init some variables / constants

//pinpad variables for movement and stuff
int pinpad_x = 0;
int pinpad_y = 0;
char pinpad_values[5][3] =
    {{'1', '2', '3'}, {'4', '5', '6'}, {'7', '8', '9'}, {'x', '0', 'x'}, {'x', '.', 'x'}};
// pinpad constants for display
const int pinpad_x_base_offset = 84;
const int pinpad_y_base_offset = 4;
const int pinpad_x_offset = 15;
const int pinpad_y_offset = 12;

//TVM values
double tvm_n = 0;
double tvm_i = 0;
double tvm_pv = 0;
double tvm_pmt = 0;
double tvm_fv = 0;
//TVM Strings
char tvm_Strings[5][9] = {"0", "0", "0", "0", "0"};

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

    // Now draw the numbers as strings
    canvas_draw_str(canvas, 33, 47, tvm_Strings[3]); //PMT
    canvas_draw_str(canvas, 33, 35, tvm_Strings[2]); //PV
    canvas_draw_str(canvas, 33, 23, tvm_Strings[1]); //I
    canvas_draw_str(canvas, 33, 12, tvm_Strings[0]); //N
    canvas_draw_str(canvas, 33, 59, tvm_Strings[4]); //FV

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

int contains_char(const char* str, char c) {
    // Iterate through each character of the string
    while(*str != '\0') {
        // If the current character matches the desired character
        if(*str == c) {
            return 1; // Return true
        }
        str++; // Move to the next character
    }
    return 0; // Return false if the character is not found
}

void addNumberToTVMString() {
    // Find the length of the string
    int length = strlen(tvm_Strings[tvm_cursor_position]);

    // do some sanity checks
    // if user inputs '.', but one is present, don't add one
    if(contains_char(tvm_Strings[tvm_cursor_position], '.') &&
       pinpad_values[pinpad_y][pinpad_x] == '.') {
        return;
    }
    //if the value is "0", delete it and replace
    if(length == 1 && tvm_Strings[tvm_cursor_position][0] == '0') {
        tvm_Strings[tvm_cursor_position][0] = pinpad_values[pinpad_y][pinpad_x];
        return;
    }

    // Check if there is space to append the character
    if(length < 7) {
        // Append the character and update the null terminator
        tvm_Strings[tvm_cursor_position][length] = pinpad_values[pinpad_y][pinpad_x];
        tvm_Strings[tvm_cursor_position][length + 1] = '\0';
    }
}

void turnTVMnumberNegative() {
    // Find the length of the string
    int length = strlen(tvm_Strings[tvm_cursor_position]);

    //if the number is already negative, make it positive
    // Shift all characters one position to the left
    if(tvm_Strings[tvm_cursor_position][0] == '-') {
        for(int i = 0; tvm_Strings[tvm_cursor_position][i] != '\0'; i++) {
            tvm_Strings[tvm_cursor_position][i] = tvm_Strings[tvm_cursor_position][i + 1];
        }
    } else {
        // make it negative if there is a number to make negative
        if(length < 8 && tvm_Strings[tvm_cursor_position][0] != '0') {
            for(int i = length; i >= 0; i--) {
                tvm_Strings[tvm_cursor_position][i + 1] = tvm_Strings[tvm_cursor_position][i];
            }
            // Insert the - at the beginning of the string
            tvm_Strings[tvm_cursor_position][0] = '-';
        }
    }
}

// nothing from any of the standard libraries worked, so we do this manually
double str_to_double(const char* str) {
    double result = 0.0;
    int dot_position = -1;
    int i = 0;
    int sign = 1;

    if(str[0] == '-') {
        sign = -1;
        i++;
    }

    for(; str[i] != '\0'; i++) {
        if(str[i] == '.') {
            dot_position = i;
            continue;
        }
        result = result * 10 + (str[i] - '0');
    }

    if(dot_position != -1) {
        int exponent = dot_position - i + 1;
        result *= pow(10, exponent);
    }

    return sign * result;
}

void computeTVM() {
    tvm_cpt_now = true;
    // convert all strings to doubles
    tvm_n = str_to_double(tvm_Strings[0]);
    tvm_i = str_to_double(tvm_Strings[1]) / 100;
    tvm_pv = str_to_double(tvm_Strings[2]);
    tvm_pmt = str_to_double(tvm_Strings[3]);
    tvm_fv = str_to_double(tvm_Strings[4]);
    // log it because it used to give issue
    FURI_LOG_I("INFORMATION tag", "Number of Period (tvm_n): %.2f", tvm_n);
    FURI_LOG_I("INFORMATION tag", "Interest Rate (tvm_i): %.2lf", tvm_i);
    FURI_LOG_I("INFORMATION tag", "Present value (tvm_pv): %.2lf", tvm_pv);
    FURI_LOG_I("INFORMATION tag", "Payment (tvm_pmt): %.2lf", tvm_pmt);
    FURI_LOG_I("INFORMATION tag", "Future value (tvm_fv): %.2lf", tvm_fv);

    // solve for each situation:

    //solve FV
    if(tvm_cursor_position == 4) {
        tvm_fv = findFV(tvm_n, tvm_i, tvm_pv, tvm_pmt);
        snprintf(tvm_Strings[4], sizeof(tvm_Strings[4]), "%.4f", tvm_fv);
        FURI_LOG_I("INFORMATION tag", "Future value solved (tvm_fv): %.4f", tvm_fv);
    }
    // solve PV
    else if(tvm_cursor_position == 2) {
        tvm_pv = findPV(tvm_n, tvm_i, tvm_pmt, tvm_fv);
        snprintf(tvm_Strings[2], sizeof(tvm_Strings[2]), "%.4f", tvm_pv);
        FURI_LOG_I("INFORMATION tag", "Present value solved (tvm_pv): %.4f", tvm_pv);

    }
    // solve I
    else if(tvm_cursor_position == 1) {
        tvm_i = findI(tvm_n, tvm_pmt, tvm_pv, tvm_fv) * 100;
        snprintf(tvm_Strings[1], sizeof(tvm_Strings[1]), "%.4f", tvm_i);

        FURI_LOG_I("INFORMATION tag", "Interest Rate solved (tvm_i): %.4f", tvm_i);

    }
    //solve N
    else if(tvm_cursor_position == 0) {
        tvm_n = findN(tvm_i, tvm_pv, tvm_pmt, tvm_fv);

        snprintf(tvm_Strings[0], sizeof(tvm_Strings[0]), "%.4f", tvm_n);
        FURI_LOG_I("INFORMATION tag", "N of Periods solved (tvm_n): %.4f", tvm_n);
    }
    //solve PMT
    else if(tvm_cursor_position == 3) {
        tvm_pmt = findPMT(tvm_n, tvm_i, tvm_pv, tvm_fv);

        snprintf(tvm_Strings[3], sizeof(tvm_Strings[3]), "%.4f", tvm_pmt);
        FURI_LOG_I("INFORMATION tag", "N of Periods solved (tvm_n): %.4f", tvm_pmt);
    }
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
                //something was pressed, denoting calculation is probably meaningless now
                tvm_cpt_now = false;
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
                    //if a number or . is inputted, handle that
                    if(pinpad_y <= 2 || (pinpad_x == 1)) {
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
                    // if compute pressed, handle that
                    else if(pinpad_x == 2 && pinpad_y == 3) {
                        computeTVM();
                    }
                    break;
                case InputKeyBack:
                    // if back pressed, wipe out the current string
                    strcpy(tvm_Strings[tvm_cursor_position], "0");
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