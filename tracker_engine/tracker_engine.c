#include "tracker_engine.h"

#include "../flizzer_tracker_hal.h"

#include <furi_hal.h>

void tracker_engine_init(TrackerEngine *tracker_engine, uint8_t rate, SoundEngine *sound_engine)
{
    memset(tracker_engine, 0, sizeof(TrackerEngine));

    furi_hal_interrupt_set_isr_ex(FuriHalInterruptIdTIM2, 14, tracker_engine_timer_isr, (void *)tracker_engine);
    tracker_engine_init_hardware(rate);

    tracker_engine->sound_engine = sound_engine;
    tracker_engine->rate = rate;
}

void tracker_engine_deinit_song(TrackerSong *song, bool free_song)
{
    for (int i = 0; i < MAX_PATTERNS; i++)
    {
        if (song->pattern[i].step != NULL)
        {
            free(song->pattern[i].step);
        }
    }

    for (int i = 0; i < MAX_INSTRUMENTS; i++)
    {
        if (song->instrument[i] != NULL)
        {
            free(song->instrument[i]);
        }
    }

    if (free_song)
    {
        free(song);
    }
}

void tracker_engine_deinit(TrackerEngine *tracker_engine, bool free_song)
{
    tracker_engine_deinit_song(tracker_engine->song, free_song);

    furi_hal_interrupt_set_isr_ex(FuriHalInterruptIdTIM2, 13, NULL, NULL);
    tracker_engine_stop();
}

uint8_t tracker_engine_get_note(TrackerSongPatternStep *step)
{
    return (step->note & 0x7f);
}

uint8_t tracker_engine_get_instrument(TrackerSongPatternStep *step)
{
    return ((step->note & 0x80) >> 3) | ((step->inst_vol & 0xf0) >> 4);
}

uint8_t tracker_engine_get_volume(TrackerSongPatternStep *step)
{
    return (step->inst_vol & 0xf) | ((step->command & 0x8000) >> 11);
}

uint16_t tracker_engine_get_command(TrackerSongPatternStep *step)
{
    return (step->command & 0x7fff);
}

void tracker_engine_set_note(TrackerEngine *tracker_engine, uint8_t chan, uint16_t note, bool update_note)
{
    if (update_note)
        tracker_engine->channel[chan].note = note;

    sound_engine_set_channel_frequency(tracker_engine->sound_engine, &tracker_engine->sound_engine->channel[chan], note);
}

void tracker_engine_set_song(TrackerEngine *tracker_engine, TrackerSong *song)
{
    tracker_engine->song = song;
}

void tracker_engine_trigger_instrument_internal(TrackerEngine *tracker_engine, uint8_t chan, Instrument *pinst, uint16_t note)
{
    SoundEngineChannel *se_channel = &tracker_engine->sound_engine->channel[chan];
    TrackerEngineChannel *te_channel = &tracker_engine->channel[chan];

    te_channel->flags = TEC_PLAYING | (te_channel->channel_flags & TEC_DISABLED);

    if (!(pinst->flags & TE_PROG_NO_RESTART) && pinst->program_period > 0)
    {
        te_channel->flags |= TEC_PROGRAM_RUNNING;

        te_channel->program_counter = 0;
        te_channel->program_loop = 0;
        te_channel->program_period = pinst->program_period;
        te_channel->program_tick = 0;
    }

    te_channel->instrument = pinst;

    se_channel->waveform = pinst->waveform;
    se_channel->flags = pinst->sound_engine_flags;

    te_channel->channel_flags = pinst->flags;

    te_channel->arpeggio_note = 0;
    te_channel->fixed_note = 0xffff;

    note += (uint16_t)(((int16_t)pinst->base_note - MIDDLE_C) << 8);
    tracker_engine_set_note(tracker_engine, chan, note + (int16_t)pinst->finetune, true);

    te_channel->last_note = te_channel->target_note = note + (int16_t)pinst->finetune;

    if (pinst->sound_engine_flags & SE_ENABLE_KEYDOWN_SYNC)
    {
        te_channel->vibrato_position = te_channel->pwm_position = 0;
        se_channel->accumulator = 0;
        se_channel->lfsr = RANDOM_SEED;
    }

    if (pinst->flags & TE_SET_CUTOFF)
    {
        te_channel->filter_cutoff = ((uint16_t)pinst->filter_cutoff << 3);
        te_channel->filter_resonance = ((uint16_t)pinst->filter_resonance << 5);

        sound_engine_filter_set_coeff(&se_channel->filter, te_channel->filter_cutoff, te_channel->filter_resonance);
    }

    if (pinst->sound_engine_flags & SE_ENABLE_FILTER)
    {
        te_channel->filter_type = pinst->filter_type;
        se_channel->filter_mode = te_channel->filter_type;
        se_channel->filter_cutoff = te_channel->filter_cutoff;
        se_channel->filter_resonace = te_channel->filter_resonance;
    }

    if (pinst->flags & TE_SET_PW)
    {
        te_channel->pw = (pinst->pw << 4);
        se_channel->pw = (pinst->pw << 4);
    }

    se_channel->ring_mod = pinst->ring_mod;
    se_channel->hard_sync = pinst->hard_sync;

    te_channel->slide_speed = 0;

    se_channel->adsr.a = pinst->adsr.a;
    se_channel->adsr.d = pinst->adsr.d;
    se_channel->adsr.s = pinst->adsr.s;
    se_channel->adsr.r = pinst->adsr.r;
    se_channel->adsr.volume = pinst->adsr.volume;

    te_channel->volume = pinst->adsr.volume;

    sound_engine_enable_gate(tracker_engine->sound_engine, &tracker_engine->sound_engine->channel[chan], true);
}

void tracker_engine_execute_track_command(TrackerEngine *tracker_engine, uint8_t chan, TrackerSongPatternStep *step, bool first_tick)
{
    UNUSED(first_tick);
    UNUSED(tracker_engine);
    UNUSED(chan);

    uint8_t vol = tracker_engine_get_volume(step);

    if (vol != MUS_NOTE_VOLUME_NONE)
    {
        tracker_engine->sound_engine->channel[chan].adsr.volume = (int32_t)tracker_engine->sound_engine->channel[chan].adsr.volume * (int32_t)tracker_engine->channel[chan].volume / MAX_ADSR_VOLUME * (int32_t)vol / (MUS_NOTE_VOLUME_NONE - 1);
    }

    // TODO: add actual big ass function that executes commands; add arpeggio commands there
}

void tracker_engine_advance_channel(TrackerEngine *tracker_engine, uint8_t chan)
{
    SoundEngineChannel *se_channel = &tracker_engine->sound_engine->channel[chan];
    TrackerEngineChannel *te_channel = &tracker_engine->channel[chan];

    if (te_channel->flags & TEC_PLAYING)
    {
        if (!(se_channel->flags & SE_ENABLE_GATE))
        {
            te_channel->flags &= ~(TEC_PLAYING);
        }

        if (te_channel->slide_speed != 0)
        {
            if (te_channel->target_note > te_channel->note)
            {
                te_channel->target_note += fmin(te_channel->slide_speed, te_channel->target_note - te_channel->note);
            }

            else if (te_channel->target_note < te_channel->note)
            {
                te_channel->target_note -= fmin(te_channel->slide_speed, te_channel->note - te_channel->target_note);
            }
        }

        // TODO: add instrument program execution

        // TODO: add PWM and vibrato execution
        uint16_t vib = 0;
        int32_t chn_note = (te_channel->fixed_note != 0xffff ? te_channel->fixed_note : te_channel->note) + vib + ((int16_t)te_channel->arpeggio_note << 8);

        if (chn_note < 0)
        {
            chn_note = 0;
        }

        if (chn_note > ((12 * 7 + 11) << 8))
        {
            chn_note = ((12 * 7 + 11) << 8); // highest note is B-7
        }

        tracker_engine_set_note(tracker_engine, chan, (uint16_t)chn_note, false);
    }
}

void tracker_engine_advance_tick(TrackerEngine *tracker_engine)
{
    if (!(tracker_engine->playing))
        return;

    if (!(tracker_engine->sound_engine))
        return;

    TrackerSong *song = tracker_engine->song;

    for (int chan = 0; chan < SONG_MAX_CHANNELS; ++chan)
    {
        SoundEngineChannel *se_channel = &tracker_engine->sound_engine->channel[chan];
        TrackerEngineChannel *te_channel = &tracker_engine->channel[chan];

        if (tracker_engine->song)
        {
            uint16_t sequence_position = tracker_engine->sequence_position;
            uint8_t current_pattern = song->sequence.sequence_step[sequence_position].pattern_indices[chan];
            uint8_t pattern_step = tracker_engine->pattern_position;

            TrackerSongPattern *pattern = &song->pattern[current_pattern];

            uint8_t note_delay = 0; // TODO: add note delay command

            if (tracker_engine->current_tick == note_delay)
            {
                uint8_t note = tracker_engine_get_note(&pattern->step[pattern_step]);
                uint8_t inst = tracker_engine_get_instrument(&pattern->step[pattern_step]);

                Instrument *pinst = NULL;

                if (inst == MUS_NOTE_INSTRUMENT_NONE)
                {
                    pinst = te_channel->instrument;
                }

                else
                {
                    if (inst < song->num_instruments)
                    {
                        pinst = song->instrument[inst];
                        te_channel->instrument = pinst;
                    }
                }

                // TODO: add note cut command

                if (note == MUS_NOTE_RELEASE)
                {
                    sound_engine_enable_gate(tracker_engine->sound_engine, se_channel, 0);
                }

                else if (pinst && note != MUS_NOTE_RELEASE && note != MUS_NOTE_CUT && note != MUS_NOTE_NONE)
                {
                    te_channel->slide_speed = 0;

                    // TODO: add setting slide speed if slide command is there

                    // te_channel->slide_speed = pinst->slide_speed;

                    uint8_t prev_adsr_volume = se_channel->adsr.volume;

                    tracker_engine_trigger_instrument_internal(tracker_engine, chan, pinst, note << 8);
                    te_channel->note = (note << 8) + pinst->finetune;

                    te_channel->target_note = (note << 8) + pinst->finetune;

                    if (inst == MUS_NOTE_INSTRUMENT_NONE)
                    {
                        se_channel->adsr.volume = prev_adsr_volume;
                    }
                }
            }

            tracker_engine_execute_track_command(tracker_engine, chan, &pattern->step[pattern_step], tracker_engine->current_tick == note_delay);
        }

        tracker_engine_advance_channel(tracker_engine, chan); // this will be executed even if the song pointer is NULL; handy for live instrument playback from inst editor ("jams")
    }

    if (tracker_engine->song)
    {
        tracker_engine->current_tick++;

        if (tracker_engine->current_tick >= song->speed)
        {
            // TODO: add pattern loop and pattern skip commands

            tracker_engine->pattern_position++;

            tracker_engine->current_tick = 0;

            if (tracker_engine->pattern_position >= song->pattern_length)
            {
                tracker_engine->pattern_position = 0;

                tracker_engine->sequence_position++;

                if (tracker_engine->sequence_position >= song->num_sequence_steps)
                {
                    tracker_engine->playing = false; // TODO: add song loop handling
                    tracker_engine->sequence_position--;
                    tracker_engine->pattern_position = song->pattern_length - 1;
                }
            }
        }
    }
}