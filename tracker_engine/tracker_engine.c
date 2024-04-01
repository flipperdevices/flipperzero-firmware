#include "tracker_engine.h"

#include "../flizzer_tracker_hal.h"
#include "../macros.h"

#include "../sound_engine/sound_engine_osc.h"
#include <furi_hal.h>

void tracker_engine_init(TrackerEngine* tracker_engine, uint8_t rate, SoundEngine* sound_engine) {
    memset(tracker_engine, 0, sizeof(TrackerEngine));

    furi_hal_interrupt_set_isr_ex(
        FuriHalInterruptIdTIM2,
        FuriHalInterruptPriorityHighest,
        tracker_engine_timer_isr,
        (void*)tracker_engine);

    tracker_engine_init_hardware(rate);

    tracker_engine->sound_engine = sound_engine;
    tracker_engine->rate = rate;
}

void tracker_engine_deinit_song(TrackerSong* song, bool free_song) {
    for(int i = 0; i < MAX_PATTERNS; i++) {
        if(song->pattern[i].step != NULL) {
            free(song->pattern[i].step);
        }
    }

    for(int i = 0; i < MAX_INSTRUMENTS; i++) {
        if(song->instrument[i] != NULL) {
            free(song->instrument[i]);
        }
    }

    if(free_song) {
        free(song);
    }
}

void tracker_engine_deinit(TrackerEngine* tracker_engine, bool free_song) {
    tracker_engine_deinit_song(tracker_engine->song, free_song);

    furi_hal_interrupt_set_isr_ex(
        FuriHalInterruptIdTIM2, FuriHalInterruptPriorityHighest, NULL, NULL);
    tracker_engine_stop();
}

void set_note(TrackerSongPatternStep* step, uint8_t note) {
    step->note &= 0x80;
    step->note |= (note & 0x7f);
}

void set_instrument(TrackerSongPatternStep* step, uint8_t inst) {
    step->note &= 0x7f;
    step->inst_vol &= 0x0f;

    step->note |= ((inst & 0x10) << 3);
    step->inst_vol |= ((inst & 0xf) << 4);
}

void set_volume(TrackerSongPatternStep* step, uint8_t vol) {
    step->command &= 0x7fff;
    step->inst_vol &= 0xf0;

    step->command |= ((vol & 0x10) << 11);
    step->inst_vol |= (vol & 0xf);
}

void set_command(TrackerSongPatternStep* step, uint16_t command) {
    step->command &= 0x8000;
    step->command |= command & (0x7fff);
}

void set_default_instrument(Instrument* inst) {
    memset(inst, 0, sizeof(Instrument));

    inst->flags = TE_SET_CUTOFF | TE_SET_PW | TE_ENABLE_VIBRATO;
    inst->sound_engine_flags = SE_ENABLE_KEYDOWN_SYNC;

    inst->base_note = MIDDLE_C;

    inst->waveform = SE_WAVEFORM_PULSE;
    inst->pw = 0x80;

    inst->adsr.a = 0x4;
    inst->adsr.d = 0x28;
    inst->adsr.volume = 0x80;

    inst->filter_type = FIL_OUTPUT_LOWPASS;
    inst->filter_cutoff = 0xd0;

    inst->program_period = 1;

    for(int i = 0; i < INST_PROG_LEN; i++) {
        inst->program[i] = TE_PROGRAM_NOP;
    }

    inst->vibrato_speed = 0x60;
    inst->vibrato_depth = 0x20;
    inst->vibrato_delay = 0x20;
}

void set_empty_pattern(TrackerSongPattern* pattern, uint16_t pattern_length) {
    for(uint16_t i = 0; i < pattern_length; i++) {
        TrackerSongPatternStep* step = &pattern->step[i];

        set_note(step, MUS_NOTE_NONE);
        set_instrument(step, MUS_NOTE_INSTRUMENT_NONE);
        set_volume(step, MUS_NOTE_VOLUME_NONE);
        set_command(step, 0);
    }
}

uint8_t tracker_engine_get_note(TrackerSongPatternStep* step) {
    return (step->note & 0x7f);
}

uint8_t tracker_engine_get_instrument(TrackerSongPatternStep* step) {
    return ((step->note & 0x80) >> 3) | ((step->inst_vol & 0xf0) >> 4);
}

uint8_t tracker_engine_get_volume(TrackerSongPatternStep* step) {
    return (step->inst_vol & 0xf) | ((step->command & 0x8000) >> 11);
}

uint16_t tracker_engine_get_command(TrackerSongPatternStep* step) {
    return (step->command & 0x7fff);
}

void tracker_engine_set_note(
    TrackerEngine* tracker_engine,
    uint8_t chan,
    uint16_t note,
    bool update_note) {
    if(update_note) tracker_engine->channel[chan].note = note;

    sound_engine_set_channel_frequency(
        tracker_engine->sound_engine, &tracker_engine->sound_engine->channel[chan], note);
}

void tracker_engine_set_song(TrackerEngine* tracker_engine, TrackerSong* song) {
    tracker_engine->song = song;
}

void tracker_engine_trigger_instrument_internal(
    TrackerEngine* tracker_engine,
    uint8_t chan,
    Instrument* pinst,
    uint16_t note) {
    SoundEngineChannel* se_channel = &tracker_engine->sound_engine->channel[chan];
    TrackerEngineChannel* te_channel = &tracker_engine->channel[chan];

    te_channel->channel_flags = TEC_PLAYING | (te_channel->channel_flags & TEC_DISABLED);

    te_channel->program_period = pinst->program_period;

    if(!(pinst->flags & TE_PROG_NO_RESTART) && pinst->program_period > 0) {
        te_channel->channel_flags |= TEC_PROGRAM_RUNNING;

        te_channel->program_counter = 0;
        te_channel->program_loop = 1;
        te_channel->program_tick = 0;
    }

    te_channel->instrument = pinst;

    se_channel->waveform = pinst->waveform;
    se_channel->flags = pinst->sound_engine_flags;

    te_channel->flags = pinst->flags;

    te_channel->arpeggio_note = 0;
    te_channel->fixed_note = 0xffff;

    note += (uint16_t)(((int16_t)pinst->base_note - MIDDLE_C) << 8);
    tracker_engine_set_note(tracker_engine, chan, note + (int16_t)pinst->finetune, true);

    te_channel->last_note = te_channel->target_note = note + (int16_t)pinst->finetune;

    te_channel->extarp1 = te_channel->extarp2 = 0;

    if(pinst->flags & TE_ENABLE_VIBRATO) {
        te_channel->vibrato_speed = pinst->vibrato_speed;
        te_channel->vibrato_depth = pinst->vibrato_depth;
        te_channel->vibrato_delay = pinst->vibrato_delay;
    }

    if(pinst->flags & TE_ENABLE_PWM) {
        te_channel->pwm_speed = pinst->pwm_speed;
        te_channel->pwm_depth = pinst->pwm_depth;
        te_channel->pwm_delay = pinst->pwm_delay;
    }

    if(pinst->sound_engine_flags & SE_ENABLE_KEYDOWN_SYNC) {
        te_channel->vibrato_position = ((ACC_LENGTH / 2 / 2) << 9);
        te_channel->pwm_position = ((ACC_LENGTH / 2 / 2) << 9);

        se_channel->accumulator = 0;
        se_channel->lfsr = RANDOM_SEED;
    }

    if(pinst->flags & TE_SET_CUTOFF) {
        te_channel->filter_cutoff = ((uint16_t)pinst->filter_cutoff << 3);
        te_channel->filter_resonance = (uint16_t)pinst->filter_resonance;

        se_channel->filter.low = 0;
        se_channel->filter.high = 0;
        se_channel->filter.band = 0;

        sound_engine_filter_set_coeff(
            &se_channel->filter, te_channel->filter_cutoff, te_channel->filter_resonance);
    }

    if(pinst->sound_engine_flags & SE_ENABLE_FILTER) {
        te_channel->filter_type = pinst->filter_type;
        se_channel->filter_mode = te_channel->filter_type;
    }

    if(pinst->flags & TE_SET_PW) {
        te_channel->pw = (pinst->pw << 4);
        se_channel->pw = (pinst->pw << 4);
    }

    se_channel->ring_mod = pinst->ring_mod;
    se_channel->hard_sync = pinst->hard_sync;

    te_channel->slide_speed = pinst->slide_speed;

    se_channel->adsr.a = pinst->adsr.a;
    se_channel->adsr.d = pinst->adsr.d;
    se_channel->adsr.s = pinst->adsr.s;
    se_channel->adsr.r = pinst->adsr.r;
    se_channel->adsr.volume = pinst->adsr.volume;
    se_channel->adsr.volume = (int32_t)se_channel->adsr.volume *
                              (int32_t)tracker_engine->master_volume / MAX_ADSR_VOLUME;

    te_channel->volume = pinst->adsr.volume;
    te_channel->volume =
        (int32_t)te_channel->volume * (int32_t)tracker_engine->master_volume / MAX_ADSR_VOLUME;

    sound_engine_enable_gate(
        tracker_engine->sound_engine, &tracker_engine->sound_engine->channel[chan], true);
}

void tracker_engine_execute_track_command(
    TrackerEngine* tracker_engine,
    uint8_t chan,
    TrackerSongPatternStep* step,
    bool first_tick) {
    UNUSED(first_tick);
    UNUSED(tracker_engine);
    UNUSED(chan);

    uint8_t vol = tracker_engine_get_volume(step);
    uint16_t opcode = tracker_engine_get_command(step);

    if(vol != MUS_NOTE_VOLUME_NONE &&
       !(tracker_engine->channel[chan].channel_flags & TEC_DISABLED)) {
        tracker_engine->sound_engine->channel[chan].adsr.volume =
            (int32_t)tracker_engine->channel[chan].volume * (int32_t)vol / (MUS_NOTE_VOLUME_NONE);
        // tracker_engine->sound_engine->channel[chan].adsr.volume = (int32_t)tracker_engine->sound_engine->channel[chan].adsr.volume * (int32_t)tracker_engine->channel[chan].instrument->adsr.volume / MAX_ADSR_VOLUME * (int32_t)tracker_engine->master_volume / MAX_ADSR_VOLUME;
    }

    if(tracker_engine->channel[chan].instrument != NULL && opcode != 0) {
        if((opcode & 0x7f00) == TE_EFFECT_ARPEGGIO) {
            tracker_engine->channel[chan].extarp1 = ((opcode & 0xf0) >> 4);
            tracker_engine->channel[chan].extarp2 = (opcode & 0xf);
        }

        else {
            do_command(opcode, tracker_engine, chan, tracker_engine->current_tick, false);
        }
    }

    if(tracker_engine->channel[chan].channel_flags & TEC_DISABLED) {
        tracker_engine->sound_engine->channel[chan].adsr.volume = 0;
    }
}

void tracker_engine_execute_program_tick(
    TrackerEngine* tracker_engine,
    uint8_t chan,
    uint8_t advance) {
    TrackerEngineChannel* te_channel = &tracker_engine->channel[chan];
    uint8_t tick = te_channel->program_tick;
    uint8_t visited[INST_PROG_LEN] = {0};

do_it_again:;

    const uint16_t inst = te_channel->instrument->program[tick];

    if((inst & 0x7fff) == TE_PROGRAM_END) {
        te_channel->channel_flags &= ~(TEC_PROGRAM_RUNNING);
        return;
    }

    uint8_t dont_reloop = 0;

    if((inst & 0x7fff) != TE_PROGRAM_NOP) {
        switch(inst & 0x7f00) {
        case TE_PROGRAM_JUMP: {
            if(!visited[tick]) {
                visited[tick] = 1;
                tick = inst & (INST_PROG_LEN - 1);
            }

            else
                return;

            break;
        }

        case TE_PROGRAM_LOOP_BEGIN:
            break;

        case TE_PROGRAM_LOOP_END: {
            if(te_channel->program_loop == (inst & 0xff)) {
                if(advance) te_channel->program_loop = 1;
            }

            else {
                if(advance) ++te_channel->program_loop;

                uint8_t l = 0;

                while((te_channel->instrument->program[tick] & 0x7f00) != TE_PROGRAM_LOOP_BEGIN &&
                      tick > 0) {
                    --tick;
                    if(!(te_channel->instrument->program[tick] & 0x8000)) ++l;
                }

                --tick;

                dont_reloop = l <= 1;
            }

            break;
        }

        default: {
            do_command(inst, tracker_engine, chan, te_channel->program_counter, true);
            break;
        }
        }
    }

    if((inst & 0x7fff) == TE_PROGRAM_NOP || (inst & 0x7f00) != TE_PROGRAM_JUMP) {
        ++tick;
        if(tick >= INST_PROG_LEN) {
            tick = 0;
        }
    }

    // skip to next on msb

    if(((inst & 0x8000) || ((inst & 0x7f00) == TE_PROGRAM_LOOP_BEGIN) ||
        ((inst & 0x7f00) == TE_PROGRAM_JUMP)) &&
       (inst & 0x7fff) != TE_PROGRAM_NOP && !dont_reloop) {
        goto do_it_again;
    }

    if(advance) {
        te_channel->program_tick = tick;
    }
}

void tracker_engine_advance_channel(TrackerEngine* tracker_engine, uint8_t chan) {
    SoundEngineChannel* se_channel = &tracker_engine->sound_engine->channel[chan];
    TrackerEngineChannel* te_channel = &tracker_engine->channel[chan];

    if(te_channel->channel_flags & TEC_PLAYING) {
        if(!(se_channel->flags & SE_ENABLE_GATE)) {
            te_channel->flags &= ~(TEC_PLAYING);
        }

        if(te_channel->slide_speed != 0) {
            if(te_channel->target_note > te_channel->note) {
                te_channel->note += my_min(
                    te_channel->slide_speed * 4, te_channel->target_note - te_channel->note);
            }

            else if(te_channel->target_note < te_channel->note) {
                te_channel->note -= my_min(
                    te_channel->slide_speed * 4, te_channel->note - te_channel->target_note);
            }
        }

        if(te_channel->channel_flags & TEC_PROGRAM_RUNNING) {
            uint8_t u = (te_channel->program_counter + 1) >= te_channel->program_period;
            tracker_engine_execute_program_tick(tracker_engine, chan, u);
            ++te_channel->program_counter;

            if(u) te_channel->program_counter = 0;
        }

        int16_t vib = 0;
        int32_t pwm = 0;

        if(te_channel->flags & TE_ENABLE_VIBRATO) {
            if(te_channel->vibrato_delay > 0) {
                te_channel->vibrato_delay--;
            }

            else {
                te_channel->vibrato_position += ((uint32_t)te_channel->vibrato_speed << 21);
                vib = (int32_t)(sound_engine_triangle(te_channel->vibrato_position >> 9) -
                                WAVE_AMP / 2) *
                      (int32_t)te_channel->vibrato_depth / (256 * 128);
            }
        }

        if(te_channel->flags & TE_ENABLE_PWM) {
            if(te_channel->pwm_delay > 0) {
                te_channel->pwm_delay--;
            }

            else {
                te_channel->pwm_position +=
                    ((uint32_t)te_channel->pwm_speed
                     << 20); // so minimum PWM speed is even lower than minimum vibrato speed
                pwm = ((int32_t)sound_engine_triangle((te_channel->pwm_position) >> 9) -
                       WAVE_AMP / 2) *
                      (int32_t)te_channel->pwm_depth / (256 * 16);
            }

            int16_t final_pwm = (int16_t)tracker_engine->channel[chan].pw + pwm;

            if(final_pwm < 0) {
                final_pwm = 0;
            }

            if(final_pwm > 0xfff) {
                final_pwm = 0xfff;
            }

            tracker_engine->sound_engine->channel[chan].pw = final_pwm;
        }

        else {
            tracker_engine->sound_engine->channel[chan].pw = tracker_engine->channel[chan].pw;
        }

        int32_t chn_note = (int16_t)(te_channel->fixed_note != 0xffff ? te_channel->fixed_note :
                                                                        te_channel->note) +
                           vib + ((int16_t)te_channel->arpeggio_note << 8) 
                           + te_channel->finetune_note;

        if(chn_note < 0) {
            chn_note = 0;
        }

        if(chn_note > ((12 * 7 + 11) << 8)) {
            chn_note = ((12 * 7 + 11) << 8); // highest note is B-7
        }

        tracker_engine_set_note(tracker_engine, chan, (uint16_t)chn_note, false);
    }

    if(tracker_engine->channel[chan].channel_flags &
       TEC_DISABLED) // so we can't set some non-zero volme from inst program too
    {
        tracker_engine->sound_engine->channel[chan].adsr.volume = 0;
    }
}

void tracker_engine_advance_tick(TrackerEngine* tracker_engine) {
    if(!(tracker_engine->playing)) return;

    if(!(tracker_engine->sound_engine)) return;

    TrackerSong* song = tracker_engine->song;

    uint16_t opcode = 0;

    for(uint8_t chan = 0; chan < SONG_MAX_CHANNELS; chan++) {
        SoundEngineChannel* se_channel = &tracker_engine->sound_engine->channel[chan];
        TrackerEngineChannel* te_channel = &tracker_engine->channel[chan];

        if(tracker_engine->song) {
            uint16_t sequence_position = tracker_engine->sequence_position;
            uint8_t current_pattern =
                song->sequence.sequence_step[sequence_position].pattern_indices[chan];
            uint8_t pattern_step = tracker_engine->pattern_position;

            TrackerSongPattern* pattern = &song->pattern[current_pattern];

            uint8_t note_delay = 0;

            opcode = tracker_engine_get_command(&pattern->step[pattern_step]);

            if((opcode & 0x7ff0) == TE_EFFECT_EXT_NOTE_DELAY) {
                note_delay = (opcode & 0xf);
            }

            if(tracker_engine->current_tick == note_delay) {
                uint8_t note = tracker_engine_get_note(&pattern->step[pattern_step]);
                uint8_t inst = tracker_engine_get_instrument(&pattern->step[pattern_step]);

                Instrument* pinst = NULL;

                if(inst == MUS_NOTE_INSTRUMENT_NONE) {
                    pinst = te_channel->instrument;
                }

                else {
                    if(inst < song->num_instruments) {
                        pinst = song->instrument[inst];
                        te_channel->instrument = pinst;
                    }
                }

                if(note == MUS_NOTE_CUT) {
                    sound_engine_enable_gate(tracker_engine->sound_engine, se_channel, 0);
                    se_channel->adsr.volume = 0;
                    te_channel->volume = 0;
                }

                if(note == MUS_NOTE_RELEASE) {
                    sound_engine_enable_gate(tracker_engine->sound_engine, se_channel, 0);
                }

                else if(
                    pinst && note != MUS_NOTE_RELEASE && note != MUS_NOTE_CUT &&
                    note != MUS_NOTE_NONE) {
                    uint8_t prev_adsr_volume = se_channel->adsr.volume;

                    if((opcode & 0x7f00) == TE_EFFECT_SLIDE) {
                        if(pinst->flags & TE_RETRIGGER_ON_SLIDE) {
                            uint16_t temp_note = te_channel->note;
                            tracker_engine_trigger_instrument_internal(
                                tracker_engine, chan, pinst, note << 8);
                            te_channel->note = temp_note;
                        }

                        te_channel->target_note =
                            ((note + pinst->base_note - MIDDLE_C) << 8) + pinst->finetune;
                        te_channel->slide_speed = (opcode & 0xff);
                    }

                    else if((opcode & 0x7f00) == TE_EFFECT_LEGATO) {
                        te_channel->note = te_channel->target_note = te_channel->last_note =
                            ((note + pinst->base_note - MIDDLE_C) << 8) + pinst->finetune;
                    }

                    else {
                        tracker_engine_trigger_instrument_internal(
                            tracker_engine, chan, pinst, note << 8);
                        te_channel->note =
                            ((note + pinst->base_note - MIDDLE_C) << 8) + pinst->finetune;

                        te_channel->target_note =
                            ((note + pinst->base_note - MIDDLE_C) << 8) + pinst->finetune;
                    }

                    if(inst == MUS_NOTE_INSTRUMENT_NONE) {
                        se_channel->adsr.volume = prev_adsr_volume;
                    }
                }
            }

            tracker_engine_execute_track_command(
                tracker_engine,
                chan,
                &pattern->step[pattern_step],
                tracker_engine->current_tick == note_delay);
        }

        tracker_engine_advance_channel(
            tracker_engine,
            chan); // this will be executed even if the song pointer is NULL; handy for live instrument playback from inst editor ("jams")
    }

    if(tracker_engine->song) {
        tracker_engine->current_tick++;

        if(tracker_engine->current_tick >= song->speed) {
            bool flag = true;

            for(int chan = 0; chan < SONG_MAX_CHANNELS; ++chan) {
                uint16_t sequence_position = tracker_engine->sequence_position;
                uint8_t current_pattern =
                    song->sequence.sequence_step[sequence_position].pattern_indices[chan];
                uint8_t pattern_step = tracker_engine->pattern_position;

                TrackerSongPattern* pattern = &song->pattern[current_pattern];

                opcode = tracker_engine_get_command(&pattern->step[pattern_step]);

                if((opcode & 0x7ff0) == TE_EFFECT_EXT_PATTERN_LOOP) {
                    if(opcode & 0xf) // loop end
                    {
                        if(!(tracker_engine->in_loop)) {
                            tracker_engine->loops_left = (opcode & 0xf);
                            tracker_engine->in_loop = true;

                            for(int j = tracker_engine->pattern_position; j >= 0; j--) {
                                if(tracker_engine_get_command(&pattern->step[j]) ==
                                   TE_EFFECT_EXT_PATTERN_LOOP) // search for loop start
                                {
                                    tracker_engine->pattern_position =
                                        fmax((int16_t)j - 1, 0); // jump to loop start

                                    goto out;
                                }
                            }
                        }

                        else {
                            tracker_engine->loops_left--;

                            if(tracker_engine->loops_left == 0) {
                                tracker_engine->in_loop = false;
                                goto out;
                            }

                            for(int j = tracker_engine->pattern_position; j >= 0; j--) {
                                if(tracker_engine_get_command(&pattern->step[j]) ==
                                   TE_EFFECT_EXT_PATTERN_LOOP) // search for loop start
                                {
                                    tracker_engine->pattern_position =
                                        fmax((int16_t)j - 1, 0); // jump to loop start

                                    goto out;
                                }
                            }
                        }
                    }

                    else // loop start
                    {
                    }

                out:;
                }

                if((opcode & 0x7f00) == TE_EFFECT_SKIP_PATTERN) {
                    tracker_engine->sequence_position++;
                    tracker_engine->pattern_position = 0;

                    flag = false;

                    if(tracker_engine->sequence_position >= song->num_sequence_steps) {
                        tracker_engine->playing = false;
                        tracker_engine->sequence_position--;
                        tracker_engine->pattern_position = song->pattern_length - 1;

                        for(int i = 0; i < SONG_MAX_CHANNELS; i++) {
                            sound_engine_enable_gate(
                                tracker_engine->sound_engine,
                                &tracker_engine->sound_engine->channel[i],
                                false);
                        }

                        goto end_process;
                    }
                }
            }

            if(flag) {
                tracker_engine->pattern_position++;
            }

            tracker_engine->current_tick = 0;

            if(tracker_engine->pattern_position >= song->pattern_length) {
                tracker_engine->pattern_position = 0;

                if(song->loop_start != 0 || song->loop_end != 0) {
                    if(tracker_engine->sequence_position == song->loop_end) {
                        tracker_engine->sequence_position =
                            song->loop_start; // infinite loop between loop start and loop end
                    }

                    else {
                        tracker_engine->sequence_position++;
                    }
                }

                else {
                    tracker_engine->sequence_position++;
                }

                if(tracker_engine->sequence_position >= song->num_sequence_steps) {
                    tracker_engine->playing = false;
                    tracker_engine->sequence_position--;
                    tracker_engine->pattern_position = song->pattern_length - 1;

                    for(int i = 0; i < SONG_MAX_CHANNELS; i++) {
                        sound_engine_enable_gate(
                            tracker_engine->sound_engine,
                            &tracker_engine->sound_engine->channel[i],
                            false);
                    }
                }
            }
        }
    }

end_process:;
}