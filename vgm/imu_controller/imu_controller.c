#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <sys/time.h>
#include "engine/sensors/imu.h"
#include "imu_controller.h"

// Number of readings per second for the IMU
#define BUTTON_READINGS_PER_SECOND 20

// Default configuration for the IMU controller
const ImuControllerConfig IMU_CONTROLLER_DEFAULT_CONFIG = {
    .roll_up = 8.0f, // 8 degrees triggers an Up button press
    .roll_down = -8.0f, // -8 degrees triggers a Down button press
    .roll_hysteresis = 3.0f, // 3 degrees of movement is required to release the Up or Down button press
    .pitch_left = 20.0f, // 20 degrees triggers a Left button press
    .pitch_right = -20.0f, // -20 degrees triggers a Right button press
    .pitch_hysteresis = 5.0f, // 5 degrees of movement is required to release the Left or Right button press
    .button_state_long_ms = 300, // 300ms is the time required to trigger a long press event
    .button_state_repeat_ms = 150, // 150ms is the time required to trigger a repeat press event
    .vibro_duration = 25, // 25ms is the duration of the vibration when a button is pressed or released
};

// Current virtual button state
typedef enum {
    ButtonStateReleased = 0,
    ButtonStatePressed,
    ButtonStateLongPressed,
    ButtonStateRepeatPressed,
} ButtonState;

// IMU controller structure
struct ImuController {
    const ImuControllerConfig* config;
    ImuControllerQueue queue_message;
    FuriMutex* mutex;
    Imu* imu;
    void* context;
    FuriTimer* timer;
    uint32_t button_press_tick[InputKeyMAX];
    ButtonState button_state[InputKeyMAX];
};

/**
 * @brief      Release a virtual button
 * @param      controller  The controller
 * @param      key         The key
 */
static void imu_controller_release(ImuController* controller, InputKey key) {
    // If the button was already released, ignore the request.
    if(controller->button_state[key] == ButtonStateReleased) {
        return;
    }

    if(controller->config->vibro_duration) {
        furi_hal_vibro_on(true);
    }

    // If the button was pressed, send a InputTypeShort event
    // (otherwise we already sent a InputTypeLong event).
    if(controller->button_state[key] == ButtonStatePressed) {
        controller->queue_message(controller->context, InputTypeShort, key);
    }

    // Send a InputTypeRelease event
    controller->queue_message(controller->context, InputTypeRelease, key);
    // Reset the button state
    controller->button_press_tick[key] = 0;
    controller->button_state[key] = ButtonStateReleased;

    if(controller->config->vibro_duration) {
        furi_delay_ms(controller->config->vibro_duration);
        furi_hal_vibro_on(false);
    }
}

/**
 * @brief      Press a virtual button
 * @param      controller  The controller
 * @param      key         The key
*/
static void imu_controller_press(ImuController* controller, InputKey key) {
    if(controller->button_press_tick[key]) {
        // If the button was already pressed, check if we need to send a InputTypeLong or InputTypeRepeat event.
        uint32_t duration = furi_get_tick() - controller->button_press_tick[key];

        // If the button was pressed for a long time, send a InputTypeLong event.
        if(controller->config->button_state_long_ms &&
           (controller->button_state[key] == ButtonStatePressed &&
            duration >= controller->config->button_state_long_ms)) {
            // Send a InputTypeLong event
            controller->queue_message(controller->context, InputTypeLong, key);
            // Update the button state
            controller->button_press_tick[key] = furi_get_tick();
            controller->button_state[key] = ButtonStateLongPressed;
        }
        // If we already sent a InputTypeLong, check if we need to send a InputTypeRepeat event.
        else if(
            controller->config->button_state_repeat_ms &&
            (controller->button_state[key] >= ButtonStateLongPressed &&
             duration > controller->config->button_state_repeat_ms)) {
            // Send a InputTypeRepeat event
            controller->queue_message(controller->context, InputTypeRepeat, key);
            // Update the button state
            controller->button_press_tick[key] = furi_get_tick();
            controller->button_state[key] = ButtonStateRepeatPressed;
        }

        return;
    }

    if(controller->config->vibro_duration) {
        furi_hal_vibro_on(true);
    }

    // Release the button in the opposite direction
    switch(key) {
    case InputKeyUp:
        imu_controller_release(controller, InputKeyDown);
        break;
    case InputKeyDown:
        imu_controller_release(controller, InputKeyUp);
        break;
    case InputKeyLeft:
        imu_controller_release(controller, InputKeyRight);
        break;
    case InputKeyRight:
        imu_controller_release(controller, InputKeyLeft);
        break;
    default:
        break;
    }

    // Send a InputTypePress event
    controller->queue_message(controller->context, InputTypePress, key);

    // Update the button state
    controller->button_press_tick[key] = furi_get_tick();
    controller->button_state[key] = ButtonStatePressed;

    if(controller->config->vibro_duration) {
        furi_delay_ms(controller->config->vibro_duration);
        furi_hal_vibro_on(false);
    }
}

/**
 * @brief      IMU controller callback
 * @details    This function is called periodically to check the IMU orientation and send virtual button events.
 * @param      context  The context
 */
static void imu_controller_callback(void* context) {
    ImuController* imu_controller = (ImuController*)context;

    // Acquire the mutex to access the IMU
    furi_mutex_acquire(imu_controller->mutex, FuriWaitForever);

    if(imu_present(imu_controller->imu)) {
        // Get the roll (up/down)
        float roll = imu_roll_get(imu_controller->imu);
        if(roll > imu_controller->config->roll_up) {
            imu_controller_press(imu_controller, InputKeyUp);
        } else if(roll < imu_controller->config->roll_down) {
            imu_controller_press(imu_controller, InputKeyDown);
        }
        // If the roll is outside the hysteresis range, release the up and down buttons
        else if(
            roll < imu_controller->config->roll_up - imu_controller->config->roll_hysteresis &&
            roll > imu_controller->config->roll_down + imu_controller->config->roll_hysteresis) {
            imu_controller_release(imu_controller, InputKeyUp);
            imu_controller_release(imu_controller, InputKeyDown);
        }

        // Get the pitch (left/right)
        float pitch = imu_pitch_get(imu_controller->imu);
        if(pitch > imu_controller->config->pitch_left) {
            imu_controller_press(imu_controller, InputKeyLeft);
        } else if(pitch < imu_controller->config->pitch_right) {
            imu_controller_press(imu_controller, InputKeyRight);
        }
        // If the pitch is outside the hysteresis range, release the left and right buttons
        else if(
            pitch < imu_controller->config->pitch_left - imu_controller->config->pitch_hysteresis &&
            pitch > imu_controller->config->pitch_right + imu_controller->config->pitch_hysteresis) {
            imu_controller_release(imu_controller, InputKeyLeft);
            imu_controller_release(imu_controller, InputKeyRight);
        }
    }

    // Release the mutex, so other threads can access the IMU
    furi_mutex_release(imu_controller->mutex);
}

/**
 * @brief      Release all virtual buttons
 * @param      context  The context
 */
static void imu_controller_release_buttons(void* context) {
    ImuController* imu_controller = (ImuController*)context;
    imu_controller_release(imu_controller, InputKeyUp);
    imu_controller_release(imu_controller, InputKeyDown);
    imu_controller_release(imu_controller, InputKeyLeft);
    imu_controller_release(imu_controller, InputKeyRight);
}

/**
 * @brief      Allocate a new IMU controller
 * @param      context     The context for callbacks
 * @param      config          The configuration for pitch and roll
 * @param      queue_message   A callback to queue input messages
 * @return     The new IMU controller
 */
ImuController* imu_controller_alloc(
    void* context,
    const ImuControllerConfig* config,
    ImuControllerQueue queue_message) {
    ImuController* imu_controller = malloc(sizeof(ImuController));
    imu_controller->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    imu_controller->queue_message = queue_message;
    imu_controller->imu = imu_alloc();
    imu_controller->context = context;
    imu_controller->config = config;
    imu_controller->timer =
        furi_timer_alloc(imu_controller_callback, FuriTimerTypePeriodic, imu_controller);

    return imu_controller;
}

/**
 * @brief      Free the IMU controller
 * @param      imu_controller  The IMU controller
 */
void imu_controller_free(ImuController* imu_controller) {
    furi_timer_stop(imu_controller->timer);
    furi_timer_free(imu_controller->timer);
    imu_controller_release_buttons(imu_controller);
    imu_free(imu_controller->imu);
    furi_mutex_free(imu_controller->mutex);
    free(imu_controller);
}

/**
 * @brief      Start the IMU controller
 * @details    This function starts the IMU controller, which will periodically check the IMU orientation and send virtual button events.
 * @param      imu_controller  The IMU controller
 */
void imu_controller_start(ImuController* imu_controller) {
    furi_timer_start(
        imu_controller->timer, furi_kernel_get_tick_frequency() / BUTTON_READINGS_PER_SECOND);
}

/**
 * @brief      Stop the IMU controller
 * @details    This function stops the IMU controller, which will stop checking the IMU orientation and sending virtual button events.
 * @param      imu_controller  The IMU controller
 */
void imu_controller_stop(ImuController* imu_controller) {
    furi_timer_stop(imu_controller->timer);
}

/**
 * @brief      Recalibrate the IMU controller
 * @details    This function stops the IMU controller, recalibrates the IMU and starts the IMU controller again.
 * @param      imu_controller  The IMU controller
 */
void imu_controller_recalibrate(ImuController* imu_controller) {
    imu_controller_stop(imu_controller);
    furi_mutex_acquire(imu_controller->mutex, FuriWaitForever);
    imu_free(imu_controller->imu);
    imu_controller->imu = imu_alloc();
    furi_mutex_release(imu_controller->mutex);
    imu_controller_start(imu_controller);
}