static inline void app_tim_ic_init(bool both) {
    HAL_TIM_OC_Stop(&htim8, TIM_CHANNEL_2);

    TIM_IC_InitTypeDef sConfigIC = {0};
    sConfigIC.ICPolarity = both ? TIM_INPUTCHANNELPOLARITY_BOTHEDGE : TIM_INPUTCHANNELPOLARITY_FALLING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;
    HAL_TIM_IC_ConfigChannel(&htim8, &sConfigIC, TIM_CHANNEL_2);

    HAL_TIM_IC_Start_IT(&htim8, TIM_CHANNEL_2);
}

static inline void app_tim_pulse(uint32_t width) {
    htim8.State = HAL_TIM_STATE_BUSY;

    __HAL_TIM_DISABLE(&htim8);

    __HAL_TIM_SET_COUNTER(&htim8, 0);

    TIM_OC_InitTypeDef sConfigOC;
    sConfigOC.OCMode = TIM_OCMODE_INACTIVE;
    sConfigOC.Pulse = (uint16_t)(width);
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    // HAL_TIM_OC_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2);

    htim8.Lock = HAL_LOCKED;

    /* Configure the TIM Channel 2 in Output Compare */
    TIM_OC2_SetConfig(htim8.Instance, &sConfigOC);

    htim8.Lock = HAL_UNLOCKED;

    // TIM_CCxChannelCmd(htim8.Instance, TIM_CHANNEL_2, TIM_CCx_ENABLE);

    /* Reset the CCxE Bit */
    htim8.Instance->CCER &= ~(TIM_CCER_CC1E << (TIM_CHANNEL_2 & 0x1FU));

    /* Set or reset the CCxE Bit */
    htim8.Instance->CCER |= (uint32_t)(TIM_CCx_ENABLE << (TIM_CHANNEL_2 & 0x1FU));

    __HAL_TIM_MOE_ENABLE(&htim8);
    __HAL_TIM_ENABLE(&htim8);

    htim8.State = HAL_TIM_STATE_READY;
}

static inline void app_tim_stop() {
    HAL_TIM_OC_Stop(&htim8, TIM_CHANNEL_2);
    HAL_TIM_IC_Stop(&htim8, TIM_CHANNEL_2);
}

void register_tim8_callback_ch2(void(*callback)(uint16_t ccr, TimerEvent tim_event));
