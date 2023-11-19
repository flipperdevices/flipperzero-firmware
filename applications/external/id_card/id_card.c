#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>

#include <stdint.h>
#include <stdlib.h>

#include "id_card_icons.h"

#define NAME "John DOE"
#define NUMBER "06 12 34 56 78"
#define EMAIL "john.doe@example.com"

#define TAG "Id Card"

typedef struct {
    FuriMessageQueue* input_queue;
    ViewPort* view_port;
    Gui* gui;
} Id_card;

void draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);

    canvas_draw_str_aligned(canvas, 1, 5, AlignLeft, AlignTop, "Name: ");

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);

    canvas_draw_str_aligned(canvas, 33, 5, AlignLeft, AlignTop, NAME);

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);

    canvas_draw_str_aligned(canvas, 1, 20, AlignLeft, AlignTop, "N: ");

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);

    canvas_draw_str_aligned(canvas, 15, 20, AlignLeft, AlignTop, NUMBER);

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);

    canvas_draw_str_aligned(canvas, 1, 35, AlignLeft, AlignTop, "EMAIL: ");

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);

    canvas_draw_str_aligned(canvas, 5, 45, AlignLeft, AlignTop, EMAIL);

    canvas_draw_icon(canvas, 95, 5, &I_icon_30x30);
}

void input_callback(InputEvent* event, void* context) {
    Id_card* app = context;
    furi_message_queue_put(app->input_queue, event, 0);
}

int32_t id_card_main(void* p) {
    UNUSED(p);

    Id_card app;

    // Alloc
    app.view_port = view_port_alloc();
    app.input_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Callbacks
    view_port_draw_callback_set(app.view_port, draw_callback, &app);
    view_port_input_callback_set(app.view_port, input_callback, &app);

    // GUI
    app.gui = furi_record_open("gui");
    gui_add_view_port(app.gui, app.view_port, GuiLayerFullscreen);

    // Input handling
    InputEvent input;
    uint8_t exit_loop = 0;

    FURI_LOG_I(TAG, "Start the main loop.");
    while(1) {
        furi_check(
            furi_message_queue_get(app.input_queue, &input, FuriWaitForever) == FuriStatusOk);

        switch(input.key) {
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyOk:
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyBack:
            exit_loop = 1;
            break;
        default:
            break;
        }

        if(exit_loop) {
            break;
        }

        view_port_update(app.view_port);
    }

    // Free structures
    view_port_enabled_set(app.view_port, false);
    gui_remove_view_port(app.gui, app.view_port);
    furi_record_close("gui");
    view_port_free(app.view_port);

    return 0;
}
