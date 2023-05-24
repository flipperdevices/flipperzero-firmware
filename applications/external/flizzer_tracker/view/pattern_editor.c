#include "pattern_editor.h"
#include "../macros.h"

#include <flizzer_tracker_icons.h>

#define PATTERN_EDITOR_Y ((tracker->focus == EDIT_PATTERN) ? 4 : (64 - (6 * 5) - 1))

static const char* notenames[] = {
    "C-",
    "C#",
    "D-",
    "D#",
    "E-",
    "F-",
    "F#",
    "G-",
    "G#",
    "A-",
    "A#",
    "B-",
};

char* notename(uint8_t note) {
    static char buffer[6];

    if(note == MUS_NOTE_CUT) {
        snprintf(buffer, sizeof(buffer), "%s", "OFF");
        return buffer;
    }

    if(note == MUS_NOTE_RELEASE) {
        snprintf(buffer, sizeof(buffer), "%s", "   ");
        return buffer;
    }

    if(note == 0xf0) // external arpeggio notes
    {
        snprintf(buffer, sizeof(buffer), "%s", "EXT.0");
        return buffer;
    }

    if(note == 0xf1) {
        snprintf(buffer, sizeof(buffer), "%s", "EXT.1");
        return buffer;
    }

    else {
        uint8_t final_note = my_min(12 * 7 + 11, note);
        snprintf(buffer, sizeof(buffer), "%s%d", notenames[final_note % 12], final_note / 12);
    }

    return buffer;
}

char to_char(uint8_t number) {
    return to_char_array[number];
}

void draw_pattern_view(Canvas* canvas, FlizzerTrackerApp* tracker) {
    char command_buffer[6] = {0};
    char buffer[11] = {0};

    canvas_draw_line(canvas, 0, PATTERN_EDITOR_Y, 127, PATTERN_EDITOR_Y);

    for(int i = 0; i < SONG_MAX_CHANNELS; ++i) {
        uint8_t sequence_position = tracker->tracker_engine.sequence_position;
        uint8_t current_pattern =
            tracker->tracker_engine.song->sequence.sequence_step[sequence_position]
                .pattern_indices[i];
        uint16_t pattern_step = tracker->tracker_engine.pattern_position;

        uint16_t pattern_length = tracker->tracker_engine.song->pattern_length;

        TrackerSongPattern* pattern = &tracker->tracker_engine.song->pattern[current_pattern];

        for(uint8_t pos = 0; pos < ((tracker->focus == EDIT_PATTERN) ? 9 : 5); ++pos) {
            TrackerSongPatternStep* step = NULL;

            if(pattern_step - ((tracker->focus == EDIT_PATTERN) ? 4 : 2) + pos >= 0 &&
               pattern_step - ((tracker->focus == EDIT_PATTERN) ? 4 : 2) + pos < pattern_length) {
                step =
                    &pattern->step[pattern_step + pos - ((tracker->focus == EDIT_PATTERN) ? 4 : 2)];
            }

            uint8_t string_x = i * 32;
            uint8_t string_y =
                PATTERN_EDITOR_Y + 6 * pos + 6 + ((tracker->focus == EDIT_PATTERN) ? 3 : 1);

            if(step) {
                uint8_t note = tracker_engine_get_note(step);
                uint8_t inst = tracker_engine_get_instrument(step);
                uint8_t vol = tracker_engine_get_volume(step);
                uint16_t command = tracker_engine_get_command(step);

                char inst_ch = to_char(inst);
                char vol_ch = to_char(vol);
                char command_ch = to_char(command >> 8);

                if(inst == MUS_NOTE_INSTRUMENT_NONE) {
                    inst_ch = '-';
                }

                if(vol == MUS_NOTE_VOLUME_NONE) {
                    vol_ch = '-';
                }

                if(command == 0) {
                    snprintf(command_buffer, sizeof(command_buffer), "---");
                }

                else {
                    snprintf(
                        command_buffer,
                        sizeof(command_buffer),
                        "%c%02X",
                        command_ch,
                        (command & 0xff));
                }

                snprintf(
                    buffer,
                    sizeof(buffer),
                    "%s%c%c%s",
                    (note == MUS_NOTE_NONE ? "---" : notename(note)),
                    inst_ch,
                    vol_ch,
                    command_buffer);

                canvas_draw_str(canvas, string_x, string_y, buffer);

                if(note == MUS_NOTE_RELEASE) {
                    canvas_draw_icon(canvas, string_x, string_y - 5, &I_note_release);
                }
            }
        }
    }

    if(tracker->editing && tracker->focus == EDIT_PATTERN) {
        uint16_t x = tracker->current_channel * 32 + tracker->patternx * 4 +
                     (tracker->patternx > 0 ? 4 : 0) - 1;
        uint16_t y = PATTERN_EDITOR_Y + 6 * ((tracker->focus == EDIT_PATTERN) ? 4 : 2) +
                     ((tracker->focus == EDIT_PATTERN) ? 3 : 1);

        canvas_draw_box(canvas, x, y, (tracker->patternx > 0 ? 5 : 9), 7);
    }

    if(!(tracker->editing) && tracker->focus == EDIT_PATTERN) {
        uint16_t x = tracker->current_channel * 32 + tracker->patternx * 4 +
                     (tracker->patternx > 0 ? 4 : 0) - 1;
        uint16_t y = PATTERN_EDITOR_Y + 6 * ((tracker->focus == EDIT_PATTERN) ? 4 : 2) +
                     ((tracker->focus == EDIT_PATTERN) ? 3 : 1);

        canvas_draw_frame(canvas, x, y, (tracker->patternx > 0 ? 5 : 9), 7);
    }

    canvas_set_color(canvas, ColorBlack);

    for(int i = 1; i < SONG_MAX_CHANNELS; ++i) {
        for(int y = PATTERN_EDITOR_Y + 1; y < 64; y += 2) {
            canvas_draw_dot(canvas, i * 32 - 1, y);
        }
    }

    for(int i = 0; i < SONG_MAX_CHANNELS; ++i) {
        if(tracker->tracker_engine.channel[i].channel_flags & TEC_DISABLED) {
            canvas_draw_icon(canvas, 13 + 32 * i, PATTERN_EDITOR_Y - 3, &I_channel_off);
        }

        else {
            canvas_draw_icon(canvas, 13 + 32 * i, PATTERN_EDITOR_Y - 3, &I_channel_on);
        }
    }

    canvas_set_color(canvas, ColorXOR);
}

#define SEQ_SLIDER_X (4 * (4 * 2 + 1) + 2)
#define SEQ_SLIDER_Y (32)

void draw_sequence_view(Canvas* canvas, FlizzerTrackerApp* tracker) {
    char buffer[4];

    uint8_t sequence_position = tracker->tracker_engine.sequence_position;
    TrackerSong* song = &tracker->song;

    for(int pos = sequence_position - 2; pos < sequence_position + 3; pos++) {
        if(pos >= 0 && pos < tracker->song.num_sequence_steps) {
            for(int i = 0; i < SONG_MAX_CHANNELS; ++i) {
                uint8_t current_pattern =
                    tracker->tracker_engine.song->sequence.sequence_step[pos].pattern_indices[i];

                uint8_t x = i * (4 * 2 + 1) + 3;
                uint8_t y = (pos - (sequence_position - 2)) * 6 + 5;

                snprintf(buffer, sizeof(buffer), "%02X", current_pattern);
                canvas_draw_str(canvas, x, y, buffer);
            }
        }
    }

    if(song->loop_start != 0 || song->loop_end != 0) {
        canvas_set_color(canvas, ColorBlack);

        for(int pos = sequence_position - 2; pos < sequence_position + 3; pos++) {
            if(pos >= 0 && pos < tracker->song.num_sequence_steps) {
                if(pos == song->loop_start) {
                    int16_t y = (pos - (sequence_position - 2)) * 6;

                    canvas_draw_line(canvas, 0, fmax(y, 0), 1, fmax(y, 0));
                    canvas_draw_line(canvas, 0, fmax(y, 0), 0, fmax(y + 4, 0));
                }

                if(pos > song->loop_start && pos < song->loop_end) {
                    int16_t y = (pos - (sequence_position - 2)) * 6;

                    canvas_draw_line(canvas, 0, fmax(y - 1, 0), 0, fmax(y + 4, 0));
                }

                if(pos == song->loop_end) {
                    int16_t y = (pos - (sequence_position - 2)) * 6;

                    canvas_draw_line(canvas, 0, fmax(y + 4, 0), 1, fmax(y + 4, 0));
                    canvas_draw_line(canvas, 0, fmax(y - 1, 0), 0, fmax(y + 4, 0));

                    break;
                }
            }
        }

        canvas_set_color(canvas, ColorXOR);
    }

    canvas_set_color(canvas, ColorBlack);

    canvas_draw_line(canvas, SEQ_SLIDER_X, 0, SEQ_SLIDER_X + 2, 0);
    canvas_draw_line(canvas, SEQ_SLIDER_X, SEQ_SLIDER_Y, SEQ_SLIDER_X + 2, SEQ_SLIDER_Y);

    canvas_draw_line(canvas, SEQ_SLIDER_X, 0, SEQ_SLIDER_X, SEQ_SLIDER_Y);
    canvas_draw_line(canvas, SEQ_SLIDER_X + 2, 0, SEQ_SLIDER_X + 2, SEQ_SLIDER_Y);

    uint8_t start_pos =
        sequence_position * (SEQ_SLIDER_Y - 2) / tracker->song.num_sequence_steps + 1;
    uint8_t slider_length = (SEQ_SLIDER_Y - 2) / tracker->song.num_sequence_steps + 1;

    canvas_draw_line(
        canvas, SEQ_SLIDER_X + 1, start_pos, SEQ_SLIDER_X + 1, (start_pos + slider_length));

    canvas_set_color(canvas, ColorXOR);

    if(tracker->editing && tracker->focus == EDIT_SEQUENCE) {
        uint8_t x = tracker->current_channel * (4 + 4 + 1) + (tracker->current_digit ? 4 : 0) + 2;
        uint8_t y = 11;

        canvas_draw_box(canvas, x, y, 5, 7);
    }

    if(!(tracker->editing) && tracker->focus == EDIT_SEQUENCE) {
        uint8_t x = tracker->current_channel * (4 + 4 + 1) + (tracker->current_digit ? 4 : 0) + 2;
        uint8_t y = 11;

        canvas_draw_frame(canvas, x, y, 5, 7);
    }
}

#define member_size(type, member) sizeof(((type*)0)->member)

#define SONG_HEADER_SIZE                                                                        \
    (member_size(TrackerSong, song_name) + member_size(TrackerSong, speed) +                    \
     member_size(TrackerSong, rate) + member_size(TrackerSong, loop_start) +                    \
     member_size(TrackerSong, loop_end) + member_size(TrackerSong, num_patterns) +              \
     member_size(TrackerSong, num_sequence_steps) + member_size(TrackerSong, num_instruments) + \
     member_size(TrackerSong, pattern_length))

uint32_t calculate_song_size(TrackerSong* song) {
    uint32_t song_size =
        SONG_HEADER_SIZE + sizeof(Instrument) * song->num_instruments +
        sizeof(TrackerSongPatternStep) * song->num_patterns * song->pattern_length +
        sizeof(TrackerSongSequenceStep) * song->num_sequence_steps;
    return song_size;
}

void draw_generic_n_digit_field(
    FlizzerTrackerApp* tracker,
    Canvas* canvas,
    uint8_t focus,
    uint8_t param,
    const char* text,
    uint8_t x,
    uint8_t y,
    uint8_t digits) // last 1-2 symbols are digits we are editing
{
    canvas_draw_str(canvas, x, y, text);

    if(tracker->focus == focus && tracker->selected_param == param && tracker->editing) {
        bool select_string = true;

        if(tracker->focus == EDIT_SONGINFO) {
            if(param != SI_SONGNAME && param != SI_INSTRUMENTNAME) {
                select_string = false;
            }
        }

        if(tracker->focus == EDIT_INSTRUMENT) {
            if(param != INST_INSTRUMENTNAME) {
                select_string = false;
            }
        }

        if(!(select_string)) {
            if(tracker->focus == EDIT_INSTRUMENT && param == INST_CURRENTINSTRUMENT) {
                canvas_draw_box(canvas, x + strlen(text) * 4 - digits * 4 - 1, y - 6, 5, 7);
            }

            else {
                canvas_draw_box(
                    canvas,
                    x + strlen(text) * 4 - digits * 4 + tracker->current_digit * 4 - 1,
                    y - 6,
                    5,
                    7);
            }
        }

        else {
            canvas_draw_box(canvas, x - 1, y - 6, fmax(5, strlen(text) * 4 + 1), 7);
        }
    }

    if(tracker->focus == focus && tracker->selected_param == param && !(tracker->editing)) {
        bool select_string = true;

        if(tracker->focus == EDIT_SONGINFO) {
            if(param != SI_SONGNAME && param != SI_INSTRUMENTNAME) {
                select_string = false;
            }
        }

        if(tracker->focus == EDIT_INSTRUMENT) {
            if(param != INST_INSTRUMENTNAME) {
                select_string = false;
            }
        }

        if(!(select_string)) {
            if(tracker->focus == EDIT_INSTRUMENT && param == INST_CURRENTINSTRUMENT) {
                canvas_draw_frame(canvas, x + strlen(text) * 4 - digits * 4 - 1, y - 6, 5, 7);
            }

            else {
                canvas_draw_frame(
                    canvas,
                    x + strlen(text) * 4 - digits * 4 + tracker->current_digit * 4 - 1,
                    y - 6,
                    5,
                    7);
            }
        }

        else {
            canvas_draw_frame(canvas, x - 1, y - 6, fmax(5, strlen(text) * 4 + 1), 7);
        }
    }
}

void draw_songinfo_view(Canvas* canvas, FlizzerTrackerApp* tracker) {
    char buffer[30];

    snprintf(
        buffer,
        sizeof(buffer),
        "PAT.P.%02X/%02X",
        tracker->tracker_engine.pattern_position,
        tracker->song.pattern_length - 1);
    draw_generic_n_digit_field(tracker, canvas, EDIT_SONGINFO, SI_PATTERNPOS, buffer, 42, 5, 2);
    snprintf(
        buffer,
        sizeof(buffer),
        "SEQ.P.%02X/%02X",
        tracker->tracker_engine.sequence_position,
        tracker->song.num_sequence_steps - 1);
    draw_generic_n_digit_field(tracker, canvas, EDIT_SONGINFO, SI_SEQUENCEPOS, buffer, 42, 11, 2);
    snprintf(buffer, sizeof(buffer), "SPD.%02X", tracker->song.speed);
    draw_generic_n_digit_field(tracker, canvas, EDIT_SONGINFO, SI_SONGSPEED, buffer, 42, 17, 2);
    snprintf(buffer, sizeof(buffer), "RATE %02X", tracker->song.rate);
    draw_generic_n_digit_field(
        tracker, canvas, EDIT_SONGINFO, SI_SONGRATE, buffer, 42 + 4 * 7, 17, 2);
    snprintf(buffer, sizeof(buffer), "VOL %02X", tracker->tracker_engine.master_volume);
    draw_generic_n_digit_field(
        tracker, canvas, EDIT_SONGINFO, SI_MASTERVOL, buffer, 42 + 4 * 7 + 4 * 8, 17, 2);

    snprintf(buffer, sizeof(buffer), "SONG:");
    canvas_draw_str(canvas, 42, 23, buffer);
    snprintf(buffer, sizeof(buffer), "%s", tracker->song.song_name);
    draw_generic_n_digit_field(
        tracker, canvas, EDIT_SONGINFO, SI_SONGNAME, buffer, 42 + 4 * 5, 23, 1);

    snprintf(buffer, sizeof(buffer), "INST:%c", to_char(tracker->current_instrument));
    draw_generic_n_digit_field(
        tracker, canvas, EDIT_SONGINFO, SI_CURRENTINSTRUMENT, buffer, 42, 29, 1);
    snprintf(
        buffer, sizeof(buffer), "%s", tracker->song.instrument[tracker->current_instrument]->name);
    draw_generic_n_digit_field(
        tracker, canvas, EDIT_SONGINFO, SI_INSTRUMENTNAME, buffer, 42 + 4 * 7, 29, 1);

    uint32_t song_size = calculate_song_size(&tracker->song);
    uint32_t free_bytes = memmgr_get_free_heap();
    canvas_draw_line(canvas, 128 - 4 * 10 - 2, 0, 128 - 4 * 10 - 2, 10);

    char song_size_buffer[19];
    char free_bytes_buffer[19];

    if(song_size > 9999) {
        snprintf(
            song_size_buffer,
            sizeof(song_size_buffer),
            "TUNE:%ld%c%01ldK",
            song_size / 1024,
            '.',
            (song_size % 1024) / 103);
    }

    else {
        snprintf(song_size_buffer, sizeof(song_size_buffer), "TUNE:%ld", song_size);
    }

    if(free_bytes > 9999) {
        snprintf(
            free_bytes_buffer,
            sizeof(song_size_buffer),
            "FREE:%ld%c%01ldK",
            free_bytes / 1024,
            '.',
            (free_bytes % 1024) / 103);
    }

    else {
        snprintf(free_bytes_buffer, sizeof(song_size_buffer), "FREE:%ld", free_bytes);
    }

    canvas_draw_str(canvas, 128 - 4 * 10, 5, song_size_buffer);
    canvas_draw_str(canvas, 128 - 4 * 10, 11, free_bytes_buffer);
}