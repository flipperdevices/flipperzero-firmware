#include "flizzer_tracker.h"
#include "init_deinit.h"
#include "event.h"
#include "view/pattern_editor.h"

#define FLIZZER_TRACKER_FOLDER "/ext/flizzer_tracker"

#include <flizzer_tracker_icons.h>

/*
Fontname: -Raccoon-Fixed4x6-Medium-R-Normal--6-60-75-75-P-40-ISO10646-1
Copyright: 
Glyphs: 95/203
BBX Build Mode: 0
*/
const uint8_t u8g2_font_tom_thumb_4x6_tr[725] U8G2_FONT_SECTION("u8g2_font_tom_thumb_4x6_tr") =
	"_\0\2\2\2\3\3\4\4\3\6\0\377\5\377\5\0\0\352\1\330\2\270 \5\340\315\0!\6\265\310"
	"\254\0\42\6\213\313$\25#\10\227\310\244\241\206\12$\10\227\310\215\70b\2%\10\227\310d\324F\1"
	"&\10\227\310(\65R\22'\5\251\313\10(\6\266\310\251\62)\10\226\310\304\224\24\0*\6\217\312\244"
	"\16+\7\217\311\245\225\0,\6\212\310)\0-\5\207\312\14.\5\245\310\4/\7\227\310Ve\4\60"
	"\7\227\310-k\1\61\6\226\310\255\6\62\10\227\310h\220\312\1\63\11\227\310h\220\62X\0\64\10\227"
	"\310$\65b\1\65\10\227\310\214\250\301\2\66\10\227\310\315\221F\0\67\10\227\310\314TF\0\70\10\227"
	"\310\214\64\324\10\71\10\227\310\214\64\342\2:\6\255\311\244\0;\7\222\310e\240\0<\10\227\310\246\32"
	"d\20=\6\217\311l\60>\11\227\310d\220A*\1\77\10\227\310\314\224a\2@\10\227\310UC\3"
	"\1A\10\227\310UC\251\0B\10\227\310\250\264\322\2C\7\227\310\315\32\10D\10\227\310\250d-\0"
	"E\10\227\310\214\70\342\0F\10\227\310\214\70b\4G\10\227\310\315\221\222\0H\10\227\310$\65\224\12"
	"I\7\227\310\254X\15J\7\227\310\226\252\2K\10\227\310$\265\222\12L\7\227\310\304\346\0M\10\227"
	"\310\244\61\224\12N\10\227\310\244q\250\0O\7\227\310UV\5P\10\227\310\250\264b\4Q\10\227\310"
	"Uj$\1R\10\227\310\250\64V\1S\10\227\310m\220\301\2T\7\227\310\254\330\2U\7\227\310$"
	"W\22V\10\227\310$\253L\0W\10\227\310$\65\206\12X\10\227\310$\325R\1Y\10\227\310$U"
	"V\0Z\7\227\310\314T\16[\7\227\310\214X\16\134\10\217\311d\220A\0]\7\227\310\314r\4^"
	"\5\213\313\65_\5\207\310\14`\6\212\313\304\0a\7\223\310\310\65\2b\10\227\310D\225\324\2c\7"
	"\223\310\315\14\4d\10\227\310\246\245\222\0e\6\223\310\235\2f\10\227\310\246\264b\2g\10\227\307\35"
	"\61%\0h\10\227\310D\225\254\0i\6\265\310\244\1j\10\233\307f\30U\5k\10\227\310\304\264T"
	"\1l\7\227\310\310\326\0m\7\223\310<R\0n\7\223\310\250d\5o\7\223\310U\252\2p\10\227"
	"\307\250\244V\4q\10\227\307-\225d\0r\6\223\310\315\22s\10\223\310\215\70\22\0t\10\227\310\245"
	"\25\243\0u\7\223\310$+\11v\10\223\310$\65R\2w\7\223\310\244q\4x\7\223\310\244\62\25"
	"y\11\227\307$\225dJ\0z\7\223\310\254\221\6{\10\227\310\251\32D\1|\6\265\310(\1}\11"
	"\227\310\310\14RR\0~\6\213\313\215\4\0\0\0\4\377\377\0";

typedef enum
{
	EventTypeInput,
} EventType;

typedef struct
{
	EventType type;
	InputEvent input;
} FlizzerTrackerEvent;

typedef enum
{
	PARAM_FREQUENCY,
	PARAM_WAVEFORM,
	PARAM_PW,
	PARAM_ENABLE_FILTER,
	PARAM_FILTER_CUTOFF,
	PARAM_FILTER_RESONANCE,
	PARAM_FILTER_TYPE,
} SelectedParam;

const char* wave_names[] = 
{
	"NONE",
	"NOISE",
	"PULSE",
	"TRIANGLE",
	"SAWTOOTH",
	"METAL NOISE",
	"SINE",
};

const char* filter_names[] = 
{
	"NONE",
	"LOWPASS",
	"HIGHPASS",
	"BANDPASS",
};

#define NUM_PARAMS 10

static void draw_callback(Canvas* canvas, void* ctx) 
{
	FlizzerTrackerApp* tracker = (FlizzerTrackerApp*)ctx;

	canvas_clear(canvas);

	canvas_set_color(canvas, ColorXOR);

	canvas_set_custom_font(canvas, u8g2_font_tom_thumb_4x6_tr);

	draw_pattern_view(canvas, tracker);

	/*char buffer[30] = {0};

	snprintf(buffer, 20, "FREQUENCY:%ld Hz", tracker->frequency);
	
	canvas_draw_str(canvas, 0, 6, buffer);

	snprintf(buffer, 20, "WAVEFORM:%s", wave_names[tracker->current_waveform_index]);
	
	canvas_draw_str(canvas, 0, 12, buffer);

	snprintf(buffer, 20, "PULSE WIDTH:$%03X", tracker->pw);
	
	canvas_draw_str(canvas, 0, 18, buffer);

	snprintf(buffer, 20, "FILTER:%s", (tracker->flags & SE_ENABLE_FILTER) ? "ENABLED" : "DISABLED");

	canvas_draw_str(canvas, 0, 24, buffer);

	snprintf(buffer, 20, "CUTOFF:%d", tracker->cutoff);
	
	canvas_draw_str(canvas, 0, 30, buffer);

	snprintf(buffer, 20, "RESONANCE:%d", tracker->resonance);
	
	canvas_draw_str(canvas, 0, 36, buffer);

	snprintf(buffer, 20, "TYPE:%s", filter_names[tracker->filter_type]);
	
	canvas_draw_str(canvas, 0, 42, buffer);

	snprintf(buffer, 20, "TR.ENG.CALLS: %d", tracker->sound_engine.channel[0].adsr.volume);
	
	canvas_draw_str(canvas, 0, 48, buffer);

	canvas_draw_str(canvas, 70, tracker->selected_param * 6 + 6, "<");

    uint32_t bytes = memmgr_get_free_heap();
	snprintf(buffer, 20, "BYTES FREE:%ld", bytes);

	canvas_draw_str(canvas, 0, 64, buffer);*/

	//canvas_draw_icon(canvas, 0, 0, &I_test);
}

static void input_callback(InputEvent* input_event, void* ctx) 
{
	// Проверяем, что контекст не нулевой
	furi_assert(ctx);
	FuriMessageQueue* event_queue = ctx;

	FlizzerTrackerEvent event = {.type = EventTypeInput, .input = *input_event};
	furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

const uint8_t waveforms[] = 
{
	SE_WAVEFORM_NONE,
	SE_WAVEFORM_NOISE,
	SE_WAVEFORM_PULSE,
	SE_WAVEFORM_TRIANGLE,
	SE_WAVEFORM_SAW,
	SE_WAVEFORM_NOISE_METAL,
	SE_WAVEFORM_SINE,
};

int32_t flizzer_tracker_app(void* p) 
{
	UNUSED(p);

	// Текущее событие типа кастомного типа FlizzerTrackerEvent
	FlizzerTrackerEvent event;
	// Очередь событий на 8 элементов размера FlizzerTrackerEvent
	FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(FlizzerTrackerEvent));

	FlizzerTrackerApp* tracker = init_tracker(44100, 50, true, 1024);

	// Создаем новый view port
	ViewPort* view_port = view_port_alloc();
	// Создаем callback отрисовки, без контекста
	view_port_draw_callback_set(view_port, draw_callback, tracker);
	// Создаем callback нажатий на клавиши, в качестве контекста передаем
	// нашу очередь сообщений, чтоб запихивать в неё эти события
	view_port_input_callback_set(view_port, input_callback, event_queue);

	// Создаем GUI приложения
	Gui* gui = furi_record_open(RECORD_GUI);
	// Подключаем view port к GUI в полноэкранном режиме
	gui_add_view_port(gui, view_port, GuiLayerFullscreen);

	tracker->notification = furi_record_open(RECORD_NOTIFICATION);
	notification_message(tracker->notification, &sequence_display_backlight_enforce_on);

	//tracker->sound_engine.channel[0].waveform = SE_WAVEFORM_NOISE;

	tracker->frequency = 440;
	tracker->current_waveform_index = 1;

	/*sound_engine_set_channel_frequency(&tracker->sound_engine, &tracker->sound_engine.channel[0], ((12 * 4) << 8));

	tracker->sound_engine.channel[0].adsr.a = 0x10;
	tracker->sound_engine.channel[0].adsr.d = 0x10;
	tracker->sound_engine.channel[0].adsr.s = 0xff;
	tracker->sound_engine.channel[0].adsr.volume = 0x80;
	tracker->sound_engine.channel[0].adsr.envelope_state = ATTACK;

	SoundEngine* eng = &(tracker->sound_engine);

	tracker->sound_engine.channel[0].adsr.envelope_speed = envspd(eng, tracker->sound_engine.channel[0].adsr.a);*/

	tracker->song.speed = 5;
	tracker->song.num_instruments = 2;
	tracker->song.num_patterns = 2;
	tracker->song.num_sequence_steps = 1;
	tracker->song.pattern_length = 64;

	tracker->song.sequence.sequence_step[0].pattern_indices[0] = 0;
	tracker->song.sequence.sequence_step[0].pattern_indices[1] = 1;
	tracker->song.sequence.sequence_step[0].pattern_indices[2] = 2;
	tracker->song.sequence.sequence_step[0].pattern_indices[3] = 2;

	tracker->song.pattern[0].step = malloc(64 * sizeof(TrackerSongPatternStep));
	tracker->song.pattern[1].step = malloc(64 * sizeof(TrackerSongPatternStep));
	tracker->song.pattern[2].step = malloc(64 * sizeof(TrackerSongPatternStep));

	memset(tracker->song.pattern[0].step, 0, 64 * sizeof(TrackerSongPatternStep));
	memset(tracker->song.pattern[1].step, 0, 64 * sizeof(TrackerSongPatternStep));
	memset(tracker->song.pattern[2].step, 0, 64 * sizeof(TrackerSongPatternStep));

	tracker->song.instrument[0] = malloc(sizeof(Instrument));
	tracker->song.instrument[1] = malloc(sizeof(Instrument));

	for(int i = 0; i < 64; ++i)
	{
		set_note(&tracker->song.pattern[0].step[i], MUS_NOTE_NONE);
		set_note(&tracker->song.pattern[1].step[i], MUS_NOTE_NONE);
		set_note(&tracker->song.pattern[2].step[i], MUS_NOTE_NONE);

		set_instrument(&tracker->song.pattern[0].step[i], MUS_NOTE_INSTRUMENT_NONE);
		set_instrument(&tracker->song.pattern[1].step[i], MUS_NOTE_INSTRUMENT_NONE);
		set_instrument(&tracker->song.pattern[2].step[i], MUS_NOTE_INSTRUMENT_NONE);

		set_volume(&tracker->song.pattern[0].step[i], MUS_NOTE_VOLUME_NONE);
		set_volume(&tracker->song.pattern[1].step[i], MUS_NOTE_VOLUME_NONE);
		set_volume(&tracker->song.pattern[2].step[i], MUS_NOTE_VOLUME_NONE);
	}

	for(int i = 0; i < 64; i += 8)
	{
		set_note(&tracker->song.pattern[0].step[0 + i], 12 * 5);
		set_note(&tracker->song.pattern[0].step[2 + i], 12 * 5 + 2);
		set_note(&tracker->song.pattern[0].step[4 + i], 12 * 5 - 2);
		set_note(&tracker->song.pattern[0].step[6 + i], 12 * 5 + 4);

		set_instrument(&tracker->song.pattern[0].step[0 + i], 0);
		set_instrument(&tracker->song.pattern[0].step[2 + i], 0);
		set_instrument(&tracker->song.pattern[0].step[4 + i], 0);
		set_instrument(&tracker->song.pattern[0].step[6 + i], 0);
	}

	for(int i = 0; i < 64; i++)
	{
		set_note(&tracker->song.pattern[1].step[i], 12 * 7 + 11);

		set_instrument(&tracker->song.pattern[1].step[i], 1);
	}

	tracker->song.instrument[0]->adsr.a = 0x2;
	tracker->song.instrument[0]->adsr.d = 0x9;
	tracker->song.instrument[0]->adsr.volume = 0x80;
	tracker->song.instrument[0]->waveform = SE_WAVEFORM_TRIANGLE;

	tracker->song.instrument[1]->adsr.a = 0x0;
	tracker->song.instrument[1]->adsr.d = 0x3;
	tracker->song.instrument[1]->adsr.volume = 0x18;
	tracker->song.instrument[1]->waveform = SE_WAVEFORM_NOISE;

	tracker->tracker_engine.playing = false;
	play();

	// Бесконечный цикл обработки очереди событий
	while(1) 
	{
		// Выбираем событие из очереди в переменную event (ждём бесконечно долго, если очередь пуста)
		// и проверяем, что у нас получилось это сделать
		furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

		// Наше событие — это нажатие кнопки
		if(event.type == EventTypeInput) 
		{
			// Если нажата кнопка "назад", то выходим из цикла, а следовательно и из приложения
			if(event.input.key == InputKeyBack && event.input.type == InputTypeShort) 
			{
				break;
			}

			if(event.input.key == InputKeyOk && event.input.type == InputTypeShort) 
			{
				tracker->tracker_engine.playing = !(tracker->tracker_engine.playing);
			}

			if(event.input.key == InputKeyUp && event.input.type == InputTypeShort) 
			{
				if(tracker->selected_param > 0)
				{
					tracker->selected_param--;
				}
			}

			if(event.input.key == InputKeyDown && event.input.type == InputTypeShort) 
			{
				if(tracker->selected_param < NUM_PARAMS - 1)
				{
					tracker->selected_param++;
				}
			}

			if(event.input.key == InputKeyRight && event.input.type == InputTypeShort) 
			{
				switch(tracker->selected_param)
				{
					case PARAM_FREQUENCY:
					{
						tracker->frequency += 25;

						sound_engine_set_channel_frequency(&tracker->sound_engine, &tracker->sound_engine.channel[0], tracker->frequency * 1024);

						break;
					}

					case PARAM_WAVEFORM:
					{
						if(tracker->current_waveform_index < 6)
						{
							tracker->current_waveform_index++;
						}
						
						tracker->sound_engine.channel[0].waveform = waveforms[tracker->current_waveform_index];

						break;
					}

					case PARAM_PW:
					{
						if(tracker->pw + 0x80 < 0xFFF)
						{
							tracker->pw += 0x80;
						}

						else
						{
							tracker->pw = 0x80;
						}

						tracker->sound_engine.channel[0].pw = tracker->pw;

						break;
					}

					case PARAM_ENABLE_FILTER:
					{
						tracker->flags ^= SE_ENABLE_FILTER;
						
						tracker->sound_engine.channel[0].flags = tracker->flags;

						break;
					}

					case PARAM_FILTER_CUTOFF:
					{
						tracker->cutoff += 10;

						if(tracker->cutoff > 0xFFF)
						{
							tracker->cutoff = 0xFFF;
						}
						
						tracker->sound_engine.channel[0].filter_cutoff = tracker->cutoff;

						sound_engine_filter_set_coeff(&tracker->sound_engine.channel[0].filter, tracker->cutoff, tracker->resonance * 50);

						break;
					}

					case PARAM_FILTER_RESONANCE:
					{
						tracker->resonance++;
						sound_engine_filter_set_coeff(&tracker->sound_engine.channel[0].filter, tracker->cutoff, tracker->resonance * 50);
						break;
					}

					case PARAM_FILTER_TYPE:
					{
						if(tracker->filter_type < 3)
						{
							tracker->filter_type++;
						}

						tracker->sound_engine.channel[0].filter_mode = tracker->filter_type;
						break;
					}
				}
			}

			if(event.input.key == InputKeyLeft && event.input.type == InputTypeShort) 
			{
				switch(tracker->selected_param)
				{
					case PARAM_FREQUENCY:
					{
						if(tracker->frequency > 25)
						{
							tracker->frequency -= 25;
						}

						sound_engine_set_channel_frequency(&tracker->sound_engine, &tracker->sound_engine.channel[0], tracker->frequency * 1024);

						break;
					}

					case PARAM_WAVEFORM:
					{
						if(tracker->current_waveform_index > 0)
						{
							tracker->current_waveform_index--;
						}
						
						tracker->sound_engine.channel[0].waveform = waveforms[tracker->current_waveform_index];

						break;
					}

					case PARAM_PW:
					{
						if(tracker->pw - 0x80 > 0)
						{
							tracker->pw -= 0x80;
						}

						else
						{
							tracker->pw = 0xF80;
						}

						tracker->sound_engine.channel[0].pw = tracker->pw;

						break;
					}

					case PARAM_ENABLE_FILTER:
					{
						tracker->flags ^= SE_ENABLE_FILTER;
						
						tracker->sound_engine.channel[0].flags = tracker->flags;

						break;
					}

					case PARAM_FILTER_CUTOFF:
					{
						if(tracker->cutoff > 10)
						{
							tracker->cutoff -= 10;
						}
						
						tracker->sound_engine.channel[0].filter_cutoff = tracker->cutoff;
						sound_engine_filter_set_coeff(&tracker->sound_engine.channel[0].filter, tracker->cutoff, tracker->resonance * 50);

						break;
					}

					case PARAM_FILTER_RESONANCE:
					{
						if(tracker->resonance > 0)
						{
							tracker->resonance--;
							sound_engine_filter_set_coeff(&tracker->sound_engine.channel[0].filter, tracker->cutoff, tracker->resonance * 50);
						}

						break;
					}

					case PARAM_FILTER_TYPE:
					{
						if(tracker->filter_type > 0)
						{
							tracker->filter_type--;
						}

						tracker->sound_engine.channel[0].filter_mode = tracker->filter_type;
						break;
					}
				}
			}
		}
	}

	stop();

	notification_message(tracker->notification, &sequence_display_backlight_enforce_auto);
	furi_record_close(RECORD_NOTIFICATION);

	// Специальная очистка памяти, занимаемой очередью
	furi_message_queue_free(event_queue);

	// Чистим созданные объекты, связанные с интерфейсом
	gui_remove_view_port(gui, view_port);
	view_port_free(view_port);
	furi_record_close(RECORD_GUI);

	deinit_tracker(tracker);

	return 0;
}