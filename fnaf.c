#include "fnaf.h"
#include "menu.h"

static void app_draw_callback(Canvas* canvas, void* ctx) {
    Fnaf* fnaf = ctx;

    canvas_clear(canvas);
    canvas_set_bitmap_mode(canvas, 1);

    switch (fnaf->current_view) {
    case main_menu:
        draw_menu(canvas, fnaf);
        break;
    case office:
        break;
    case cameras:
        //  add more lmao that's not all
        canvas_draw_icon(canvas, -1, 0, &I_cameras);
        break;
    case screamer:
        canvas_draw_str(canvas, 33, 34, "Scary image");
        break;
    case game_over:
        canvas_draw_str(canvas, 37, 34, "Game Over");
        break;
    default:
        furi_crash("Something unexpected happened just now");
        break;
    }

}

static void app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    Fnaf* fnaf = ctx;
    furi_message_queue_put(fnaf->event_queue, input_event, FuriWaitForever);
}

void fnaf_alloc(Fnaf* fnaf) {
    fnaf->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    fnaf->view_port = view_port_alloc();
    view_port_draw_callback_set(fnaf->view_port, app_draw_callback, fnaf);
    view_port_input_callback_set(fnaf->view_port, app_input_callback, fnaf);

    fnaf->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(fnaf->gui, fnaf->view_port, GuiLayerFullscreen);

    fnaf->current_view = main_menu;
    fnaf->menu_cursor = NewGame;
}

void fnaf_free(Fnaf* fnaf) {
    view_port_enabled_set(fnaf->view_port, false);
    gui_remove_view_port(fnaf->gui, fnaf->view_port);
    view_port_free(fnaf->view_port);
    furi_message_queue_free(fnaf->event_queue);

    furi_record_close(RECORD_GUI);
}

int32_t flipperzero_fnaf(void* p) {
    UNUSED(p);
    Fnaf* fnaf = malloc(sizeof(*fnaf));
    fnaf_alloc(fnaf);

    // Make saves later
    fnaf->progress = 0;

    bool running = true;
    while (running) {
        if (furi_message_queue_get(fnaf->event_queue, &fnaf->event, 100) == FuriStatusOk) {
            switch (fnaf->current_view) {
            case main_menu:
                running = menu_input(fnaf);
                break;
            case office:
                break;
            case cameras:
                break;
            case screamer:
                if (fnaf->event.key == InputKeyBack) {
                    running = false;
                }
                break;
            case game_over:
                break;
            default:
                break;
            }
        }
        view_port_update(fnaf->view_port);
    }

    fnaf_free(fnaf);
    free(fnaf);
    return 0;
}
