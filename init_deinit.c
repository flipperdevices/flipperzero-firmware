#include "init_deinit.h"
#include "input_event.h"

TrackerView *tracker_view_alloc(FlizzerTrackerApp *tracker)
{
    TrackerView *tracker_view = malloc(sizeof(TrackerView));
    tracker_view->view = view_alloc();
    tracker_view->context = tracker;
    view_set_context(tracker_view->view, tracker_view);
    view_allocate_model(tracker_view->view, ViewModelTypeLocking, sizeof(TrackerViewModel));
    view_set_draw_callback(tracker_view->view, draw_callback);
    view_set_input_callback(tracker_view->view, input_callback);

    return tracker_view;
}

void tracker_view_free(TrackerView *tracker_view)
{
    furi_assert(tracker_view);
    view_free(tracker_view->view);
    free(tracker_view);
}

FlizzerTrackerApp *init_tracker(uint32_t sample_rate, uint8_t rate, bool external_audio_output, uint32_t audio_buffer_size)
{
    FlizzerTrackerApp *tracker = malloc(sizeof(FlizzerTrackerApp));
    memset(tracker, 0, sizeof(FlizzerTrackerApp));

    sound_engine_init(&tracker->sound_engine, sample_rate, external_audio_output, audio_buffer_size);
    tracker_engine_init(&tracker->tracker_engine, rate, &tracker->sound_engine);

    tracker->tracker_engine.song = &tracker->song;

    tracker->current_note = MIDDLE_C;

    // Очередь событий на 8 элементов размера FlizzerTrackerEvent
    tracker->event_queue = furi_message_queue_alloc(8, sizeof(FlizzerTrackerEvent));

    tracker->gui = furi_record_open(RECORD_GUI);
    tracker->view_dispatcher = view_dispatcher_alloc();

    tracker->tracker_view = tracker_view_alloc(tracker);

    view_dispatcher_add_view(tracker->view_dispatcher, VIEW_TRACKER, tracker->tracker_view->view);
    view_dispatcher_attach_to_gui(tracker->view_dispatcher, tracker->gui, ViewDispatcherTypeFullscreen);

    with_view_model(
        tracker->tracker_view->view, TrackerViewModel * model, { model->tracker = tracker; }, true);

    tracker->text_input = text_input_alloc();
    view_dispatcher_add_view(tracker->view_dispatcher, VIEW_KEYBOARD, text_input_get_view(tracker->text_input));

    tracker->pattern_submenu = submenu_alloc();
    tracker->instrument_submenu = submenu_alloc();

    submenu_add_item(tracker->pattern_submenu, "Exit", SUBMENU_PATTERN_EXIT, submenu_callback, tracker);
    submenu_add_item(tracker->instrument_submenu, "Exit", SUBMENU_INSTRUMENT_EXIT, submenu_callback, tracker);

    view_dispatcher_add_view(tracker->view_dispatcher, VIEW_SUBMENU_PATTERN, submenu_get_view(tracker->pattern_submenu));
    view_dispatcher_add_view(tracker->view_dispatcher, VIEW_SUBMENU_INSTRUMENT, submenu_get_view(tracker->instrument_submenu));

    tracker->notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(tracker->notification, &sequence_display_backlight_enforce_on);

    return tracker;
}

void deinit_tracker(FlizzerTrackerApp *tracker)
{
    notification_message(tracker->notification, &sequence_display_backlight_enforce_auto);
    furi_record_close(RECORD_NOTIFICATION);

    // Специальная очистка памяти, занимаемой очередью
    furi_message_queue_free(tracker->event_queue);

    view_dispatcher_remove_view(tracker->view_dispatcher, VIEW_SUBMENU_INSTRUMENT);
    view_dispatcher_remove_view(tracker->view_dispatcher, VIEW_SUBMENU_PATTERN);
    view_dispatcher_remove_view(tracker->view_dispatcher, VIEW_KEYBOARD);
    view_dispatcher_remove_view(tracker->view_dispatcher, VIEW_TRACKER);

    text_input_free(tracker->text_input);

    submenu_free(tracker->pattern_submenu);
    submenu_free(tracker->instrument_submenu);

    view_dispatcher_free(tracker->view_dispatcher);

    tracker_view_free(tracker->tracker_view);
    furi_record_close(RECORD_GUI);

    sound_engine_deinit(&tracker->sound_engine);

    if (tracker->tracker_engine.song == NULL)
    {
        tracker_engine_set_song(&tracker->tracker_engine, &tracker->song);
    }

    tracker_engine_deinit(&tracker->tracker_engine, false);

    FURI_CRITICAL_ENTER();
    LL_TIM_DeInit(TRACKER_ENGINE_TIMER);
    LL_TIM_DeInit(SAMPLE_RATE_TIMER);
    LL_TIM_DeInit(SPEAKER_PWM_TIMER);
    FURI_CRITICAL_EXIT();

    free(tracker);
}