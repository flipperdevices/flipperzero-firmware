#include "sequence.h"

void delete_sequence_step(FlizzerTrackerApp* tracker) {
    uint8_t sequence_position = tracker->tracker_engine.sequence_position;
    uint8_t* pattern = &tracker->tracker_engine.song->sequence.sequence_step[sequence_position]
                            .pattern_indices[tracker->current_channel];
    *pattern = 0;
}

void edit_sequence_step(FlizzerTrackerApp* tracker, int8_t delta) {
    uint8_t digit = tracker->current_digit;

    uint8_t sequence_position = tracker->tracker_engine.sequence_position;
    uint8_t pattern_index = tracker->tracker_engine.song->sequence.sequence_step[sequence_position]
                                .pattern_indices[tracker->current_channel];

    uint8_t* pattern = &tracker->tracker_engine.song->sequence.sequence_step[sequence_position]
                            .pattern_indices[tracker->current_channel];
    uint8_t temp_pattern = *pattern;

    switch(digit) {
    case 0: // upper nibble
    {
        int8_t nibble = ((pattern_index & 0xf0) >> 4);

        if(nibble + delta < 0) {
            nibble = 0xf;
        }

        else if(nibble + delta > 0xf) {
            nibble = 0;
        }

        else {
            nibble += delta;
        }

        temp_pattern &= 0x0f;
        temp_pattern |= (nibble << 4);

        break;
    }

    case 1: // lower nibble
    {
        int8_t nibble = (pattern_index & 0x0f);

        if(nibble + delta < 0) {
            nibble = 0xf;
        }

        else if(nibble + delta > 0xf) {
            nibble = 0;
        }

        else {
            nibble += delta;
        }

        temp_pattern &= 0xf0;
        temp_pattern |= nibble;

        break;
    }
    }

    if(check_and_allocate_pattern(&tracker->song, temp_pattern)) {
        *pattern = temp_pattern;
    }
}

void sequence_edit_event(FlizzerTrackerApp* tracker, FlizzerTrackerEvent* event) {
    if(event->input.key == InputKeyOk && event->input.type == InputTypeShort &&
       !tracker->tracker_engine.playing) {
        tracker->editing = !tracker->editing;
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
    }

    if(event->input.key == InputKeyRight && event->input.type == InputTypeShort) {
        tracker->current_digit++;

        if(tracker->current_digit > 1) {
            tracker->current_channel++;

            tracker->current_digit = 0;

            if(tracker->current_channel > SONG_MAX_CHANNELS - 1) {
                tracker->current_channel = 0;
            }
        }
    }

    if(event->input.key == InputKeyLeft && event->input.type == InputTypeShort) {
        tracker->current_digit--;

        if(tracker->current_digit > 1) // unsigned int overflow
        {
            tracker->current_channel--;

            tracker->current_digit = 1;

            if(tracker->current_channel > SONG_MAX_CHANNELS - 1) // unsigned int overflow
            {
                tracker->current_channel = SONG_MAX_CHANNELS - 1;
            }
        }
    }

    if(event->input.key == InputKeyDown && event->input.type == InputTypeShort) {
        if(!(tracker->editing)) {
            tracker->tracker_engine.sequence_position++;

            if(tracker->tracker_engine.sequence_position >=
               tracker->tracker_engine.song->num_sequence_steps) {
                tracker->tracker_engine.sequence_position = 0;
            }
        }

        if(tracker->editing) {
            edit_sequence_step(tracker, -1);
        }
    }

    if(event->input.key == InputKeyUp && event->input.type == InputTypeShort) {
        if(!(tracker->editing)) {
            int16_t temp_sequence_position = tracker->tracker_engine.sequence_position - 1;

            if(temp_sequence_position < 0) {
                tracker->tracker_engine.sequence_position =
                    tracker->tracker_engine.song->num_sequence_steps - 1;
            }

            else {
                tracker->tracker_engine.sequence_position--;
            }
        }

        if(tracker->editing) {
            edit_sequence_step(tracker, 1);
        }
    }

    if(event->input.key == InputKeyRight && event->input.type == InputTypeLong &&
       !(tracker->editing)) // set loop begin or loop end for the song
    {
        TrackerSong* song = &tracker->song;

        if(song->loop_start == song->loop_end && song->loop_end == 0) // if both are 0
        {
            song->loop_end = tracker->tracker_engine.sequence_position;
        }

        else {
            if(tracker->tracker_engine.sequence_position < song->loop_end) {
                song->loop_start = tracker->tracker_engine.sequence_position;
            }

            if(tracker->tracker_engine.sequence_position > song->loop_start) {
                song->loop_end = tracker->tracker_engine.sequence_position;
            }
        }
    }

    if(event->input.key == InputKeyLeft && event->input.type == InputTypeLong &&
       !(tracker->editing)) // erase loop begin and loop end points
    {
        TrackerSong* song = &tracker->song;

        song->loop_start = song->loop_end = 0;
    }

    if(event->input.key == InputKeyUp && event->input.type == InputTypeLong &&
       !(tracker->editing)) // jump to the beginning
    {
        tracker->tracker_engine.sequence_position = 0;
    }

    if(event->input.key == InputKeyDown && event->input.type == InputTypeLong &&
       !(tracker->editing)) // jump to the end
    {
        tracker->tracker_engine.sequence_position = tracker->song.num_sequence_steps - 1;
    }

    if(event->input.key == InputKeyBack && event->input.type == InputTypeShort &&
       tracker->editing) {
        delete_sequence_step(tracker);
    }
}