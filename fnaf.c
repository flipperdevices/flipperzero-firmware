#include "fnaf.h"
#include "views/menu.h"
#include "views/text_view.h"
#include "views/cameras.h"
#include "views/office.h"

static void app_draw_callback(Canvas* canvas, void* ctx) {
    Fnaf* fnaf = ctx;

    canvas_clear(canvas);
    canvas_set_bitmap_mode(canvas, 1);

    switch (fnaf->current_view) {
    case main_menu:
        draw_menu(canvas, fnaf);
        break;
    case night_number:
        text_view(canvas, fnaf);
        break;
    case office:
        canvas_draw_str(canvas, 33, 34, "OFFICE");
        break;
    case cameras:
        draw_cameras(canvas, fnaf);
        break;
    case night_complete:
        text_view(canvas, fnaf);
        break;
    case screamer:
        canvas_draw_str(canvas, 33, 34, "Scary image");
        break;
    case game_over:
        text_view(canvas, fnaf);
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

    fnaf->animatronics = malloc(sizeof(*fnaf->animatronics));

    fnaf->timer_bonnie = furi_timer_alloc(timer_callback_bonnie, FuriTimerTypePeriodic, fnaf);
    fnaf->timer_chica = furi_timer_alloc(timer_callback_chica, FuriTimerTypePeriodic, fnaf);
    fnaf->timer_freddy = furi_timer_alloc(timer_callback_freddy, FuriTimerTypePeriodic, fnaf);
    fnaf->timer_foxy = furi_timer_alloc(timer_callback_foxy, FuriTimerTypePeriodic, fnaf);
    UNUSED(fnaf->timer_bonnie);
    UNUSED(fnaf->timer_chica);
    UNUSED(fnaf->timer_freddy);
    UNUSED(fnaf->timer_foxy);

    fnaf->power_left = 100;
    fnaf->current_view = main_menu;
    fnaf->menu_cursor = NewGame;
    fnaf->camera_cursor = cam1A;
}

void fnaf_free(Fnaf* fnaf) {
    free(fnaf->animatronics);
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
    reset_animatronic_positions(fnaf);

    bool running = true;
    while (running) {
        if (furi_message_queue_get(fnaf->event_queue, &fnaf->event, 100) == FuriStatusOk) {
            if (fnaf->event.key == InputKeyBack && fnaf->event.type == InputTypeShort) {
                if (fnaf->current_view != main_menu) fnaf->current_view = main_menu; else
                    running = false;
            }
            switch (fnaf->current_view) {
            case main_menu:
                running = menu_input(fnaf);
                break;
            case night_number:
                if (fnaf->event.key == InputKeyOk && fnaf->event.type == InputTypeShort) {
                    fnaf->current_view = office;
                }

                // TESTING ONLY
                if (fnaf->event.key == InputKeyRight && fnaf->event.type == InputTypeShort) {
                    fnaf->progress += 1;
                    if (fnaf->progress > 6) fnaf->progress = 0;
                }
                // END

                break;
            case office:
                break;
            case cameras:
                cameras_switching(fnaf);
                break;
            case night_complete:

                break;
            case screamer:
                break;
            case game_over:
                break;
            default:
                break;
            }
        }
        // view_port_update(fnaf->view_port);
    }

    fnaf_free(fnaf);
    free(fnaf);
    return 0;
}
