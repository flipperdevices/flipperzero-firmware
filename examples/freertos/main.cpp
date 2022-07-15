#include "main.h"
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#if defined STM32F1
    #include <stm32f1xx_hal.h>

    // STM32VL-Discovery green led - PC9
    #define LED_PORT                GPIOC
    #define LED_PIN                 GPIO_PIN_9
    // STM32VL-Discovery blue led - PC8
    //#define LED_PIN                 GPIO_PIN_8
    #define LED_PORT_CLK_ENABLE     __HAL_RCC_GPIOC_CLK_ENABLE
#elif defined STM32H7
    #include <stm32h7xx_hal.h>

    // STM32H743ZI blue LED
    #define LED_PORT                GPIOB
    #define LED_PIN                 GPIO_PIN_7
    #define LED_PORT_CLK_ENABLE     __HAL_RCC_GPIOB_CLK_ENABLE
#elif defined STM32F4
    #include <stm32f4xx_hal.h>

    // STM32F4-Discovery green led - PD12
    #define LED_PORT                GPIOD
    #define LED_PIN                 GPIO_PIN_12
    #define LED_PORT_CLK_ENABLE     __HAL_RCC_GPIOD_CLK_ENABLE
#elif defined STM32L5
    #include <stm32l5xx_hal.h>

    // NUCLEO-L552ZE-Q blue led - PB7
    #define LED_PORT                GPIOB
    #define LED_PIN                 GPIO_PIN_7
    #define LED_PORT_CLK_ENABLE     __HAL_RCC_GPIOB_CLK_ENABLE
#endif

static void blinky::blinkTask(void *arg)
{
    for(;;)
    {
        vTaskDelay(500);
        HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
    }
}

void blinky::init()
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
    SystemInit();
    blinky::init();
    
    xTaskCreate(blinky::blinkTask, "blinky", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    
    vTaskStartScheduler();
    for (;;);
    
    return 0;
}

extern "C" void vApplicationTickHook(void)
{
}

extern "C" void vApplicationIdleHook(void)
{
}

extern "C" void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for(;;);
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    (void) pcTaskName;
    (void) pxTask;

    taskDISABLE_INTERRUPTS();
    for(;;);
}
