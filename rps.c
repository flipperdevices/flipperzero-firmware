#include <stdlib.h>

#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>

#include "rps.h"
#include "rps_icons.h"

#define QUEUE_SIZE 8
#define LOG_TAG "RPS"

void input_callback(InputEvent* input_event, void* ctx) {
    FURI_LOG_D(LOG_TAG, "Input callback fired!");
    RPSApp_t* app = ctx;

    switch (input_event->type)
    {
    case InputTypeShort:
            furi_message_queue_put(app->event_queue, input_event, 0);
        break;
    
    default:
        break;
    }
}

void render_callback(Canvas* canvas, void* ctx) {
    FURI_LOG_T(LOG_TAG, "Render callback fired!");
    RPSApp_t* app = ctx;
    UNUSED(app);
    canvas_clear(canvas);
    canvas_draw_icon(canvas, 0, 0, &I_rock);
    canvas_draw_icon(canvas, 30, 0, &I_paper);
    canvas_draw_icon(canvas, 60, 0, &I_scissor);
}

RPSApp_t* state_init() {
    FURI_LOG_D(LOG_TAG, "Initing app...");
    RPSApp_t* app = malloc(sizeof(RPSApp_t));

    app->event_queue = furi_message_queue_alloc(QUEUE_SIZE, sizeof(InputEvent));
    app->view_port = view_port_alloc();
    app->gui = furi_record_open(RECORD_GUI);
    app->mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    view_port_input_callback_set(app->view_port, input_callback, app);
    view_port_draw_callback_set(app->view_port, render_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    return app;
}

void state_free(RPSApp_t* app) {
    FURI_LOG_D(LOG_TAG, "Freeing app...");
    furi_message_queue_free(app->event_queue);
    gui_remove_view_port(app->gui, app->view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(app->view_port);
    furi_mutex_free(app->mutex);
    free(app);
}

bool main_loop(RPSApp_t* app) {
    InputEvent event;
    FuriStatus status = FuriStatusErrorTimeout;
    while ((status = furi_message_queue_get(app->event_queue, &event, 60000)) == FuriStatusErrorTimeout) ;

    furi_check(furi_mutex_acquire(app->mutex, FuriWaitForever) == FuriStatusOk);

    FURI_LOG_D(LOG_TAG, "mainnnnn");
    switch (event.key) {
        case InputKeyBack:
            FURI_LOG_D(LOG_TAG, "exit");
            furi_mutex_release(app->mutex);
            return false;
        
        default:
            FURI_LOG_D(LOG_TAG, "%d", event.key);
            break;
    }

    furi_mutex_release(app->mutex);
    return true;
}

int32_t rps_app(void) {
    RPSApp_t* app = state_init();
    while (main_loop(app)) ;
    state_free(app);
    return 0;
}
