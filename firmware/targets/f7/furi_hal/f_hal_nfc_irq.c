#include "f_hal_nfc_i.h"

#include <lib/drivers/st25r3916.h>
#include <furi_hal_resources.h>

static void f_hal_nfc_int_callback() {
    f_hal_nfc_set_event(FHalNfcEventInternalTypeIrq);
}

uint32_t f_hal_nfc_get_irq(FuriHalSpiBusHandle* handle) {
    uint32_t irq = 0;
    while(furi_hal_gpio_read_port_pin(gpio_nfc_irq_rfid_pull.port, gpio_nfc_irq_rfid_pull.pin)) {
        irq |= st25r3916_get_irq(handle);
    }
    return irq;
}

void f_hal_nfc_init_gpio_isr() {
    furi_hal_gpio_init(&gpio_nfc_irq_rfid_pull, GpioModeInterruptRise, GpioPullDown, GpioSpeedVeryHigh);
    furi_hal_gpio_add_int_callback(&gpio_nfc_irq_rfid_pull, f_hal_nfc_int_callback, NULL);
    furi_hal_gpio_enable_int_callback(&gpio_nfc_irq_rfid_pull);
}

void f_hal_nfc_deinit_gpio_isr() {
    furi_hal_gpio_init(&gpio_nfc_irq_rfid_pull, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_disable_int_callback(&gpio_nfc_irq_rfid_pull);
}
