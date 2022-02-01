#include <furi_hal_ibutton.h>
#include <furi_hal_resources.h>

void furi_hal_ibutton_start_drive() {
    furi_hal_ibutton_pin_high();
    hal_gpio_init(&ibutton_gpio, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);
}

void furi_hal_ibutton_start_drive_in_isr() {
    hal_gpio_init(&ibutton_gpio, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);
    __HAL_GPIO_EXTI_CLEAR_IT(ibutton_gpio.pin);
}

void furi_hal_ibutton_start_interrupt() {
    furi_hal_ibutton_pin_high();
    hal_gpio_init(&ibutton_gpio, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedLow);
}

void furi_hal_ibutton_start_interrupt_in_isr() {
    hal_gpio_init(&ibutton_gpio, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedLow);
    __HAL_GPIO_EXTI_CLEAR_IT(ibutton_gpio.pin);
}

void furi_hal_ibutton_add_interrupt(GpioExtiCallback cb, void* context) {
    hal_gpio_add_int_callback(&ibutton_gpio, cb, context);
}

void furi_hal_ibutton_remove_interrupt() {
    hal_gpio_remove_int_callback(&ibutton_gpio);
}

void furi_hal_ibutton_stop() {
    furi_hal_ibutton_pin_high();
    hal_gpio_init(&ibutton_gpio, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

void furi_hal_ibutton_pin_low() {
    hal_gpio_write(&ibutton_gpio, false);
}

void furi_hal_ibutton_pin_high() {
    hal_gpio_write(&ibutton_gpio, true);
}

bool furi_hal_ibutton_pin_get_level() {
    return hal_gpio_read(&ibutton_gpio);
}
