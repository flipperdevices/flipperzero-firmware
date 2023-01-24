#include "flizzer_tracker.h"
#include "init_deinit.h"
#include "input_event.h"
#include "util.h"
#include "view/instrument_editor.h"
#include "view/pattern_editor.h"
#include "diskop.h"

#include <flizzer_tracker_icons.h>

/*
Fontname: -Raccoon-Fixed4x6-Medium-R-Normal--6-60-75-75-P-40-ISO10646-1
Copyright:
Glyphs: 95/203
BBX Build Mode: 0
*/
// this is a modified version with dot and semicolon moved 1 pixel to the left; lowercase symbols removed to save space
const uint8_t u8g2_font_tom_thumb_4x6_tr[479] U8G2_FONT_SECTION("u8g2_font_tom_thumb_4x6_tr") =
    "A\0\2\2\2\3\2\3\4\3\5\0\0\5\0\5\0\0\340\0\0\1\306 \4@\62!\5u\62+"
    "\42\6\313\63I\5#\10W\62i\250\241\2$\10Wr#\216\230\0%\10W\62\31\265Q\0&\10"
    "W\62J\215\224\4'\5\351\63\2(\6vr\252\14)\7V\62\61%\5*\6O\63\251\3+\7"
    "\317ri%\0,\5Jr\12-\5G\63\3.\5E\62\1/\7W\262U\31\1\60\7Wr\313"
    "Z\0\61\6Vr\253\1\62\7W\62\32\244r\63\11W\62\32\244\14\26\0\64\7W\62I\215X\65"
    "\10W\62#j\260\0\66\7Wrs\244\21\67\7W\62\63\225\21\70\10W\62#\15\65\2\71\10W"
    "\62#\215\270\0:\5\315\62);\7Rr\31(\0<\10W\262\251\6\31\4=\6\317\62\33\14>"
    "\11W\62\31d\220J\0?\10W\62\63e\230\0@\7Wr\325\320@A\7Wr\325P*B\10"
    "W\62*\255\264\0C\7Wr\263\6\2D\7W\62*Y\13E\7W\62#\216\70F\10W\62#"
    "\216\30\1G\7Wrs\244$H\10W\62I\15\245\2I\7W\62+V\3J\7W\262\245\252\0"
    "K\10W\62I\255\244\2L\6W\62\261\71M\10W\62i\14\245\2N\7W\62i\34*O\7W"
    "r\225U\1P\10W\62*\255\30\1Q\7Wr\225\32IR\7W\62*\215US\10Wr\33d"
    "\260\0T\7W\62+\266\0U\7W\62\311\225\4V\10W\62\311*\23\0W\10W\62I\215\241\2"
    "X\10W\62I\265T\0Y\10W\62I\225\25\0Z\7W\62\63\225\3[\7W\62#\226\3\134\7"
    "\317\62\31d\20]\7W\62\263\34\1^\5\313s\15_\5G\62\3`\5\312\63\61\0\0\0";

void draw_callback(Canvas *canvas, void *ctx)
{
    TrackerViewModel *model = (TrackerViewModel *)ctx;
    FlizzerTrackerApp *tracker = (FlizzerTrackerApp *)(model->tracker);

    canvas_set_color(canvas, ColorXOR);

    if(tracker->is_loading)
    {
        canvas_draw_str(canvas, 10, 10, "Loading...");
        return;
    }

    if(tracker->is_saving)
    {
        canvas_draw_str(canvas, 10, 10, "Saving...");
        return;
    }

    canvas_set_custom_font(canvas, u8g2_font_tom_thumb_4x6_tr);

    switch (tracker->mode)
    {
        case PATTERN_VIEW:
        {
            if (tracker->tracker_engine.song == NULL)
            {
                stop();
                tracker_engine_set_song(&tracker->tracker_engine, &tracker->song);
            }

            draw_songinfo_view(canvas, tracker);
            draw_sequence_view(canvas, tracker);
            draw_pattern_view(canvas, tracker);
            break;
        }

        case INST_EDITOR_VIEW:
        {
            draw_instrument_view(canvas, tracker);
            draw_instrument_program_view(canvas, tracker);
            break;
        }

        default:
            break;
    }
}

bool input_callback(InputEvent *input_event, void *ctx)
{
    // Проверяем, что контекст не нулевой
    furi_assert(ctx);
    TrackerView *tracker_view = (TrackerView *)ctx;
    FlizzerTrackerApp *tracker = (FlizzerTrackerApp *)(tracker_view->context);

    bool consumed = false;

    if (input_event->key == InputKeyBack && input_event->type == InputTypeShort)
    {
        tracker->period = furi_get_tick() - tracker->current_time;
        tracker->current_time = furi_get_tick();

        tracker->was_it_back_keypress = true;
    }

    else if (input_event->type == InputTypeShort || input_event->type == InputTypeLong)
    {
        tracker->was_it_back_keypress = false;
        tracker->period = 0;
    }

    uint32_t final_period = (tracker->was_it_back_keypress ? tracker->period : 0);

    FlizzerTrackerEvent event = {.type = EventTypeInput, .input = *input_event, .period = final_period};

    if(!(tracker->is_loading) && !(tracker->is_saving))
    {
        furi_message_queue_put(tracker->event_queue, &event, FuriWaitForever);
    }

    consumed = true;
    return consumed;
}

int32_t flizzer_tracker_app(void *p)
{
    UNUSED(p);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    bool st = storage_simply_mkdir(storage, FLIZZER_TRACKER_FOLDER);
    UNUSED(st);
    furi_record_close(RECORD_STORAGE);

    FlizzerTrackerApp *tracker = init_tracker(44100, 50, true, 1024);

    // Текущее событие типа кастомного типа FlizzerTrackerEvent
    FlizzerTrackerEvent event;

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

    for (int i = 0; i < 64; ++i)
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

    for (int i = 0; i < 64; i += 8)
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

    for (int i = 0; i < 64; i++)
    {
        set_note(&tracker->song.pattern[1].step[i], 12 * 7 + 11);

        set_instrument(&tracker->song.pattern[1].step[i], 1);
    }

    set_default_instrument(tracker->song.instrument[0]);
    set_default_instrument(tracker->song.instrument[1]);

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

    view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_TRACKER);

    // Бесконечный цикл обработки очереди событий
    while (!(tracker->quit))
    {
        // Выбираем событие из очереди в переменную event (ждём бесконечно долго, если очередь пуста)
        // и проверяем, что у нас получилось это сделать
        furi_check(furi_message_queue_get(tracker->event_queue, &event, FuriWaitForever) == FuriStatusOk);

        // Наше событие — это нажатие кнопки
        if (event.type == EventTypeInput)
        {
            process_input_event(tracker, &event);
        }

        if(event.type == EventTypeSaveSong)
        {
            save_song(tracker, tracker->filepath);
        }

        if(event.type == EventTypeLoadSong)
        {
            stop_song(tracker);

            tracker->dialogs = furi_record_open(RECORD_DIALOGS);
            tracker->is_loading = true;

            FuriString* path;
            path = furi_string_alloc();
            furi_string_set(path, FLIZZER_TRACKER_FOLDER);

            DialogsFileBrowserOptions browser_options;
            dialog_file_browser_set_basic_options(&browser_options, SONG_FILE_EXT, &I_flizzer_tracker_module);
            browser_options.base_path = FLIZZER_TRACKER_FOLDER;
            browser_options.hide_ext = false;

            bool ret = dialog_file_browser_show(tracker->dialogs, path, path, &browser_options);

            furi_record_close(RECORD_DIALOGS);

            if(ret)
            {
                bool result = load_song_util(tracker, path);
                UNUSED(result);
            }

            else
            {
                furi_string_free(path);
            }

        }
    }

    stop();

    deinit_tracker(tracker);

    return 0;
}