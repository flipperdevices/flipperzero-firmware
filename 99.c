#include "99.h"

void render_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 12, "99 bottles of beer on the wall.");
}

static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    furi_message_queue_put((FuriMessageQueue*)ctx, input_event, FuriWaitForever);
}

BeerApp* beer_app_alloc() {
    BeerApp* app = malloc(sizeof(BeerApp));

    app->view_port = view_port_alloc();
    view_port_draw_callback_set(app->view_port, render_callback, app);
    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    view_port_input_callback_set(app->view_port, input_callback, app->event_queue);

    return app;
}

void beer_app_free(BeerApp** app) {
    furi_assert(*app);

    view_port_enabled_set((*app)->view_port, false);
    gui_remove_view_port((*app)->gui, (*app)->view_port);
    view_port_free((*app)->view_port);

    furi_record_close(RECORD_GUI);
    furi_message_queue_free((*app)->event_queue);

    free(*app);
}

int32_t beer_app(void* p) {
    UNUSED(p);

    __attribute__((__cleanup__(beer_app_free))) BeerApp* app = beer_app_alloc();

    for(InputEvent event;;) {
        furi_check(
            furi_message_queue_get(app->event_queue, &event, FuriWaitForever) == FuriStatusOk);

        if(event.key == InputKeyBack) {
            break;
        }
    }

    return 0;
}
