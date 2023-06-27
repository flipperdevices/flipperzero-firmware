#include "pc_monitor.h"

static void render_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    PcMonitorApp* app = ctx;

    if(app->bt_state == BtStateRecieving) {
        canvas_clear(canvas);
        canvas_set_color(canvas, ColorBlack);
        canvas_set_font(canvas, FontKeyboard);

        uint8_t line = 0;
        char str[32];

        canvas_draw_str(canvas, 0, 10, "CPU");
        snprintf(str, 32, "%d%%", app->data.cpu_usage);
        elements_progress_bar_with_text(canvas, 20, 1, 108, app->data.cpu_usage / 100.0f, str);

        line = 1;
        canvas_draw_str(canvas, 0, 10 + 14 * line, "RAM");
        snprintf(
            str,
            32,
            "%.1f/%.1f %s",
            app->data.ram_used / (double)10,
            app->data.ram_max / (double)10,
            app->data.ram_unit);
        elements_progress_bar_with_text(
            canvas, 20, 1 + 14 * line, 108, app->data.ram_used / (float)app->data.ram_max, str);

        line = 2;
        canvas_draw_str(canvas, 0, 10 + 14 * line, "GPU");
        snprintf(str, 32, "%d%%", app->data.gpu_usage);
        elements_progress_bar_with_text(
            canvas, 20, 1 + 14 * line, 108, app->data.gpu_usage / 100.0f, str);
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
                                               "Connection lost!");
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
        app->bt_state = BtStateRecieving;
        FURI_LOG_D(
            TAG,
            "SerialServiceEventTypeDataReceived. Size: %u. Data: %s",
            event.data.size,
            (char*)event.data.buffer);

        uint8_t step = 0;

        // strtok is not in the API
        char temp_str[32] = {0};
        uint8_t lastIndex = 0;
        for(size_t i = 0; i <= strlen((char*)event.data.buffer); i++) {
            if(event.data.buffer[i] == ':' || event.data.buffer[i] == '\0') {
                char* sub_str = temp_str + lastIndex;
                printf("step: %d, sub_str: %s\r\n", step, sub_str);

                if(step == 0) app->data.cpu_usage = atoi(sub_str);
                if(step == 1) app->data.gpu_usage = atoi(sub_str);
                if(step == 2) app->data.ram_used = atoi(sub_str);
                if(step == 3) app->data.ram_max = atoi(sub_str);
                if(step == 4) strcpy(app->data.ram_unit, sub_str);

                step++;
                lastIndex = i + 1;
                //memset(temp_str, 0, 32);
                //for(size_t j = 0; j < 32; j++) temp_str[j] = 0;
            }

            temp_str[i] = event.data.buffer[i];
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

    if(furi_hal_bt_is_active()) {
        furi_hal_bt_serial_set_event_callback(BT_SERIAL_BUFFER_SIZE, bt_serial_callback, app);
        furi_hal_bt_start_advertising();

        app->bt_state = BtStateWaiting;
        FURI_LOG_D(TAG, "Bluetooth is active!");
    } else {
        app->bt_state = BtStateInactive;
        FURI_LOG_W(TAG, "Please, enable the Bluetooth and restart the app");
    }

    InputEvent event;
    while(furi_message_queue_get(app->event_queue, &event, FuriWaitForever) == FuriStatusOk) {
        if(event.type == InputTypeShort && event.key == InputKeyBack) break;
    }

    furi_hal_bt_serial_set_event_callback(0, NULL, NULL);

    pc_monitor_free(app);

    return 0;
}