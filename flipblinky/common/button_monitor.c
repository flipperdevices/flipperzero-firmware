#include "button_monitor_i.h"

// Left to right order of the switches.
const GpioPin* const pin_sw1 = &gpio_ext_pb2;
const GpioPin* const pin_sw2 = &gpio_ext_pb3;
const GpioPin* const pin_sw3 = &gpio_ext_pa4;
const GpioPin* const pin_sw4 = &gpio_ext_pa6;

static int32_t button_monitor_worker(void* context);

/**
 * @brief Allocates a new button monitor.
 * @details Allocates a new button monitor. The button monitor 
 * will start running immediately, but the callback will be NULL.
 * @return The new button monitor.
*/
ButtonMonitor* button_monitor_alloc() {
    ButtonMonitor* button_monitor = malloc(sizeof(ButtonMonitor));
    button_monitor->last_pins = 0;
    button_monitor->running = true;
    button_monitor->callback = NULL;
    button_monitor->thread =
        furi_thread_alloc_ex("ButtonMonitorWorker", 1024, button_monitor_worker, button_monitor);
    furi_hal_gpio_init(pin_sw1, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_gpio_init(pin_sw2, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_gpio_init(pin_sw3, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_gpio_init(pin_sw4, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);

    furi_thread_start(button_monitor->thread);

    return button_monitor;
}

/**
 * @brief Frees a button monitor.
 * @param bm The button monitor to free.
*/
void button_monitor_free(ButtonMonitor* bm) {
    furi_hal_gpio_init_simple(pin_sw1, GpioModeAnalog);
    furi_hal_gpio_init_simple(pin_sw2, GpioModeAnalog);
    furi_hal_gpio_init_simple(pin_sw3, GpioModeAnalog);
    furi_hal_gpio_init_simple(pin_sw4, GpioModeAnalog);
    bm->running = false;
    furi_thread_join(bm->thread);
    furi_thread_free(bm->thread);
    free(bm);
}

/**
 * @brief Sets the callback for the button monitor.
 * @param km The button monitor.
 * @param callback The callback to call when a button event occurs.
 * @param context The context for the callback.
*/
void button_monitor_set_callback(
    ButtonMonitor* km,
    ButtonMonitorEventCallback callback,
    void* context) {
    km->callback = callback;
    km->context = context;
}

/**
 * @brief Gets the last button debounced status.
 * @param km The button monitor.
 * @return The last button status.
*/
SwitchIds button_monitor_get_last_status(ButtonMonitor* km) {
    return km->last_pins;
}

/**
 * @brief Gets the current button instant status.
 * @details Gets the current button instant status. 
 * This function does not debounce the button status.
 * @return The current button status.
*/
static SwitchIds button_monitor_get_pin_status() {
    return (furi_hal_gpio_read(pin_sw1) ? 0 : SwitchId1) |
           (furi_hal_gpio_read(pin_sw2) ? 0 : SwitchId2) |
           (furi_hal_gpio_read(pin_sw3) ? 0 : SwitchId3) |
           (furi_hal_gpio_read(pin_sw4) ? 0 : SwitchId4);
}

/**
 * @brief Gets the current button debounced status.
 * @details This function debounces the current button status.
 * @return The current button status.
*/
static SwitchIds button_monitor_get_debounced_pin_status() {
    SwitchIds pin_status = 0;
    uint32_t counter = 0;
    furi_delay_ms(50);
    while(counter < 100) {
        SwitchIds new_pin_status = button_monitor_get_pin_status();
        if(pin_status != new_pin_status) {
            counter = 0;
            pin_status = new_pin_status;
        }
        counter++;
    }
    return pin_status;
}

/**
 * @brief The worker thread for the button monitor.
 * @param context The button monitor.
 * @return 0.
*/
static int32_t button_monitor_worker(void* context) {
    ButtonMonitor* button_monitor = context;
    while(button_monitor->running) {
        SwitchIds pins = button_monitor_get_pin_status();
        if(pins != button_monitor->last_pins) {
            pins = button_monitor_get_debounced_pin_status();
            FURI_LOG_D("TAG", "pins: %d", pins);
            if(pins != button_monitor->last_pins) {
                if(button_monitor->callback) {
                    button_monitor->callback(
                        button_monitor->context, button_monitor->last_pins, pins);
                }
                button_monitor->last_pins = pins;
            }
        }
        furi_delay_ms(1);
    }

    return 0;
}
