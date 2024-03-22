#pragma once

#include <furi.h>

typedef struct ImuController ImuController;

/**
 * @brief      The configuration for pitch and roll for the IMU controller
*/
typedef struct ImuControllerConfig {
    /// @brief The degrees of roll for a InputKeyUp event
    float roll_up;

    /// @brief The degrees of roll for a InputKeyDown event
    float roll_down;

    /// @brief The hysteresis for the roll before releasing the InputKeyUp and InputKeyDown events
    float roll_hysteresis;

    /// @brief The degrees of pitch for a InputKeyLeft event
    float pitch_left;

    /// @brief The degrees of pitch for a InputKeyRight event
    float pitch_right;

    /// @brief The hysteresis for the pitch before releasing the InputKeyLeft and InputKeyRight events
    float pitch_hysteresis;

    /// @brief Duration in milliseconds for a InputTypeLong event (300 ms recommended)
    uint32_t button_state_long_ms;

    /// @brief Duration in milliseconds for a InputTypeRepeat event (150 ms recommended)
    uint32_t button_state_repeat_ms;

    /// @brief Number of millisecond for vibration on press and release
    uint8_t vibro_duration;
} ImuControllerConfig;

extern const ImuControllerConfig IMU_CONTROLLER_DEFAULT_CONFIG;

/**
 * @brief      A callback to queue input messages from the IMU controller to the event queue
 * @param      event_queue  The event queue
 * @param      type         The input type
 * @param      key          The input key
*/
typedef void (*ImuControllerQueue)(FuriMessageQueue* event_queue, InputType type, InputKey key);

/**
 * @brief      Allocate a new IMU controller
 * @param      event_queue     The event queue
 * @param      config          The configuration for pitch and roll
 * @param      queue_message   A callback to queue input messages
 * @return     The new IMU controller
 */
ImuController* imu_controller_alloc(
    FuriMessageQueue* event_queue,
    const ImuControllerConfig* config,
    ImuControllerQueue queue_message);

/**
 * @brief      Free the IMU controller
 * @param      imu_controller  The IMU controller
 */
void imu_controller_free(ImuController* imu_controller);

/**
 * @brief      Start the IMU controller
 * @details    This function starts the IMU controller, which will periodically check the IMU orientation and send virtual button events.
 * @param      imu_controller  The IMU controller
 */
void imu_controller_start(ImuController* imu_controller);

/**
 * @brief      Stop the IMU controller
 * @details    This function stops the IMU controller, which will stop checking the IMU orientation and sending virtual button events.
 * @param      imu_controller  The IMU controller
 */
void imu_controller_stop(ImuController* imu_controller);

/**
 * @brief      Recalibrate the IMU controller
 * @details    This function stops the IMU controller, recalibrates the IMU and starts the IMU controller again.
 * @param      imu_controller  The IMU controller
 */
void imu_controller_recalibrate(ImuController* imu_controller);