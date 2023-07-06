#include "pattern.h"

uint8_t get_field(uint8_t patternx) {
    uint8_t field = 0;

    if(patternx <= 1) field = 0;
    if(patternx == 2) field = 1;
    if(patternx == 3) field = 2;
    if(patternx > 3) field = 3;

    return field;
}

void edit_note(
    FlizzerTrackerApp* tracker,
    TrackerSongPatternStep* step,
    int8_t delta) // here we need data about last note if we place a new note
{
    int16_t note = tracker_engine_get_note(step);

    if(note == MUS_NOTE_RELEASE) {
        if(delta < 0) {
            set_note(step, MUS_NOTE_CUT);
        }

        return;
    }

    if(note == MUS_NOTE_CUT) {
        if(delta > 0) {
            set_note(step, MUS_NOTE_RELEASE);
        }

        return;
    }

    if(note == MUS_NOTE_NONE) {
        note =
            tracker->current_note; // remember which note we entered earlier and use it as reference
    }

    clamp(note, delta, 0, MAX_NOTE);

    set_note(step, (uint8_t)note);
    set_instrument(step, tracker->current_instrument);

    tracker->current_note = (uint8_t)note;
}

void edit_instrument(FlizzerTrackerApp* tracker, TrackerSongPatternStep* step, int8_t delta) {
    int16_t inst = tracker_engine_get_instrument(step);

    if(inst == MUS_NOTE_INSTRUMENT_NONE) {
        if(delta > 0) {
            inst = tracker->current_instrument;
        }

        else {
            inst = MUS_NOTE_INSTRUMENT_NONE - 1;
        }
    }

    clamp(inst, delta, 0, tracker->song.num_instruments - 1);
    tracker->current_instrument = inst; // remember last instrument
    set_instrument(step, (uint8_t)inst);
}

void edit_volume(FlizzerTrackerApp* tracker, TrackerSongPatternStep* step, int8_t delta) {
    int16_t vol = tracker_engine_get_volume(step);

    vol = tracker->current_volume;

    if(vol + delta < 0) {
        vol = MUS_NOTE_VOLUME_NONE - 1 - delta;
    }

    if(vol + delta >= MUS_NOTE_VOLUME_NONE) {
        vol = 0 - delta;
    }

    clamp(vol, delta, 0, MUS_NOTE_VOLUME_NONE - 1);

    set_volume(step, (uint8_t)vol);

    tracker->current_volume = vol;
}

void edit_command(TrackerSongPatternStep* step, uint8_t digit, int8_t delta) {
    int32_t command = tracker_engine_get_command(step);

    switch(digit) {
    case 0: // upper 7 bits
    {
        int16_t fx_name = ((command & 0x7f00) >> 8);

        if(fx_name + delta > 35) // loop
        { // 0-9 and then A-Z
            fx_name = 0;
        }

        else if(fx_name + delta < 0) {
            fx_name = 35;
        }

        else {
            fx_name += delta;
        }

        command &= 0x00ff;

        command |= (fx_name << 8);

        set_command(step, (uint16_t)command);

        break;
    }

    case 1: // upper digit of command param
    {
        int8_t upper_digit = ((command & 0x00f0) >> 4);

        if(upper_digit + delta > 0xf) // loop
        {
            upper_digit = 0;
        }

        else if(upper_digit + delta < 0) {
            upper_digit = 0xf;
        }

        else {
            upper_digit += delta;
        }

        command &= 0xff0f;

        command |= (upper_digit << 4);

        set_command(step, (uint16_t)command);

        break;
    }

    case 2: // lower digit of command param
    {
        int8_t lower_digit = (command & 0x000f);

        if(lower_digit + delta > 0xf) // loop
        {
            lower_digit = 0;
        }

        else if(lower_digit + delta < 0) {
            lower_digit = 0xf;
        }

        else {
            lower_digit += delta;
        }

        command &= 0xfff0;

        command |= lower_digit;

        set_command(step, (uint16_t)command);

        break;
    }

    default:
        break;
    }
}

void delete_field(TrackerSongPatternStep* step, uint8_t field) {
    switch(field) {
    case 0: // note
    {
        set_note(step, MUS_NOTE_NONE);
        set_instrument(step, MUS_NOTE_INSTRUMENT_NONE); // also delete instrument
        break;
    }

    case 1: // instrument
    {
        set_instrument(step, MUS_NOTE_INSTRUMENT_NONE);
        break;
    }

    case 2: // volume
    {
        set_volume(step, MUS_NOTE_VOLUME_NONE);
        break;
    }

    case 3: // command
    {
        set_command(step, 0);
        break;
    }

    default:
        break;
    }
}

void edit_pattern_step(FlizzerTrackerApp* tracker, TrackerSongPatternStep* step, int8_t delta) {
    switch(get_field(tracker->patternx)) {
    case 0: // note
    {
        if(tracker->patternx) // editing octave
        {
            edit_note(tracker, step, 12 * delta);
        }

        else // editing note
        {
            edit_note(tracker, step, delta);
        }

        break;
    }

    case 1: // instrument
    {
        edit_instrument(tracker, step, delta);
        break;
    }

    case 2: // volume
    {
        edit_volume(tracker, step, delta);
        break;
    }

    case 3: // command
    {
        uint8_t digit = 0;
        if(tracker->patternx == 4) digit = 0;
        if(tracker->patternx == 5) digit = 1;
        if(tracker->patternx == 6) digit = 2;
        edit_command(step, digit, delta);
        break;
    }

    default:
        break;
    }
}

void pattern_edit_event(FlizzerTrackerApp* tracker, FlizzerTrackerEvent* event) {
    if(event->input.key == InputKeyLeft && event->input.type == InputTypeLong &&
       !(tracker->editing)) {
        flipbit(
            tracker->tracker_engine.channel[tracker->current_channel].channel_flags, TEC_DISABLED);
        return;
    }

    if(event->input.key == InputKeyDown && event->input.type == InputTypeLong &&
       !(tracker->editing)) {
        tracker->tracker_engine.pattern_position =
            tracker->tracker_engine.song->pattern_length - 1; // go to pattern last row
        return;
    }

    if(event->input.key == InputKeyUp && event->input.type == InputTypeLong &&
       !(tracker->editing)) {
        tracker->tracker_engine.pattern_position = 0; // return to pattern 1st row
        return;
    }

    uint8_t sequence_position = tracker->tracker_engine.sequence_position;
    uint8_t current_pattern =
        tracker->tracker_engine.song->sequence.sequence_step[sequence_position]
            .pattern_indices[tracker->current_channel];
    uint16_t pattern_step = tracker->tracker_engine.pattern_position;

    uint16_t pattern_length = tracker->tracker_engine.song->pattern_length;

    TrackerSongPattern* pattern = &tracker->tracker_engine.song->pattern[current_pattern];

    TrackerSongPatternStep* step = NULL;

    if(pattern_step < pattern_length) {
        step = &pattern->step[pattern_step];
    }

    if(!(step)) return;

    if(event->input.key == InputKeyOk && event->input.type == InputTypeShort &&
       !tracker->tracker_engine.playing) {
        tracker->editing = !tracker->editing;

        if(tracker->editing) {
            // stop_song(tracker);
        }
    }

    if(event->input.key == InputKeyOk && event->input.type == InputTypeLong) {
        if(!(tracker->editing)) {
            if(tracker->tracker_engine.playing) {
                stop_song(tracker);
            }

            else {
                if(tracker->tracker_engine.pattern_position == tracker->song.pattern_length - 1 &&
                   tracker->tracker_engine.sequence_position ==
                       tracker->song.num_sequence_steps -
                           1) // if we are at the very end of the song
                {
                    stop_song(tracker);
                }

                else {
                    play_song(tracker, true);
                }
            }
        }

        else {
            if(get_field(tracker->patternx) == 0) {
                set_note(step, MUS_NOTE_RELEASE);
            }
        }
    }

    if(event->input.key == InputKeyRight && event->input.type == InputTypeShort) {
        tracker->patternx++;

        if(tracker->patternx > MAX_PATTERNX - 1) {
            tracker->current_channel++;

            tracker->patternx = 0;

            if(tracker->current_channel > SONG_MAX_CHANNELS - 1) {
                tracker->current_channel = 0;
            }
        }
    }

    if(event->input.key == InputKeyLeft && event->input.type == InputTypeShort) {
        tracker->patternx--;

        if(tracker->patternx > MAX_PATTERNX - 1) // unsigned int overflow
        {
            tracker->current_channel--;

            tracker->patternx = MAX_PATTERNX - 1;

            if(tracker->current_channel > SONG_MAX_CHANNELS - 1) // unsigned int overflow
            {
                tracker->current_channel = SONG_MAX_CHANNELS - 1;
            }
        }
    }

    if(event->input.key == InputKeyDown && event->input.type == InputTypeShort) {
        if(!(tracker->editing)) {
            tracker->tracker_engine.pattern_position++;

            if(tracker->tracker_engine.pattern_position >
                   tracker->tracker_engine.song->pattern_length - 1 &&
               tracker->tracker_engine.sequence_position <
                   tracker->tracker_engine.song->num_sequence_steps - 1) {
                tracker->tracker_engine.pattern_position = 0;
                tracker->tracker_engine.sequence_position++;
            }

            else if(
                tracker->tracker_engine.pattern_position >
                tracker->tracker_engine.song->pattern_length - 1) {
                tracker->tracker_engine.pattern_position =
                    tracker->tracker_engine.song->pattern_length - 1;
            }
        }

        if(tracker->editing) {
            edit_pattern_step(tracker, step, -1);
        }
    }

    if(event->input.key == InputKeyUp && event->input.type == InputTypeShort) {
        if(!(tracker->editing)) {
            int16_t temp_pattern_position = tracker->tracker_engine.pattern_position - 1;

            if(temp_pattern_position < 0) {
                if(tracker->tracker_engine.sequence_position > 0) {
                    tracker->tracker_engine.sequence_position--;
                    tracker->tracker_engine.pattern_position =
                        tracker->tracker_engine.song->pattern_length - 1;
                }
            }

            else {
                tracker->tracker_engine.pattern_position--;
            }
        }

        if(tracker->editing) {
            edit_pattern_step(tracker, step, 1);
        }
    }

    if(event->input.key == InputKeyBack && event->input.type == InputTypeShort &&
       tracker->editing) {
        uint8_t field = get_field(tracker->patternx);

        delete_field(step, field);
    }
}