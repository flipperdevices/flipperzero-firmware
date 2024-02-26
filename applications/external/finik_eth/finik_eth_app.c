
#include "finik_eth_app.h"

#include <furi.h>
#include <furi_hal_power.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>
#include <notification/notification_messages.h>

#include "eth_worker.h"
#include "eth_worker_i.h"
#include "eth_view_process.h"

#define TAG "FinikEthApp"

static void draw_process_selector(Canvas* canvas, DrawProcess selector, CursorPosition cursor) {
    uint8_t y = 0;
    if(selector == PROCESS_INIT) y = 11;
    if(selector == PROCESS_DHCP) y = 22;
    if(selector == PROCESS_STATIC) y = 34;
    if(selector == PROCESS_PING) y = 44;
    if(selector == PROCESS_RESET) y = 55;

    if(cursor == CURSOR_INSIDE_PROCESS) {
        canvas_invert_color(canvas);
        canvas_draw_line(canvas, 25, y + 1, 25, y + 7);
        canvas_invert_color(canvas);
        canvas_draw_line(canvas, 0, y + 1, 0, y + 7);
        canvas_draw_line(canvas, 1, y, 25, y);
        canvas_draw_line(canvas, 1, y + 8, 25, y + 8);
    } else {
        canvas_draw_line(canvas, 0, y + 1, 0, y + 7);
        canvas_draw_line(canvas, 23, y + 1, 23, y + 7);
        canvas_draw_line(canvas, 1, y, 22, y);
        canvas_draw_line(canvas, 1, y + 8, 22, y + 8);
    }

    if(cursor == CURSOR_CLICK_PROCESS) {
        canvas_draw_box(canvas, 1, y, 22, 9);
    }
}

static void draw_battery_cunsumption(Canvas* canvas, double cons) {
    FuriString* string = furi_string_alloc_set("aaaaaaaa");
    if(cons >= 0) {
        furi_string_printf(string, "--");
    } else if(cons < -1) {
        furi_string_printf(string, "%1.1fk", -cons);
    } else {
        furi_string_printf(string, "%3.f", -(cons * 1000));
    }

    canvas_draw_str(canvas, 112, 7, furi_string_get_cstr(string));
    furi_string_free(string);
}

static void finik_eth_app_draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    FinikEthApp* app = ctx;

    canvas_clear(canvas);

    DrawProcess process = app->draw_process;
    CursorPosition cursor = app->cursor_position;
    float consumption = app->info.current_gauge;

    canvas_set_font(canvas, FontSecondary);

    if(cursor == CURSOR_EXIT_ICON) {
        canvas_draw_icon(canvas, 0, 0, &I_exit_128x64px);
    } else {
        canvas_draw_icon(canvas, 0, 0, &I_main_128x64px);
        draw_process_selector(canvas, process, cursor);
        draw_battery_cunsumption(canvas, (double)consumption);
        ethernet_view_process_draw(app->eth_worker->active_process, canvas);
        if(furi_hal_power_is_otg_enabled()) {
            canvas_draw_str(canvas, 22, 6, "+");
        } else {
            canvas_draw_str(canvas, 22, 6, "-");
        }
    }
}

static void finik_eth_battery_info_update_model(void* ctx) {
    furi_assert(ctx);
    FinikEthApp* app = ctx;
    power_get_info(app->power, &app->info);
}

static void finik_eth_app_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

FinikEthApp* finik_eth_app_alloc() {
    FinikEthApp* app = malloc(sizeof(FinikEthApp));

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->eth_worker = eth_worker_alloc();

    view_port_draw_callback_set(app->view_port, finik_eth_app_draw_callback, app);
    view_port_input_callback_set(app->view_port, finik_eth_app_input_callback, app->event_queue);

    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    app->power = furi_record_open(RECORD_POWER);

    return app;
}

void finik_eth_app_free(FinikEthApp* app) {
    furi_assert(app);

    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);

    furi_message_queue_free(app->event_queue);
    eth_worker_free(app->eth_worker);

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_POWER);
}

void finit_eth_app_key_handler(FinikEthApp* app, InputKey key) {
    if(app->cursor_position == CURSOR_CHOOSE_PROCESS) {
        if(key == InputKeyUp || key == InputKeyDown) {
            app->draw_process =
                (app->draw_process + PROCESS_RESET + (key == InputKeyDown ? 2 : 0)) %
                (PROCESS_RESET + 1);
            eth_worker_set_active_process(app->eth_worker, (EthWorkerProcess)app->draw_process);
        } else if(key == InputKeyOk) {
            ethernet_view_process_move(app->eth_worker->active_process, 0);
            app->cursor_position = CURSOR_CLICK_PROCESS;
            view_port_update(app->view_port);
            furi_delay_ms(150);
            eth_run(app->eth_worker, (EthWorkerProcess)app->draw_process);
            app->cursor_position = CURSOR_CHOOSE_PROCESS;
        } else if(key == InputKeyRight) {
            eth_worker_set_active_process(app->eth_worker, (EthWorkerProcess)app->draw_process);
            app->cursor_position = CURSOR_INSIDE_PROCESS;
        } else if(key == InputKeyBack) {
            app->cursor_position = CURSOR_EXIT_ICON;
        }
    } else if(app->cursor_position == CURSOR_INSIDE_PROCESS) {
        if(app->eth_worker->active_process->editing) {
            ethernet_view_process_keyevent(app->eth_worker->active_process, key);
        } else if(key == InputKeyLeft) {
            app->eth_worker->active_process->editing = 0;
            app->cursor_position = CURSOR_CHOOSE_PROCESS;
        } else if(key == InputKeyBack) {
            ethernet_view_process_move(app->eth_worker->active_process, 0);
            app->eth_worker->active_process->editing = 0;
            app->cursor_position = CURSOR_CHOOSE_PROCESS;
        } else if(key == InputKeyUp) {
            ethernet_view_process_move(app->eth_worker->active_process, -1);
        } else if(key == InputKeyDown) {
            ethernet_view_process_move(app->eth_worker->active_process, 1);
        } else if(key == InputKeyOk || key == InputKeyRight) {
            app->eth_worker->active_process->editing = 1;
        }
    } else if(app->cursor_position == CURSOR_EXIT_ICON) {
        if(key == InputKeyBack) {
            app->cursor_position = CURSOR_EXIT;
        } else if(key == InputKeyOk) {
            app->cursor_position = CURSOR_CHOOSE_PROCESS;
        }
    }
}

int32_t finik_eth_app(void* p) {
    UNUSED(p);
    FinikEthApp* app = finik_eth_app_alloc();

    InputEvent event;
    uint8_t long_press = 0;
    InputKey long_press_key = 0;

    while(1) {
        finik_eth_battery_info_update_model(app);
        if(furi_message_queue_get(app->event_queue, &event, 200) == FuriStatusOk) {
            if(event.type == InputTypePress) {
                finit_eth_app_key_handler(app, event.key);
            } else if(event.type == InputTypeLong) {
                long_press = 1;
                long_press_key = event.key;
            } else if(event.type == InputTypeRelease) {
                long_press = 0;
                long_press_key = 0;
            }
        }
        if(long_press && long_press_key != InputKeyBack) {
            finit_eth_app_key_handler(app, long_press_key);
        }
        if(app->cursor_position == CURSOR_EXIT) {
            eth_run(app->eth_worker, EthWorkerProcessExit);
            break;
        }
        view_port_update(app->view_port);
    }

    finik_eth_app_free(app);
    return 0;
}
