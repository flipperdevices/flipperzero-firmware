void(*tim8_callback_ch2)(uint16_t ccr, TimerEvent tim_event);

void register_tim8_callback_ch2(void(*callback)(uint16_t ccr, TimerEvent tim_event)) {
  tim8_callback_ch2 = callback;
}
/**
  * @brief This function handles TIM8 capture compare interrupt.
  */
void TIM8_CC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM8_CC_IRQn 0 */

  /* Capture compare 2 event */
  if (__HAL_TIM_GET_FLAG(&htim8, TIM_FLAG_CC2) != RESET)
  {
    if (__HAL_TIM_GET_IT_SOURCE(&htim8, TIM_IT_CC2) != RESET)
    {
      __HAL_TIM_CLEAR_IT(&htim8, TIM_IT_CC2);
      htim8.Channel = HAL_TIM_ACTIVE_CHANNEL_2;
      /* Input capture event */
      if ((htim8.Instance->CCMR1 & TIM_CCMR1_CC2S) != 0x00U)
      {
/*
#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
        htim->IC_CaptureCallback(htim);
#else
        HAL_TIM_IC_CaptureCallback(htim);
#endif
*/
        if(tim8_callback_ch2 != NULL) {
          tim8_callback_ch2(htim8.Instance->CCR2, TimerEventInputCapture);
        }
      }
      /* Output compare event */
      else
      {
        if(tim8_callback_ch2 != NULL) {
          tim8_callback_ch2(0, TimerEventEndOfPulse);
        }
/*
#if (USE_HAL_TIM_REGISTER_CALLBACKS == 1)
        htim->OC_DelayElapsedCallback(htim);
        htim->PWM_PulseFinishedCallback(htim);
#else
        HAL_TIM_OC_DelayElapsedCallback(htim);
        HAL_TIM_PWM_PulseFinishedCallback(htim);
#endif
*/
      }
      htim8.Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
    }
  }


  /* USER CODE END TIM8_CC_IRQn 0 */
  // HAL_TIM_IRQHandler(&htim8);
  /* USER CODE BEGIN TIM8_CC_IRQn 1 */

  /* USER CODE END TIM8_CC_IRQn 1 */
}