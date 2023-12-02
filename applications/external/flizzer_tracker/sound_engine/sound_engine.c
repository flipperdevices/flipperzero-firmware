#include "sound_engine.h"
#include "../flizzer_tracker_hal.h"

#include <furi_hal.h>

#define PI 3.1415

void sound_engine_init(
    SoundEngine* sound_engine,
    uint32_t sample_rate,
    bool external_audio_output,
    uint32_t audio_buffer_size) {
    if(sound_engine->audio_buffer) {
        free(sound_engine->audio_buffer);
    }

    if(sound_engine->sine_lut) {
        free(sound_engine->sine_lut);
    }

    memset(sound_engine, 0, sizeof(SoundEngine));

    sound_engine->audio_buffer = malloc(audio_buffer_size * sizeof(sound_engine->audio_buffer[0]));
    memset(sound_engine->audio_buffer, 0, sizeof(SoundEngine));
    sound_engine->audio_buffer_size = audio_buffer_size;
    sound_engine->sample_rate = sample_rate;
    sound_engine->external_audio_output = external_audio_output;

    for(int i = 0; i < NUM_CHANNELS; ++i) {
        sound_engine->channel[i].lfsr = RANDOM_SEED;
    }

    for(int i = 0; i < SINE_LUT_SIZE; ++i) {
        sound_engine->sine_lut[i] = (uint8_t)((sinf(i / 64.0 * PI) + 1.0) * 127.0);
    }

    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma1Ch1, NULL, NULL);

    furi_hal_interrupt_set_isr_ex(
        FuriHalInterruptIdDma1Ch1, 15, sound_engine_dma_isr, sound_engine);

    sound_engine_init_hardware(
        sample_rate, external_audio_output, sound_engine->audio_buffer, audio_buffer_size);
}

void sound_engine_deinit(SoundEngine* sound_engine) {
    free(sound_engine->audio_buffer);

    if(!(sound_engine->external_audio_output)) {
        if(furi_hal_speaker_is_mine()) {
            furi_hal_speaker_release();
        }
    }

    else {
        furi_hal_gpio_init(&gpio_ext_pa6, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    }

    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma1Ch1, NULL, NULL);
    sound_engine_stop();
    sound_engine_deinit_timer();
}

void sound_engine_set_channel_frequency(
    SoundEngine* sound_engine,
    SoundEngineChannel* channel,
    uint16_t note) {
    uint32_t frequency = get_freq(note);

    if(frequency != 0) {
        channel->frequency = (uint64_t)(ACC_LENGTH) / (uint64_t)1024 * (uint64_t)(frequency) /
                             (uint64_t)sound_engine->sample_rate;
    }

    else {
        channel->frequency = 0;
    }
}

void sound_engine_enable_gate(SoundEngine* sound_engine, SoundEngineChannel* channel, bool enable) {
    if(enable) {
        channel->adsr.envelope = 0;
        channel->adsr.envelope_speed = envspd(sound_engine, channel->adsr.a);
        channel->adsr.envelope_state = ATTACK;

        channel->flags |= SE_ENABLE_GATE;

        if(channel->flags & SE_ENABLE_KEYDOWN_SYNC) {
            channel->accumulator = 0;
        }
    }

    else {
        channel->adsr.envelope_state = RELEASE;
        channel->adsr.envelope_speed = envspd(sound_engine, channel->adsr.r);
    }
}

void sound_engine_fill_buffer(
    SoundEngine* sound_engine,
    uint16_t* audio_buffer,
    uint32_t audio_buffer_size) {
    int32_t channel_output[NUM_CHANNELS];
    int32_t channel_output_final[NUM_CHANNELS];

    for(uint32_t i = 0; i < audio_buffer_size; ++i) {
        int32_t output = WAVE_AMP * 2;

        for(uint32_t chan = 0; chan < NUM_CHANNELS; ++chan) {
            SoundEngineChannel* channel = &sound_engine->channel[chan];

            if(channel->frequency > 0) {
                uint32_t prev_acc = channel->accumulator;

                channel->accumulator += channel->frequency;

                channel->sync_bit |= (channel->accumulator & ACC_LENGTH);

                channel->accumulator &= ACC_LENGTH - 1;

                if(channel->flags & SE_ENABLE_HARD_SYNC) {
                    uint8_t hard_sync_src = channel->hard_sync == 0xff ? i : channel->hard_sync;

                    if(sound_engine->channel[hard_sync_src].sync_bit) {
                        channel->accumulator = 0;
                    }
                }

                channel_output[chan] =
                    sound_engine_osc(sound_engine, channel, prev_acc) - WAVE_AMP / 2;

                if(channel->flags & SE_ENABLE_RING_MOD) {
                    uint8_t ring_mod_src = channel->ring_mod == 0xff ? i : channel->ring_mod;
                    channel_output[chan] =
                        channel_output[chan] * channel_output[ring_mod_src] / WAVE_AMP;
                }

                channel_output_final[chan] = sound_engine_cycle_and_output_adsr(
                    channel_output[chan], sound_engine, &channel->adsr, &channel->flags);

                if(channel->flags & SE_ENABLE_FILTER) {
                    if(channel->filter_mode != 0) {
                        sound_engine_filter_cycle(&channel->filter, channel_output_final[chan]);

                        switch(channel->filter_mode) {
                        case FIL_OUTPUT_LOWPASS: {
                            channel_output_final[chan] =
                                sound_engine_output_lowpass(&channel->filter);
                            break;
                        }

                        case FIL_OUTPUT_HIGHPASS: {
                            channel_output_final[chan] =
                                sound_engine_output_highpass(&channel->filter);
                            break;
                        }

                        case FIL_OUTPUT_BANDPASS: {
                            channel_output_final[chan] =
                                sound_engine_output_bandpass(&channel->filter);
                            break;
                        }

                        case FIL_OUTPUT_LOW_HIGH: {
                            channel_output_final[chan] =
                                sound_engine_output_lowpass(&channel->filter) +
                                sound_engine_output_highpass(&channel->filter);
                            break;
                        }

                        case FIL_OUTPUT_HIGH_BAND: {
                            channel_output_final[chan] =
                                sound_engine_output_highpass(&channel->filter) +
                                sound_engine_output_bandpass(&channel->filter);
                            break;
                        }

                        case FIL_OUTPUT_LOW_BAND: {
                            channel_output_final[chan] =
                                sound_engine_output_lowpass(&channel->filter) +
                                sound_engine_output_bandpass(&channel->filter);
                            break;
                        }

                        case FIL_OUTPUT_LOW_HIGH_BAND: {
                            channel_output_final[chan] =
                                sound_engine_output_lowpass(&channel->filter) +
                                sound_engine_output_highpass(&channel->filter) +
                                sound_engine_output_bandpass(&channel->filter);
                            break;
                        }
                        }
                    }
                }

                output += channel_output_final[chan];
            }
        }

        //audio_buffer[i] = output / (64 * 4);
        audio_buffer[i] = output >> 8;
    }
}