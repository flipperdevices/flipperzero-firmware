#include "logic_analyzer_app.h"
#include "logic_analyzer_icons.h"

#define COUNT(x) ((size_t)(sizeof(x) / sizeof((x)[0])))

static void render_callback(Canvas* const canvas, void* cb_ctx);

static const GpioPin* gpios[] = {
    &gpio_ext_pc0,
    &gpio_ext_pc1,
    &gpio_ext_pc3,
    &gpio_ext_pb2,
    &gpio_ext_pb3,
    &gpio_ext_pa4,
    &gpio_ext_pa6,
    &gpio_ext_pa7};

// static const char* gpio_names[] = {"PC0", "PC1", "PC3", "PB2", "PB3", "PA4", "PA6", "PA7"};

static void render_callback(Canvas* const canvas, void* cb_ctx) {
    AppFSM* app = cb_ctx;
    furi_mutex_acquire(app->mutex, FuriWaitForever);

    if(app == NULL) {
        return;
    }

    if(!app->processing) {
        furi_mutex_release(app->mutex);
        return;
    }

    char buffer[64];
    int y = 10;

    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, "State");
    y += 10;

    if(app->uart) {
        UsbUartState st;
        usb_uart_get_state(app->uart, &st);

        snprintf(buffer, sizeof(buffer), "Rx %ld / Tx %ld", st.rx_cnt, st.tx_cnt);
        canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
        y += 20;
    }
    canvas_set_font(canvas, FontSecondary);

    if(app->sump) {
        snprintf(
            buffer,
            sizeof(buffer),
            "%02X %lX %ld %lX %lX %X",
            app->sump->flags,
            app->sump->divider,
            app->sump->delay_count,
            app->sump->trig_mask,
            app->sump->trig_values,
            app->sump->trig_config);

        canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
        y += 10;

        if(app->sump->armed) {
            snprintf(
                buffer,
                sizeof(buffer),
                "Captured: %u / %ld",
                app->capture_pos,
                app->sump->read_count);
            canvas_draw_str_aligned(canvas, 5, y, AlignLeft, AlignBottom, buffer);
            y += 20;
        }

        if(app->sump->armed) {
            elements_button_center(canvas, "Trigger");
        }
    }

    furi_mutex_release(app->mutex);
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    /* better skip than sorry */
    if(furi_message_queue_get_count(event_queue) < QUEUE_SIZE) {
        AppEvent event = {.type = EventKeyPress, .input = *input_event};
        furi_message_queue_put(event_queue, &event, 100);
    }
}

static bool message_process(AppFSM* app) {
    bool processing = true;
    AppEvent event;
    FuriStatus event_status = furi_message_queue_get(app->event_queue, &event, 100);

    if(event_status != FuriStatusOk) {
        return true;
    }

    switch(event.type) {
    case EventKeyPress: {
        if(event.input.type != InputTypePress) {
            break;
        }

        switch(event.input.key) {
        case InputKeyUp:
            break;

        case InputKeyDown:
            break;

        case InputKeyRight:
            break;

        case InputKeyLeft:
            break;

        case InputKeyOk:
            /* when armed, trigger by pressing the button */
            if(app->sump->armed) {
                for(size_t pos = app->capture_pos; pos < app->sump->read_count; pos++) {
                    app->capture_buffer[app->sump->read_count - 1 - pos] = 0;
                }
                app->sump->armed = false;
                AppEvent event = {.type = EventBufferFilled};
                furi_message_queue_put(app->event_queue, &event, 100);
            }
            break;

        case InputKeyBack:
            processing = false;
            break;

        default:
            break;
        }

        break;
    }

    case EventBufferFilled: {
        usb_uart_tx_data(app->uart, app->capture_buffer, app->sump->read_count);
        break;
    }

    default: {
        break;
    }
    }

    return processing;
}

size_t data_received(void* ctx, uint8_t* data, size_t length) {
    AppFSM* app = (AppFSM*)ctx;

    snprintf(
        app->state_string,
        sizeof(app->state_string),
        "Rx: %02x '%c' (total %u)",
        data[0],
        data[0],
        length);

    return sump_handle(app->sump, data, length);
}

void tx_sump_tx(void* ctx, uint8_t* data, size_t length) {
    AppFSM* app = (AppFSM*)ctx;

    usb_uart_tx_data(app->uart, data, length);
}

static uint8_t levels_get(AppFSM* app) {
    UNUSED(app);
    uint32_t port_a = GPIOA->IDR;
    uint32_t port_b = GPIOB->IDR;
    uint32_t port_c = GPIOC->IDR;

    /*   7  6  5  4  3  2  1  0
        A7 A6 A4 B3 B2 C3 C1 C0 */

    uint8_t ret = (port_a & 0xC0) | ((port_a & 0x10) << 1) | ((port_b & 0x0C) << 1) |
                  ((port_c & 0x08) >> 1) | (port_c & 0x03);

    return ret;
}

static int32_t capture_thread_worker(void* context) {
    AppFSM* app = (AppFSM*)context;
    uint8_t prev_levels = 0;

    while(app->processing) {
        app->current_levels = levels_get(app);

        if(app->sump->armed) {
            uint8_t relevant_levels = app->current_levels & app->sump->trig_mask;
            uint8_t prev_relevant_levels = prev_levels & app->sump->trig_mask;

            if(relevant_levels != prev_relevant_levels) {
                prev_levels = app->current_levels;
                app->capture_buffer[app->sump->read_count - 1 - app->capture_pos++] =
                    app->current_levels;

                if(app->capture_pos >= app->sump->read_count) {
                    app->sump->armed = false;
                    AppEvent event = {.type = EventBufferFilled};
                    furi_message_queue_put(app->event_queue, &event, 100);
                }
            }
        } else {
            prev_levels = app->current_levels;
            app->capture_pos = 0;
            app->triggered = false;
            prev_levels = 0;
            furi_delay_ms(50);
        }
    }

    return 0;
}

static bool app_init(AppFSM* const app) {
    strcpy(app->state_string, "none");
    app->mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    if(!app->mutex) {
        FURI_LOG_E(TAG, "cannot create mutex\r\n");
        free(app);
        return false;
    }

    app->processing = true;

    app->notification = furi_record_open(RECORD_NOTIFICATION);
    app->gui = furi_record_open(RECORD_GUI);
    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->storage = furi_record_open(RECORD_STORAGE);

    app->view_port = view_port_alloc();
    app->event_queue = furi_message_queue_alloc(QUEUE_SIZE, sizeof(AppEvent));

    view_port_draw_callback_set(app->view_port, render_callback, app);
    view_port_input_callback_set(app->view_port, input_callback, app->event_queue);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    UsbUartConfig uart_config;

    uart_config.vcp_ch = 1;
    uart_config.rx_data = &data_received;
    uart_config.rx_data_ctx = app;

    app->uart = usb_uart_enable(&uart_config);
    app->sump = sump_alloc();
    app->sump->tx_data = tx_sump_tx;
    app->sump->tx_data_ctx = app;

    app->capture_buffer = malloc(MAX_SAMPLE_MEM);

    for(size_t io = 0; io < COUNT(gpios); io++) {
        furi_hal_gpio_init(gpios[io], GpioModeInput, GpioPullNo, GpioSpeedVeryHigh);
    }

    app->capture_thread = furi_thread_alloc_ex("capture_thread", 1024, capture_thread_worker, app);
    furi_thread_start(app->capture_thread);

    return true;
}

static void app_deinit(AppFSM* const app) {
    view_port_enabled_set(app->view_port, false);
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_message_queue_free(app->event_queue);
    furi_mutex_free(app->mutex);

    furi_thread_join(app->capture_thread);
    furi_thread_free(app->capture_thread);

    free(app->capture_buffer);

    sump_free(app->sump);

    usb_uart_disable(app->uart);

    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
}

int32_t logic_analyzer_app_main(void* p) {
    UNUSED(p);

    AppFSM* app = malloc(sizeof(AppFSM));
    app_init(app);

    notification_message_block(app->notification, &sequence_display_backlight_enforce_on);

    while(app->processing) {
        app->processing = message_process(app);

        view_port_update(app->view_port);
    }

    notification_message_block(app->notification, &sequence_display_backlight_enforce_auto);

    app_deinit(app);
    free(app);

    return 0;
}
