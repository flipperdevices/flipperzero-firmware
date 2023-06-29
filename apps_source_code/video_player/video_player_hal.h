#include <stm32wbxx_ll_dma.h>
#include <stm32wbxx_ll_gpio.h>
#include <stm32wbxx_ll_tim.h>

#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

#include "video_player.h"

#define SPEAKER_PWM_TIMER TIM16
#define SAMPLE_RATE_TIMER TIM1

#define SPEAKER_PWM_TIMER_CHANNEL LL_TIM_CHANNEL_CH1

#define TIMER_BASE_CLOCK 64000000 /* CPU frequency, 64 MHz */

#define DMA_INSTANCE DMA1, LL_DMA_CHANNEL_1

void player_init_hardware_and_play(VideoPlayerApp* player);
void video_player_dma_isr(void* context);
void player_deinit_hardware();

void player_stop();
void player_start();