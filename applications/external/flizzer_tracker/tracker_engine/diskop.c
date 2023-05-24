#include "diskop.h"

void load_instrument_inner(Stream* stream, Instrument* inst, uint8_t version) {
    UNUSED(version);

    size_t rwops = stream_read(stream, (uint8_t*)inst->name, sizeof(inst->name));
    rwops = stream_read(stream, (uint8_t*)&inst->waveform, sizeof(inst->waveform));
    rwops = stream_read(stream, (uint8_t*)&inst->flags, sizeof(inst->flags));
    rwops =
        stream_read(stream, (uint8_t*)&inst->sound_engine_flags, sizeof(inst->sound_engine_flags));

    rwops = stream_read(stream, (uint8_t*)&inst->base_note, sizeof(inst->base_note));
    rwops = stream_read(stream, (uint8_t*)&inst->finetune, sizeof(inst->finetune));

    rwops = stream_read(stream, (uint8_t*)&inst->slide_speed, sizeof(inst->slide_speed));

    rwops = stream_read(stream, (uint8_t*)&inst->adsr, sizeof(inst->adsr));
    rwops = stream_read(stream, (uint8_t*)&inst->pw, sizeof(inst->pw));

    if(inst->sound_engine_flags & SE_ENABLE_RING_MOD) {
        rwops = stream_read(stream, (uint8_t*)&inst->ring_mod, sizeof(inst->ring_mod));
    }

    if(inst->sound_engine_flags & SE_ENABLE_HARD_SYNC) {
        rwops = stream_read(stream, (uint8_t*)&inst->hard_sync, sizeof(inst->hard_sync));
    }

    uint8_t progsteps = 0;

    rwops = stream_read(stream, (uint8_t*)&progsteps, sizeof(progsteps));

    if(progsteps > 0) {
        rwops = stream_read(stream, (uint8_t*)inst->program, progsteps * sizeof(inst->program[0]));
    }

    rwops = stream_read(stream, (uint8_t*)&inst->program_period, sizeof(inst->program_period));

    if(inst->flags & TE_ENABLE_VIBRATO) {
        rwops = stream_read(stream, (uint8_t*)&inst->vibrato_speed, sizeof(inst->vibrato_speed));
        rwops = stream_read(stream, (uint8_t*)&inst->vibrato_depth, sizeof(inst->vibrato_depth));
        rwops = stream_read(stream, (uint8_t*)&inst->vibrato_delay, sizeof(inst->vibrato_delay));
    }

    if(inst->flags & TE_ENABLE_PWM) {
        rwops = stream_read(stream, (uint8_t*)&inst->pwm_speed, sizeof(inst->pwm_speed));
        rwops = stream_read(stream, (uint8_t*)&inst->pwm_depth, sizeof(inst->pwm_depth));
        rwops = stream_read(stream, (uint8_t*)&inst->pwm_delay, sizeof(inst->pwm_delay));
    }

    if(inst->sound_engine_flags & SE_ENABLE_FILTER) {
        rwops = stream_read(stream, (uint8_t*)&inst->filter_cutoff, sizeof(inst->filter_cutoff));
        rwops =
            stream_read(stream, (uint8_t*)&inst->filter_resonance, sizeof(inst->filter_resonance));
        rwops = stream_read(stream, (uint8_t*)&inst->filter_type, sizeof(inst->filter_type));
    }

    UNUSED(rwops);
}

bool load_song_inner(TrackerSong* song, Stream* stream) {
    uint8_t version = 0;
    size_t rwops = stream_read(stream, (uint8_t*)&version, sizeof(version));

    if(version >
       TRACKER_ENGINE_VERSION) // if song is of newer version this version of tracker engine can't support
    {
        return false;
    }

    tracker_engine_deinit_song(song, false);
    memset(song, 0, sizeof(TrackerSong));

    rwops = stream_read(stream, (uint8_t*)song->song_name, sizeof(song->song_name));
    rwops = stream_read(stream, (uint8_t*)&song->loop_start, sizeof(song->loop_start));
    rwops = stream_read(stream, (uint8_t*)&song->loop_end, sizeof(song->loop_end));
    rwops = stream_read(stream, (uint8_t*)&song->pattern_length, sizeof(song->pattern_length));

    rwops = stream_read(stream, (uint8_t*)&song->speed, sizeof(song->speed));
    rwops = stream_read(stream, (uint8_t*)&song->rate, sizeof(song->rate));

    rwops =
        stream_read(stream, (uint8_t*)&song->num_sequence_steps, sizeof(song->num_sequence_steps));

    for(uint16_t i = 0; i < song->num_sequence_steps; i++) {
        rwops = stream_read(
            stream,
            (uint8_t*)&song->sequence.sequence_step[i],
            sizeof(song->sequence.sequence_step[0]));
    }

    rwops = stream_read(stream, (uint8_t*)&song->num_patterns, sizeof(song->num_patterns));

    for(uint16_t i = 0; i < song->num_patterns; i++) {
        song->pattern[i].step = (TrackerSongPatternStep*)malloc(
            sizeof(TrackerSongPatternStep) * (song->pattern_length));
        set_empty_pattern(&song->pattern[i], song->pattern_length);
        rwops = stream_read(
            stream,
            (uint8_t*)song->pattern[i].step,
            sizeof(TrackerSongPatternStep) * (song->pattern_length));
    }

    rwops = stream_read(stream, (uint8_t*)&song->num_instruments, sizeof(song->num_instruments));

    for(uint16_t i = 0; i < song->num_instruments; i++) {
        song->instrument[i] = (Instrument*)malloc(sizeof(Instrument));
        set_default_instrument(song->instrument[i]);
        load_instrument_inner(stream, song->instrument[i], version);
    }

    UNUSED(rwops);
    return false;
}

bool load_song(TrackerSong* song, Stream* stream) {
    char header[sizeof(SONG_FILE_SIG) + 2] = {0};
    size_t rwops = stream_read(stream, (uint8_t*)&header, sizeof(SONG_FILE_SIG) - 1);
    header[sizeof(SONG_FILE_SIG)] = '\0';

    if(strcmp(header, SONG_FILE_SIG) == 0) {
        bool result = load_song_inner(song, stream);
        UNUSED(result);
    }

    UNUSED(rwops);
    return false;
}