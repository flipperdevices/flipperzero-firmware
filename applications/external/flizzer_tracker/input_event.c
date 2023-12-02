#include "input_event.h"

#include "diskop.h"

#define AUDIO_MODES_COUNT 2

extern bool audio_modes_values[];
extern char* audio_modes_text[];

void return_from_keyboard_callback(void* ctx) {
    FlizzerTrackerApp* tracker = (FlizzerTrackerApp*)ctx;

    if(!tracker->is_loading && !tracker->is_saving && !tracker->is_loading_instrument &&
       !tracker->is_saving_instrument) {
        uint8_t string_length = 0;
        char* string = NULL;

        if(tracker->focus == EDIT_SONGINFO && tracker->mode == PATTERN_VIEW) {
            switch(tracker->selected_param) {
            case SI_SONGNAME: {
                string_length = MUS_SONG_NAME_LEN;
                string = (char*)&tracker->song.song_name;
                break;
            }

            case SI_INSTRUMENTNAME: {
                string_length = MUS_INST_NAME_LEN;
                string = (char*)&tracker->song.instrument[tracker->current_instrument]->name;
                break;
            }
            }
        }

        if(tracker->focus == EDIT_INSTRUMENT && tracker->mode == INST_EDITOR_VIEW) {
            switch(tracker->selected_param) {
            case INST_INSTRUMENTNAME: {
                string_length = MUS_INST_NAME_LEN;
                string = (char*)&tracker->song.instrument[tracker->current_instrument]->name;
                break;
            }
            }
        }

        if(string == NULL || string_length == 0) return;

        for(uint8_t i = 0; i < string_length;
            i++) // I tinyfied the font by deleting lowercase chars, and I don't like the lowercase chars of any 3x5 pixels font
        {
            string[i] = toupper(string[i]);
        }
    }

    view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_TRACKER);

    if(tracker->is_saving) {
        stop_song(tracker);

        tracker->filepath = furi_string_alloc();
        furi_string_cat_printf(
            tracker->filepath, "%s/%s%s", FLIZZER_TRACKER_FOLDER, tracker->filename, SONG_FILE_EXT);

        if(storage_file_exists(tracker->storage, furi_string_get_cstr(tracker->filepath))) {
            view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_FILE_OVERWRITE);
            return;
        }

        else {
            FlizzerTrackerEvent event = {.type = EventTypeSaveSong, .input = {{0}}, .period = 0};
            furi_message_queue_put(tracker->event_queue, &event, FuriWaitForever);
        }
    }

    if(tracker->is_saving_instrument) {
        stop_song(tracker);

        tracker->filepath = furi_string_alloc();
        furi_string_cat_printf(
            tracker->filepath,
            "%s/%s%s",
            FLIZZER_TRACKER_INSTRUMENTS_FOLDER,
            tracker->filename,
            INST_FILE_EXT);

        if(storage_file_exists(tracker->storage, furi_string_get_cstr(tracker->filepath))) {
            view_dispatcher_switch_to_view(
                tracker->view_dispatcher, VIEW_INSTRUMENT_FILE_OVERWRITE);
            return;
        }

        else {
            FlizzerTrackerEvent event = {
                .type = EventTypeSaveInstrument, .input = {{0}}, .period = 0};
            furi_message_queue_put(tracker->event_queue, &event, FuriWaitForever);
        }
    }
}

void overwrite_file_widget_yes_input_callback(GuiButtonType result, InputType type, void* ctx) {
    UNUSED(result);

    FlizzerTrackerApp* tracker = (FlizzerTrackerApp*)ctx;

    if(type == InputTypeShort) {
        tracker->is_saving = true;
        view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_TRACKER);
        // save_song(tracker, tracker->filepath);
        static FlizzerTrackerEvent event = {
            .type = EventTypeSaveSong, .input = {{0}}, .period = 0};
        furi_message_queue_put(tracker->event_queue, &event, FuriWaitForever);
    }
}

void overwrite_file_widget_no_input_callback(GuiButtonType result, InputType type, void* ctx) {
    UNUSED(result);

    FlizzerTrackerApp* tracker = (FlizzerTrackerApp*)ctx;

    if(type == InputTypeShort) {
        tracker->is_saving = false;
        furi_string_free(tracker->filepath);
        view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_TRACKER);
    }
}

void overwrite_instrument_file_widget_yes_input_callback(
    GuiButtonType result,
    InputType type,
    void* ctx) {
    UNUSED(result);

    FlizzerTrackerApp* tracker = (FlizzerTrackerApp*)ctx;

    if(type == InputTypeShort) {
        tracker->is_saving_instrument = true;
        view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_TRACKER);
        // save_song(tracker, tracker->filepath);
        static FlizzerTrackerEvent event = {
            .type = EventTypeSaveInstrument, .input = {{0}}, .period = 0};
        furi_message_queue_put(tracker->event_queue, &event, FuriWaitForever);
    }
}

void overwrite_instrument_file_widget_no_input_callback(
    GuiButtonType result,
    InputType type,
    void* ctx) {
    UNUSED(result);

    FlizzerTrackerApp* tracker = (FlizzerTrackerApp*)ctx;

    if(type == InputTypeShort) {
        tracker->is_saving_instrument = false;
        furi_string_free(tracker->filepath);
        view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_TRACKER);
    }
}

uint32_t submenu_settings_exit_callback(void* context) {
    UNUSED(context);
    return VIEW_SUBMENU_PATTERN;
}

uint32_t submenu_exit_callback(void* context) {
    UNUSED(context);
    return VIEW_TRACKER;
}

void submenu_callback(void* context, uint32_t index) {
    FlizzerTrackerApp* tracker = (FlizzerTrackerApp*)context;

    switch(tracker->mode) {
    case PATTERN_VIEW: {
        switch(index) {
        case SUBMENU_PATTERN_EXIT: {
            tracker->quit = true;

            static InputEvent inevent = {.sequence = 0, .key = InputKeyLeft, .type = InputTypeMAX};
            FlizzerTrackerEvent event = {
                .type = EventTypeInput,
                .input = inevent,
                .period =
                    0}; // making an event so tracker does not wait for next keypress and exits immediately
            furi_message_queue_put(tracker->event_queue, &event, FuriWaitForever);
            view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_TRACKER);
            break;
        }

        case SUBMENU_PATTERN_HELP: {
            tracker->showing_help = true;
            view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_TRACKER);
            break;
        }

        case SUBMENU_PATTERN_SAVE_SONG: {
            text_input_set_header_text(tracker->text_input, "Song filename:");
            memset(&tracker->filename, 0, FILE_NAME_LEN);
            text_input_set_result_callback(
                tracker->text_input,
                return_from_keyboard_callback,
                tracker,
                (char*)&tracker->filename,
                FILE_NAME_LEN,
                true);

            tracker->is_saving = true;

            view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_KEYBOARD);
            break;
        }

        case SUBMENU_PATTERN_LOAD_SONG: {
            FlizzerTrackerEvent event = {.type = EventTypeLoadSong, .input = {{0}}, .period = 0};
            furi_message_queue_put(tracker->event_queue, &event, FuriWaitForever);
            view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_TRACKER);
            break;
        }

        case SUBMENU_PATTERN_SETTINGS: {
            view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_SETTINGS);
            break;
        }

        default:
            break;
        }

        break;
    }

    case INST_EDITOR_VIEW: {
        switch(index) {
        case SUBMENU_INSTRUMENT_EXIT: {
            tracker->quit = true;

            static InputEvent inevent = {.sequence = 0, .key = InputKeyLeft, .type = InputTypeMAX};
            FlizzerTrackerEvent event = {
                .type = EventTypeInput,
                .input = inevent,
                .period =
                    0}; // making an event so tracker does not wait for next keypress and exits immediately
            furi_message_queue_put(tracker->event_queue, &event, FuriWaitForever);
            view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_TRACKER);
            break;
        }

        case SUBMENU_INSTRUMENT_SAVE: {
            text_input_set_header_text(tracker->text_input, "Instrument filename:");
            memset(&tracker->filename, 0, FILE_NAME_LEN);
            text_input_set_result_callback(
                tracker->text_input,
                return_from_keyboard_callback,
                tracker,
                (char*)&tracker->filename,
                FILE_NAME_LEN,
                true);

            tracker->is_saving_instrument = true;

            view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_KEYBOARD);
            break;
        }

        case SUBMENU_INSTRUMENT_LOAD: {
            FlizzerTrackerEvent event = {
                .type = EventTypeLoadInstrument, .input = {{0}}, .period = 0};
            furi_message_queue_put(tracker->event_queue, &event, FuriWaitForever);
            view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_TRACKER);
            break;
        }

        default:
            break;
        }

        break;
    }

    default:
        break;
    }
}

void submenu_copypaste_callback(void* context, uint32_t index) {
    FlizzerTrackerApp* tracker = (FlizzerTrackerApp*)context;

    uint8_t sequence_position = tracker->tracker_engine.sequence_position;
    uint8_t current_pattern_index =
        tracker->tracker_engine.song->sequence.sequence_step[sequence_position]
            .pattern_indices[tracker->current_channel];

    TrackerSongPattern* source_pattern;

    if(tracker->source_pattern_index >= 0) {
        source_pattern = &tracker->song.pattern[tracker->source_pattern_index];
    }

    TrackerSongPattern* current_pattern = &tracker->song.pattern[current_pattern_index];

    uint16_t pattern_length = tracker->tracker_engine.song->pattern_length;

    switch(index) {
    case SUBMENU_PATTERN_COPYPASTE_COPY: {
        tracker->source_pattern_index = current_pattern_index;
        tracker->cut_pattern = false;
        break;
    }

    case SUBMENU_PATTERN_COPYPASTE_PASTE: {
        if(tracker->source_pattern_index >= 0) {
            memcpy(
                current_pattern->step,
                source_pattern->step,
                sizeof(TrackerSongPatternStep) * pattern_length);

            if(tracker->cut_pattern) {
                set_empty_pattern(source_pattern, pattern_length);
                tracker->cut_pattern = false;
            }
        }
        break;
    }

    case SUBMENU_PATTERN_COPYPASTE_CUT: {
        tracker->source_pattern_index = current_pattern_index;
        tracker->cut_pattern = true;
        break;
    }

    case SUBMENU_PATTERN_COPYPASTE_CLEAR: {
        set_empty_pattern(current_pattern, pattern_length);
        break;
    }

    default:
        break;
    }

    view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_TRACKER);
}

void audio_output_changed_callback(VariableItem* item) {
    FlizzerTrackerApp* tracker = (FlizzerTrackerApp*)variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, audio_modes_text[(index > 1 ? 1 : index)]);

    if(tracker) {
        tracker->external_audio = (bool)index;

        tracker->external_audio = audio_modes_values[(index > 1 ? 1 : index)];

        // sound_engine_init(&tracker->sound_engine, tracker->sound_engine.sample_rate, tracker->external_audio, tracker->sound_engine.audio_buffer_size);
        // sound_engine_init_hardware(tracker->sound_engine.sample_rate, tracker->external_audio, tracker->sound_engine.audio_buffer, tracker->sound_engine.audio_buffer_size);

        FlizzerTrackerEvent event = {.type = EventTypeSetAudioMode, .input = {{0}}, .period = 0};
        furi_message_queue_put(tracker->event_queue, &event, FuriWaitForever);

        UNUSED(event);
    }
}

void cycle_focus(FlizzerTrackerApp* tracker) {
    switch(tracker->mode) {
    case PATTERN_VIEW: {
        tracker->focus++;

        if(tracker->focus > EDIT_SONGINFO) {
            tracker->focus = EDIT_PATTERN;
        }

        break;
    }

    case INST_EDITOR_VIEW: {
        tracker->focus++;

        if(tracker->focus > EDIT_PROGRAM) {
            tracker->focus = EDIT_INSTRUMENT;

            if(tracker->current_digit > 1) {
                tracker->current_digit = 1;
            }
        }

        break;
    }

    default:
        break;
    }
}

void cycle_view(FlizzerTrackerApp* tracker) {
    if(tracker->mode == PATTERN_VIEW) {
        tracker->mode = INST_EDITOR_VIEW;
        tracker->focus = EDIT_INSTRUMENT;

        tracker->selected_param = 0;
        tracker->current_digit = 0;

        return;
    }

    if(tracker->mode == INST_EDITOR_VIEW) {
        tracker->mode = PATTERN_VIEW;
        tracker->focus = EDIT_PATTERN;

        if(tracker->tracker_engine.song == NULL) {
            stop_song(tracker);
            tracker_engine_set_song(&tracker->tracker_engine, &tracker->song);
        }

        tracker->selected_param = 0;
        tracker->current_digit = 0;

        return;
    }
}

void process_input_event(FlizzerTrackerApp* tracker, FlizzerTrackerEvent* event) {
    if(event->input.key == InputKeyBack && event->input.type == InputTypeShort &&
       tracker->showing_help) {
        tracker->showing_help = false;
        return;
    }

    if(tracker->showing_help || tracker->is_loading || tracker->is_saving ||
       tracker->is_loading_instrument || tracker->is_saving_instrument)
        return; //do not react until these are finished

    if(event->input.key == InputKeyBack && event->input.type == InputTypeShort &&
       event->period > 0 && event->period < 300 && !(tracker->editing)) {
        cycle_view(tracker);
        stop_song(tracker);
        return;
    }

    else if(
        event->input.key == InputKeyBack && event->input.type == InputTypeShort &&
        !(tracker->editing)) {
        cycle_focus(tracker);
        //stop_song(tracker);
        return;
    }

    if(event->input.key == InputKeyBack && event->input.type == InputTypeLong) {
        switch(tracker->mode) {
        case PATTERN_VIEW: {
            if(tracker->focus == EDIT_PATTERN) {
                submenu_set_selected_item(
                    tracker->pattern_copypaste_submenu, SUBMENU_PATTERN_COPYPASTE_COPY);
                view_dispatcher_switch_to_view(
                    tracker->view_dispatcher, VIEW_SUBMENU_PATTERN_COPYPASTE);
            }

            else {
                submenu_set_selected_item(tracker->pattern_submenu, SUBMENU_PATTERN_LOAD_SONG);
                view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_SUBMENU_PATTERN);
            }
            break;
        }

        case INST_EDITOR_VIEW: {
            submenu_set_selected_item(tracker->instrument_submenu, SUBMENU_INSTRUMENT_LOAD);
            view_dispatcher_switch_to_view(tracker->view_dispatcher, VIEW_SUBMENU_INSTRUMENT);
            break;
        }

        default:
            break;
        }

        return;
    }

    switch(tracker->focus) {
    case EDIT_PATTERN: {
        pattern_edit_event(tracker, event);
        break;
    }

    case EDIT_SEQUENCE: {
        sequence_edit_event(tracker, event);
        break;
    }

    case EDIT_SONGINFO: {
        songinfo_edit_event(tracker, event);
        break;
    }

    case EDIT_INSTRUMENT: {
        instrument_edit_event(tracker, event);
        break;
    }

    case EDIT_PROGRAM: {
        instrument_program_edit_event(tracker, event);
        break;
    }

    default:
        break;
    }
}