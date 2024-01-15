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

    // Clear the screen.
    canvas_set_color(canvas, ColorBlack);

    // Draw a frame.
    canvas_draw_frame(canvas, 0, 0, FRAME_WIDTH, FRAME_HEIGHT);
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

static void camera_suite_view_wifi_camera_enter(void* context) {
    UNUSED(context);

    // Start wifi camera stream.
    furi_hal_uart_tx(FuriHalUartIdUSART1, (uint8_t[]){'W'}, 1);
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

    // Disable console.
    furi_hal_console_disable();

    // 115200 is the default baud rate for the ESP32-CAM.
    furi_hal_uart_set_br(FuriHalUartIdUSART1, 230400);

    return instance;
}

void camera_suite_view_wifi_camera_free(CameraSuiteViewWiFiCamera* instance) {
    furi_assert(instance);

    // Remove the IRQ callback.
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, NULL, NULL);

    // Re-enable the console.
    furi_hal_console_enable();

    with_view_model(
        instance->view, CameraSuiteViewWiFiCameraModel * model, { UNUSED(model); }, true);

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
