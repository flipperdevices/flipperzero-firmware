#include <furi.h>
#include <furi_hal.h>
#include <gui/elements.h>
#include <gui/gui.h>
#include <gui/view_port.h>
#include <stdlib.h>
#include "coffee.h"
#include <math.h>

#define TAG "COFFEE EEPROM"

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    double credit;
    char status[20];
} CoffeeContext;


static void coffee_render_callback(Canvas* const canvas, void* ctx) {
    CoffeeContext* context = ctx;
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    elements_button_left(canvas, "Virgin");
    elements_button_right(canvas, "10E");
    elements_button_center(canvas, "Dump");
    canvas_set_font(canvas, FontPrimary);
    char str[340];
    snprintf(str, 340, "Credit: %.2f EUR", context->credit);
    canvas_draw_str_aligned(canvas, 64, 8, AlignCenter, AlignCenter, str);
    canvas_draw_str_aligned(canvas, 64, 26, AlignCenter, AlignCenter, context->status);
}

/* This function is called from the GUI thread. All it does is put the event
   into the application's queue so it can be processed later. */
static void coffee_input_callback(InputEvent* event, void* ctx) {
    CoffeeContext* context = ctx;
    furi_message_queue_put(context->event_queue, event, FuriWaitForever);
}
/* Allocate the memory and initialise the variables */
static CoffeeContext* coffee_context_alloc() {
    CoffeeContext* context = malloc(sizeof(CoffeeContext));

    context->view_port = view_port_alloc();
    view_port_draw_callback_set(context->view_port, coffee_render_callback, context);
    view_port_input_callback_set(context->view_port, coffee_input_callback, context);

    context->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    context->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(context->gui, context->view_port, GuiLayerFullscreen);
    return context;
}

/* Starts the reader thread and handles the input */
static void coffee_run(CoffeeContext* context) {
    /* Start the reader thread. It will talk to the thermometer in the background. */
    context->credit = read_credit();
    /* An endless loop which handles the input*/
    for(bool is_running = true; is_running;) {
        InputEvent event;
        /* Wait for an input event. Input events come from the GUI thread via a callback. */
        const FuriStatus status =
            furi_message_queue_get(context->event_queue, &event, FuriWaitForever);

        if(status == FuriStatusOk) {
           if(event.type == InputTypePress) {
                    switch(event.key) {
                    case InputKeyUp:
                        break;
                    case InputKeyDown:
                        break;
                    case InputKeyMAX:
                        break;
                    case InputKeyRight:
                        write_10_eur();
                        context->credit = read_credit();
                        snprintf(context->status, sizeof(context->status), "Write done!");
                        break;
                    case InputKeyLeft:
                        virgin();
                        context->credit = read_credit();
                        snprintf(context->status, sizeof(context->status), "Virgin done!");
                        break;
                    case InputKeyOk:
                        dump();
                        snprintf(context->status, sizeof(context->status), "Dumped to logs");
                        break;
                    case InputKeyBack:
                        is_running = false;
                        break;
                    }
                }
        }
    }

}

/* Release the unused resources and deallocate memory */
static void coffee_context_free(CoffeeContext* context) {
    view_port_enabled_set(context->view_port, false);
    gui_remove_view_port(context->gui, context->view_port);
    furi_message_queue_free(context->event_queue);
    view_port_free(context->view_port);
    furi_record_close(RECORD_GUI);
}

/* The application's entry point. Execution starts from here. */
int32_t coffee_eeprom_main(void* p) {
    UNUSED(p);

    /* Allocate all of the necessary structures */
    CoffeeContext* context = coffee_context_alloc();

    /* Start the applicaton's main loop. It won't return until the application was requested to exit. */
    coffee_run(context);

    /* Release all unneeded resources */
    coffee_context_free(context);

    return 0;
}