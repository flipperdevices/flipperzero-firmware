#include "songinfo.h"

#include <ctype.h>

void return_from_keyboard_callback(void* ctx)
{
    FlizzerTrackerApp *tracker = (FlizzerTrackerApp*)ctx;

    uint8_t string_length = 0;
    char* string = NULL;

    if(tracker->focus == EDIT_SONGINFO && tracker->mode == PATTERN_VIEW)
    {
        switch(tracker->selected_param)
        {
            case SI_SONGNAME:
            {
                string_length = MUS_SONG_NAME_LEN;
                string = (char*)&tracker->song.song_name;
                break;
            }

            case SI_INSTRUMENTNAME:
            {
                string_length = MUS_INST_NAME_LEN;
                string = (char*)&tracker->song.instrument[tracker->current_instrument]->name;
                break;
            }
        }
    }

    if(string == NULL || string_length == 0) return;

    for(uint8_t i = 0; i < string_length; i++) //I tinyfied the font by deleting lowercase chars, and I don't like the lowercase chars of any 3x5 pixels font
    {
        string[i] = toupper(string[i]);
    }
    
    view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_TRACKER);
}

void edit_songinfo_param(FlizzerTrackerApp *tracker, uint8_t selected_param, int8_t delta)
{
    if (!(tracker->current_digit))
    {
        delta *= 16;
    }

    switch (selected_param)
    {
        case SI_PATTERNPOS:
        {
            if ((int16_t)tracker->song.pattern_length + (int16_t)delta > 1 && (int16_t)tracker->song.pattern_length + (int16_t)delta <= 0xff)
            {
                tracker->song.pattern_length += delta;
            }

            break;
        }

        case SI_SEQUENCEPOS:
        {
            if ((int16_t)tracker->song.num_sequence_steps + (int16_t)delta > 1 && (int16_t)tracker->song.num_sequence_steps + (int16_t)delta <= 0xff)
            {
                tracker->song.num_sequence_steps += delta;
            }

            break;
        }

        case SI_SONGSPEED:
        {
            if ((int16_t)tracker->song.speed + (int16_t)delta > 1 && (int16_t)tracker->song.speed + (int16_t)delta <= 0xff)
            {
                tracker->song.speed += delta;
            }

            break;
        }

        case SI_SONGRATE:
        {
            if ((int16_t)tracker->song.rate + (int16_t)delta > 1 && (int16_t)tracker->song.rate + (int16_t)delta <= 0xff)
            {
                tracker->song.rate += delta;
            }

            break;
        }

        case SI_MASTERVOL:
        {
            if ((int16_t)tracker->tracker_engine.master_volume + (int16_t)delta > 0 && (int16_t)tracker->tracker_engine.master_volume + (int16_t)delta <= 0xff)
            {
                tracker->tracker_engine.master_volume += delta;
            }

            break;
        }

        case SI_SONGNAME:
        {
            text_input_set_header_text(tracker->text_input, "Song name:");
            text_input_set_result_callback(tracker->text_input, return_from_keyboard_callback, tracker, (char*)&tracker->song.song_name, MUS_SONG_NAME_LEN + 1, false);

            view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_KEYBOARD);
            break;
        }

        case SI_CURRENTINSTRUMENT:
        {
            int16_t inst = tracker->current_instrument;

            if (inst + delta >= MUS_NOTE_INSTRUMENT_NONE)
            {
                if (delta > 0)
                {
                    inst = 0;
                }

                else
                {
                    inst = MUS_NOTE_INSTRUMENT_NONE - 1;
                }
            }

            clamp(inst, delta, 0, tracker->song.num_instruments - 1);

            tracker->current_instrument = inst;

            break;
        }

        case SI_INSTRUMENTNAME:
        {
            text_input_set_header_text(tracker->text_input, "Instrument name:");
            text_input_set_result_callback(tracker->text_input, return_from_keyboard_callback, tracker, (char*)&tracker->song.instrument[tracker->current_instrument]->name, MUS_INST_NAME_LEN + 1, false);

            view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_KEYBOARD);
            break;
        }

        default:
            break;
    }
}

void songinfo_edit_event(FlizzerTrackerApp *tracker, FlizzerTrackerEvent *event)
{
    if (event->input.key == InputKeyOk && event->input.type == InputTypeShort && !tracker->tracker_engine.playing)
    {
        tracker->editing = !tracker->editing;
    }

    if (event->input.key == InputKeyRight && event->input.type == InputTypeShort && tracker->editing)
    {
        switch(tracker->selected_param)
        {
            default:
            {
                tracker->current_digit++;

                if (tracker->current_digit > 1)
                {
                    tracker->selected_param++;

                    tracker->current_digit = 0;

                    if (tracker->selected_param > SI_PARAMS - 1)
                    {
                        tracker->selected_param = 0;
                    }
                }

                break;
            }

            case SI_CURRENTINSTRUMENT:
            case SI_SONGNAME:
            case SI_INSTRUMENTNAME:
            {
                tracker->selected_param++;

                tracker->current_digit = 0;

                if (tracker->selected_param > SI_PARAMS - 1)
                {
                    tracker->selected_param = 0;
                }

                break;
            }
        }
    }

    if (event->input.key == InputKeyLeft && event->input.type == InputTypeShort && tracker->editing)
    {
        switch(tracker->selected_param)
        {
            default:
            {
                tracker->current_digit--;

                if (tracker->current_digit > 1) // unsigned int overflow
                {
                    tracker->selected_param--;

                    tracker->current_digit = 1;

                    if (tracker->selected_param > SI_PARAMS - 1) // unsigned int overflow
                    {
                        tracker->selected_param = SI_PARAMS - 1;
                    }
                }

                break;
            }

            case SI_CURRENTINSTRUMENT:
            case SI_SONGNAME:
            case SI_INSTRUMENTNAME:
            {
                tracker->selected_param--;

                tracker->current_digit = 0;

                if (tracker->selected_param > SI_PARAMS - 1) // unsigned int overflow
                {
                    tracker->selected_param = SI_PARAMS - 1;
                }

                break;
            }
        }
    }

    if (event->input.key == InputKeyDown && event->input.type == InputTypeShort)
    {
        if (tracker->editing)
        {
            edit_songinfo_param(tracker, tracker->selected_param, -1);
        }
    }

    if (event->input.key == InputKeyUp && event->input.type == InputTypeShort)
    {
        if (tracker->editing)
        {
            edit_songinfo_param(tracker, tracker->selected_param, 1);
        }
    }
}