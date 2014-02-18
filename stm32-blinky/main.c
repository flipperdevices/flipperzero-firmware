#if defined STM32F1
# include <stm32f10x.h>
#elif defined STM32F4
# include <stm32f4xx.h>
#endif

void initGPIO()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    GPIO_InitTypeDef GPIO_Config;
#if defined STM32F1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_Config.GPIO_Pin =  GPIO_Pin_8;
    GPIO_Config.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Config.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOC, &GPIO_Config);
    GPIO_PinRemapConfig( GPIO_FullRemap_TIM3, ENABLE );
#elif defined STM32F4
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_Config.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Config.GPIO_OType = GPIO_OType_PP;
    GPIO_Config.GPIO_Pin = GPIO_Pin_6;
    GPIO_Config.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Config.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_Init(GPIOA, &GPIO_Config);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);
#endif
}

void initTimers()
{
	SystemCoreClockUpdate();

    TIM_TimeBaseInitTypeDef TIM_BaseConfig;
    TIM_OCInitTypeDef TIM_OCConfig;

    // 10 kHz timer.
    TIM_BaseConfig.TIM_Prescaler = (uint16_t)(SystemCoreClock / 10000) - 1;
    // 10000 / 5000 = 1 Hz blinking.
    TIM_BaseConfig.TIM_Period = 5000;
    TIM_BaseConfig.TIM_ClockDivision = 0;
    TIM_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;
    // 2500 / 5000 = 50% duty cycle.
    TIM_OCConfig.TIM_Pulse = 2499;
    TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High;

#if defined STM32F1
    TIM_OC3Init(TIM3, &TIM_OCConfig);
#elif defined STM32F4
    // For STM32F4 timer clock = SystemCoreClock / 2 by default.
    TIM_BaseConfig.TIM_Prescaler = (uint16_t)(SystemCoreClock / 2 / 10000) - 1;
    TIM_OC1Init(TIM3, &TIM_OCConfig);
#endif

    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_TimeBaseInit(TIM3, &TIM_BaseConfig);
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
