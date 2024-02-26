#include "pc_monitor.h"

static void render_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    PcMonitorApp* app = ctx;

    if(app->bt_state == BtStateRecieving) {
        canvas_clear(canvas);
        canvas_set_color(canvas, ColorBlack);
        canvas_set_font(canvas, FontKeyboard);

        uint8_t line = 0;
        uint8_t spacing = app->lines_count ? SCREEN_HEIGHT / app->lines_count : 0;
        uint8_t margin_top = spacing ? (spacing - LINE_HEIGHT) / 2 : 0;
        char str[32];

        if(app->data.cpu_usage <= 100) {
            if(app->lines_count) {
                canvas_draw_str(canvas, 1, margin_top + line * spacing + 9, "CPU");
                snprintf(str, 32, "%d%%", app->data.cpu_usage);
                elements_progress_bar_with_text(
                    canvas,
                    BAR_X,
                    margin_top + line * spacing,
                    BAR_WIDTH,
                    app->data.cpu_usage / 100.0f,
                    str);
            }

            line++;
        }

        if(app->data.ram_usage <= 100) {
            if(app->lines_count) {
                canvas_draw_str(canvas, 1, margin_top + line * spacing + 9, "RAM");
                snprintf(
                    str,
                    32,
                    "%.1f/%.1f %s",
                    (double)(app->data.ram_max * 0.1f * app->data.ram_usage * 0.01f),
                    (double)(app->data.ram_max * 0.1f),
                    app->data.ram_unit);
                elements_progress_bar_with_text(
                    canvas,
                    BAR_X,
                    margin_top + line * spacing,
                    BAR_WIDTH,
                    app->data.ram_usage * 0.01f,
                    str);
            }

            line++;
        }

        if(app->data.gpu_usage <= 100) {
            if(app->lines_count) {
                canvas_draw_str(canvas, 1, margin_top + line * spacing + 9, "GPU");
                snprintf(str, 32, "%d%%", app->data.gpu_usage);
                elements_progress_bar_with_text(
                    canvas,
                    BAR_X,
                    margin_top + line * spacing,
                    BAR_WIDTH,
                    app->data.gpu_usage / 100.0f,
                    str);
            }

            line++;
        }

        if(app->data.vram_usage <= 100) {
            if(app->lines_count) {
                canvas_draw_str(canvas, 1, margin_top + line * spacing + 9, "VRAM");
                snprintf(
                    str,
                    32,
                    "%.1f/%.1f %s",
                    (double)(app->data.vram_max * 0.1f * app->data.vram_usage * 0.01f),
                    (double)(app->data.vram_max * 0.1f),
                    app->data.vram_unit);
                elements_progress_bar_with_text(
                    canvas,
                    BAR_X,
                    margin_top + line * spacing,
                    BAR_WIDTH,
                    app->data.vram_usage * 0.01f,
                    str);
            }

            line++;
        }

        if(line == 0) app->bt_state = BtStateNoData;
        app->lines_count = line;
    } else {
        canvas_draw_str_aligned(
            canvas,
            64,
            32,
            AlignCenter,
            AlignCenter,
            app->bt_state == BtStateChecking ? "Checking BLE..." :
            app->bt_state == BtStateInactive ? "BLE inactive!" :
            app->bt_state == BtStateWaiting  ? "Waiting for data..." :
            app->bt_state == BtStateLost     ? "Connection lost!" :
                                               "No data!");
    }
}

static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

static uint16_t bt_serial_callback(SerialServiceEvent event, void* ctx) {
    furi_assert(ctx);
    PcMonitorApp* app = ctx;

    if(event.event == SerialServiceEventTypeDataReceived) {
        FURI_LOG_D(
            TAG,
            "SerialServiceEventTypeDataReceived. Size: %u/%u. Data: %s",
            event.data.size,
            sizeof(DataStruct),
            (char*)event.data.buffer);

        if(event.data.size == sizeof(DataStruct)) {
            memcpy(&app->data, event.data.buffer, sizeof(DataStruct));
            app->bt_state = BtStateRecieving;
            app->last_packet = furi_hal_rtc_get_timestamp();
        }
    }

    return 0;
}

static PcMonitorApp* pc_monitor_alloc() {
    PcMonitorApp* app = malloc(sizeof(PcMonitorApp));
    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    app->notification = furi_record_open(RECORD_NOTIFICATION);
    app->gui = furi_record_open(RECORD_GUI);
    app->bt = furi_record_open(RECORD_BT);

    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);
    view_port_draw_callback_set(app->view_port, render_callback, app);
    view_port_input_callback_set(app->view_port, input_callback, app->event_queue);
    return app;
}

static void pc_monitor_free(PcMonitorApp* app) {
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_message_queue_free(app->event_queue);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_BT);
    free(app);
}

int32_t pc_monitor_app(void* p) {
    UNUSED(p);
    PcMonitorApp* app = pc_monitor_alloc();

    bt_disconnect(app->bt);

    // Wait 2nd core to update nvm storage
    furi_delay_ms(200);

    bt_keys_storage_set_storage_path(app->bt, APP_DATA_PATH(".bt_serial.keys"));

    BleProfileSerialParams params = {
        .device_name_prefix = "PC Mon",
        .mac_xor = 0x0002,
    };
    app->ble_serial_profile = bt_profile_start(app->bt, ble_profile_serial, &params);

    furi_check(app->ble_serial_profile);

    ble_profile_serial_set_event_callback(
        app->ble_serial_profile, BT_SERIAL_BUFFER_SIZE, bt_serial_callback, app);
    furi_hal_bt_start_advertising();

    app->bt_state = BtStateWaiting;
    FURI_LOG_D(TAG, "Bluetooth is active!");

    // Main loop
    InputEvent event;
    while(true) {
        if(furi_message_queue_get(app->event_queue, &event, 1) == FuriStatusOk) {
            if(event.type == InputTypeShort && event.key == InputKeyBack) break;
        }

        if(app->bt_state == BtStateRecieving &&
           (furi_hal_rtc_get_timestamp() - app->last_packet > 5))
            app->bt_state = BtStateLost;
    }

    ble_profile_serial_set_event_callback(app->ble_serial_profile, 0, NULL, NULL);

    bt_disconnect(app->bt);

    // Wait 2nd core to update nvm storage
    furi_delay_ms(200);

    bt_keys_storage_set_default_path(app->bt);

    furi_check(bt_profile_restore_default(app->bt));

    pc_monitor_free(app);

    return 0;
}