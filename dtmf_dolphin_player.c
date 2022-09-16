#include "dtmf_dolphin_player.h"

#define DTMF_DOLPHIN_SAMPLE_RATE (8000)

typedef struct {
    DTMFDolphinEvent type;
} DTMFDolphinPlayerEvent;

// Keep this here for accessibility in local scope event without context
DTMFDolphinPlayer* player;

void dtmf_dolphin_dma_isr(void* ctx) {
    FuriMessageQueue *event_queue = ctx;

    if (LL_DMA_IsActiveFlag_HT1(DMA1)) {
        LL_DMA_ClearFlag_HT1(DMA1);

        DTMFDolphinPlayerEvent event = {.type = DTMFDolphinPlayerEventHalfTransfer};
        furi_message_queue_put(event_queue, &event, 0);
    }

    if(LL_DMA_IsActiveFlag_TC1(DMA1)) {
        LL_DMA_ClearFlag_TC1(DMA1);

        DTMFDolphinPlayerEvent event = {.type = DTMFDolphinPlayerEventFullTransfer};
        furi_message_queue_put(event_queue, &event, 0);
    }
}

bool dtmf_dolphin_player_init(void* context) {
    player = context;

    return false;
}

void dtmf_dolphin_player_clear_samples() {
    for (size_t i = 0; i < player->sample_count; i++) {
        player->sample_buffer[i] = 0;
    }
}

bool dtmf_dolphin_player_generate_waveform(size_t index) {
    uint16_t* sample_buffer_start = &player->sample_buffer[index];
    if (!player->wf1_freq)
        return false;

    // Generate basic sine wave sample to fill sample_count
    for (size_t i = 0; i < player->half_samples; i++) {
        // float data = sin(i * PERIOD_2_PI / player->wf1_period) + 1;
        float data = sin(player->wf1_pos * PERIOD_2_PI / player->wf1_period) + 1;
        player->wf1_pos = (player->wf1_pos + 1) % player->wf1_period;

        data *= player->volume;

        // Downmix second tone with the first
        if (player->wf2_freq) {
            data /= 2;

            float data_2 = sin(player->wf2_pos * PERIOD_2_PI / player->wf2_period) + 1;
            player->wf2_pos = (player->wf2_pos + 1) % player->wf2_period;

            data_2 *= player->volume / 2;

            data += data_2;
        }

        data = tanhf(data);

        data *= UINT8_MAX / 2; // scale -128..127
        data += UINT8_MAX / 2; // to unsigned

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

bool dtmf_dolphin_player_play_tones(float *freq) {
    player->wf1_pos = 0;
    player->wf2_pos = 0;
    player->wf1_freq = 0;
    player->wf2_freq = 0;
    player->wf1_period = 0;
    player->wf2_period = 0;
    if (freq[0]) {
        player->wf1_freq = freq[0];
        player->wf1_period = player->sample_count / freq[0] * 4;
    }
    if (freq[1]) {
        player->wf2_freq = freq[1];
        player->wf2_period = player->sample_count / freq[1] * 4;
    }
    dtmf_dolphin_player_clear_samples();

    dtmf_dolphin_player_generate_waveform(0);
    dtmf_dolphin_player_generate_waveform(player->half_samples);

    dtmf_dolphin_speaker_init();
    dtmf_dolphin_dma_init((uint32_t)player->sample_buffer, player->sample_count);

    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma1Ch1, dtmf_dolphin_dma_isr, player->queue);

    dtmf_dolphin_dma_start();
    dtmf_dolphin_speaker_start();

    player->playing = true;

    return true;
}

bool dtmf_dolphin_player_stop_tones() {
    player->playing = false;

    dtmf_dolphin_speaker_stop();
    dtmf_dolphin_dma_stop();

    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma1Ch1, NULL, NULL);
   
    return true;
}

bool dtmf_dolphin_player_handle_tick() {
    DTMFDolphinPlayerEvent event;

    if(furi_message_queue_get(player->queue, &event, FuriWaitForever) == FuriStatusOk) {
        if (player->playing) {
            if(event.type == DTMFDolphinPlayerEventHalfTransfer) {
                dtmf_dolphin_player_generate_waveform(0);
                // uint16_t* sample_buffer_start = &player->sample_buffer[0];
                // for (size_t i = 0; i < player->half_samples; i++) {
                //     sample_buffer_start[i] = player->buffer_buffer[i];
                // }
                return true;
            } else if (event.type == DTMFDolphinPlayerEventFullTransfer) {
                dtmf_dolphin_player_generate_waveform(player->half_samples);
                // uint16_t* sample_buffer_start = &player->sample_buffer[player->half_samples];
                // for (size_t i = 0; i < player->half_samples; i++) {
                //     sample_buffer_start[i] = player->buffer_buffer[i];
                // }
                return true;
            }
        } else {
            return true;
        }
    }
    return false;
}