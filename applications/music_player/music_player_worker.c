#include "music_player_worker.h"

#include <furi_hal.h>
#include <furi.h>

#include <storage/storage.h>
#include <lib/flipper_format/flipper_format.h>

#include <m-array.h>

#define TAG "MusicPlayerWorker"

#define MUSIC_PLAYER_FILETYPE "Flipper Music Format"
#define MUSIC_PLAYER_VERSION 0

#define SEMITONE_PAUSE 0xFF

#define NOTE_C4 261.63f
#define NOTE_C4_SEMITONE (4.0f * 12.0f)
#define TWO_POW_TWELTH_ROOT 1.059463094359f

typedef struct {
    uint8_t semitone;
    uint8_t duration;
} NoteBlock;

ARRAY_DEF(NoteBlockArray, NoteBlock, M_POD_OPLIST);

struct MusicPlayerWorker {
    NoteBlockArray_t notes;
    FuriThread* thread;
    uint32_t bpm;
    uint32_t duration;
    uint32_t octave;
    bool should_work;
};

static int32_t music_player_worker_thread_callback(void* context) {
    furi_assert(context);
    MusicPlayerWorker* instance = context;

    NoteBlockArray_it_t it;
    NoteBlockArray_it(it, instance->notes);

    while(instance->should_work) {
        if(NoteBlockArray_end_p(it)) {
            NoteBlockArray_it(it, instance->notes);
            osDelay(10);
        } else {
            NoteBlock* note_block = NoteBlockArray_ref(it);

            float note_from_a4 = (float)note_block->semitone - NOTE_C4_SEMITONE;
            float frequency = NOTE_C4 * powf(TWO_POW_TWELTH_ROOT, note_from_a4);
            float duration =
                60.0 * osKernelGetTickFreq() * 4 / instance->bpm / note_block->duration;
            uint32_t next_tick = furi_hal_get_tick() + duration;
            float volume = 1.0f;
            furi_hal_speaker_start(frequency, volume);
            while(furi_hal_get_tick() < next_tick) {
                volume -= 0.0054321;
                furi_hal_speaker_set_volume(volume);
                furi_hal_delay_ms(2);
            }
            furi_hal_speaker_stop();

            NoteBlockArray_next(it);
        }
    }

    return 0;
}

MusicPlayerWorker* music_player_worker_alloc() {
    MusicPlayerWorker* instance = malloc(sizeof(MusicPlayerWorker));

    NoteBlockArray_init(instance->notes);

    instance->thread = furi_thread_alloc();
    furi_thread_set_name(instance->thread, "MusicPlayerWorker");
    furi_thread_set_stack_size(instance->thread, 1024);
    furi_thread_set_context(instance->thread, instance);
    furi_thread_set_callback(instance->thread, music_player_worker_thread_callback);

    return instance;
}

void music_player_worker_free(MusicPlayerWorker* instance) {
    furi_assert(instance);
    furi_thread_free(instance->thread);
    NoteBlockArray_reset(instance->notes);
    free(instance);
}

static bool is_digit(const char c) {
    return isdigit(c) != 0;
}

static bool is_letter(const char c) {
    return islower(c) != 0 || isupper(c) != 0;
}

static bool is_space(const char c) {
    return c == ' ' || c == '\t';
}

static size_t extract_number(const char* string, uint32_t* number) {
    size_t ret = 0;
    while(*string != '\0' && is_digit(*string)) {
        *number *= 10;
        *number += (*string - '0');
        string++;
        ret++;
    }
    return ret;
}

static size_t extract_char(const char* string, char* symbol) {
    if(*string != '\0' && is_letter(*string)) {
        *symbol = *string;
        return 1;
    } else {
        return 0;
    }
}

static size_t extract_sharp(const char* string, char* symbol) {
    if(*string != '\0' && *string == '#') {
        *symbol = *string;
        return 1;
    } else {
        return 0;
    }
}

static size_t skip_till_comma(const char* string) {
    size_t ret = 0;
    while(*string != '\0' && *string != ',') {
        string++;
        ret++;
    }
    if(*string != ',') {
        ret = 0;
    }
    return ret;
}

static bool
    music_player_worker_add_note(MusicPlayerWorker* instance, uint8_t semitone, uint8_t duration) {
    NoteBlock note_block;
    note_block.semitone = semitone;
    note_block.duration = duration;

    NoteBlockArray_push_back(instance->notes, note_block);

    return true;
}

static int8_t note_to_semitone(const char note) {
    switch(note) {
    case 'C':
        return 0;
    // C#
    case 'D':
        return 2;
    // D#
    case 'E':
        return 4;
    case 'F':
        return 5;
    // F#
    case 'G':
        return 7;
    // G#
    case 'A':
        return 9;
    // A#
    case 'B':
        return 11;
    default:
        return 0;
    }
}

static bool music_player_worker_parse_notes(MusicPlayerWorker* instance, const char* cursor) {
    bool result = true;

    while(*cursor != '\0') {
        if(!is_space(*cursor)) {
            uint32_t duration = 0;
            char note_char = '\0';
            char sharp_char = '\0';
            uint32_t octave = 0;

            // Parsing
            cursor += extract_number(cursor, &duration);
            cursor += extract_char(cursor, &note_char);
            cursor += extract_sharp(cursor, &sharp_char);
            cursor += extract_number(cursor, &octave);

            // Post processing
            note_char = toupper(note_char);
            if(!duration) {
                duration = instance->duration;
            }
            if(!octave) {
                octave = instance->octave;
            }

            // Validation
            bool is_valid = true;
            is_valid &= (duration >= 1 && duration <= 128);
            is_valid &= ((note_char >= 'A' && note_char <= 'G') || note_char == 'P');
            is_valid &= (sharp_char == '#' || sharp_char == '\0');
            is_valid &= (octave >= 0 && octave <= 16);
            if(!is_valid) {
                FURI_LOG_E(TAG, "Invalid note definition");
                FURI_LOG_E(
                    TAG,
                    "Parsed definition: %c%c%u %u",
                    note_char == '\0' ? '_' : note_char,
                    sharp_char == '\0' ? '_' : sharp_char,
                    octave,
                    duration);
                result = false;
                break;
            }

            // Note to semitones
            uint8_t semitone = 0;
            if(note_char == 'P') {
                semitone = SEMITONE_PAUSE;
            } else {
                semitone += octave * 12;
                semitone += note_to_semitone(note_char);
                semitone += sharp_char == '#' ? 1 : 0;
            }

            if(music_player_worker_add_note(instance, semitone, duration)) {
                FURI_LOG_D(
                    TAG,
                    "Added note: %c%c%u = %u %u",
                    note_char == '\0' ? '_' : note_char,
                    sharp_char == '\0' ? '_' : sharp_char,
                    octave,
                    semitone,
                    duration);
            } else {
                FURI_LOG_E(
                    TAG,
                    "Invalid note: %c%c%u = %u %u",
                    note_char == '\0' ? '_' : note_char,
                    sharp_char == '\0' ? '_' : sharp_char,
                    octave,
                    semitone,
                    duration);
            }
            cursor += skip_till_comma(cursor);
        }

        if(*cursor != '\0') cursor++;
    }

    return result;
}

bool music_player_worker_load(MusicPlayerWorker* instance, const char* file_path) {
    furi_assert(instance);
    furi_assert(file_path);

    bool result = false;
    string_t temp_str;
    string_init(temp_str);

    Storage* storage = furi_record_open("storage");
    FlipperFormat* file = flipper_format_file_alloc(storage);

    do {
        if(!flipper_format_file_open_existing(file, file_path)) break;

        uint32_t version = 0;
        if(!flipper_format_read_header(file, temp_str, &version)) break;
        if(string_cmp_str(temp_str, MUSIC_PLAYER_FILETYPE) || (version != MUSIC_PLAYER_VERSION)) {
            FURI_LOG_E(TAG, "Incorrect file format or version");
            break;
        }

        if(!flipper_format_read_uint32(file, "BPM", &instance->bpm, 1)) {
            FURI_LOG_E(TAG, "BPM is missing");
            break;
        }
        if(!flipper_format_read_uint32(file, "Duration", &instance->duration, 1)) {
            FURI_LOG_E(TAG, "Duration is missing");
            break;
        }
        if(!flipper_format_read_uint32(file, "Octave", &instance->octave, 1)) {
            FURI_LOG_E(TAG, "Octave is missing");
            break;
        }
        if(!flipper_format_read_string(file, "Notes", temp_str)) {
            FURI_LOG_E(TAG, "Notes is missing");
            break;
        }

        if(!music_player_worker_parse_notes(instance, string_get_cstr(temp_str))) {
            break;
        }

        result = true;
    } while(false);

    furi_record_close("storage");
    flipper_format_free(file);
    string_clear(temp_str);

    return result;
}

void music_player_worker_start(MusicPlayerWorker* instance) {
    furi_assert(instance);
    furi_assert(instance->should_work == false);

    instance->should_work = true;
    furi_thread_start(instance->thread);
}

void music_player_worker_stop(MusicPlayerWorker* instance) {
    furi_assert(instance);
    furi_assert(instance->should_work == true);

    instance->should_work = false;
    furi_thread_join(instance->thread);
}
