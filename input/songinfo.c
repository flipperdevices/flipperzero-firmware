#include "songinfo.h"

#include <gui/modules/text_input.h>

void edit_songinfo_param(FlizzerTrackerApp* tracker, uint8_t selected_param, int8_t delta)
{
	if(!(tracker->current_digit))
	{
		delta *= 16;
	}

	switch(selected_param)
	{
		case SI_PATTERNPOS:
		{
			if((int16_t)tracker->song.pattern_length + (int16_t)delta > 1 && (int16_t)tracker->song.pattern_length + (int16_t)delta <= 0xff)
			{
				tracker->song.pattern_length += delta;
			}

			break;
		}

		case SI_SEQUENCEPOS:
		{
			if((int16_t)tracker->song.num_sequence_steps + (int16_t)delta > 1 && (int16_t)tracker->song.num_sequence_steps + (int16_t)delta <= 0xff)
			{
				tracker->song.num_sequence_steps += delta;
			}

			break;
		}

		case SI_SONGSPEED:
		{
			if((int16_t)tracker->song.speed + (int16_t)delta > 1 && (int16_t)tracker->song.speed + (int16_t)delta <= 0xff)
			{
				tracker->song.speed += delta;
			}

			break;
		}

		case SI_SONGRATE:
		{
			if((int16_t)tracker->song.rate + (int16_t)delta > 1 && (int16_t)tracker->song.rate + (int16_t)delta <= 0xff)
			{
				tracker->song.rate += delta;
			}

			break;
		}

		case SI_MASTERVOL:
		{
			if((int16_t)tracker->tracker_engine.master_volume + (int16_t)delta > 0 && (int16_t)tracker->tracker_engine.master_volume + (int16_t)delta <= 0xff)
			{
				tracker->tracker_engine.master_volume += delta;
			}

			break;
		}

		case SI_SONGNAME:
		{
			break;
		}

		default: break;
	}
}

void songinfo_edit_event(FlizzerTrackerApp* tracker, FlizzerTrackerEvent* event)
{
	if(event->input.key == InputKeyOk && event->input.type == InputTypeShort && !tracker->tracker_engine.playing)
	{
		tracker->editing = !tracker->editing;
	}

	if(event->input.key == InputKeyRight && event->input.type == InputTypeShort && tracker->editing)
	{
		tracker->current_digit++;

		if(tracker->current_digit > 1)
		{
			tracker->selected_param++;

			tracker->current_digit = 0;

			if(tracker->selected_param > SI_PARAMS - 1)
			{
				tracker->selected_param = 0;
			}
		}
	}

	if(event->input.key == InputKeyLeft && event->input.type == InputTypeShort && tracker->editing)
	{
		tracker->current_digit--;

		if(tracker->current_digit > 1) //unsigned int overflow
		{
			tracker->selected_param--;

			tracker->current_digit = 1;

			if(tracker->selected_param > SI_PARAMS - 1) //unsigned int overflow
			{
				tracker->selected_param = SI_PARAMS - 1;
			}
		}
	}

	if(event->input.key == InputKeyDown && event->input.type == InputTypeShort)
	{
		if(tracker->editing)
		{
			edit_songinfo_param(tracker, tracker->selected_param, -1);
		}
	}

	if(event->input.key == InputKeyUp && event->input.type == InputTypeShort)
	{
		if(tracker->editing)
		{
			edit_songinfo_param(tracker, tracker->selected_param, 1);
		}
	}
}