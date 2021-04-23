#include <api-hal-gpio.h>
#include <api-hal-version.h>

#define GET_SYSCFG_EXTI_PORT(gpio)  (((gpio) == (GPIOA)) ? LL_SYSCFG_EXTI_PORTA :\
                                     ((gpio) == (GPIOB)) ? LL_SYSCFG_EXTI_PORTB :\
                                     ((gpio) == (GPIOC)) ? LL_SYSCFG_EXTI_PORTC :\
                                     ((gpio) == (GPIOD)) ? LL_SYSCFG_EXTI_PORTD :\
                                     ((gpio) == (GPIOE)) ? LL_SYSCFG_EXTI_PORTE : LL_SYSCFG_EXTI_PORTH)

#define GPIO_PIN_MAP(pin, prefix)   (((pin) == (LL_GPIO_PIN_0)) ? prefix##0 :\
                                     ((pin) == (LL_GPIO_PIN_1)) ? prefix##1 :\
                                     ((pin) == (LL_GPIO_PIN_2)) ? prefix##2 :\
                                     ((pin) == (LL_GPIO_PIN_3)) ? prefix##3 :\
                                     ((pin) == (LL_GPIO_PIN_4)) ? prefix##4 :\
                                     ((pin) == (LL_GPIO_PIN_5)) ? prefix##5 :\
                                     ((pin) == (LL_GPIO_PIN_6)) ? prefix##6 :\
                                     ((pin) == (LL_GPIO_PIN_7)) ? prefix##7 :\
                                     ((pin) == (LL_GPIO_PIN_8)) ? prefix##8 :\
                                     ((pin) == (LL_GPIO_PIN_9)) ? prefix##9 :\
                                     ((pin) == (LL_GPIO_PIN_10)) ? prefix##10 :\
                                     ((pin) == (LL_GPIO_PIN_11)) ? prefix##11 :\
                                     ((pin) == (LL_GPIO_PIN_12)) ? prefix##12 :\
                                     ((pin) == (LL_GPIO_PIN_13)) ? prefix##13 :\
                                     ((pin) == (LL_GPIO_PIN_14)) ? prefix##14 : prefix##15)

#define GET_SYSCFG_EXTI_LINE(pin)   GPIO_PIN_MAP(pin, LL_SYSCFG_EXTI_LINE)
#define GET_EXTI_LINE(pin)          GPIO_PIN_MAP(pin, LL_EXTI_LINE_)

void hal_gpio_init(
    const GpioPin* gpio,
    const GpioMode mode,
    const GpioPull pull,
    const GpioSpeed speed) {

    uint32_t sys_exti_port = GET_SYSCFG_EXTI_PORT(gpio->port);
    uint32_t sys_exti_line = GET_SYSCFG_EXTI_LINE(gpio->pin);
    uint32_t exti_line = GET_EXTI_LINE(gpio->pin);

    // Configure gpio with interrupts disabled
    __disable_irq();
    LL_GPIO_SetPinSpeed(gpio->port, gpio->pin, speed);
    LL_GPIO_SetPinPull(gpio->port, gpio->pin, pull);
    if(mode >= GpioModeInterruptRise) {
        // Set pin in interrupt mode
        LL_GPIO_SetPinMode(gpio->port, gpio->pin, LL_GPIO_MODE_INPUT);
        LL_SYSCFG_SetEXTISource(sys_exti_port, exti_line);
        if(mode == GpioModeInterruptRise || mode == GpioModeInterruptRiseFall) {
            LL_EXTI_EnableIT_0_31(sys_exti_line);
            LL_EXTI_EnableRisingTrig_0_31(sys_exti_line);
        }
        if(mode == GpioModeInterruptFall || mode == GpioModeInterruptRiseFall) {
            LL_EXTI_EnableIT_0_31(exti_line);
            LL_EXTI_EnableFallingTrig_0_31(exti_line);
        }
        if(mode == GpioModeEventRise || mode == GpioModeInterruptRiseFall) {
            LL_EXTI_EnableEvent_0_31(exti_line);
            LL_EXTI_EnableRisingTrig_0_31(exti_line);
        }
        if(mode == GpioModeEventFall || mode == GpioModeInterruptRiseFall) {
            LL_EXTI_EnableEvent_0_31(exti_line);
            LL_EXTI_EnableFallingTrig_0_31(exti_line);
        }
    } else {
        // Disable interrupt if it was set
        if(LL_SYSCFG_GetEXTISource(sys_exti_line) == sys_exti_port &&
           LL_EXTI_IsEnabledIT_0_31(exti_line)) {
            LL_EXTI_DisableIT_0_31(exti_line);
            LL_EXTI_DisableRisingTrig_0_31(exti_line);
            LL_EXTI_DisableFallingTrig_0_31(exti_line);
           }
        // Set not interrupt pin modes
        if(mode == GpioModeInput) {
            LL_GPIO_SetPinMode(gpio->port, gpio->pin, LL_GPIO_MODE_INPUT);
        } else if(mode == GpioModeOutputPushPull) {
            LL_GPIO_SetPinMode(gpio->port, gpio->pin, LL_GPIO_MODE_OUTPUT);
            LL_GPIO_SetPinOutputType(gpio->port, gpio->pin, LL_GPIO_OUTPUT_PUSHPULL);
        } else if(mode == GpioModeOutputOpenDrain) {
            LL_GPIO_SetPinMode(gpio->port, gpio->pin, LL_GPIO_MODE_OUTPUT);
            LL_GPIO_SetPinOutputType(gpio->port, gpio->pin, LL_GPIO_OUTPUT_OPENDRAIN);
        } else if(mode == GpioModeAnalog) {
            LL_GPIO_SetPinMode(gpio->port, gpio->pin, LL_GPIO_MODE_ANALOG);
        }
    }
    __enable_irq();
}

extern COMP_HandleTypeDef hcomp1;

bool get_rfid_in_level() {
    bool value = false;
    if (api_hal_version_get_hw_version() > 7) {
        value = (HAL_COMP_GetOutputLevel(&hcomp1) == COMP_OUTPUT_LEVEL_LOW);
    } else {
        value = (HAL_COMP_GetOutputLevel(&hcomp1) == COMP_OUTPUT_LEVEL_HIGH);
    }

#ifdef INVERT_RFID_IN
    return !value;
#else
    return value;
#endif
}
