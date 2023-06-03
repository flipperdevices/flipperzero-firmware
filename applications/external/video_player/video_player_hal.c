#include "video_player_hal.h"
#include "video_player.h"

void video_player_dma_isr(void* context) {
    VideoPlayerApp* player = (VideoPlayerApp*)context;

    // half of transfer
    if(LL_DMA_IsActiveFlag_HT1(DMA1)) {
        LL_DMA_ClearFlag_HT1(DMA1);
        // fill first half of the buffer
        //uint8_t* audio_buffer = player->audio_buffer;

        //stream_read(player->stream, player->image_buffer, player->image_buffer_length);
        //stream_read(player->stream, audio_buffer, player->audio_chunk_size);

        static VideoPlayerEvent event = {.type = EventType1stHalf};

        furi_message_queue_put(player->event_queue, &event, 0);
    }

    // transfer complete
    if(LL_DMA_IsActiveFlag_TC1(DMA1)) {
        LL_DMA_ClearFlag_TC1(DMA1);
        // fill second half of buffer
        //uint8_t* audio_buffer = &player->audio_buffer[player->audio_chunk_size];

        //stream_read(player->stream, player->image_buffer, player->image_buffer_length);
        //stream_read(player->stream, audio_buffer, player->audio_chunk_size);

        static VideoPlayerEvent event = {.type = EventType2ndHalf};

        furi_message_queue_put(player->event_queue, &event, 0);
    }
}

void player_init_hardware_and_play(VideoPlayerApp* player) {
    //DMA:

    uint32_t address = (uint32_t)player->audio_buffer;
    uint32_t size = player->audio_chunk_size * 2;
    uint32_t dma_dst = (uint32_t) & (SPEAKER_PWM_TIMER->CCR1);

    LL_DMA_ConfigAddresses(DMA_INSTANCE, address, dma_dst, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetDataLength(DMA_INSTANCE, size);

    LL_DMA_SetPeriphRequest(DMA_INSTANCE, LL_DMAMUX_REQ_TIM1_UP);
    LL_DMA_SetDataTransferDirection(DMA_INSTANCE, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetChannelPriorityLevel(DMA_INSTANCE, LL_DMA_PRIORITY_VERYHIGH);
    LL_DMA_SetMode(DMA_INSTANCE, LL_DMA_MODE_CIRCULAR);
    LL_DMA_SetPeriphIncMode(DMA_INSTANCE, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA_INSTANCE, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(DMA_INSTANCE, LL_DMA_PDATAALIGN_HALFWORD);
    LL_DMA_SetMemorySize(DMA_INSTANCE, LL_DMA_PDATAALIGN_HALFWORD);

    LL_DMA_EnableIT_TC(DMA_INSTANCE);
    LL_DMA_EnableIT_HT(DMA_INSTANCE);

    // ==== TIMERS:

    furi_hal_bus_enable(FuriHalBusTIM1);

    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

    TIM_InitStruct.Prescaler = 1;
    TIM_InitStruct.Autoreload = 255;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    LL_TIM_Init(SPEAKER_PWM_TIMER, &TIM_InitStruct);

    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_ENABLE;
    TIM_OC_InitStruct.CompareValue = 0;
    LL_TIM_OC_Init(SPEAKER_PWM_TIMER, SPEAKER_PWM_TIMER_CHANNEL, &TIM_OC_InitStruct);
    LL_TIM_EnableAllOutputs(SPEAKER_PWM_TIMER);
    LL_TIM_EnableCounter(SPEAKER_PWM_TIMER);

    //SAMPLE RATE TIMER:

    LL_TIM_InitTypeDef TIM_InitStruct1 = {0};
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct1 = {0};

    TIM_InitStruct1.Prescaler = 0;
    TIM_InitStruct1.Autoreload =
        (TIMER_BASE_CLOCK / (uint32_t)player->sample_rate - 1); // to support various sample rates
    TIM_InitStruct1.CounterMode = LL_TIM_COUNTERMODE_UP;
    LL_TIM_Init(SAMPLE_RATE_TIMER, &TIM_InitStruct1);

    TIM_OC_InitStruct1.OCMode = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct1.OCState = LL_TIM_OCSTATE_ENABLE;
    LL_TIM_OC_Init(SAMPLE_RATE_TIMER, SPEAKER_PWM_TIMER_CHANNEL, &TIM_OC_InitStruct1);

    LL_TIM_EnableAllOutputs(SAMPLE_RATE_TIMER);

    SAMPLE_RATE_TIMER->CNT = 0;

    furi_hal_gpio_init_ex(
        &gpio_ext_pa6, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn14TIM16);

    /*if(!(furi_hal_speaker_is_mine())) 
    {
        bool unu = furi_hal_speaker_acquire(1000);
        UNUSED(unu);
    }*/

    furi_hal_interrupt_set_isr_ex(FuriHalInterruptIdDma1Ch1, 15, video_player_dma_isr, player);

    // START!!
    LL_TIM_EnableDMAReq_UPDATE(SAMPLE_RATE_TIMER);
    LL_DMA_EnableChannel(DMA_INSTANCE);
    LL_TIM_EnableCounter(SAMPLE_RATE_TIMER);
}

void player_deinit_hardware() {
    LL_DMA_DisableChannel(DMA_INSTANCE);

    if(furi_hal_speaker_is_mine()) {
        furi_hal_speaker_release();
    }

    furi_hal_gpio_init(&gpio_ext_pa6, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_interrupt_set_isr_ex(FuriHalInterruptIdDma1Ch1, 15, NULL, NULL);

    if(furi_hal_bus_is_enabled(FuriHalBusTIM1)) {
        furi_hal_bus_disable(FuriHalBusTIM1);
    }
}

void player_stop() {
    LL_TIM_DisableCounter(SAMPLE_RATE_TIMER);
}

void player_start() {
    LL_TIM_EnableCounter(SAMPLE_RATE_TIMER);
}
