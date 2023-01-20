#include "util.h"

void set_note(TrackerSongPatternStep *step, uint8_t note)
{
    step->note &= 0x80;
    step->note |= (note & 0x7f);
}

void set_instrument(TrackerSongPatternStep *step, uint8_t inst)
{
    step->note &= 0x7f;
    step->inst_vol &= 0x0f;

    step->note |= ((inst & 0x10) << 3);
    step->inst_vol |= ((inst & 0xf) << 4);
}

void set_volume(TrackerSongPatternStep *step, uint8_t vol)
{
    step->command &= 0x7fff;
    step->inst_vol &= 0xf0;

    step->command |= ((vol & 0x10) << 11);
    step->inst_vol |= (vol & 0xf);
}

void set_command(TrackerSongPatternStep *step, uint16_t command)
{
    step->command &= 0x8000;
    step->command |= command & (0x7fff);
}

void play_song(FlizzerTrackerApp *tracker, bool from_cursor)
{
    tracker->tracker_engine.playing = true;
    tracker->was_editing = tracker->editing;
    tracker->editing = false;

    if (!(from_cursor))
    {
        tracker->tracker_engine.pattern_position = 0;
    }

    tracker_engine_set_rate(tracker->song.rate);

    tracker_engine_set_song(&tracker->tracker_engine, &tracker->song);

    play();
}

void stop_song(FlizzerTrackerApp *tracker)
{
    tracker->tracker_engine.playing = false;
    tracker->editing = tracker->was_editing;
    stop();
}

bool is_pattern_empty(TrackerSong *song, uint8_t pattern)
{
    TrackerSongPattern song_pattern = song->pattern[pattern];

    for (int i = 0; i < song->pattern_length; i++)
    {
        TrackerSongPatternStep *step = &song_pattern.step[i];

        if (tracker_engine_get_note(step) != MUS_NOTE_NONE ||
            tracker_engine_get_instrument(step) != MUS_NOTE_INSTRUMENT_NONE ||
            tracker_engine_get_volume(step) != MUS_NOTE_VOLUME_NONE ||
            tracker_engine_get_command(step) != 0)
        {
            return false;
        }
    }

    return true;
}

void set_empty_pattern(TrackerSongPattern* pattern, uint16_t pattern_length)
{
    for (uint16_t i = 0; i < pattern_length; i++)
    {
        TrackerSongPatternStep *step = &pattern->step[i];

        set_note(step, MUS_NOTE_NONE);
        set_instrument(step, MUS_NOTE_INSTRUMENT_NONE);
        set_volume(step, MUS_NOTE_VOLUME_NONE);
        set_command(step, 0);
    }
}

bool check_and_allocate_pattern(TrackerSong *song, uint8_t pattern)
{
    if (pattern < song->num_patterns) // we can set this pattern since it already exists
    {
        return true;
    }

    else
    {
        if (song->pattern[pattern - 1].step == NULL)
            return false; // if we hop through several patterns (e.g. editing upper digit)

        if (!(is_pattern_empty(song, pattern - 1))) // don't let the user flood the song with empty patterns
        {
            song->pattern[pattern].step = malloc(sizeof(TrackerSongPatternStep) * song->pattern_length);
            set_empty_pattern(&song->pattern[pattern], song->pattern_length);
            song->num_patterns++;
            return true;
        }

        else
        {
            return false;
        }
    }
}

void resize_pattern(TrackerSongPattern* pattern, uint16_t old_length, uint16_t new_length)
{
    TrackerSongPattern temp;
    temp.step = malloc((new_length) * sizeof(TrackerSongPatternStep));

    set_empty_pattern(&temp, new_length);
    memcpy(temp.step, pattern->step, fmin(old_length, new_length) * sizeof(TrackerSongPatternStep));

    free(pattern->step);
    pattern->step = temp.step;
}

void change_pattern_length(TrackerSong* song, uint16_t new_length)
{
    for(int i = 0; i < MAX_PATTERNS; i++)
    {
        if(song->pattern[i].step)
        {
            resize_pattern(&song->pattern[i], song->pattern_length, new_length);
        }
    }

    song->pattern_length = new_length;
}