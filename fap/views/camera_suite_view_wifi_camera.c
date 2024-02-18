#include "../camera_suite.h"
#include "camera_suite_view_wifi_camera.h"

#include "../helpers/camera_suite_haptic.h"
#include "../helpers/camera_suite_led.h"
#include "../helpers/camera_suite_speaker.h"
#include "../helpers/camera_suite_custom_event.h"
// #include "../helpers/camera_suite_uart.h"

static void camera_suite_view_wifi_camera_draw(Canvas* canvas, void* wifi_model) {
    furi_assert(canvas);
    furi_assert(wifi_model);

    UartDumpModel* model = wifi_model;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_frame(canvas, 0, 0, FLIPPER_SCREEN_HEIGHT, FLIPPER_SCREEN_HEIGHT);

    canvas_draw_str_aligned(canvas, 3, 3, AlignLeft, AlignTop, "Starting WiFi Stream at:");

    // Draw log from camera.
    canvas_draw_str_aligned(canvas, 3, 13, AlignLeft, AlignTop, furi_string_get_cstr(model->log));
}

static int32_t camera_suite_wifi_camera_worker(void* wifi_view_instance) {
    furi_assert(wifi_view_instance);

    CameraSuiteViewWiFiCamera* instance = wifi_view_instance;
    CameraSuite* app_instance = instance->context;

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
                length = furi_stream_buffer_receive(app_instance->rx_stream, data, buffer_size, 0);
                if(length > 0) {
                    data[length] = '\0';

                    with_view_model(
                        instance->view,
                        UartDumpModel * model,
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

            with_view_model(
                instance->view, UartDumpModel * model, { UNUSED(model); }, true);
        }
    }

    return 0;
}

static bool
    camera_suite_view_wifi_camera_input(InputEvent* input_event, void* wifi_view_instance) {
    furi_assert(wifi_view_instance);
    furi_assert(input_event);

    CameraSuiteViewWiFiCamera* instance = wifi_view_instance;
    CameraSuite* app_instance = instance->context;
    uint8_t data[1];

    if(input_event->type == InputTypeRelease) {
        switch(input_event->key) {
        default:
            with_view_model(
                instance->view,
                UartDumpModel * model,
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
            with_view_model(
                instance->view,
                UartDumpModel * model,
                {
                    UNUSED(model);

                    // Stop camera WiFi stream.
                    data[0] = 'w';
                    furi_hal_serial_tx(app_instance->serial_handle, data, 1);
                    furi_delay_ms(50);

                    // Go back to the main menu.
                    instance->callback(
                        CameraSuiteCustomEventSceneWiFiCameraBack, instance->context);
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

static void camera_suite_view_wifi_camera_model_init(UartDumpModel* const model) {
    model->log = furi_string_alloc();
    furi_string_reserve(model->log, 4096);
}

static void camera_suite_view_wifi_camera_enter(void* wifi_view_instance) {
    furi_assert(wifi_view_instance);

    CameraSuiteViewWiFiCamera* instance = wifi_view_instance;
    CameraSuite* app_instance = instance->context;

    // Start wifi camera stream.
    uint8_t data[1] = {'W'};
    furi_hal_serial_tx(app_instance->serial_handle, data, 1);
    furi_delay_ms(50);

    with_view_model(
        instance->view,
        UartDumpModel * model,
        { camera_suite_view_wifi_camera_model_init(model); },
        true);
}

static void wifi_camera_callback(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxEvent event,
    void* wifi_view_instance) {
    furi_assert(handle);
    furi_assert(wifi_view_instance);

    CameraSuiteViewWiFiCamera* instance = wifi_view_instance;
    CameraSuite* app_instance = instance->context;

    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
        furi_stream_buffer_send(app_instance->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(app_instance->worker_thread), WorkerEventRx);
    }
}

CameraSuiteViewWiFiCamera* camera_suite_view_wifi_camera_alloc() {
    CameraSuiteViewWiFiCamera* instance = malloc(sizeof(CameraSuiteViewWiFiCamera));

    // Allocate the view object
    instance->view = view_alloc();

    // Allocate model
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(UartDumpModel));

    // Set context for the view
    view_set_context(instance->view, instance);

    // Set draw callback
    view_set_draw_callback(instance->view, (ViewDrawCallback)camera_suite_view_wifi_camera_draw);

    // Set input callback
    view_set_input_callback(instance->view, camera_suite_view_wifi_camera_input);

    // Set enter callback
    view_set_enter_callback(instance->view, camera_suite_view_wifi_camera_enter);

    // Set exit callback
    view_set_exit_callback(instance->view, camera_suite_view_wifi_camera_exit);

    // Allocate the UART worker thread for the camera.
    // CameraSuite* app_instance = instance->context;
    // camera_suite_uart_alloc(app_instance, wifi_camera_callback);

    with_view_model(
        instance->view,
        UartDumpModel * model,
        { camera_suite_view_wifi_camera_model_init(model); },
        true);

    return instance;
}

void camera_suite_view_wifi_camera_free(CameraSuiteViewWiFiCamera* instance) {
    furi_assert(instance);

    CameraSuite* app_instance = instance->context;

    // Free the worker thread.
    furi_thread_flags_set(furi_thread_get_id(app_instance->worker_thread), WorkerEventStop);
    furi_thread_join(app_instance->worker_thread);
    furi_thread_free(app_instance->worker_thread);

    // Free the allocated stream buffer.
    furi_stream_buffer_free(app_instance->rx_stream);

    // camera_suite_uart_free(app_instance);

    with_view_model(
        instance->view, UartDumpModel * model, { furi_string_free(model->log); }, true);
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
