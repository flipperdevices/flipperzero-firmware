#include "hangman.h"

char* hangman_get_random_word() {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    Stream* stream = file_stream_alloc(storage);
    FuriString* line = furi_string_alloc();

    if(file_stream_open(stream, DICT_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) {
        int32_t offset = furi_hal_random_get() % stream_size(stream);

        if(offset > 0) {
            bool seek_result = stream_seek(stream, offset, StreamOffsetFromStart) &&
                               stream_seek_to_char(stream, '\n', StreamDirectionForward) &&
                               stream_read_line(stream, line);

            if(!seek_result) {
                stream_rewind(stream);
            }

            stream_read_line(stream, line);
        } else {
            stream_read_line(stream, line);
        }
    } else {
        furi_crash(NULL);
    }

    size_t word_len = furi_string_size(line) * 2;
    char* word = malloc(word_len + 1);
    const char* word_bin = furi_string_get_cstr(line);

    // decode to utf-8
    for(size_t i = 0; i < word_len; i += 2) {
        word[i] = 0xD0; // Russian letter prefix code
        word[i + 1] = word_bin[i / 2];
    }
    
    word[word_len - 1] = 0;

    furi_string_free(line);
    file_stream_close(stream);
    stream_free(stream);
    furi_record_close(RECORD_STORAGE);

    return word;
}

void hangman_draw_utf8_str(Canvas* canvas, uint8_t x, uint8_t y, const char* str) {
    FuriStringUTF8State state = FuriStringUTF8StateStarting;
    FuriStringUnicodeValue value = 0;

    for(; *str; str++) {
        furi_string_utf8_decode(*str, &state, &value);
        if(state == FuriStringUTF8StateError) furi_crash(NULL);

        if(state == FuriStringUTF8StateStarting) {
            canvas_draw_glyph(canvas, x, y, value);
            x += hangman_GetGlyphWidth(&canvas->fb, value);
        }
    }
}

void hangman_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

HangmanApp* hangman_app_alloc() {
    HangmanApp* app = malloc(sizeof(HangmanApp));

    furi_hal_random_init();
    app->word = hangman_get_random_word();

    app->view_port = view_port_alloc();
    view_port_draw_callback_set(app->view_port, hangman_render_callback, app);
    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    app->event_queue = furi_message_queue_alloc(1, sizeof(InputEvent));
    view_port_input_callback_set(app->view_port, hangman_input_callback, app->event_queue);

    return app;
}

void hangman_app_free(HangmanApp** app) {
    furi_assert(*app);

    view_port_enabled_set((*app)->view_port, false);
    gui_remove_view_port((*app)->gui, (*app)->view_port);
    view_port_free((*app)->view_port);

    furi_record_close(RECORD_GUI);
    furi_message_queue_free((*app)->event_queue);

    free((*app)->word);
    free(*app);
}

void hangman_wait_a_key(HangmanApp* app) {
    for(InputEvent event;;) {
        if(furi_message_queue_get(app->event_queue, &event, 0) == FuriStatusOk) {
            if(event.type == InputTypePress) break;
        }
    }
}
