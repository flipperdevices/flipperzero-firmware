#if defined STM32L0
    #include <stm32l0xx_ll_gpio.h>
    #include <stm32l0xx_ll_cortex.h>
    #include <stm32l0xx_ll_rcc.h>

    // STM32L0538-Discovery green led - PB4
    #define LED_PORT                GPIOB
    #define LED_PIN                 LL_GPIO_PIN_4
    #define LED_PORT_CLK_ENABLE()   { RCC->IOPENR |= RCC_IOPENR_GPIOBEN; }
#elif defined STM32F1
    #include <stm32f1xx_ll_gpio.h>
    #include <stm32f1xx_ll_cortex.h>
    #include <stm32f1xx_ll_rcc.h>

    // STM32VL-Discovery green led - PC9
    #define LED_PORT                GPIOC
    #define LED_PIN                 LL_GPIO_PIN_9
    #define LED_PORT_CLK_ENABLE()   { RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; }
#elif defined STM32F4
    #include <stm32f4xx_ll_gpio.h>
    #include <stm32f4xx_ll_cortex.h>
    #include <stm32f4xx_ll_rcc.h>

    // STM32F4-Discovery green led - PD12
    #define LED_PORT                GPIOD
    #define LED_PIN                 LL_GPIO_PIN_12
    #define LED_PORT_CLK_ENABLE()   { RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; }
#endif

void SysTick_Handler(void)
{
    static int counter = 0;
    counter++;

    // 1 Hz blinking
    if ((counter % 500) == 0)
        LL_GPIO_TogglePin(LED_PORT, LED_PIN);
}

void initGPIO()
{
    LED_PORT_CLK_ENABLE();

    LL_GPIO_SetPinMode(LED_PORT, LED_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(LED_PORT, LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);
}

int main(void)
{
    initGPIO();

    // 1kHz ticks
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

    while(1);

    return 0;
}
