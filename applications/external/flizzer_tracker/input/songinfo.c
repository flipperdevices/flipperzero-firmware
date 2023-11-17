#include "songinfo.h"

#include "../diskop.h"

extern void return_from_keyboard_callback(void* ctx);

void edit_songinfo_param(FlizzerTrackerApp* tracker, uint8_t selected_param, int8_t delta) {
    if(!(tracker->current_digit)) {
        delta *= 16;
    }

    switch(selected_param) {
    case SI_PATTERNPOS: {
        uint16_t new_length = tracker->song.pattern_length;

        if((int16_t)new_length + (int16_t)delta > 0 &&
           (int16_t)new_length + (int16_t)delta <= 0x100) {
            new_length += delta;
            change_pattern_length(&tracker->song, new_length);

            if(tracker->tracker_engine.pattern_position >= new_length) {
                tracker->tracker_engine.pattern_position = new_length - 1;
            }
        }

        break;
    }

    case SI_SEQUENCEPOS: {
        if((int16_t)tracker->song.num_sequence_steps + (int16_t)delta > 0 &&
           (int16_t)tracker->song.num_sequence_steps + (int16_t)delta <= 0x100) {
            tracker->song.num_sequence_steps += delta;

            if(tracker->tracker_engine.sequence_position >= tracker->song.num_sequence_steps) {
                tracker->tracker_engine.sequence_position = tracker->song.num_sequence_steps - 1;
            }
        }

        break;
    }

    case SI_SONGSPEED: {
        if((int16_t)tracker->song.speed + (int16_t)delta > 1 &&
           (int16_t)tracker->song.speed + (int16_t)delta <= 0xff) {
            tracker->song.speed += delta;
        }

        break;
    }

    case SI_SONGRATE: {
        if((int16_t)tracker->song.rate + (int16_t)delta > 1 &&
           (int16_t)tracker->song.rate + (int16_t)delta <= 0xff) {
            tracker->song.rate += delta;
        }

        break;
    }

    case SI_MASTERVOL: {
        if((int16_t)tracker->tracker_engine.master_volume + (int16_t)delta > 0 &&
           (int16_t)tracker->tracker_engine.master_volume + (int16_t)delta <= 0xff) {
            tracker->tracker_engine.master_volume += delta;
        }

        break;
    }

    case SI_SONGNAME: {
        text_input_set_header_text(tracker->text_input, "Song name:");
        text_input_set_result_callback(
            tracker->text_input,
            return_from_keyboard_callback,
            tracker,
            (char*)&tracker->song.song_name,
            MUS_SONG_NAME_LEN + 1,
            false);

        view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_KEYBOARD);
        break;
    }

    case SI_CURRENTINSTRUMENT: {
        int16_t inst = tracker->current_instrument;

        int8_t inst_delta = delta > 0 ? 1 : -1;

        inst += inst_delta;

        clamp(inst, 0, 0, tracker->song.num_instruments - 1);

        tracker->current_instrument = inst;

        break;
    }

    case SI_INSTRUMENTNAME: {
        text_input_set_header_text(tracker->text_input, "Instrument name:");
        text_input_set_result_callback(
            tracker->text_input,
            return_from_keyboard_callback,
            tracker,
            (char*)&tracker->song.instrument[tracker->current_instrument]->name,
            MUS_INST_NAME_LEN + 1,
            false);

        view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_KEYBOARD);
        break;
    }

    default:
        break;
    }
}

void songinfo_edit_event(FlizzerTrackerApp* tracker, FlizzerTrackerEvent* event) {
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
        switch(tracker->selected_param) {
        default: {
            tracker->current_digit++;

            if(tracker->current_digit > 1) {
                tracker->selected_param++;

                tracker->current_digit = 0;

                if(tracker->selected_param > SI_PARAMS - 1) {
                    tracker->selected_param = 0;
                }
            }

            break;
        }

        case SI_CURRENTINSTRUMENT:
        case SI_SONGNAME:
        case SI_INSTRUMENTNAME: {
            tracker->selected_param++;

            tracker->current_digit = 0;

            if(tracker->selected_param > SI_PARAMS - 1) {
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

                if(tracker->selected_param > SI_PARAMS - 1) // unsigned int overflow
                {
                    tracker->selected_param = SI_PARAMS - 1;
                }
            }

            break;
        }

        case SI_CURRENTINSTRUMENT:
        case SI_SONGNAME:
        case SI_INSTRUMENTNAME: {
            tracker->selected_param--;

            tracker->current_digit = 0;

            if(tracker->selected_param > SI_PARAMS - 1) // unsigned int overflow
            {
                tracker->selected_param = SI_PARAMS - 1;
            }

            break;
        }
        }
    }

    if(event->input.key == InputKeyDown && event->input.type == InputTypeShort) {
        if(tracker->editing) {
            edit_songinfo_param(tracker, tracker->selected_param, -1);
        }
    }

    if(event->input.key == InputKeyUp && event->input.type == InputTypeShort) {
        if(tracker->editing) {
            edit_songinfo_param(tracker, tracker->selected_param, 1);
        }
    }
}