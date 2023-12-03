#include "malveke_pin_test.h"
#include <stdio.h>  // Para sprintf
#include <string.h> // Para strlen


static void gb_live_camera_view_draw_callback(Canvas* canvas, void* _model) {
    UartDumpModel* model = _model;
    UNUSED(model);
    // Prepare canvas

    char show_pin[20];
    snprintf(show_pin, sizeof(show_pin), "PIN: %d", model->pin );


    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, 0, 0, FRAME_WIDTH, FRAME_HEIGTH);
    canvas_draw_str(canvas, 8, 28, show_pin);
    // canvas_draw_str(canvas, 60, 28,(char*) model->pin);

    
}
static uint32_t gb_live_camera_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}
static void gb_live_camera_app_free(UartEchoApp* app) {
    furi_assert(app);

    // furi_thread_flags_set(furi_thread_get_id(app->worker_thread), WorkerEventStop);
    // furi_thread_join(app->worker_thread);
    // furi_thread_free(app->worker_thread);

    // furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, NULL, NULL);
    // furi_hal_uart_set_irq_cb(FuriHalUartIdLPUART1, NULL, NULL);
    // furi_hal_uart_deinit(FuriHalUartIdLPUART1);

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, 0);

    view_free(app->view);
    view_dispatcher_free(app->view_dispatcher);

    // Close gui record
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    app->gui = NULL;

    // furi_stream_buffer_free(app->rx_stream);

    // Free rest
    free(app);
}
static bool gb_live_camera_view_input_callback(InputEvent* event, void* context) {
    UartEchoApp* app = context;
    if (event->type == InputTypePress){
        
        if (event->key == InputKeyRight){
            with_view_model(
                app->view,
                UartDumpModel * model,
                {
                    if(model->pin + 1 <= 31) {
                        model->pin += 1;
                        char gbpin_start_command[80]; // A reasonably sized buffer.
                        snprintf(gbpin_start_command, sizeof(gbpin_start_command), "gbpin -p %d\n", model->pin);
                        furi_hal_uart_tx(FuriHalUartIdUSART1, (uint8_t*)gbpin_start_command, strlen(gbpin_start_command));
                    }
                    
                },
                true);
           
        }
        else if (event->key == InputKeyLeft){
            with_view_model(
                app->view,
                UartDumpModel * model,
                {
                    if(model->pin -1 >= 2) {
                        model->pin -= 1;
                        char gbpin_start_command[80]; // A reasonably sized buffer.
                        snprintf(gbpin_start_command, sizeof(gbpin_start_command), "gbpin -p %d\n", model->pin);
                        furi_hal_uart_tx(FuriHalUartIdUSART1, (uint8_t*)gbpin_start_command, strlen(gbpin_start_command));
                    }
                },
                true);
        }
    }
    return false;
}
static UartEchoApp* gb_live_camera_app_alloc() {
    UartEchoApp* app = malloc(sizeof(UartEchoApp));

    // app->rx_stream = furi_stream_buffer_alloc(2048, 1);

    // Gui
    app->gui = furi_record_open(RECORD_GUI);

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Views
    app->view = view_alloc();
    view_set_context(app->view, app);
    view_set_draw_callback(app->view, gb_live_camera_view_draw_callback);
    view_set_input_callback(app->view, gb_live_camera_view_input_callback);
    view_allocate_model(app->view, ViewModelTypeLocking, sizeof(UartDumpModel));


    with_view_model(
        app->view,
        UartDumpModel * model,
        {
            model->pin = 2;
        },
        true);


    view_set_previous_callback(app->view, gb_live_camera_exit);
    view_dispatcher_add_view(app->view_dispatcher, 0, app->view);
    view_dispatcher_switch_to_view(app->view_dispatcher, 0);

    // app->worker_thread = furi_thread_alloc_ex("UsbUartWorker", 2048, gb_live_camera_worker, app);
    // furi_thread_start(app->worker_thread);

    // Enable uart listener (UART & UART1)
    // furi_hal_console_disable();
    furi_hal_uart_set_br(FuriHalUartIdUSART1, 115200);
    // furi_hal_uart_set_irq_cb(FuriHalUartIdLPUART1, gb_live_camera_on_irq_cb, app);
    furi_hal_power_disable_otg();
    // furi_hal_power_suppress_charge_enter();
    furi_delay_ms(1); 
    return app;
}


int32_t malveke_pin_test_app(void* p) {
    UNUSED(p);

    UartEchoApp* app = gb_live_camera_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    gb_live_camera_app_free(app);
    
    furi_hal_power_disable_otg();

    return 0;
}