#include "malveke_gb_emulator.h"
#include <malveke_gb_emulator_icons.h>



static void malveke_gb_emulator_view_draw_callback(Canvas* canvas, void* _model) {
    UartDumpModel* model = _model;

    // Prepare canvas
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, 0, 0, FRAME_WIDTH, FRAME_HEIGTH);
    
    for(size_t p = 0; p < FRAME_BUFFER_LENGTH; ++p) {
        uint8_t x = p % ROW_BUFFER_LENGTH; // 0 .. 15
        uint8_t y = p / ROW_BUFFER_LENGTH; // 0 .. 63

        for(uint8_t i = 0; i < 8; ++i) {
            if((model->pixels[p] & (1 << (7 - i))) != 0) {
                canvas_draw_dot(canvas, (x * 8) + i, y);
            }
        }
    }

    if (!model->initialized){
        canvas_draw_icon(canvas, 60, 7, &I_malveke_67x49);
        canvas_set_font(canvas, FontSecondary);

        canvas_draw_str(canvas, 4, 25, "Connect");
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 4, 35, "MALVEKE");
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 4, 44, "into Flipper");
        elements_button_center(canvas, "Ok"); 
        // canvas_set_font(canvas, FontPrimary);
        // canvas_draw_str(canvas, 8, 28, "GAME BOY");
        // canvas_draw_icon(canvas, 76, 8, &I_gbcam_48x49);
        // canvas_set_font(canvas, FontSecondary);
        // canvas_draw_str(canvas, 8, 18, "WAITING");
        // canvas_set_font(canvas, FontPrimary);
        // canvas_draw_str(canvas, 8, 38, "CAMERA...");
        // canvas_set_font(canvas, FontSecondary);
        // canvas_draw_str(canvas, 9, 47, "Insert Cartridge");
        // elements_button_center(canvas, "Ok"); 
    }
}


static bool malveke_gb_emulator_view_input_callback(InputEvent* event, void* context) {
    UartEchoApp* instance = context;
    UNUSED(instance);
    if (event->type == InputTypePress){
        if (event->key == InputKeyUp){
            const char gbemulator_command_up[] = "U\n";
            furi_hal_uart_tx(FuriHalUartIdUSART1, (uint8_t*)gbemulator_command_up, strlen(gbemulator_command_up));
        }
        else if (event->key == InputKeyDown){
            const char gbemulator_command_down[] = "D\n";
            furi_hal_uart_tx(FuriHalUartIdUSART1, (uint8_t*)gbemulator_command_down, strlen(gbemulator_command_down));
        }
        else if (event->key == InputKeyRight){
            const char gbemulator_command_right[] = ">\n";
            furi_hal_uart_tx(FuriHalUartIdUSART1, (uint8_t*)gbemulator_command_right, strlen(gbemulator_command_right));
        }
        else if (event->key == InputKeyLeft){
            const char gbemulator_command_left[] = "<\n";
            furi_hal_uart_tx(FuriHalUartIdUSART1, (uint8_t*)gbemulator_command_left, strlen(gbemulator_command_left));
        }
        else if (event->key == InputKeyOk){
            with_view_model(
                instance->view,
                UartDumpModel * model,
                {
                    if(!model->initialized) {
                        model->initialized = true;
                    } else {
                        const char gbemulator_command_OK[] = "S\n";
                        furi_hal_uart_tx(FuriHalUartIdUSART1, (uint8_t*)gbemulator_command_OK, strlen(gbemulator_command_OK));  
                    }
                },
                false);

            
        }
    }
    return false;
}

static uint32_t malveke_gb_emulator_exit(void* context) {
    UNUSED(context);
    const char stop_command[] = "stopgblivecamera\n";
    furi_hal_uart_tx(FuriHalUartIdUSART1, (uint8_t*)stop_command, strlen(stop_command));
    return VIEW_NONE;
}

static void malveke_gb_emulator_on_irq_cb(UartIrqEvent ev, uint8_t data, void* context) {
    furi_assert(context);
    UartEchoApp* app = context;

    if(ev == UartIrqEventRXNE) {
        furi_stream_buffer_send(app->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(app->worker_thread), WorkerEventRx);
    }
}

static void process_ringbuffer(UartDumpModel* model, uint8_t byte) {

    //// 1. Phase: filling the ringbuffer
    if (model->ringbuffer_index == 0 && byte != 'Y'){ // First char has to be 'Y' in the buffer.
        return;
    }
    
    if (model->ringbuffer_index == 1 && byte != ':'){ // Second char has to be ':' in the buffer or reset.
        model->ringbuffer_index = 0;
        process_ringbuffer(model, byte);
        return;
    }

    model->row_ringbuffer[model->ringbuffer_index] = byte; // Assign current byte to the ringbuffer;
    ++model->ringbuffer_index; // Increment the ringbuffer index

    if (model->ringbuffer_index < RING_BUFFER_LENGTH){ // Let's wait 'till the buffer fills.
        return;
    }

    //// 2. Phase: flushing the ringbuffer to the framebuffer
    model->ringbuffer_index = 0; // Let's reset the ringbuffer
    // model->initialized = true; // We've successfully established the connection
    size_t row_start_index = model->row_ringbuffer[2] * ROW_BUFFER_LENGTH; // Third char will determine the row number

    if (row_start_index > LAST_ROW_INDEX){ // Failsafe
        row_start_index = 0;
    }

    for (size_t i = 0; i < ROW_BUFFER_LENGTH; ++i) {
        model->pixels[row_start_index + i] = model->row_ringbuffer[i+3]; // Writing the remaining 16 bytes into the frame buffer
    }
}

static int32_t malveke_gb_emulator_worker(void* context) {
    furi_assert(context);
    UartEchoApp* app = context;

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_EVENTS_MASK, FuriFlagWaitAny, FuriWaitForever);
        furi_check((events & FuriFlagError) == 0);

        if(events & WorkerEventStop) break;
        if(events & WorkerEventRx) {
            size_t length = 0;
            do {
                size_t intended_data_size = 64;
                uint8_t data[intended_data_size];
                length = furi_stream_buffer_receive(app->rx_stream, data, intended_data_size, 0);

                if(length > 0) {
                    with_view_model(
                        app->view,
                        UartDumpModel * model, {
                            for(size_t i = 0; i < length; i++) {
                                process_ringbuffer(model, data[i]);
                            }
                        },
                        false);
                }
            } while(length > 0);

            notification_message(app->notification, &sequence_notification);
            with_view_model(app->view, UartDumpModel * model, { UNUSED(model); }, true);
        }
    }

    return 0;
}

static UartEchoApp* malveke_gb_emulator_app_alloc() {
    UartEchoApp* app = malloc(sizeof(UartEchoApp));

    app->rx_stream = furi_stream_buffer_alloc(2048, 1);

    // Gui
    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Views
    app->view = view_alloc();
    view_set_context(app->view, app);
    view_set_draw_callback(app->view, malveke_gb_emulator_view_draw_callback);
    view_set_input_callback(app->view, malveke_gb_emulator_view_input_callback);
    view_allocate_model(app->view, ViewModelTypeLocking, sizeof(UartDumpModel));

    with_view_model(
        app->view,
        UartDumpModel * model,
        {
            for(size_t i = 0; i < FRAME_BUFFER_LENGTH; i++) {
                model->pixels[i] = 0;
            }
        },
        true);

    view_set_previous_callback(app->view, malveke_gb_emulator_exit);
    view_dispatcher_add_view(app->view_dispatcher, 0, app->view);
    view_dispatcher_switch_to_view(app->view_dispatcher, 0);

    app->worker_thread = furi_thread_alloc_ex("UsbUartWorker", 2048, malveke_gb_emulator_worker, app);
    furi_thread_start(app->worker_thread);

    // Enable uart listener (UART & UART1)
    // furi_hal_console_disable();
    furi_hal_uart_set_br(FuriHalUartIdUSART1, 115200);
    furi_hal_uart_init(FuriHalUartIdLPUART1, 250000);
    furi_hal_uart_set_br(FuriHalUartIdLPUART1, 250000);
    furi_hal_uart_set_irq_cb(FuriHalUartIdLPUART1, malveke_gb_emulator_on_irq_cb, app);
    // furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, malveke_gb_emulator_on_irq_cb, app);
    furi_hal_power_enable_otg();
    furi_delay_ms(1); 
    return app;
}

static void malveke_gb_emulator_app_free(UartEchoApp* app) {
    furi_assert(app);

    furi_thread_flags_set(furi_thread_get_id(app->worker_thread), WorkerEventStop);
    furi_thread_join(app->worker_thread);
    furi_thread_free(app->worker_thread);

    // furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, NULL, NULL);
    furi_hal_uart_set_irq_cb(FuriHalUartIdLPUART1, NULL, NULL);
    furi_hal_uart_deinit(FuriHalUartIdLPUART1);

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, 0);

    view_free(app->view);
    view_dispatcher_free(app->view_dispatcher);

    // Close gui record
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    app->gui = NULL;

    furi_stream_buffer_free(app->rx_stream);

    // Free rest
    free(app);
}

int32_t malveke_gb_emulator_app(void* p) {
    UNUSED(p);

    UartEchoApp* app = malveke_gb_emulator_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    malveke_gb_emulator_app_free(app);
    
    furi_hal_power_disable_otg();

    return 0;
}