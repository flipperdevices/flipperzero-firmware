#if defined STM32L0
    #include <stm32l0xx_hal.h>

    // STM32L0538-Discovery green led - PB4
    #define LED_PORT                GPIOB
    #define LED_PIN                 GPIO_PIN_4
    #define LED_PORT_CLK_ENABLE     __HAL_RCC_GPIOB_CLK_ENABLE
#elif defined STM32F1
    #include <stm32f1xx_hal.h>

    // STM32VL-Discovery green led - PC9
    #define LED_PORT                GPIOC
    #define LED_PIN                 GPIO_PIN_9
    #define LED_PORT_CLK_ENABLE     __HAL_RCC_GPIOC_CLK_ENABLE
#elif defined STM32F4
    #include <stm32f4xx_hal.h>

    // STM32F4-Discovery green led - PD12
    #define LED_PORT                GPIOD
    #define LED_PIN                 GPIO_PIN_12
    #define LED_PORT_CLK_ENABLE     __HAL_RCC_GPIOD_CLK_ENABLE
#endif

//This prevent name mangling for functions used in C/assembly files.
extern "C"
{
    void SysTick_Handler(void)
    {
        HAL_IncTick();

        // 1 Hz blinking
        if ((HAL_GetTick() % 500) == 0)
        {
            HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
        }
    }
}

void initGPIO()
{
    GPIO_InitTypeDef GPIO_Config;

    GPIO_Config.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Config.Pull = GPIO_NOPULL;
    GPIO_Config.Speed = GPIO_SPEED_FREQ_HIGH;

    GPIO_Config.Pin = LED_PIN;

    LED_PORT_CLK_ENABLE();
    HAL_GPIO_Init(LED_PORT, &GPIO_Config);
}

int main(void)
{
    HAL_Init();
    initGPIO();
    // 1kHz ticks
    HAL_SYSTICK_Config(SystemCoreClock / 1000);

    while(1);
    return 0;
}
