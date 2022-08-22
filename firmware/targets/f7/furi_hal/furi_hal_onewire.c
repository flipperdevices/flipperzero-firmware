#include <furi_hal_onewire.h>
#include <furi_hal_interrupt.h>
#include <furi_hal_resources.h>

#include <stm32wbxx_ll_tim.h>
#include <stm32wbxx_ll_exti.h>

#include <furi.h>

#define FURI_HAL_IBUTTON_TIMER TIM1
#define FURI_HAL_IBUTTON_TIMER_IRQ FuriHalInterruptIdTim1UpTim16

void furi_hal_onewire_start_drive(const GpioPin* gpio) {
    furi_hal_onewire_pin_high(gpio);
    furi_hal_gpio_init(gpio, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);
}

void furi_hal_onewire_start_drive_in_isr(const GpioPin* gpio) {
    furi_hal_gpio_init(gpio, GpioModeOutputOpenDrain, GpioPullNo, GpioSpeedLow);
    LL_EXTI_ClearFlag_0_31(gpio->pin);
}

void furi_hal_onewire_start_interrupt(const GpioPin* gpio) {
    furi_hal_onewire_pin_high(gpio);
    furi_hal_gpio_init(gpio, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedLow);
}

void furi_hal_onewire_start_interrupt_in_isr(const GpioPin* gpio) {
    furi_hal_gpio_init(gpio, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedLow);
    LL_EXTI_ClearFlag_0_31(gpio->pin);
}

void furi_hal_onewire_stop(const GpioPin* gpio) {
    furi_hal_onewire_pin_high(gpio);
    furi_hal_gpio_init(gpio, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

void furi_hal_onewire_add_interrupt(const GpioPin* gpio, GpioExtiCallback cb, void* context) {
    furi_hal_gpio_add_int_callback(gpio, cb, context);
}

void furi_hal_onewire_remove_interrupt(const GpioPin* gpio) {
    furi_hal_gpio_remove_int_callback(gpio);
}

void furi_hal_onewire_pin_low(const GpioPin* gpio) {
    furi_hal_gpio_write(gpio, false);
}

void furi_hal_onewire_pin_high(const GpioPin* gpio) {
    furi_hal_gpio_write(gpio, true);
}

bool furi_hal_onewire_pin_get_level(const GpioPin* gpio) {
    return furi_hal_gpio_read(gpio);
}
