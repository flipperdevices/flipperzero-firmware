#include "flizzer_tracker.h"
#include "init_deinit.h"
#include "util.h"
#include "input_event.h"
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

static void draw_callback(Canvas* canvas, void* ctx) 
{
	FlizzerTrackerApp* tracker = (FlizzerTrackerApp*)ctx;

	canvas_set_color(canvas, ColorXOR);

	canvas_set_custom_font(canvas, u8g2_font_tom_thumb_4x6_tr);

	switch(tracker->mode)
	{
		case PATTERN_VIEW:
		{
			draw_songinfo_view(canvas, tracker);
			draw_sequence_view(canvas, tracker);
			draw_pattern_view(canvas, tracker);
			break;
		}

		default: break;
	}
}

static void input_callback(InputEvent* input_event, void* ctx) 
{
	// Проверяем, что контекст не нулевой
	furi_assert(ctx);
	FlizzerTrackerApp* tracker = (FlizzerTrackerApp*)ctx;

	if(input_event->key == InputKeyBack && input_event->type == InputTypeShort)
	{
		tracker->period = furi_get_tick() - tracker->current_time;
		tracker->current_time = furi_get_tick();

		tracker->was_it_back_keypress = true;
	}

	else if(input_event->type == InputTypeShort || input_event->type == InputTypeLong)
	{
		tracker->was_it_back_keypress = false;
		tracker->period = 0;
	}

	uint32_t final_period = (tracker->was_it_back_keypress ? tracker->period : 0);

	FlizzerTrackerEvent event = {.type = EventTypeInput, .input = *input_event, .period = final_period};
	furi_message_queue_put(tracker->event_queue, &event, FuriWaitForever);
}

int32_t flizzer_tracker_app(void* p) 
{
	UNUSED(p);

	FlizzerTrackerApp* tracker = init_tracker(44100, 50, true, 1024);

	// Текущее событие типа кастомного типа FlizzerTrackerEvent
	FlizzerTrackerEvent event;
	// Очередь событий на 8 элементов размера FlizzerTrackerEvent
	tracker->event_queue = furi_message_queue_alloc(8, sizeof(FlizzerTrackerEvent));

	// Создаем новый view port
	ViewPort* view_port = view_port_alloc();
	// Создаем callback отрисовки, без контекста
	view_port_draw_callback_set(view_port, draw_callback, tracker);
	// Создаем callback нажатий на клавиши, в качестве контекста передаем
	// нашу очередь сообщений, чтоб запихивать в неё эти события
	view_port_input_callback_set(view_port, input_callback, tracker);

	// Создаем GUI приложения
	Gui* gui = furi_record_open(RECORD_GUI);
	// Подключаем view port к GUI в полноэкранном режиме
	gui_add_view_port(gui, view_port, GuiLayerFullscreen);

	tracker->view_dispatcher = view_dispatcher_alloc();
	view_dispatcher_attach_to_gui(tracker->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

	tracker->notification = furi_record_open(RECORD_NOTIFICATION);
	notification_message(tracker->notification, &sequence_display_backlight_enforce_on);

	tracker->tracker_engine.master_volume = 0x80;

	tracker->song.speed = 5;
	tracker->song.rate = tracker->tracker_engine.rate;
	tracker->song.num_instruments = 2;
	tracker->song.num_patterns = 3;
	tracker->song.num_sequence_steps = 4;
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
	while(!(tracker->quit)) 
	{
		// Выбираем событие из очереди в переменную event (ждём бесконечно долго, если очередь пуста)
		// и проверяем, что у нас получилось это сделать
		furi_check(furi_message_queue_get(tracker->event_queue, &event, FuriWaitForever) == FuriStatusOk);

		// Наше событие — это нажатие кнопки
		if(event.type == EventTypeInput) 
		{
			process_input_event(tracker, &event);
		}
	}

	stop();

	notification_message(tracker->notification, &sequence_display_backlight_enforce_auto);
	furi_record_close(RECORD_NOTIFICATION);

	// Специальная очистка памяти, занимаемой очередью
	furi_message_queue_free(tracker->event_queue);

	// Чистим созданные объекты, связанные с интерфейсом
	gui_remove_view_port(gui, view_port);
	view_port_free(view_port);
	furi_record_close(RECORD_GUI);

	view_dispatcher_free(tracker->view_dispatcher);

	deinit_tracker(tracker);

	return 0;
}