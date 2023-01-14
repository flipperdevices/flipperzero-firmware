#include "pattern_editor.h"

#define PATTERN_EDITOR_Y (64 - (6 * 5) - 1)

static const char* notenames[] =
{
	"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-",
};

char* notename(uint8_t note)
{
	static char buffer[4];

	snprintf(buffer, sizeof(buffer), "%s%d", notenames[note % 12], note / 12);

	return buffer;
}

static const char to_char_array[] = 
{
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z',
};

char to_char(uint8_t number)
{
	return to_char_array[number];
}

void draw_pattern_view(Canvas* canvas, FlizzerTrackerApp* tracker)
{
	char command_buffer[6] = {0};
	char buffer[11] = {0};

	canvas_draw_line(canvas, 0, PATTERN_EDITOR_Y, 127, PATTERN_EDITOR_Y);

	for(int i = 1; i < SONG_MAX_CHANNELS; ++i)
	{
		for(int y = PATTERN_EDITOR_Y + 1; y < 64; y += 2)
		{
			canvas_draw_dot(canvas, i * 32 - 1, y);
		}
	}

	for(int i = 0; i < SONG_MAX_CHANNELS; ++i)
	{
		uint8_t sequence_position = tracker->tracker_engine.sequence_position;
		uint8_t current_pattern = tracker->tracker_engine.song->sequence.sequence_step[sequence_position].pattern_indices[i];
		uint8_t pattern_step = tracker->tracker_engine.pattern_position;

		uint8_t pattern_length = tracker->tracker_engine.song->pattern_length;

		TrackerSongPattern* pattern = &tracker->tracker_engine.song->pattern[current_pattern];

		for(uint8_t pos = 0; pos < 5; ++pos)
		{
			TrackerSongPatternStep* step = NULL;

			if(pattern_step - 2 + pos >= 0 && pattern_step - 2 + pos < pattern_length)
			{
				step = &pattern->step[pattern_step + pos - 2];
			}

			uint8_t string_x = i * 32;
			uint8_t string_y = PATTERN_EDITOR_Y + 6 * pos + 6 + 1;
			
			if(step)
			{
				uint8_t note = tracker_engine_get_note(step);
				uint8_t inst = tracker_engine_get_instrument(step);
				uint8_t vol = tracker_engine_get_volume(step);
				uint16_t command = tracker_engine_get_command(step);

				char inst_ch = to_char(inst);
				char vol_ch = to_char(vol);
				char command_ch = to_char(command >> 8);
				
				if(inst == MUS_NOTE_INSTRUMENT_NONE)
				{
					inst_ch = '-';
				}

				if(vol == MUS_NOTE_VOLUME_NONE)
				{
					vol_ch = '-';
				}

				if(command == 0)
				{
					snprintf(command_buffer, sizeof(command_buffer), "---");
				}

				else
				{
					snprintf(command_buffer, sizeof(command_buffer), "%c%02X", command_ch, (command & 0xff));
				}

				snprintf(buffer, sizeof(buffer), "%s%c%c%s", (note == MUS_NOTE_NONE ? "---" : notename(note)), inst_ch, vol_ch, command_buffer);

				canvas_draw_str(canvas, string_x, string_y, buffer);
			}
		}
	}

	if(tracker->editing && tracker->focus == EDIT_PATTERN)
	{
		uint8_t x = tracker->current_channel * 32 + tracker->patternx * 4 + (tracker->patternx > 0 ? 4 : 0) - 1;
		uint8_t y = PATTERN_EDITOR_Y + 6 * 2 + 1;

		canvas_draw_box(canvas, x, y, (tracker->patternx > 0 ? 5 : 9), 7);
	}
}

void draw_sequence_view(Canvas* canvas, FlizzerTrackerApp* tracker)
{
	UNUSED(canvas);
	UNUSED(tracker);
}

#define member_size(type, member) sizeof(((type *)0)->member)

#define SONG_HEADER_SIZE (member_size(TrackerSong, song_name) + member_size(TrackerSong, speed) + member_size(TrackerSong, rate) \
 + member_size(TrackerSong, loop_start) + member_size(TrackerSong, loop_end) + member_size(TrackerSong, num_patterns) \
 + member_size(TrackerSong, num_sequence_steps) + member_size(TrackerSong, num_instruments) + member_size(TrackerSong, pattern_length))

uint32_t calculate_song_size(TrackerSong* song)
{
	uint32_t song_size = SONG_HEADER_SIZE + sizeof(Instrument) * song->num_instruments + sizeof(TrackerSongPatternStep) * song->num_patterns * song->pattern_length + sizeof(TrackerSongSequenceStep) * song->num_sequence_steps;
	return song_size;
}

void draw_songinfo_view(Canvas* canvas, FlizzerTrackerApp* tracker)
{
	uint32_t song_size = calculate_song_size(&tracker->song);
	uint32_t free_bytes = memmgr_get_free_heap();
	canvas_draw_line(canvas, 128 - 4 * 10 - 2, 0, 128 - 4 * 10 - 2, 10);

	char song_size_buffer[12];
	char free_bytes_buffer[12];

	if(song_size > 999)
	{
		snprintf(song_size_buffer, sizeof(song_size_buffer), "TUNE:%.1fK", (double)song_size / (double)1024.0);
	}

	else
	{
		snprintf(song_size_buffer, sizeof(song_size_buffer), "TUNE:%ld", song_size);
	}

	if(free_bytes > 999)
	{
		snprintf(free_bytes_buffer, sizeof(song_size_buffer), "FREE:%.1fK", (double)free_bytes / (double)1024.0);
	}

	else
	{
		snprintf(free_bytes_buffer, sizeof(song_size_buffer), "FREE:%ld", free_bytes);
	}

	canvas_draw_str(canvas, 128 - 4 * 10, 5, song_size_buffer);
	canvas_draw_str(canvas, 128 - 4 * 10, 11, free_bytes_buffer);
}