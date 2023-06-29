#include "util.h"
#include "macros.h"

void reset_buffer(SoundEngine* sound_engine) {
    for(uint16_t i = 0; i < sound_engine->audio_buffer_size; i++) {
        sound_engine->audio_buffer[i] = 512;
    }
}

void stop_song(FlizzerTrackerApp* tracker) {
    tracker->tracker_engine.playing = false;
    tracker->editing = tracker->was_editing;

    for(int i = 0; i < SONG_MAX_CHANNELS; i++) {
        tracker->sound_engine.channel[i].adsr.volume = 0;
        tracker->tracker_engine.channel[i].channel_flags &= ~(TEC_PROGRAM_RUNNING);
    }

    stop();

    reset_buffer(&tracker->sound_engine);
}

void play_song(FlizzerTrackerApp* tracker, bool from_cursor) {
    uint16_t temppos = tracker->tracker_engine.pattern_position;

    stop_song(tracker);

    sound_engine_dma_init(
        (uint32_t)tracker->sound_engine.audio_buffer, tracker->sound_engine.audio_buffer_size);

    tracker->tracker_engine.playing = true;

    tracker->was_editing = tracker->editing;
    tracker->editing = false;

    if(!(from_cursor)) {
        tracker->tracker_engine.pattern_position = 0;
        temppos = 0;
    }

    tracker_engine_timer_init(tracker->song.rate);

    /*sound_engine_init_hardware(tracker->sound_engine.sample_rate,
    tracker->sound_engine.external_audio_output,
    tracker->sound_engine.audio_buffer,
    tracker->sound_engine.audio_buffer_size);
    tracker_engine_init_hardware(tracker->song.rate);*/

    tracker->tracker_engine.current_tick = 0;
    tracker_engine_set_song(&tracker->tracker_engine, &tracker->song);

    for(uint8_t i = 0; i < SONG_MAX_CHANNELS; i++) {
        bool was_disabled = tracker->tracker_engine.channel[i].channel_flags & TEC_DISABLED;

        memset(&tracker->sound_engine.channel[i], 0, sizeof(SoundEngineChannel));
        memset(&tracker->tracker_engine.channel[i], 0, sizeof(TrackerEngineChannel));

        if(was_disabled) {
            tracker->tracker_engine.channel[i].channel_flags |= TEC_DISABLED;
        }
    }

    tracker->tracker_engine.pattern_position = temppos;

    play();
}

bool is_pattern_empty(TrackerSong* song, uint8_t pattern) {
    TrackerSongPattern song_pattern = song->pattern[pattern];

    for(int i = 0; i < song->pattern_length; i++) {
        TrackerSongPatternStep* step = &song_pattern.step[i];

        if(tracker_engine_get_note(step) != MUS_NOTE_NONE ||
           tracker_engine_get_instrument(step) != MUS_NOTE_INSTRUMENT_NONE ||
           tracker_engine_get_volume(step) != MUS_NOTE_VOLUME_NONE ||
           tracker_engine_get_command(step) != 0) {
            return false;
        }
    }

    return true;
}

bool check_and_allocate_pattern(TrackerSong* song, uint8_t pattern) {
    if(pattern < song->num_patterns) // we can set this pattern since it already exists
    {
        return true;
    }

    else {
        if(song->pattern[pattern - 1].step == NULL)
            return false; // if we hop through several patterns (e.g. editing upper digit)

        if(!(is_pattern_empty(
               song, pattern - 1))) // don't let the user flood the song with empty patterns
        {
            song->pattern[pattern].step =
                malloc(sizeof(TrackerSongPatternStep) * song->pattern_length);
            set_empty_pattern(&song->pattern[pattern], song->pattern_length);
            song->num_patterns++;
            return true;
        }

        else {
            return false;
        }
    }
}

void resize_pattern(TrackerSongPattern* pattern, uint16_t old_length, uint16_t new_length) {
    TrackerSongPattern temp;
    temp.step = malloc((new_length) * sizeof(TrackerSongPatternStep));

    set_empty_pattern(&temp, new_length);
    memcpy(
        temp.step, pattern->step, my_min(old_length, new_length) * sizeof(TrackerSongPatternStep));

    free(pattern->step);
    pattern->step = temp.step;
}

void change_pattern_length(TrackerSong* song, uint16_t new_length) {
    for(int i = 0; i < MAX_PATTERNS; i++) {
        if(song->pattern[i].step) {
            resize_pattern(&song->pattern[i], song->pattern_length, new_length);
        }
    }

    song->pattern_length = new_length;
}

bool is_default_instrument(Instrument* inst) {
    Instrument* ref = malloc(sizeof(Instrument));
    set_default_instrument(ref);
    bool is_default = memcmp(ref, inst, sizeof(Instrument)) != 0 ? false : true;
    free(ref);
    return is_default;
}

bool check_and_allocate_instrument(TrackerSong* song, uint8_t inst) {
    if(inst < song->num_instruments) // we can go to this instrument since it already exists
    {
        return true;
    }

    else {
        if(inst >= MAX_INSTRUMENTS) return false;

        if(!(is_default_instrument(
               song->instrument
                   [inst - 1]))) // don't let the user flood the song with default instrument
        {
            song->instrument[inst] = malloc(sizeof(Instrument));
            set_default_instrument(song->instrument[inst]);
            song->num_instruments++;
            return true;
        }

        else {
            return false;
        }
    }
}

void set_default_song(FlizzerTrackerApp* tracker) {
    tracker->tracker_engine.master_volume = 0x80;

    tracker->song.speed = 6;
    tracker->song.rate = tracker->tracker_engine.rate;
    tracker->song.num_instruments = 1;
    tracker->song.num_patterns = 5;
    tracker->song.num_sequence_steps = 1;
    tracker->song.pattern_length = 64;

    tracker->song.sequence.sequence_step[0].pattern_indices[0] = 1;
    tracker->song.sequence.sequence_step[0].pattern_indices[1] = 2;
    tracker->song.sequence.sequence_step[0].pattern_indices[2] = 3;
    tracker->song.sequence.sequence_step[0].pattern_indices[3] = 4;

    for(int i = 0; i < 5; i++) {
        tracker->song.pattern[i].step = malloc(64 * sizeof(TrackerSongPatternStep));
        memset(tracker->song.pattern[i].step, 0, 64 * sizeof(TrackerSongPatternStep));
    }

    for(int i = 0; i < 64; ++i) {
        for(int j = 0; j < 5; j++) {
            set_note(&tracker->song.pattern[j].step[i], MUS_NOTE_NONE);

            set_instrument(&tracker->song.pattern[j].step[i], MUS_NOTE_INSTRUMENT_NONE);

            set_volume(&tracker->song.pattern[j].step[i], MUS_NOTE_VOLUME_NONE);
        }
    }

    tracker->song.instrument[0] = malloc(sizeof(Instrument));

    set_default_instrument(tracker->song.instrument[0]);

    tracker->tracker_engine.playing = false;
}