#include <stm32f10x.h>

void initGPIO()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_Config;
    GPIO_Config.GPIO_Pin =  GPIO_Pin_6;
    GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_Config);
}

void initTimers()
{
    TIM_TimeBaseInitTypeDef TIM_BaseConfig;
    TIM_OCInitTypeDef TIM_OCConfig;

    // 0 kHz timer.
    TIM_BaseConfig.TIM_Prescaler = (uint16_t)(SystemCoreClock / 10000) - 1;
    // 10000 / 5000 = 2 Hz blinking.
    TIM_BaseConfig.TIM_Period = 5000;
    TIM_BaseConfig.TIM_ClockDivision = 0;
    TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_BaseConfig);

    TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;
    // 2500 / 5000 = 50% duty cycle.
    TIM_OCConfig.TIM_Pulse = 2499;
    TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM3, &TIM_OCConfig);

    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);

    TIM_Cmd(TIM3, ENABLE);
}

void initAll(void)
{
    initGPIO();
    initTimers();
}

int main(void)
{
    initAll();
    for (;;);
    return 0;
}
