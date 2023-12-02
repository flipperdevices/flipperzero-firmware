#include "instrument.h"
#include "songinfo.h"

extern void return_from_keyboard_callback(void* ctx);

void edit_instrument_param(FlizzerTrackerApp* tracker, uint8_t selected_param, int8_t delta) {
    if(!(tracker->current_digit)) {
        delta *= 16;
    }

    Instrument* inst = tracker->song.instrument[tracker->current_instrument];

    switch(selected_param) {
    case INST_CURRENTINSTRUMENT: {
        int16_t inst = tracker->current_instrument;

        int8_t inst_delta = delta > 0 ? 1 : -1;

        inst += inst_delta;

        clamp(inst, 0, 0, tracker->song.num_instruments);

        if(check_and_allocate_instrument(&tracker->song, (uint8_t)inst)) {
            tracker->current_instrument = inst;
        }

        break;
    }

    case INST_INSTRUMENTNAME: {
        text_input_set_header_text(tracker->text_input, "Instrument name:");
        text_input_set_result_callback(
            tracker->text_input,
            return_from_keyboard_callback,
            tracker,
            (char*)&inst->name,
            MUS_INST_NAME_LEN + 1,
            false);

        view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_KEYBOARD);
        break;
    }

    case INST_CURRENT_NOTE: {
        int8_t note_delta = 0;

        if(delta < 0) {
            if(tracker->current_digit) {
                note_delta = -12;
            }

            else {
                note_delta = -1;
            }
        }

        if(delta > 0) {
            if(tracker->current_digit) {
                note_delta = 12;
            }

            else {
                note_delta = 1;
            }
        }

        clamp(inst->base_note, note_delta, 0, MAX_NOTE);

        break;
    }

    case INST_FINETUNE: {
        int8_t fine_delta = 0;

        if(delta < 0) {
            if(tracker->current_digit) {
                fine_delta = -1;
            }

            else {
                fine_delta = -10;
            }
        }

        if(delta > 0) {
            if(tracker->current_digit) {
                fine_delta = 1;
            }

            else {
                fine_delta = 10;
            }
        }

        inst->finetune += fine_delta;

        break;
    }

    case INST_SLIDESPEED: {
        if((int16_t)inst->slide_speed + (int16_t)delta >= 0 &&
           (int16_t)inst->slide_speed + (int16_t)delta <= 0xff) {
            inst->slide_speed += delta;
        }

        break;
    }

    case INST_SETPW: {
        flipbit(inst->flags, TE_SET_PW);
        break;
    }

    case INST_PW: {
        if((int16_t)inst->pw + (int16_t)delta >= 0 && (int16_t)inst->pw + (int16_t)delta <= 0xff) {
            inst->pw += delta;
        }

        break;
    }

    case INST_SETCUTOFF: {
        flipbit(inst->flags, TE_SET_CUTOFF);
        break;
    }

    case INST_WAVE_NOISE: {
        flipbit(inst->waveform, SE_WAVEFORM_NOISE);
        break;
    }

    case INST_WAVE_PULSE: {
        flipbit(inst->waveform, SE_WAVEFORM_PULSE);
        break;
    }

    case INST_WAVE_TRIANGLE: {
        flipbit(inst->waveform, SE_WAVEFORM_TRIANGLE);
        break;
    }

    case INST_WAVE_SAWTOOTH: {
        flipbit(inst->waveform, SE_WAVEFORM_SAW);
        break;
    }

    case INST_WAVE_NOISE_METAL: {
        flipbit(inst->waveform, SE_WAVEFORM_NOISE_METAL);
        break;
    }

    case INST_WAVE_SINE: {
        flipbit(inst->waveform, SE_WAVEFORM_SINE);
        break;
    }

    case INST_ATTACK: {
        if((int16_t)inst->adsr.a + (int16_t)delta >= 0 &&
           (int16_t)inst->adsr.a + (int16_t)delta <= 0xff) {
            inst->adsr.a += delta;
        }

        break;
    }

    case INST_DECAY: {
        if((int16_t)inst->adsr.d + (int16_t)delta >= 0 &&
           (int16_t)inst->adsr.d + (int16_t)delta <= 0xff) {
            inst->adsr.d += delta;
        }

        break;
    }

    case INST_SUSTAIN: {
        if((int16_t)inst->adsr.s + (int16_t)delta >= 0 &&
           (int16_t)inst->adsr.s + (int16_t)delta <= 0xff) {
            inst->adsr.s += delta;
        }

        break;
    }

    case INST_RELEASE: {
        if((int16_t)inst->adsr.r + (int16_t)delta >= 0 &&
           (int16_t)inst->adsr.r + (int16_t)delta <= 0xff) {
            inst->adsr.r += delta;
        }

        break;
    }

    case INST_VOLUME: {
        if((int16_t)inst->adsr.volume + (int16_t)delta >= 0 &&
           (int16_t)inst->adsr.volume + (int16_t)delta <= 0xff) {
            inst->adsr.volume += delta;
        }

        break;
    }

    case INST_ENABLEFILTER: {
        flipbit(inst->sound_engine_flags, SE_ENABLE_FILTER);
        break;
    }

    case INST_FILTERCUTOFF: {
        if((int16_t)inst->filter_cutoff + (int16_t)delta >= 0 &&
           (int16_t)inst->filter_cutoff + (int16_t)delta <= 0xff) {
            inst->filter_cutoff += delta;
        }

        break;
    }

    case INST_FILTERRESONANCE: {
        if((int16_t)inst->filter_resonance + (int16_t)delta >= 0 &&
           (int16_t)inst->filter_resonance + (int16_t)delta <= 0xff) {
            inst->filter_resonance += delta;
        }

        break;
    }

    case INST_FILTERTYPE: {
        int8_t flt_delta = (delta > 0 ? 1 : -1);

        if((int16_t)inst->filter_type + (int16_t)flt_delta >= 0 &&
           (int16_t)inst->filter_type + (int16_t)flt_delta < FIL_MODES) {
            inst->filter_type += flt_delta;
        }

        break;
    }

    case INST_ENABLERINGMOD: {
        flipbit(inst->sound_engine_flags, SE_ENABLE_RING_MOD);
        break;
    }

    case INST_RINGMODSRC: {
        if((int16_t)inst->ring_mod + (int16_t)delta >= 0 &&
           (int16_t)inst->ring_mod + (int16_t)delta < SONG_MAX_CHANNELS) {
            inst->ring_mod += delta;
        }

        if((int16_t)inst->ring_mod + (int16_t)delta < 0) {
            inst->ring_mod = 0xff; // 0xff = self
        }

        if((int16_t)inst->ring_mod == 0xff && (int16_t)delta > 0) {
            inst->ring_mod = 0;
        }

        break;
    }

    case INST_ENABLEHARDSYNC: {
        flipbit(inst->sound_engine_flags, SE_ENABLE_HARD_SYNC);
        break;
    }

    case INST_HARDSYNCSRC: {
        if((int16_t)inst->hard_sync + (int16_t)delta >= 0 &&
           (int16_t)inst->hard_sync + (int16_t)delta < SONG_MAX_CHANNELS) {
            inst->hard_sync += delta;
        }

        if((int16_t)inst->hard_sync + (int16_t)delta < 0) {
            inst->hard_sync = 0xff; // 0xff = self
        }

        if((int16_t)inst->hard_sync == 0xff && (int16_t)delta > 0) {
            inst->hard_sync = 0;
        }

        break;
    }

    case INST_RETRIGGERONSLIDE: {
        flipbit(inst->flags, TE_RETRIGGER_ON_SLIDE);
        break;
    }

    case INST_ENABLEKEYSYNC: {
        flipbit(inst->sound_engine_flags, SE_ENABLE_KEYDOWN_SYNC);
        break;
    }

    case INST_ENABLEVIBRATO: {
        flipbit(inst->flags, TE_ENABLE_VIBRATO);
        break;
    }

    case INST_VIBRATOSPEED: {
        if((int16_t)inst->vibrato_speed + (int16_t)delta >= 0 &&
           (int16_t)inst->vibrato_speed + (int16_t)delta <= 0xff) {
            inst->vibrato_speed += delta;
        }

        break;
    }

    case INST_VIBRATODEPTH: {
        if((int16_t)inst->vibrato_depth + (int16_t)delta >= 0 &&
           (int16_t)inst->vibrato_depth + (int16_t)delta <= 0xff) {
            inst->vibrato_depth += delta;
        }

        break;
    }

    case INST_VIBRATODELAY: {
        if((int16_t)inst->vibrato_delay + (int16_t)delta >= 0 &&
           (int16_t)inst->vibrato_delay + (int16_t)delta <= 0xff) {
            inst->vibrato_delay += delta;
        }

        break;
    }

    case INST_ENABLEPWM: {
        flipbit(inst->flags, TE_ENABLE_PWM);
        break;
    }

    case INST_PWMSPEED: {
        if((int16_t)inst->pwm_speed + (int16_t)delta >= 0 &&
           (int16_t)inst->pwm_speed + (int16_t)delta <= 0xff) {
            inst->pwm_speed += delta;
        }

        break;
    }

    case INST_PWMDEPTH: {
        if((int16_t)inst->pwm_depth + (int16_t)delta >= 0 &&
           (int16_t)inst->pwm_depth + (int16_t)delta <= 0xff) {
            inst->pwm_depth += delta;
        }

        break;
    }

    case INST_PWMDELAY: {
        if((int16_t)inst->pwm_delay + (int16_t)delta >= 0 &&
           (int16_t)inst->pwm_delay + (int16_t)delta <= 0xff) {
            inst->pwm_delay += delta;
        }

        break;
    }

    case INST_PROGRESTART: {
        flipbit(inst->flags, TE_PROG_NO_RESTART);
        break;
    }

    case INST_PROGRAMEPERIOD: {
        if((int16_t)inst->program_period + (int16_t)delta >= 0 &&
           (int16_t)inst->program_period + (int16_t)delta <= 0xff) {
            inst->program_period += delta;
        }

        break;
    }
    }
}

void instrument_edit_event(FlizzerTrackerApp* tracker, FlizzerTrackerEvent* event) {
    if(event->input.key == InputKeyOk && event->input.type == InputTypeShort &&
       !tracker->tracker_engine.playing) {
        tracker->editing = !(tracker->editing);
        return;
    }

    if(event->input.key == InputKeyOk && event->input.type == InputTypeLong && !tracker->editing) {
        reset_buffer(&tracker->sound_engine);
        tracker_engine_set_song(&tracker->tracker_engine, NULL);

        for(int i = 1; i < SONG_MAX_CHANNELS; i++) {
            tracker->tracker_engine.channel[i].channel_flags &= TEC_PLAYING;
            tracker->tracker_engine.sound_engine->channel[i].frequency = 0;
            tracker->tracker_engine.sound_engine->channel[i].waveform = 0;
        }

        Instrument* inst = tracker->song.instrument[tracker->current_instrument];
        tracker_engine_trigger_instrument_internal(
            &tracker->tracker_engine, 0, inst, (MIDDLE_C << 8));
        tracker->tracker_engine.playing = true;
        play();
        return;
    }

    if(event->input.key == InputKeyOk && event->input.type == InputTypeRelease &&
       !tracker->editing) {
        SoundEngineChannel* se_channel = &tracker->sound_engine.channel[0];
        sound_engine_enable_gate(&tracker->sound_engine, se_channel, false);
        return;
    }

    if(event->input.key == InputKeyRight && event->input.type == InputTypeShort) {
        switch(tracker->selected_param) {
        default: {
            tracker->current_digit++;

            if(tracker->current_digit > 1) {
                tracker->selected_param++;

                tracker->current_digit = 0;

                if(tracker->selected_param > INST_PARAMS - 1) {
                    tracker->selected_param = 0;
                }
            }

            break;
        }

        case INST_CURRENTINSTRUMENT:
        case INST_INSTRUMENTNAME:
        case INST_SETPW:
        case INST_SETCUTOFF:
        case INST_WAVE_NOISE:
        case INST_WAVE_PULSE:
        case INST_WAVE_TRIANGLE:
        case INST_WAVE_SAWTOOTH:
        case INST_WAVE_NOISE_METAL:
        case INST_WAVE_SINE:
        case INST_ENABLEFILTER:
        case INST_FILTERTYPE:
        case INST_ENABLERINGMOD:
        case INST_RINGMODSRC:
        case INST_ENABLEHARDSYNC:
        case INST_HARDSYNCSRC:
        case INST_RETRIGGERONSLIDE:
        case INST_ENABLEKEYSYNC:
        case INST_ENABLEVIBRATO:
        case INST_ENABLEPWM:
        case INST_PROGRESTART: {
            tracker->selected_param++;

            tracker->current_digit = 1;

            if(tracker->selected_param > INST_PARAMS - 1) {
                tracker->selected_param = 0;
            }

            break;
        }
        }
    }

    if(event->input.key == InputKeyLeft && event->input.type == InputTypeShort) {
        switch(tracker->selected_param) {
        default: {
            tracker->current_digit--;

            if(tracker->current_digit > 1) // unsigned int overflow
            {
                tracker->selected_param--;

                tracker->current_digit = 1;

                if(tracker->selected_param > INST_PARAMS - 1) // unsigned int overflow
                {
                    tracker->selected_param = INST_PARAMS - 1;
                }
            }

            break;
        }

        case INST_CURRENTINSTRUMENT:
        case INST_INSTRUMENTNAME:
        case INST_SETPW:
        case INST_SETCUTOFF:
        case INST_WAVE_NOISE:
        case INST_WAVE_PULSE:
        case INST_WAVE_TRIANGLE:
        case INST_WAVE_SAWTOOTH:
        case INST_WAVE_NOISE_METAL:
        case INST_WAVE_SINE:
        case INST_ENABLEFILTER:
        case INST_FILTERTYPE:
        case INST_ENABLERINGMOD:
        case INST_RINGMODSRC:
        case INST_ENABLEHARDSYNC:
        case INST_HARDSYNCSRC:
        case INST_RETRIGGERONSLIDE:
        case INST_ENABLEKEYSYNC:
        case INST_ENABLEVIBRATO:
        case INST_ENABLEPWM:
        case INST_PROGRESTART: {
            tracker->selected_param--;

            tracker->current_digit = 1;

            if(tracker->selected_param > INST_PARAMS - 1) // unsigned int overflow
            {
                tracker->selected_param = INST_PARAMS - 1;
            }

            break;
        }
        }

        return;
    }

    if(event->input.key == InputKeyDown && event->input.type == InputTypeShort) {
        if(tracker->editing) {
            edit_instrument_param(tracker, tracker->selected_param, -1);
        }

        return;
    }

    if(event->input.key == InputKeyUp && event->input.type == InputTypeShort) {
        if(tracker->editing) {
            edit_instrument_param(tracker, tracker->selected_param, 1);
        }

        return;
    }

    if(tracker->selected_param > INST_VIBRATODELAY) {
        tracker->inst_editor_shift = 6;
    }

    if(tracker->selected_param > INST_PWMDELAY) {
        tracker->inst_editor_shift = 12;
    }

    if(tracker->selected_param < INST_CURRENT_NOTE) {
        tracker->inst_editor_shift = 0;
    }
}