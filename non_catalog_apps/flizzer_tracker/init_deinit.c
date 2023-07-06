#include "init_deinit.h"
#include "input_event.h"

#include "diskop.h"

#define AUDIO_MODES_COUNT 2

TrackerView* tracker_view_alloc(FlizzerTrackerApp* tracker) {
    TrackerView* tracker_view = malloc(sizeof(TrackerView));
    tracker_view->view = view_alloc();
    tracker_view->context = tracker;
    view_set_context(tracker_view->view, tracker_view);
    view_allocate_model(tracker_view->view, ViewModelTypeLocking, sizeof(TrackerViewModel));
    view_set_draw_callback(tracker_view->view, draw_callback);
    view_set_input_callback(tracker_view->view, input_callback);

    return tracker_view;
}

void tracker_view_free(TrackerView* tracker_view) {
    furi_assert(tracker_view);
    view_free(tracker_view->view);
    free(tracker_view);
}

uint8_t my_value_index_bool(
    const bool value,
    const bool values[],
    uint8_t
        values_count) // why the fuck it gives unresolved symbol if I include it from toolbox???!!!
{
    uint8_t index = 0;

    for(uint8_t i = 0; i < values_count; i++) {
        if(value == values[i]) {
            index = i;
            break;
        }
    }

    return index;
}

FlizzerTrackerApp* init_tracker(
    uint32_t sample_rate,
    uint8_t rate,
    bool external_audio_output,
    uint32_t audio_buffer_size) {
    FlizzerTrackerApp* tracker = malloc(sizeof(FlizzerTrackerApp));
    memset(tracker, 0, sizeof(FlizzerTrackerApp));

    tracker->external_audio = external_audio_output;

    sound_engine_init(
        &tracker->sound_engine, sample_rate, external_audio_output, audio_buffer_size);
    tracker_engine_init(&tracker->tracker_engine, rate, &tracker->sound_engine);

    tracker->tracker_engine.song = &tracker->song;

    tracker->current_note = MIDDLE_C;

    // Очередь событий на 8 элементов размера FlizzerTrackerEvent
    tracker->event_queue = furi_message_queue_alloc(8, sizeof(FlizzerTrackerEvent));

    tracker->gui = furi_record_open(RECORD_GUI);
    tracker->view_dispatcher = view_dispatcher_alloc();

    tracker->tracker_view = tracker_view_alloc(tracker);

    view_dispatcher_add_view(tracker->view_dispatcher, VIEW_TRACKER, tracker->tracker_view->view);
    view_dispatcher_attach_to_gui(
        tracker->view_dispatcher, tracker->gui, ViewDispatcherTypeFullscreen);

    with_view_model(
        tracker->tracker_view->view, TrackerViewModel * model, { model->tracker = tracker; }, true);

    tracker->storage = furi_record_open(RECORD_STORAGE);
    tracker->stream = file_stream_alloc(tracker->storage);

    tracker->text_input = text_input_alloc();
    view_dispatcher_add_view(
        tracker->view_dispatcher, VIEW_KEYBOARD, text_input_get_view(tracker->text_input));

    tracker->pattern_submenu = submenu_alloc();
    tracker->pattern_copypaste_submenu = submenu_alloc();
    tracker->instrument_submenu = submenu_alloc();

    view_set_previous_callback(submenu_get_view(tracker->pattern_submenu), submenu_exit_callback);
    view_set_previous_callback(
        submenu_get_view(tracker->pattern_copypaste_submenu), submenu_exit_callback);
    view_set_previous_callback(
        submenu_get_view(tracker->instrument_submenu), submenu_exit_callback);

    submenu_add_item(
        tracker->pattern_submenu,
        "Load song",
        SUBMENU_PATTERN_LOAD_SONG,
        submenu_callback,
        tracker);
    submenu_add_item(
        tracker->pattern_submenu,
        "Save song",
        SUBMENU_PATTERN_SAVE_SONG,
        submenu_callback,
        tracker);
    submenu_add_item(
        tracker->pattern_submenu, "Settings", SUBMENU_PATTERN_SETTINGS, submenu_callback, tracker);
    submenu_add_item(
        tracker->pattern_submenu, "Help", SUBMENU_PATTERN_HELP, submenu_callback, tracker);
    submenu_add_item(
        tracker->pattern_submenu, "Exit", SUBMENU_PATTERN_EXIT, submenu_callback, tracker);

    submenu_add_item(
        tracker->instrument_submenu,
        "Load instrument",
        SUBMENU_INSTRUMENT_LOAD,
        submenu_callback,
        tracker);
    submenu_add_item(
        tracker->instrument_submenu,
        "Save instrument",
        SUBMENU_INSTRUMENT_SAVE,
        submenu_callback,
        tracker);
    submenu_add_item(
        tracker->instrument_submenu, "Exit", SUBMENU_INSTRUMENT_EXIT, submenu_callback, tracker);

    submenu_add_item(
        tracker->pattern_copypaste_submenu,
        "Copy",
        SUBMENU_PATTERN_COPYPASTE_COPY,
        submenu_copypaste_callback,
        tracker);
    submenu_add_item(
        tracker->pattern_copypaste_submenu,
        "Paste",
        SUBMENU_PATTERN_COPYPASTE_PASTE,
        submenu_copypaste_callback,
        tracker);
    submenu_add_item(
        tracker->pattern_copypaste_submenu,
        "Cut",
        SUBMENU_PATTERN_COPYPASTE_CUT,
        submenu_copypaste_callback,
        tracker);
    submenu_add_item(
        tracker->pattern_copypaste_submenu,
        "Clear",
        SUBMENU_PATTERN_COPYPASTE_CLEAR,
        submenu_copypaste_callback,
        tracker);

    view_dispatcher_add_view(
        tracker->view_dispatcher,
        VIEW_SUBMENU_PATTERN,
        submenu_get_view(tracker->pattern_submenu));
    view_dispatcher_add_view(
        tracker->view_dispatcher,
        VIEW_SUBMENU_PATTERN_COPYPASTE,
        submenu_get_view(tracker->pattern_copypaste_submenu));
    view_dispatcher_add_view(
        tracker->view_dispatcher,
        VIEW_SUBMENU_INSTRUMENT,
        submenu_get_view(tracker->instrument_submenu));

    load_config(tracker);

    tracker->settings_list = variable_item_list_alloc();
    View* view = variable_item_list_get_view(tracker->settings_list);
    view_set_previous_callback(view, submenu_settings_exit_callback);

    VariableItem* item;
    uint8_t value_index;

    item = variable_item_list_add(
        tracker->settings_list,
        "Audio output",
        AUDIO_MODES_COUNT,
        audio_output_changed_callback,
        tracker);
    value_index =
        my_value_index_bool(tracker->external_audio, audio_modes_values, AUDIO_MODES_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, audio_modes_text[value_index]);

    view_dispatcher_add_view(tracker->view_dispatcher, VIEW_SETTINGS, view);

    tracker->overwrite_file_widget = widget_alloc();

    widget_add_button_element(
        tracker->overwrite_file_widget,
        GuiButtonTypeLeft,
        "No",
        (ButtonCallback)overwrite_file_widget_no_input_callback,
        tracker);
    widget_add_button_element(
        tracker->overwrite_file_widget,
        GuiButtonTypeRight,
        "Yes",
        (ButtonCallback)overwrite_file_widget_yes_input_callback,
        tracker);

    widget_add_text_scroll_element(
        tracker->overwrite_file_widget,
        0,
        0,
        128,
        64,
        "This song file already exists,\n do you want to overwrite it?");

    view_dispatcher_add_view(
        tracker->view_dispatcher,
        VIEW_FILE_OVERWRITE,
        widget_get_view(tracker->overwrite_file_widget));

    tracker->overwrite_instrument_file_widget = widget_alloc();

    widget_add_button_element(
        tracker->overwrite_instrument_file_widget,
        GuiButtonTypeLeft,
        "No",
        (ButtonCallback)overwrite_instrument_file_widget_no_input_callback,
        tracker);
    widget_add_button_element(
        tracker->overwrite_instrument_file_widget,
        GuiButtonTypeRight,
        "Yes",
        (ButtonCallback)overwrite_instrument_file_widget_yes_input_callback,
        tracker);

    widget_add_text_scroll_element(
        tracker->overwrite_instrument_file_widget,
        0,
        0,
        128,
        64,
        "This instrument file already\nexists, do you want to\noverwrite it?");

    view_dispatcher_add_view(
        tracker->view_dispatcher,
        VIEW_INSTRUMENT_FILE_OVERWRITE,
        widget_get_view(tracker->overwrite_instrument_file_widget));

    tracker->notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(tracker->notification, &sequence_display_backlight_enforce_on);

    set_default_song(tracker);

    tracker->focus = EDIT_SONGINFO;
    tracker->source_pattern_index = -1;

    return tracker;
}

void deinit_tracker(FlizzerTrackerApp* tracker) {
    notification_message(tracker->notification, &sequence_display_backlight_enforce_auto);
    furi_record_close(RECORD_NOTIFICATION);

    // Специальная очистка памяти, занимаемой очередью
    furi_message_queue_free(tracker->event_queue);

    view_dispatcher_remove_view(tracker->view_dispatcher, VIEW_SETTINGS);
    view_dispatcher_remove_view(tracker->view_dispatcher, VIEW_FILE_OVERWRITE);
    view_dispatcher_remove_view(tracker->view_dispatcher, VIEW_SUBMENU_INSTRUMENT);
    view_dispatcher_remove_view(tracker->view_dispatcher, VIEW_INSTRUMENT_FILE_OVERWRITE);
    view_dispatcher_remove_view(tracker->view_dispatcher, VIEW_SUBMENU_PATTERN_COPYPASTE);
    view_dispatcher_remove_view(tracker->view_dispatcher, VIEW_SUBMENU_PATTERN);
    view_dispatcher_remove_view(tracker->view_dispatcher, VIEW_KEYBOARD);
    view_dispatcher_remove_view(tracker->view_dispatcher, VIEW_TRACKER);

    text_input_free(tracker->text_input);

    variable_item_list_free(tracker->settings_list);

    submenu_free(tracker->pattern_submenu);
    submenu_free(tracker->pattern_copypaste_submenu);
    submenu_free(tracker->instrument_submenu);

    widget_free(tracker->overwrite_file_widget);
    widget_free(tracker->overwrite_instrument_file_widget);

    view_dispatcher_free(tracker->view_dispatcher);

    tracker_view_free(tracker->tracker_view);
    furi_record_close(RECORD_GUI);

    stream_free(tracker->stream);
    furi_record_close(RECORD_STORAGE);

    sound_engine_deinit(&tracker->sound_engine);

    if(tracker->tracker_engine.song == NULL) {
        tracker_engine_set_song(&tracker->tracker_engine, &tracker->song);
    }

    tracker_engine_deinit(&tracker->tracker_engine, false);

    free(tracker);
}