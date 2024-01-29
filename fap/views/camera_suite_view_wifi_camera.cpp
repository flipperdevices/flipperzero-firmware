#include "../camera_suite.h"
#include "camera_suite_view_wifi_camera.h"

static void camera_suite_view_wifi_camera_draw(Canvas* canvas, void* wifi_model) {
    furi_assert(canvas);
    furi_assert(wifi_model);

    CameraSuiteViewWiFiCameraModel* model =
        static_cast<CameraSuiteViewWiFiCameraModel*>(wifi_model);

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_frame(canvas, 0, 0, FRAME_WIDTH, FRAME_HEIGHT);

    canvas_draw_str_aligned(canvas, 3, 3, AlignLeft, AlignTop, "Starting WiFi Stream at:");

    // Draw log from camera.
    canvas_draw_str_aligned(canvas, 3, 13, AlignLeft, AlignTop, furi_string_get_cstr(model->log));
}

static int32_t camera_suite_wifi_camera_worker(void* wifi_view_instance) {
    furi_assert(wifi_view_instance);

    CameraSuiteViewWiFiCamera* instance =
        static_cast<CameraSuiteViewWiFiCamera*>(wifi_view_instance);

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WORKER_EVENTS_MASK, FuriFlagWaitAny, FuriWaitForever);

        // Check if an error occurred.
        furi_check((events & FuriFlagError) == 0);

        // Check if the thread should stop.
        if(events & WorkerEventStop) {
            break;
        } else if(events & WorkerEventRx) {
            size_t length = 0;
            do {
                size_t buffer_size = 320;
                uint8_t data[buffer_size];
                length =
                    furi_stream_buffer_receive(instance->wifi_rx_stream, data, buffer_size, 0);
                if(length > 0) {
                    data[length] = '\0';

                    with_view_model_cpp(
                        instance->view,
                        CameraSuiteViewWiFiCameraModel*,
                        model,
                        {
                            furi_string_cat_printf(model->log, "%s", data);

                            // Truncate if too long.
                            model->log_strlen += length;
                            if(model->log_strlen >= 4096 - 1) {
                                furi_string_right(model->log, model->log_strlen / 2);
                                model->log_strlen = furi_string_size(model->log) + length;
                            }
                        },
                        true);
                }
            } while(length > 0);

            with_view_model_cpp(
                instance->view, CameraSuiteViewWiFiCameraModel*, model, { UNUSED(model); }, true);
        }
    }

    return 0;
}

static bool
    camera_suite_view_wifi_camera_input(InputEvent* input_event, void* wifi_view_instance) {
    furi_assert(wifi_view_instance);
    furi_assert(input_event);

    CameraSuiteViewWiFiCamera* instance =
        static_cast<CameraSuiteViewWiFiCamera*>(wifi_view_instance);
    uint8_t data[1];

    if(input_event->type == InputTypeRelease) {
        switch(input_event->key) {
        default:
            with_view_model_cpp(
                instance->view,
                CameraSuiteViewWiFiCameraModel*,
                model,
                {
                    UNUSED(model);
                    // Stop all sounds, reset the LED.
                    camera_suite_play_bad_bump(instance->context);
                    camera_suite_stop_all_sound(instance->context);
                    camera_suite_led_set_rgb(instance->context, 0, 0, 0);
                },
                true);
            break;
        }
    } else if(input_event->type == InputTypePress) {
        switch(input_event->key) {
        case InputKeyBack: {
            with_view_model_cpp(
                instance->view,
                CameraSuiteViewWiFiCameraModel*,
                model,
                {
                    UNUSED(model);

                    // Stop camera WiFi stream.
                    data[0] = 'w';
                    furi_hal_serial_tx(instance->wifi_serial_handle, data, 1);
                    furi_delay_ms(50);

                    // Go back to the main menu.
                    instance->callback(CameraSuiteCustomEventSceneCameraBack, instance->context);
                },
                true);
            break;
        }
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyOk:
        case InputKeyMAX:
        default: {
            break;
        }
        }
    }

    return false;
}

static void camera_suite_view_wifi_camera_exit(void* wifi_view_instance) {
    UNUSED(wifi_view_instance);
}

static void camera_suite_view_wifi_camera_model_init(CameraSuiteViewWiFiCameraModel* const model) {
    model->log = furi_string_alloc();
    furi_string_reserve(model->log, 4096);
}

static void camera_suite_view_wifi_camera_enter(void* wifi_view_instance) {
    furi_assert(wifi_view_instance);

    CameraSuiteViewWiFiCamera* instance =
        static_cast<CameraSuiteViewWiFiCamera*>(wifi_view_instance);

    // Start wifi camera stream.
    uint8_t data[1] = {'W'};
    furi_hal_serial_tx(instance->wifi_serial_handle, data, 1);
    furi_delay_ms(50);

    with_view_model_cpp(
        instance->view,
        CameraSuiteViewWiFiCameraModel*,
        model,
        { camera_suite_view_wifi_camera_model_init(model); },
        true);
}

static void wifi_camera_on_irq_cb(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxEvent event,
    void* wifi_view_instance) {
    furi_assert(handle);
    furi_assert(wifi_view_instance);

    CameraSuiteViewWiFiCamera* instance =
        static_cast<CameraSuiteViewWiFiCamera*>(wifi_view_instance);

    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
        furi_stream_buffer_send(instance->wifi_rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(instance->wifi_worker_thread), WorkerEventRx);
    }
}

CameraSuiteViewWiFiCamera* camera_suite_view_wifi_camera_alloc() {
    // Allocate memory for the instance
    CameraSuiteViewWiFiCamera* instance =
        (CameraSuiteViewWiFiCamera*)malloc(sizeof(CameraSuiteViewWiFiCamera));

    // Allocate the view object
    instance->view = view_alloc();

    // Allocate model
    view_allocate_model(
        instance->view, ViewModelTypeLocking, sizeof(CameraSuiteViewWiFiCameraModel));

    // Set context for the view (furi_assert crashes in events without this)
    view_set_context(instance->view, instance);

    // Allocate a stream buffer
    instance->wifi_rx_stream = furi_stream_buffer_alloc(1024, 1);

    // Allocate a thread for this camera to run on.
    // @NOTICE: THIS SEEMINGLY BREAKS THE CAMERA VIEW THREAD...
    FuriThread* thread = furi_thread_alloc_ex(
        "Camera_Suite_WiFi_Rx_Thread", 1024, camera_suite_wifi_camera_worker, instance);
    instance->wifi_worker_thread = thread;
    furi_thread_start(instance->wifi_worker_thread);

    // Set up UART thread.
    instance->wifi_serial_handle = furi_hal_serial_control_acquire(UART_CH);
    furi_check(instance->wifi_serial_handle);
    furi_hal_serial_init(instance->wifi_serial_handle, 230400);

    // Enable UART1 and set the IRQ callback.
    furi_hal_serial_async_rx_start(
        instance->wifi_serial_handle, wifi_camera_on_irq_cb, instance, false);

    // Set draw callback
    view_set_draw_callback(instance->view, (ViewDrawCallback)camera_suite_view_wifi_camera_draw);

    // Set input callback
    view_set_input_callback(instance->view, camera_suite_view_wifi_camera_input);

    // Set enter callback
    view_set_enter_callback(instance->view, camera_suite_view_wifi_camera_enter);

    // Set exit callback
    view_set_exit_callback(instance->view, camera_suite_view_wifi_camera_exit);

    with_view_model_cpp(
        instance->view,
        CameraSuiteViewWiFiCameraModel*,
        model,
        { camera_suite_view_wifi_camera_model_init(model); },
        true);

    return instance;
}

void camera_suite_view_wifi_camera_free(CameraSuiteViewWiFiCamera* instance) {
    furi_assert(instance);

    // Free the worker thread.
    furi_thread_flags_set(furi_thread_get_id(instance->wifi_worker_thread), WorkerEventStop);
    furi_thread_join(instance->wifi_worker_thread);
    furi_thread_free(instance->wifi_worker_thread);

    // Free the allocated stream buffer.
    furi_stream_buffer_free(instance->wifi_rx_stream);

    // Deinitialize the UART.
    furi_hal_serial_deinit(instance->wifi_serial_handle);
    furi_hal_serial_control_release(instance->wifi_serial_handle);

    with_view_model_cpp(
        instance->view,
        CameraSuiteViewWiFiCameraModel*,
        model,
        { furi_string_free(model->log); },
        true);
    view_free(instance->view);
    free(instance);
}

View* camera_suite_view_wifi_camera_get_view(CameraSuiteViewWiFiCamera* instance) {
    furi_assert(instance);
    return instance->view;
}

void camera_suite_view_wifi_camera_set_callback(
    CameraSuiteViewWiFiCamera* instance,
    CameraSuiteViewWiFiCameraCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}
