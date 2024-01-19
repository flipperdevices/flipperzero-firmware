#include "camera.h"

static void camera_view_draw_callback(Canvas* canvas, void* _model) {
    UartDumpModel* model = _model;

    // Prepare canvas
    //canvas_clear(canvas);
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

    if(!model->initialized) {
        /*if(!model->marauderInitialized)
        {
          // Init marauder into stream mode
          uint8_t data[] = "\nstream\n";
          furi_hal_serial_tx(app->serial_handle, data, sizeof(data));
        }*/

        canvas_draw_icon(canvas, 80, 21, &I_WarningDolphinFlip_45x42);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 8, 12, "Waiting ESP32-CAM...");
        canvas_draw_str(canvas, 20, 24, "VCC - 3V3/5V");
        canvas_draw_str(canvas, 20, 34, "GND - GND");
        canvas_draw_str(canvas, 20, 44, "U0R - TX");
        canvas_draw_str(canvas, 20, 54, "U0T - RX");
    }
}

void get_timefilename(FuriString* name) {
    FuriHalRtcDateTime datetime = {0};
    furi_hal_rtc_get_datetime(&datetime);
    furi_string_printf(
        name,
        APP_DATA_PATH("%.4d%.2d%.2d-%.2d%.2d%.2d.bmp"),
        datetime.year,
        datetime.month,
        datetime.day,
        datetime.hour,
        datetime.minute,
        datetime.second);
}

static void save_image(void* context) {
    UartEchoApp* app = context;
    furi_assert(app);

    NotificationApp* notifications = furi_record_open(RECORD_NOTIFICATION);

    // We need a storage struct (gain accesso to the filesystem API )
    Storage* storage = furi_record_open(RECORD_STORAGE);

    // storage_file_alloc gives to us a File pointer using the Storage API.
    File* file = storage_file_alloc(storage);

    if(storage_common_stat(storage, IMAGE_FILE_DIRECTORY_PATH, NULL) == FSE_NOT_EXIST) {
        storage_simply_mkdir(storage, IMAGE_FILE_DIRECTORY_PATH);
    }

    // create file name
    FuriString* file_name = furi_string_alloc();
    get_timefilename(file_name);

    // this functions open a file, using write access and creates new file if not exist.
    bool result =
        storage_file_open(file, furi_string_get_cstr(file_name), FSAM_WRITE, FSOM_OPEN_ALWAYS);
    //bool result = storage_file_open(file, EXT_PATH("DCIM/test.bmp"), FSAM_WRITE, FSOM_OPEN_ALWAYS);
    furi_string_free(file_name);

    if(result) {
        storage_file_write(file, bitmap_header, BITMAP_HEADER_LENGTH);
        with_view_model(
            app->view,
            UartDumpModel * model,
            {
                int8_t row_buffer[ROW_BUFFER_LENGTH];
                for(size_t i = 64; i > 0; --i) {
                    for(size_t j = 0; j < ROW_BUFFER_LENGTH; ++j) {
                        row_buffer[j] = model->pixels[((i - 1) * ROW_BUFFER_LENGTH) + j];
                    }
                    storage_file_write(file, row_buffer, ROW_BUFFER_LENGTH);
                }
            },
            false);
    }

    // Closing the "file descriptor"
    storage_file_close(file);

    // Freeing up memory
    storage_file_free(file);

    notification_message(notifications, result ? &sequence_success : &sequence_error);
}

static bool camera_view_input_callback(InputEvent* event, void* context) {
    if(event->type == InputTypePress) {
        uint8_t data[1];
        if(event->key == InputKeyUp) {
            data[0] = 'C';
        } else if(event->key == InputKeyDown) {
            data[0] = 'c';
        } else if(event->key == InputKeyRight) {
            data[0] = '>';
        } else if(event->key == InputKeyLeft) {
            data[0] = '<';
        } else if(event->key == InputKeyOk) {
            save_image(context);
        }
        UartEchoApp* app = context;
        furi_hal_serial_tx(app->serial_handle, data, 1);
    }

    return false;
}

static uint32_t camera_exit(void* context) {
    UNUSED(context);
    return VIEW_NONE;
}

static void
    camera_on_irq_cb(FuriHalSerialHandle* handle, FuriHalSerialRxEvent event, void* context) {
    furi_assert(context);
    UartEchoApp* app = context;

    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
        furi_stream_buffer_send(app->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(app->worker_thread), WorkerEventRx);
    }
}

static void process_ringbuffer(UartDumpModel* model, uint8_t byte) {
    //// 1. Phase: filling the ringbuffer
    if(model->ringbuffer_index == 0 && byte != 'Y') { // First char has to be 'Y' in the buffer.
        return;
    }

    if(model->ringbuffer_index == 1 &&
       byte != ':') { // Second char has to be ':' in the buffer or reset.
        model->ringbuffer_index = 0;
        process_ringbuffer(model, byte);
        return;
    }

    model->row_ringbuffer[model->ringbuffer_index] =
        byte; // Assign current byte to the ringbuffer;
    ++model->ringbuffer_index; // Increment the ringbuffer index

    if(model->ringbuffer_index < RING_BUFFER_LENGTH) { // Let's wait 'till the buffer fills.
        return;
    }

    //// 2. Phase: flushing the ringbuffer to the framebuffer
    model->ringbuffer_index = 0; // Let's reset the ringbuffer
    model->initialized = true; // We've successfully established the connection
    size_t row_start_index =
        model->row_ringbuffer[2] * ROW_BUFFER_LENGTH; // Third char will determine the row number

    if(row_start_index > LAST_ROW_INDEX) { // Failsafe
        row_start_index = 0;
    }

    for(size_t i = 0; i < ROW_BUFFER_LENGTH; ++i) {
        model->pixels[row_start_index + i] =
            model->row_ringbuffer[i + 3]; // Writing the remaining 16 bytes into the frame buffer
    }
}

static int32_t camera_worker(void* context) {
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
                    //furi_hal_serial_tx(app->serial_handle, data, length);
                    with_view_model(
                        app->view,
                        UartDumpModel * model,
                        {
                            for(size_t i = 0; i < length; i++) {
                                process_ringbuffer(model, data[i]);
                            }
                        },
                        false);
                }
            } while(length > 0);

            notification_message(app->notification, &sequence_notification);
            with_view_model(
                app->view, UartDumpModel * model, { UNUSED(model); }, true);
        }
    }

    return 0;
}

static UartEchoApp* camera_app_alloc() {
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
    view_set_draw_callback(app->view, camera_view_draw_callback);
    view_set_input_callback(app->view, camera_view_input_callback);
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

    view_set_previous_callback(app->view, camera_exit);
    view_dispatcher_add_view(app->view_dispatcher, 0, app->view);
    view_dispatcher_switch_to_view(app->view_dispatcher, 0);

    app->worker_thread = furi_thread_alloc_ex("UsbUartWorker", 2048, camera_worker, app);
    furi_thread_start(app->worker_thread);

    // Enable uart listener
    app->serial_handle = furi_hal_serial_control_acquire(FuriHalSerialIdUsart);
    furi_check(app->serial_handle);
    furi_hal_serial_init(app->serial_handle, 230400);
    furi_hal_serial_async_rx_start(app->serial_handle, camera_on_irq_cb, app, false);

    furi_hal_power_disable_external_3_3v();
    furi_hal_power_disable_otg();
    furi_delay_ms(200);
    furi_hal_power_enable_external_3_3v();
    furi_hal_power_enable_otg();
    for(int i = 0; i < 2; i++) {
        furi_delay_ms(500);
        furi_hal_serial_tx(app->serial_handle, (uint8_t[1]){'c'}, 1);
    }
    furi_delay_ms(1);
    return app;
}

static void camera_app_free(UartEchoApp* app) {
    furi_assert(app);

    furi_hal_serial_deinit(app->serial_handle);
    furi_hal_serial_control_release(app->serial_handle);

    furi_thread_flags_set(furi_thread_get_id(app->worker_thread), WorkerEventStop);
    furi_thread_join(app->worker_thread);
    furi_thread_free(app->worker_thread);

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

int32_t camera_app(void* p) {
    UNUSED(p);

    UartEchoApp* app = camera_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    camera_app_free(app);

    furi_hal_power_disable_otg();

    return 0;
}
