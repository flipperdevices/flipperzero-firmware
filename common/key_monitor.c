#include "key_monitor_i.h"

const GpioPin* const pin_sw1 = &gpio_ext_pb2;
const GpioPin* const pin_sw2 = &gpio_ext_pb3;
const GpioPin* const pin_sw3 = &gpio_ext_pa4;
const GpioPin* const pin_sw4 = &gpio_ext_pa6;

typedef enum {
    KeyMonitorEventDataWaiting = 1 << 0, // bit flag 0 - data is waiting to be processed
    KeyMonitorEventExiting = 1 << 1, // bit flag 1 - thread is exiting
} KeyMonitorEventFlags;

int32_t key_monitor_worker(void* context);
void key_monitor_switch_callback(void* context);

KeyMonitor* key_monitor_alloc(void) {
    KeyMonitor* key_monitor = malloc(sizeof(KeyMonitor));
    key_monitor->last_pins = 0;
    key_monitor->running = true;
    key_monitor->callback = NULL;
    key_monitor->thread =
        furi_thread_alloc_ex("KeyMonitorWorker", 1024, key_monitor_worker, key_monitor);
    furi_hal_gpio_init(pin_sw1, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_gpio_init(pin_sw2, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_gpio_init(pin_sw3, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);
    furi_hal_gpio_init(pin_sw4, GpioModeInput, GpioPullUp, GpioSpeedVeryHigh);

    furi_thread_start(key_monitor->thread);

    return key_monitor;
}

uint8_t key_monitor_get_last_status(KeyMonitor* km) {
    return km->last_pins;
}

uint8_t key_monitor_get_pin_status() {
    return (furi_hal_gpio_read(pin_sw1) ? 0 : SwitchId1) |
           (furi_hal_gpio_read(pin_sw2) ? 0 : SwitchId2) |
           (furi_hal_gpio_read(pin_sw3) ? 0 : SwitchId3) |
           (furi_hal_gpio_read(pin_sw4) ? 0 : SwitchId4);
}

uint8_t key_monitor_get_debounced_pin_status() {
    uint8_t pin_status = 0;
    uint32_t counter = 0;
    furi_delay_ms(50);
    while(counter < 100) {
        uint8_t new_pin_status = key_monitor_get_pin_status();
        if(pin_status != new_pin_status) {
            counter = 0;
            pin_status = new_pin_status;
        }
        counter++;
    }
    return pin_status;
}

void key_monitor_set_callback(KeyMonitor* km, KeyMonitorEventCallback callback, void* context) {
    km->callback = callback;
    km->context = context;
}

void key_monitor_switch_callback(void* context) {
    KeyMonitor* key_monitor = context;
    furi_thread_flags_set(furi_thread_get_id(key_monitor->thread), KeyMonitorEventDataWaiting);
}

int32_t key_monitor_worker(void* context) {
    KeyMonitor* key_monitor = context;
    while(key_monitor->running) {
        uint8_t pins = key_monitor_get_pin_status();
        if(pins != key_monitor->last_pins) {
            pins = key_monitor_get_debounced_pin_status();
            FURI_LOG_D("TAG", "pins: %d", pins);
            if(pins != key_monitor->last_pins) {
                if(key_monitor->callback) {
                    key_monitor->callback(key_monitor->context, key_monitor->last_pins, pins);
                }
                key_monitor->last_pins = pins;
            }
        }
        furi_delay_ms(1);
    }

    return 0;
}

void key_monitor_free(KeyMonitor* km) {
    furi_hal_gpio_init_simple(pin_sw1, GpioModeAnalog);
    furi_hal_gpio_init_simple(pin_sw2, GpioModeAnalog);
    furi_hal_gpio_init_simple(pin_sw3, GpioModeAnalog);
    furi_hal_gpio_init_simple(pin_sw4, GpioModeAnalog);
    km->running = false;
    furi_thread_join(km->thread);
    furi_thread_free(km->thread);
    free(km);
}