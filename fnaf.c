#include "fnaf.h"
#include "views/menu.h"
#include "views/text_view.h"
#include "views/cameras.h"
#include "views/office.h"
#include "views/custom_night.h"

static void app_draw_callback(Canvas* canvas, void* ctx) {
    Fnaf* fnaf = ctx;

    canvas_clear(canvas);

    switch (fnaf->current_view) {
    case main_menu:
        draw_menu(canvas, fnaf);
        break;
    case custom_night:
        draw_custom_night_menu(canvas, fnaf);
        break;
    case night_number:
        text_view(canvas, fnaf);
        break;
    case office_view:
        office_draw(canvas, fnaf);
        break;
    case cameras:
        draw_cameras(canvas, fnaf);
        break;
    case night_complete:
        text_view(canvas, fnaf);
        break;
    case jumpscare:
        if (fnaf->counter % 2 + 1 > 1) {
            canvas_draw_icon(canvas, 43, 8, &I_jumpscare_frame_2_42x56);
            fnaf->counter += 1;
        } else {
            canvas_draw_icon(canvas, 43, 8, &I_jumpscare_frame_1_42x56);
            fnaf->counter += 1;
        }
        if (fnaf->counter == 7) {
            SWITCH_VIEW(game_over);
        } else if (fnaf->counter == 0) {
            // Just in case it broke somewhere else
            save_progress(fnaf);
        }
        break;
    case game_over:
        text_view(canvas, fnaf);
        fnaf->counter += 1;
        if (fnaf->counter > 10) SWITCH_VIEW(main_menu);
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

void stop_hourly_timer(Fnaf* fnaf) {
    if (furi_timer_is_running(fnaf->hourly_timer)) {
        FURI_LOG_D(TAG, "Hourly timer stopped");
        furi_timer_stop(fnaf->hourly_timer);
    }
}

void stop_all_timers(Fnaf* fnaf) {
    FURI_LOG_D(TAG, "stop_all_timers");
    for (uint8_t i = 0; i < 4; i++) {
        if (furi_timer_is_running(fnaf->dolphins->timer[i])) {
            FURI_LOG_D(TAG, "Timer %u is stopped", i);
            furi_timer_stop(fnaf->dolphins->timer[i]);
        }
    }
    if (furi_timer_is_running(fnaf->electricity->timer)) {
        FURI_LOG_D(TAG, "Electricity timer stopped");
        furi_timer_stop(fnaf->electricity->timer);
    }
    if (furi_timer_is_running(fnaf->office->left_door_sound_timer)) {
        FURI_LOG_D(TAG, "Left door sound timer stopped");
        furi_timer_stop(fnaf->office->left_door_sound_timer);
    }
    if (furi_timer_is_running(fnaf->office->right_door_sound_timer)) {
        FURI_LOG_D(TAG, "Right door sound timer stopped");
        furi_timer_stop(fnaf->office->right_door_sound_timer);
    }
    if (furi_timer_is_running(fnaf->office->flipper_laugh_timer)) {
        FURI_LOG_D(TAG, "Flipper laugh timer stopped");
        furi_timer_stop(fnaf->office->flipper_laugh_timer);
    }
    if (furi_timer_is_running(fnaf->office->power_out_timer)) {
        FURI_LOG_D(TAG, "Power out timer stopped");
        furi_timer_stop(fnaf->office->power_out_timer);
    }
    if (furi_timer_is_running(fnaf->office->power_out_max_timer)) {
        FURI_LOG_D(TAG, "Power out max timer stopped");
        furi_timer_stop(fnaf->office->power_out_max_timer);
    }
    if (furi_timer_is_running(fnaf->dolphins->move_rand_timer)) {
        FURI_LOG_D(TAG, "Move rand timer stopped");
        furi_timer_stop(fnaf->dolphins->move_rand_timer);
    }
    if (furi_timer_is_running(fnaf->dolphins->flipper_might_move_timer)) {
        FURI_LOG_D(TAG, "Flipper might move timer stopped");
        furi_timer_stop(fnaf->dolphins->flipper_might_move_timer);
    }
}

void switch_view(Fnaf* fnaf, Views view) {
    FURI_LOG_D(TAG, "switch_view to %u", view);
    fnaf->counter = 0;
    fnaf->counter_secondary = 0;
    if (fnaf->current_view == cameras && view == office_view) {
        // start Fopper timer
    } else if (fnaf->current_view == office_view && view == cameras) {
        // stop Fopper timer if running
    }
    if (view != cameras && view != office_view) {
        stop_hourly_timer(fnaf);
        stop_all_timers(fnaf);
        fnaf->hour = 0;
    }
    if (view == custom_night)
        for (uint8_t i = 0; i < 4; i++) {
            fnaf->dolphins->AI[i] = 0;
        }

    fnaf->current_view = view;
}

uint8_t power_draw(Fnaf* fnaf) {
    return (1 + fnaf->electricity->left_door + fnaf->electricity->left_light + fnaf->electricity->monitor + fnaf->electricity->right_door + fnaf->electricity->right_light);
}

void save_progress(Fnaf* fnaf) {
    FURI_LOG_D(TAG, "save_progress called");
    if (storage_file_open(fnaf->save_data, EXT_PATH("apps_data/flipperzero_fnaf/save.txt"), FSAM_WRITE, FSOM_OPEN_ALWAYS)) {
        FURI_LOG_D(TAG, "Saving... Progress = %u", fnaf->progress);
        char buff[1] = { fnaf->progress };
        uint16_t res = storage_file_write(fnaf->save_data, buff, 1);
        FURI_LOG_D(TAG, "Written %u bytes", res);
        storage_file_close(fnaf->save_data);
    }
}

void load_progress(Fnaf* fnaf) {
    if (storage_file_open(fnaf->save_data, EXT_PATH("apps_data/flipperzero_fnaf/save.txt"), FSAM_READ, FSOM_OPEN_EXISTING)) {
        char read[1] = { 101 };
        storage_file_seek(fnaf->save_data, 0, true);
        storage_file_read(fnaf->save_data, read, 1);
        FURI_LOG_D(TAG, "Read %u", read[0]);
        if (read[0] < 6) fnaf->progress = read[0];
        storage_file_close(fnaf->save_data);
    }
}

void empty_callback(void* ctx) {
    UNUSED(ctx);
    // Do nothing
}

void fnaf_alloc(Fnaf* fnaf) {
    fnaf->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    fnaf->view_port = view_port_alloc();
    view_port_draw_callback_set(fnaf->view_port, app_draw_callback, fnaf);
    view_port_input_callback_set(fnaf->view_port, app_input_callback, fnaf);
    fnaf->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(fnaf->gui, fnaf->view_port, GuiLayerFullscreen);
    fnaf->save_data = storage_file_alloc(furi_record_open(RECORD_STORAGE));


    fnaf->dolphins = malloc(sizeof(*fnaf->dolphins));
    fnaf->electricity = malloc(sizeof(*fnaf->electricity));
    fnaf->office = malloc(sizeof(*fnaf->office));
    fnaf->cameras = malloc(sizeof(*fnaf->cameras));

    fnaf->dolphins->timer[Blipper] = furi_timer_alloc(timer_callback_blipper, FuriTimerTypePeriodic, fnaf);
    fnaf->dolphins->timer[Chipper] = furi_timer_alloc(timer_callback_chipper, FuriTimerTypePeriodic, fnaf);
    fnaf->dolphins->timer[Flipper] = furi_timer_alloc(timer_callback_flipper, FuriTimerTypePeriodic, fnaf);
    fnaf->dolphins->timer[Fopper] = furi_timer_alloc(timer_callback_fopper, FuriTimerTypePeriodic, fnaf);
    fnaf->dolphins->fopper_inactivity = furi_timer_alloc(empty_callback, FuriTimerTypeOnce, fnaf);
    fnaf->dolphins->move_rand_timer = furi_timer_alloc(move_rand_callback, FuriTimerTypePeriodic, fnaf);
    fnaf->dolphins->flipper_might_move_timer = furi_timer_alloc(flipper_might_move_callback, FuriTimerTypePeriodic, fnaf);
    fnaf->office->right_door_sound_timer = furi_timer_alloc(empty_callback, FuriTimerTypeOnce, fnaf);
    fnaf->office->left_door_sound_timer = furi_timer_alloc(empty_callback, FuriTimerTypeOnce, fnaf);
    fnaf->office->flipper_laugh_timer = furi_timer_alloc(empty_callback, FuriTimerTypeOnce, fnaf);
    fnaf->office->power_out_timer = furi_timer_alloc(power_out_callback, FuriTimerTypePeriodic, fnaf);
    fnaf->office->power_out_max_timer = furi_timer_alloc(power_out_max_callback, FuriTimerTypeOnce, fnaf);
    fnaf->hourly_timer = furi_timer_alloc(hourly_timer_callback, FuriTimerTypePeriodic, fnaf);
    fnaf->electricity->timer = furi_timer_alloc(power_timer_callback, FuriTimerTypePeriodic, fnaf);
    UNUSED(fnaf->dolphins->timer[Blipper]);
    UNUSED(fnaf->dolphins->timer[Chipper]);
    UNUSED(fnaf->dolphins->timer[Flipper]);
    UNUSED(fnaf->dolphins->timer[Fopper]);
}

void fnaf_free(Fnaf* fnaf) {

    for (uint8_t i = 0; i < 4; i++) {
        furi_timer_free(fnaf->dolphins->timer[i]);
    }
    furi_timer_free(fnaf->dolphins->fopper_inactivity);
    furi_timer_free(fnaf->dolphins->move_rand_timer);
    furi_timer_free(fnaf->dolphins->flipper_might_move_timer);
    free(fnaf->dolphins);

    furi_timer_free(fnaf->electricity->timer);
    free(fnaf->electricity);

    furi_timer_free(fnaf->office->left_door_sound_timer);
    furi_timer_free(fnaf->office->right_door_sound_timer);
    furi_timer_free(fnaf->office->flipper_laugh_timer);
    furi_timer_free(fnaf->office->power_out_timer);
    furi_timer_free(fnaf->office->power_out_max_timer);
    free(fnaf->office);

    free(fnaf->cameras);

    furi_timer_free(fnaf->hourly_timer);

    view_port_enabled_set(fnaf->view_port, false);
    gui_remove_view_port(fnaf->gui, fnaf->view_port);
    view_port_free(fnaf->view_port);
    furi_message_queue_free(fnaf->event_queue);
    storage_file_free(fnaf->save_data);

    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_GUI);
}

int32_t flipperzero_fnaf(void* p) {
    UNUSED(p);
    Fnaf* fnaf = malloc(sizeof(*fnaf));
    fnaf_alloc(fnaf);

    load_progress(fnaf);
    reset_animatronic_positions(fnaf);
    srand(furi_get_tick());

    SWITCH_VIEW(main_menu);
    fnaf->menu_cursor = 0;

    bool running = true;
    while (running) {
        if (furi_message_queue_get(fnaf->event_queue, &fnaf->event, 100) == FuriStatusOk) {
            if (fnaf->event.key == InputKeyBack && fnaf->event.type == InputTypeShort) {
                if (fnaf->current_view != main_menu) SWITCH_VIEW(main_menu); else {
                    running = false;
                }
            }
            switch (fnaf->current_view) {
            case main_menu:
                running = menu_input(fnaf);
                break;
            case custom_night:
                custom_night_input(fnaf);
                break;
            case night_number:
                break;
            case office_view:
                office_input(fnaf);
                break;
            case cameras:
                cameras_input(fnaf);
                break;
            case night_complete:
                break;
            case jumpscare:
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

    stop_hourly_timer(fnaf); // Just in case
    stop_all_timers(fnaf); // Just in case
    fnaf_free(fnaf);
    free(fnaf);
    return 0;
}
