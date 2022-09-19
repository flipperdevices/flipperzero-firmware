#include "dtmf_dolphin_audio.h"

DTMFDolphinAudio *current_player;

static void dtmf_dolphin_audio_dma_isr(void* ctx) {
    FuriMessageQueue *event_queue = ctx;

    if (LL_DMA_IsActiveFlag_HT1(DMA1)) {
        LL_DMA_ClearFlag_HT1(DMA1);

        DTMFDolphinCustomEvent event = {.type = DTMFDolphinEventDMAHalfTransfer};
        furi_message_queue_put(event_queue, &event, 0);
    }

    if(LL_DMA_IsActiveFlag_TC1(DMA1)) {
        LL_DMA_ClearFlag_TC1(DMA1);

        DTMFDolphinCustomEvent event = {.type = DTMFDolphinEventDMAFullTransfer};
        furi_message_queue_put(event_queue, &event, 0);
    }
}

void dtmf_dolphin_audio_clear_samples(DTMFDolphinAudio* player) {
    for (size_t i = 0; i < player->buffer_length; i++) {
        player->sample_buffer[i] = 0;
    }
}

DTMFDolphinOsc* dtmf_dolphin_osc_alloc() {
    DTMFDolphinOsc *osc = { 0 };
    osc->cached_freq = 0;
    osc->offset = 0;
    osc->period = 0;
    return osc;
}

DTMFDolphinAudio* dtmf_dolphin_audio_alloc() {
    DTMFDolphinAudio player;
    player.buffer_length = SAMPLE_BUFFER_LENGTH;
    player.half_buffer_length = SAMPLE_BUFFER_LENGTH / 2;
    player.buffer_buffer = malloc(sizeof(uint8_t) * player.buffer_length);
    player.sample_buffer = malloc(sizeof(uint16_t) * player.buffer_length);
    player.osc1 = dtmf_dolphin_osc_alloc();
    player.osc2 = dtmf_dolphin_osc_alloc();
    player.playing = false;
    player.volume = 2.0f;
    player.queue = furi_message_queue_alloc(10, sizeof(DTMFDolphinCustomEvent));
    dtmf_dolphin_audio_clear_samples(&player);

    return false;
}

size_t calc_waveform_period(float freq) {
    if (!freq) {
        return 0;
    }
    // DMA Rate constant, thanks to Dr_Zlo
    float dma_rate = CPU_CLOCK_FREQ \
        / 2 \
        / DTMF_DOLPHIN_HAL_DMA_PRESCALER \
        / (DTMF_DOLPHIN_HAL_DMA_AUTORELOAD + 1);

    return (uint16_t) (dma_rate / freq);
}

float sample_frame(DTMFDolphinOsc* osc, float freq) {
    float frame = 0.0;

    if (freq != osc->cached_freq || !osc->period) {
        osc->cached_freq = freq;
        osc->period = calc_waveform_period(freq);
        osc->offset = 0;
    }
    if (osc->period) {
        frame = tanhf(sin(osc->offset * PERIOD_2_PI / osc->period) + 1);
        osc->offset = (osc->offset + 1) % osc->period;
    }

    return frame;
}

void dtmf_dolphin_audio_free(DTMFDolphinAudio* player) {
    furi_message_queue_free(player->queue);
    dtmf_dolphin_osc_free(player->osc1);
    dtmf_dolphin_osc_free(player->osc2);
    free(player->buffer_buffer);
    free(player->sample_buffer);
}

void dtmf_dolphin_osc_free(DTMFDolphinOsc* osc) {
    UNUSED(osc);
    // Nothing to free now, but keeping this here in case I reimplement caching
}

bool generate_waveform(DTMFDolphinAudio* player, float freq1, float freq2, uint16_t buffer_index) {
    uint16_t* sample_buffer_start = &player->sample_buffer[buffer_index];

    for (size_t i = 0; i < player->half_buffer_length; i++) {
        float data = (sample_frame(player->osc1, freq1) / 2) + (sample_frame(player->osc2, freq2) / 2);
        data *= player->volume;
        data *= UINT8_MAX / 2;  // scale -128..127
        data += UINT8_MAX / 2;  // to unsigned

        if(data < 0) {
            data = 0;
        }

        if(data > 255) {
            data = 255;
        }

        player->buffer_buffer[i] = data;
        sample_buffer_start[i] = data;
    }

    return true;
}

bool dtmf_dolphin_audio_play_tones(float freq1, float freq2) {
    current_player = dtmf_dolphin_audio_alloc();

    generate_waveform(current_player, freq1, freq2, 0);
    generate_waveform(current_player, freq1, freq2, current_player->half_buffer_length);

    dtmf_dolphin_speaker_init();
    dtmf_dolphin_dma_init((uint32_t)current_player->sample_buffer, current_player->buffer_length);

    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma1Ch1, dtmf_dolphin_audio_dma_isr, current_player->queue);

    dtmf_dolphin_dma_start();
    dtmf_dolphin_speaker_start();

    return true;
}

bool dtmf_dolphin_audio_stop_tones() {
    current_player->playing = false;

    dtmf_dolphin_speaker_stop();
    dtmf_dolphin_dma_stop();

    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma1Ch1, NULL, NULL);

    dtmf_dolphin_audio_free(current_player);
   
    return true;
}

bool dtmf_dolphin_audio_handle_tick() {
    DTMFDolphinCustomEvent event;

    if(furi_message_queue_get(current_player->queue, &event, FuriWaitForever) == FuriStatusOk) {
        if(event.type == DTMFDolphinEventDMAHalfTransfer) {
            generate_waveform(
                current_player, 
                (double) current_player->osc1->cached_freq, 
                (double) current_player->osc2->cached_freq, 
                0);
            return true;
        } else if (event.type == DTMFDolphinEventDMAFullTransfer) {
            generate_waveform(
                current_player, 
                (double) current_player->osc1->cached_freq, 
                (double) current_player->osc2->cached_freq, 
                current_player->half_buffer_length);
            return true;
        }
    }
    return false;
}