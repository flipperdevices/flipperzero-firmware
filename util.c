#include "util.h"

void set_note(TrackerSongPatternStep* step, uint8_t note)
{
	step->note &= 0x80;
	step->note |= (note & 0x7f);
}

void set_instrument(TrackerSongPatternStep* step, uint8_t inst)
{
	step->note &= 0x7f;
	step->inst_vol &= 0x0f;

	step->note |= ((inst & 0x10) << 3);
	step->inst_vol |= ((inst & 0xf) << 4);
}

void set_volume(TrackerSongPatternStep* step, uint8_t vol)
{
	step->command &= 0x7fff;
	step->inst_vol &= 0xf0;

	step->command |= ((vol & 0x10) << 11);
	step->inst_vol |= (vol & 0xf);
}

void set_command(TrackerSongPatternStep* step, uint16_t command)
{
	step->command &= 0x8000;
	step->command |= command & (0x7fff);
}

void play_song(FlizzerTrackerApp* tracker, bool from_cursor)
{
	tracker->tracker_engine.playing = true;
	tracker->was_editing = tracker->editing;
	tracker->editing = false;

	if(!(from_cursor))
	{
		tracker->tracker_engine.pattern_position = 0;
	}

	play();
}

void stop_song(FlizzerTrackerApp* tracker)
{
	tracker->tracker_engine.playing = false;
	tracker->editing = tracker->was_editing;
	stop();
}