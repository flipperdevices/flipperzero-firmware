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
        office_draw(canvas, fnaf);
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


    fnaf->animatronics->timer[Bonnie] = furi_timer_alloc(timer_callback_bonnie, FuriTimerTypePeriodic, fnaf);
    fnaf->animatronics->timer[Chica] = furi_timer_alloc(timer_callback_chica, FuriTimerTypePeriodic, fnaf);
    fnaf->animatronics->timer[Freddy] = furi_timer_alloc(timer_callback_freddy, FuriTimerTypePeriodic, fnaf);
    fnaf->animatronics->timer[Foxy] = furi_timer_alloc(timer_callback_foxy, FuriTimerTypePeriodic, fnaf);
    UNUSED(fnaf->animatronics->timer[Bonnie]);
    UNUSED(fnaf->animatronics->timer[Chica]);
    UNUSED(fnaf->animatronics->timer[Freddy]);
    UNUSED(fnaf->animatronics->timer[Foxy]);

    fnaf->electricity = malloc(sizeof(*fnaf->electricity));
    fnaf->hourly_timer = furi_timer_alloc(hourly_timer_callback, FuriTimerTypePeriodic, fnaf);
    UNUSED(fnaf->hourly_timer);

    SWITCH_VIEW(main_menu);
    fnaf->menu_cursor = 0;
    fnaf->electricity->power_left = 999;
}

static void stop_all_timers(Fnaf* fnaf) {
    FURI_LOG_D(TAG, "stop_all_timers");
    for (uint8_t i = 0; i < 4; i++) {
        if (furi_timer_is_running(fnaf->animatronics->timer[i])) {
            FURI_LOG_D(TAG, "Timer %u is stopped", i);
            furi_timer_stop(fnaf->animatronics->timer[i]);
        }
    }
    if (furi_timer_is_running(fnaf->hourly_timer)) {
        FURI_LOG_D(TAG, "Hourly timer stopped");
        furi_timer_stop(fnaf->hourly_timer);
    }
}

void switch_view(Fnaf* fnaf, Views view) {
    FURI_LOG_D(TAG, "switch_view to %u", view);
    fnaf->counter = 0;
    fnaf->current_view = view;
    if (fnaf->current_view != cameras && fnaf->current_view != office) {
        stop_all_timers(fnaf);
        fnaf->hour = 0;
    }
}

void fnaf_free(Fnaf* fnaf) {
    free(fnaf->animatronics);
    free(fnaf->electricity);

    for (uint8_t i = 0; i < 4; i++) {
        free(fnaf->animatronics->timer[i]);
    }

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
                break;
            case office:
                office_input(fnaf);
                break;
            case cameras:
                cameras_input(fnaf);
                break;
            case night_complete:
                break;
            case screamer:
                break;
            case game_over:
                if (fnaf->event.key == InputKeyBack && fnaf->event.type == InputTypeShort) {
                    SWITCH_VIEW(main_menu);
                }
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
