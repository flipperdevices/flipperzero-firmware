#include "../camera_suite.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>
#include "../helpers/camera_suite_haptic.h"
#include "../helpers/camera_suite_speaker.h"
#include "../helpers/camera_suite_led.h"

static void camera_suite_view_wifi_camera_draw(Canvas* canvas, void* model) {
    furi_assert(canvas);
    furi_assert(model);

    CameraSuiteViewWiFiCameraModel* instance = model;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_frame(canvas, 0, 0, FRAME_WIDTH, FRAME_HEIGHT);

    canvas_draw_str_aligned(canvas, 3, 3, AlignLeft, AlignTop, "Starting WiFi Stream at:");
    canvas_draw_str_aligned(
        canvas, 3, 13, AlignLeft, AlignTop, furi_string_get_cstr(instance->log));
}

static int32_t camera_suite_wifi_camera_worker(void* context) {
    furi_assert(context);

    CameraSuiteViewWiFiCamera* instance = context;

    while(1) {
        uint32_t events =
            furi_thread_flags_wait(WIFI_WORKER_EVENTS_MASK, FuriFlagWaitAny, FuriWaitForever);

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

                    with_view_model(
                        instance->view,
                        CameraSuiteViewWiFiCameraModel * model,
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
                instance->view, CameraSuiteViewWiFiCameraModel * model, { UNUSED(model); }, true);
        }
    }

    return 0;
}

static bool camera_suite_view_wifi_camera_input(InputEvent* event, void* context) {
    furi_assert(context);
    furi_assert(event);

    CameraSuiteViewWiFiCamera* instance = context;

    if(event->type == InputTypeRelease) {
        switch(event->key) {
        default: // Stop all sounds, reset the LED.
            with_view_model(
                instance->view,
                CameraSuiteViewWiFiCameraModel * model,
                {
                    UNUSED(model);
                    camera_suite_play_bad_bump(instance->context);
                    camera_suite_stop_all_sound(instance->context);
                    camera_suite_led_set_rgb(instance->context, 0, 0, 0);
                },
                true);
            break;
        }
    } else if(event->type == InputTypePress) {
        switch(event->key) {
        case InputKeyBack: {
            with_view_model(
                instance->view,
                CameraSuiteViewWiFiCameraModel * model,
                {
                    UNUSED(model);

                    // Stop camera WiFi stream.
                    furi_hal_uart_tx(FuriHalUartIdUSART1, (uint8_t[]){'w'}, 1);
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

static void camera_suite_view_wifi_camera_exit(void* context) {
    furi_assert(context);
}

static void camera_suite_view_wifi_camera_model_init(CameraSuiteViewWiFiCameraModel* const model) {
    model->log = furi_string_alloc();
    furi_string_reserve(model->log, 4096);

    // Set the initial log message.
    // furi_string_printf(model->log, "Starting WiFi Camera Stream...:\n");
}

static void camera_suite_view_wifi_camera_enter(void* context) {
    furi_assert(context);

    // Get the camera suite instance context.
    CameraSuiteViewWiFiCamera* instance = (CameraSuiteViewWiFiCamera*)context;

    // Start wifi camera stream.
    furi_hal_uart_tx(FuriHalUartIdUSART1, (uint8_t[]){'W'}, 1);

    with_view_model(
        instance->view,
        CameraSuiteViewWiFiCameraModel * model,
        { camera_suite_view_wifi_camera_model_init(model); },
        true);
}

static void wifi_camera_on_irq_cb(UartIrqEvent uartIrqEvent, uint8_t data, void* context) {
    furi_assert(uartIrqEvent);
    furi_assert(data);
    furi_assert(context);

    // Cast `context` to `CameraSuiteViewWiFiCamera*` and store it in `instance`.
    CameraSuiteViewWiFiCamera* instance = context;

    // If `uartIrqEvent` is `UartIrqEventRXNE`, send the data to the
    // `wifi_rx_stream` and set the `WorkerEventRx` flag.
    if(uartIrqEvent == UartIrqEventRXNE) {
        furi_stream_buffer_send(instance->wifi_rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(instance->wifi_worker_thread), WorkerEventRx);
    }
}

CameraSuiteViewWiFiCamera* camera_suite_view_wifi_camera_alloc() {
    // Allocate memory for the instance
    CameraSuiteViewWiFiCamera* instance = malloc(sizeof(CameraSuiteViewWiFiCamera));

    // Allocate the view object
    instance->view = view_alloc();

    // Allocate model
    view_allocate_model(
        instance->view, ViewModelTypeLocking, sizeof(CameraSuiteViewWiFiCameraModel));

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

    // Allocate a stream buffer
    instance->wifi_rx_stream = furi_stream_buffer_alloc(1024, 1);

    // Allocate a thread for this camera to run on.
    FuriThread* thread = furi_thread_alloc_ex(
        "Camera_Suite_WiFi_Rx_Thread", 1024, camera_suite_wifi_camera_worker, instance);
    instance->wifi_worker_thread = thread;
    furi_thread_start(instance->wifi_worker_thread);

    // Disable console.
    furi_hal_console_disable();

    // 115200 is the default baud rate for the ESP32-CAM.
    furi_hal_uart_set_br(FuriHalUartIdUSART1, 230400);

    // Enable UART1 and set the IRQ callback.
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, wifi_camera_on_irq_cb, instance);

    with_view_model(
        instance->view,
        CameraSuiteViewWiFiCameraModel * model,
        { camera_suite_view_wifi_camera_model_init(model); },
        true);

    return instance;
}

void camera_suite_view_wifi_camera_free(CameraSuiteViewWiFiCamera* instance) {
    furi_assert(instance);

    // Remove the IRQ callback.
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, NULL, NULL);

    // Free the worker thread.
    furi_thread_free(instance->wifi_worker_thread);

    // Free the allocated stream buffer.
    furi_stream_buffer_free(instance->wifi_rx_stream);

    // Re-enable the console.
    furi_hal_console_enable();

    with_view_model(
        instance->view,
        CameraSuiteViewWiFiCameraModel * model,
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
