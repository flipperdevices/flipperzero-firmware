#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include "furi_hal_random.h"
#include <input/input.h>
#include <gui/elements.h>
#include "scrambler.h"
#include <furi_hal.h>

char* currentKeyPressed;
int BUFFER = 10;
char scramble[100] = {0};
int notifications_enabled = 0;

static void success_vibration() {
    furi_hal_vibro_on(false);
    furi_hal_vibro_on(true);
    furi_delay_ms(50);
    furi_hal_vibro_on(false);
    return;
}

static void draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 13, "Rubik's Cube Scrambler");

        
    if(strcmp(currentKeyPressed, "OK") == 0) {
/*         const char* moves[] = {"U", "D", "L", "R", "F", "B"};
        const char* directions[] = {"", "'", "2"};
        int index = 0;
        int prevMove = -1; // Initialize previous move to an invalid value
        for(int i = 0; i < 10; i++) {
            int move;
            do {
                move = rand() % 6;
            } while(move == prevMove);
            int direction = rand() % 3;
            prevMove = move;
            index += snprintf(
                scramble + index,
                sizeof(scramble) - index,
                "%s%s ",
                moves[move],
                directions[direction]);
        } */
        genScramble ();
        scrambleReplace();
        valid();
        strcpy(scramble, printData());
        if (notifications_enabled) {
            success_vibration();
        }
        currentKeyPressed = "";
    }
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 30, scramble);
    
    elements_button_center(canvas, "New");
    
    elements_button_left(canvas, notifications_enabled ? "On" : "Off");
};


static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t rubiks_cube_scrambler_main(void* p) {
    UNUSED(p);
    currentKeyPressed = (char*)malloc(sizeof(char) * BUFFER);
    InputEvent event;

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    ViewPort* view_port = view_port_alloc();

    view_port_draw_callback_set(view_port, draw_callback, NULL);

    view_port_input_callback_set(view_port, input_callback, event_queue);
    
    
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    while(true) {
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        if(event.key == InputKeyOk && event.type == InputTypeShort) {
            currentKeyPressed = "OK";
        }
        if (event.key == InputKeyLeft && event.type == InputTypeShort) {
            if (notifications_enabled) {
                notifications_enabled = 0;
            } else {
                notifications_enabled = 1;
                success_vibration();
            }
        }
        if(event.key == InputKeyBack) {
            break;
        }
    }

    furi_message_queue_free(event_queue);

    gui_remove_view_port(gui, view_port);

    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    return 0;
}
