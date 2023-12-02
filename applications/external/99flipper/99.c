#include "99.h"

const int BEER_MAX = 99;

char* beer_plural(uint8_t cnt) {
    return cnt == 1 ? "bottle of beer" : "bottles of beer";
}

void beer_render_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);

    uint8_t font_size = canvas_current_font_height(canvas) - 1;
    uint8_t pos = ((BeerApp*)ctx)->pos;

    elements_scrollbar(canvas, BEER_MAX - pos, BEER_MAX + 1);

    if(pos == 0) {
        elements_multiline_text_aligned(
            canvas,
            0,
            0,
            AlignLeft,
            AlignTop,
            "No more bottles of beer on\n"
            "the wall, no more bottles of\n"
            "beer.\n"
            "Go to the store and buy some\n"
            "more, 99 bottles of beer on\n"
            "the wall.");
    } else {
        FuriString* str = furi_string_alloc();
        furi_string_printf(
            str,
            "%d %s on the wall,\n%d %s of beer.",
            pos,
            beer_plural(pos),
            pos,
            beer_plural(pos));

        elements_multiline_text_aligned(
            canvas, 0, 0, AlignLeft, AlignTop, furi_string_get_cstr(str));

        furi_string_printf(str, "Take one down and pass it\naround, ");

        if(--pos > 0) {
            furi_string_cat_printf(str, "%d", pos);
        } else {
            furi_string_cat_printf(str, "no");
        }

        furi_string_cat_printf(str, " %s\non the wall.", beer_plural(pos));

        elements_multiline_text_aligned(
            canvas, 0, font_size * 3, AlignLeft, AlignTop, furi_string_get_cstr(str));

        furi_string_free(str);
    }
}

static void beer_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    furi_message_queue_put((FuriMessageQueue*)ctx, input_event, FuriWaitForever);
}

BeerApp* beer_app_alloc() {
    BeerApp* app = malloc(sizeof(BeerApp));

    app->view_port = view_port_alloc();
    view_port_draw_callback_set(app->view_port, beer_render_callback, app);
    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    view_port_input_callback_set(app->view_port, beer_input_callback, app->event_queue);

    app->pos = BEER_MAX;

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

int32_t beer_main(void* p) {
    UNUSED(p);

    __attribute__((__cleanup__(beer_app_free))) BeerApp* app = beer_app_alloc();

    for(InputEvent event;;) {
        furi_check(
            furi_message_queue_get(app->event_queue, &event, FuriWaitForever) == FuriStatusOk);

        if(event.type == InputTypeShort) {
            switch(event.key) {
            case InputKeyBack:
                return 0;

            case InputKeyDown:
                if(app->pos > 0) {
                    app->pos--;
                }

                break;

            case InputKeyUp:
                if(app->pos < BEER_MAX) {
                    app->pos++;
                }

                break;

            default:
                break;
            }

            view_port_update(app->view_port);
        }
    }
}
