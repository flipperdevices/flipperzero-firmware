#include "flizzer_tracker_hal.h"
#include "flizzer_tracker.h"

void sound_engine_dma_isr(void* ctx) {
    SoundEngine* sound_engine = (SoundEngine*)ctx;

    // sound_engine->counter++;

    // half of transfer
    if(LL_DMA_IsActiveFlag_HT1(DMA1)) {
        LL_DMA_ClearFlag_HT1(DMA1);
        // fill first half of buffer
        uint16_t* audio_buffer = sound_engine->audio_buffer;
        uint32_t audio_buffer_length = sound_engine->audio_buffer_size / 2;
        sound_engine_fill_buffer(sound_engine, audio_buffer, audio_buffer_length);
    }

    // transfer complete
    if(LL_DMA_IsActiveFlag_TC1(DMA1)) {
        LL_DMA_ClearFlag_TC1(DMA1);
        // fill second half of buffer
        uint32_t audio_buffer_length = sound_engine->audio_buffer_size / 2;
        uint16_t* audio_buffer = &sound_engine->audio_buffer[audio_buffer_length];
        sound_engine_fill_buffer(sound_engine, audio_buffer, audio_buffer_length);
    }
}

void tracker_engine_timer_isr(
    void* ctx) // the tracker engine interrupt is of higher priority than sound engine one so it can run at the middle of filling the buffer, thus allowing sample-accurate tight effect timing
{
    TrackerEngine* tracker_engine = (TrackerEngine*)ctx;
    // tracker_engine->counter++;

    if(LL_TIM_IsActiveFlag_UPDATE(TRACKER_ENGINE_TIMER)) {
        LL_TIM_ClearFlag_UPDATE(TRACKER_ENGINE_TIMER);
        tracker_engine_advance_tick(tracker_engine);
    }
}

void sound_engine_PWM_timer_init(bool external_audio_output) // external audio on pin PA6
{
    if(external_audio_output) {
        /*if(furi_hal_speaker_is_mine()) {
            furi_hal_speaker_release();
        }*/

        //LL_TIM_DisableAllOutputs(SPEAKER_PWM_TIMER);
        //LL_TIM_DisableCounter(SPEAKER_PWM_TIMER);

        if(!(furi_hal_speaker_is_mine())) {
            if(furi_hal_speaker_acquire(1000)) {
                LL_TIM_DisableAllOutputs(SPEAKER_PWM_TIMER);
                LL_TIM_DisableCounter(SPEAKER_PWM_TIMER);

                LL_TIM_InitTypeDef TIM_InitStruct = {0};
                LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

                TIM_InitStruct.Prescaler = 0;
                TIM_InitStruct.Autoreload =
                    1023; // 10-bit PWM resolution at around 60 kHz PWM rate
                TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
                LL_TIM_Init(SPEAKER_PWM_TIMER, &TIM_InitStruct);

                TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
                TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_ENABLE;
                TIM_OC_InitStruct.CompareValue = 0;
                LL_TIM_OC_Init(SPEAKER_PWM_TIMER, SPEAKER_PWM_TIMER_CHANNEL, &TIM_OC_InitStruct);

                SPEAKER_PWM_TIMER->CNT = 0;

                LL_TIM_EnableAllOutputs(SPEAKER_PWM_TIMER);
                LL_TIM_EnableCounter(SPEAKER_PWM_TIMER);
            }
        }

        furi_hal_gpio_init(&gpio_ext_pa6, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

        LL_TIM_InitTypeDef TIM_InitStruct = {0};
        LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

        TIM_InitStruct.Prescaler = 0;
        TIM_InitStruct.Autoreload = 1023; // 10-bit PWM resolution at around 60 kHz PWM rate
        TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
        LL_TIM_Init(SPEAKER_PWM_TIMER, &TIM_InitStruct);

        TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
        TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_ENABLE;
        TIM_OC_InitStruct.CompareValue = 0;
        LL_TIM_OC_Init(SPEAKER_PWM_TIMER, SPEAKER_PWM_TIMER_CHANNEL, &TIM_OC_InitStruct);

        SPEAKER_PWM_TIMER->CNT = 0;

        LL_TIM_EnableAllOutputs(SPEAKER_PWM_TIMER);
        LL_TIM_EnableCounter(SPEAKER_PWM_TIMER);

        furi_hal_gpio_init(&gpio_ext_pa6, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        //furi_hal_gpio_init_ex(&gpio_ext_pa6, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn14TIM16);
    }

    else {
        if(!(furi_hal_speaker_is_mine())) {
            if(furi_hal_speaker_acquire(1000)) {
                LL_TIM_DisableAllOutputs(SPEAKER_PWM_TIMER);
                LL_TIM_DisableCounter(SPEAKER_PWM_TIMER);

                LL_TIM_InitTypeDef TIM_InitStruct = {0};
                LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

                TIM_InitStruct.Prescaler = 0;
                TIM_InitStruct.Autoreload =
                    1023; // 10-bit PWM resolution at around 60 kHz PWM rate
                TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
                LL_TIM_Init(SPEAKER_PWM_TIMER, &TIM_InitStruct);

                TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
                TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_ENABLE;
                TIM_OC_InitStruct.CompareValue = 0;
                LL_TIM_OC_Init(SPEAKER_PWM_TIMER, SPEAKER_PWM_TIMER_CHANNEL, &TIM_OC_InitStruct);

                SPEAKER_PWM_TIMER->CNT = 0;

                LL_TIM_EnableAllOutputs(SPEAKER_PWM_TIMER);
                LL_TIM_EnableCounter(SPEAKER_PWM_TIMER);
            }
        }

        furi_hal_gpio_init(&gpio_ext_pa6, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        //furi_hal_gpio_init_ex(&gpio_ext_pa6, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn14TIM16);
    }

    furi_hal_gpio_init_ex(
        &gpio_ext_pa6, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn14TIM16);
}

void sound_engine_set_audio_output(bool external_audio_output) {
    if(external_audio_output) {
        furi_hal_gpio_init_ex(
            &gpio_ext_pa6, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn14TIM16);

        if(furi_hal_speaker_is_mine()) {
            furi_hal_speaker_release();
        }
    }

    else {
        if(!(furi_hal_speaker_is_mine())) {
            bool unu = furi_hal_speaker_acquire(1000);
            UNUSED(unu);
        }

        furi_hal_gpio_init(&gpio_ext_pa6, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    }
}

void sound_engine_timer_init(uint32_t sample_rate) // external audio on pin PA6
{
    if(!furi_hal_bus_is_enabled(FuriHalBusTIM1)) {
        furi_hal_bus_enable(FuriHalBusTIM1);
    }

    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

    TIM_InitStruct.Prescaler = 0;
    TIM_InitStruct.Autoreload =
        TIMER_BASE_CLOCK / sample_rate - 1; // to support various sample rates
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    LL_TIM_Init(SAMPLE_RATE_TIMER, &TIM_InitStruct);

    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_ENABLE;
    LL_TIM_OC_Init(SAMPLE_RATE_TIMER, SPEAKER_PWM_TIMER_CHANNEL, &TIM_OC_InitStruct);

    LL_TIM_EnableAllOutputs(SAMPLE_RATE_TIMER);

    SAMPLE_RATE_TIMER->CNT = 0;
}

void tracker_engine_timer_init(uint8_t rate) // 0-255 hz
{
    if(!furi_hal_bus_is_enabled(FuriHalBusTIM2)) {
        furi_hal_bus_enable(FuriHalBusTIM2);
    }

    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

    TIM_InitStruct.Prescaler = 0; // using 32-bit timer
    TIM_InitStruct.Autoreload =
        (uint32_t)TIMER_BASE_CLOCK / (uint32_t)rate - 1; // to support various tracker engine rates
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    LL_TIM_Init(TRACKER_ENGINE_TIMER, &TIM_InitStruct);

    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_ENABLE;
    LL_TIM_OC_Init(TRACKER_ENGINE_TIMER, SPEAKER_PWM_TIMER_CHANNEL, &TIM_OC_InitStruct);

    LL_TIM_EnableIT_UPDATE(TRACKER_ENGINE_TIMER);

    TRACKER_ENGINE_TIMER->CNT = 0;
}

void tracker_engine_set_rate(uint8_t rate) {
    if(!furi_hal_bus_is_enabled(FuriHalBusTIM2)) {
        furi_hal_bus_enable(FuriHalBusTIM2);
    }

    LL_TIM_InitTypeDef TIM_InitStruct = {0};

    TIM_InitStruct.Prescaler = 0; // using 32-bit timer
    TIM_InitStruct.Autoreload =
        (uint32_t)TIMER_BASE_CLOCK / (uint32_t)rate - 1; // to support various tracker engine rates
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    LL_TIM_Init(TRACKER_ENGINE_TIMER, &TIM_InitStruct);

    TRACKER_ENGINE_TIMER->CNT = 0;
}

void tracker_engine_init_hardware(uint8_t rate) {
    tracker_engine_timer_init(rate);
}

void sound_engine_dma_init(uint32_t address, uint32_t size) {
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
    LL_DMA_SetMemorySize(DMA_INSTANCE, LL_DMA_MDATAALIGN_HALFWORD);

    LL_DMA_EnableIT_TC(DMA_INSTANCE);
    LL_DMA_EnableIT_HT(DMA_INSTANCE);
}

void sound_engine_init_hardware(
    uint32_t sample_rate,
    bool external_audio_output,
    uint16_t* audio_buffer,
    uint32_t audio_buffer_size) {
    sound_engine_dma_init((uint32_t)audio_buffer, audio_buffer_size);
    sound_engine_timer_init(sample_rate);
    sound_engine_PWM_timer_init(external_audio_output);
}

void sound_engine_dma_start() {
    LL_DMA_EnableChannel(DMA_INSTANCE);
    LL_TIM_EnableDMAReq_UPDATE(SAMPLE_RATE_TIMER);
}

void sound_engine_dma_stop() {
    LL_DMA_DisableChannel(DMA_INSTANCE);
}

void sound_engine_start() {
    SAMPLE_RATE_TIMER->CNT = 0;
    LL_TIM_EnableCounter(SAMPLE_RATE_TIMER);

    sound_engine_dma_start();
}

void sound_engine_stop() {
    LL_TIM_DisableAllOutputs(SAMPLE_RATE_TIMER);
    LL_TIM_DisableCounter(SAMPLE_RATE_TIMER);

    sound_engine_dma_stop();
}

void sound_engine_deinit_timer() {
    LL_TIM_DisableAllOutputs(SAMPLE_RATE_TIMER);
    LL_TIM_DisableAllOutputs(SPEAKER_PWM_TIMER);

    LL_TIM_DisableCounter(SPEAKER_PWM_TIMER);

    if(furi_hal_speaker_is_mine()) {
        furi_hal_speaker_release();
    }

    if(furi_hal_bus_is_enabled(FuriHalBusTIM2)) {
        furi_hal_bus_disable(FuriHalBusTIM2);
    }
    if(furi_hal_bus_is_enabled(FuriHalBusTIM1)) {
        furi_hal_bus_disable(FuriHalBusTIM1);
    }
}

void tracker_engine_start() {
    TRACKER_ENGINE_TIMER->CNT = 0;

    LL_TIM_EnableAllOutputs(TRACKER_ENGINE_TIMER);
    LL_TIM_EnableCounter(TRACKER_ENGINE_TIMER);
}

void tracker_engine_stop() {
    LL_TIM_DisableAllOutputs(TRACKER_ENGINE_TIMER);
    LL_TIM_DisableCounter(TRACKER_ENGINE_TIMER);
}

void play() {
    tracker_engine_start();
    sound_engine_start();
}

void stop() {
    sound_engine_stop();
    tracker_engine_stop();
}