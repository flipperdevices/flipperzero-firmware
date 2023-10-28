#include "hangman.h"

char* hangman_get_random_word() {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    Stream* stream = file_stream_alloc(storage);
    FuriString* line = furi_string_alloc();

    if(file_stream_open(stream, HANGMAN_DICT_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) {
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

void hangman_draw_keyboard(Canvas* canvas, HangmanApp* context) {
    canvas_set_color(canvas, ColorBlack);

    canvas_set_custom_u8g2_font(canvas, u8g2_font_6x12_t_cyrillic);
    uint8_t glyph_w = hangman_GetGlyphWidth(&canvas->fb, 0x20);
    uint8_t glyph_h = canvas_current_font_height(canvas);

    for(uint8_t j = 0; j < 4; j++) {
        uint8_t y = 29 + j * glyph_h * .94;

        for(uint8_t i = 0; i < 8; i++) {
            uint8_t x = 42 + i * glyph_w * 1.85;
            uint8_t n = j * 8 + i;

            if(n == context->pos) {
                canvas_draw_glyph(canvas, x - 1, y, 0x0410 + n);
            }

            canvas_draw_glyph(canvas, x, y, 0x0410 + n);
        }
    }
}

void hangman_draw_word(Canvas* canvas, HangmanApp* context) {
    canvas_set_color(canvas, ColorBlack);
    canvas_set_custom_u8g2_font(canvas, u8g2_font_6x13B_t_cyrillic);

    uint8_t glyph_w = hangman_GetGlyphWidth(&canvas->fb, 0x20);
    uint8_t center_x = (canvas_width(canvas) - (glyph_w + 3) * strlen(context->word) / 2) / 2;

    uint8_t h = canvas_current_font_height(canvas);
    canvas_set_color(canvas, ColorBlack);
    hangman_draw_utf8_str(canvas, center_x, h, 3, context->word);

    canvas_set_color(canvas, ColorXOR);
    hangman_draw_utf8_str(canvas, center_x, h + 1, 3, context->word_guessed);
}

void hangman_draw_utf8_str(Canvas* canvas, uint8_t x, uint8_t y, uint8_t space, const char* str) {
    FuriStringUTF8State state = FuriStringUTF8StateStarting;
    FuriStringUnicodeValue value = 0;

    for(; *str; str++) {
        furi_string_utf8_decode(*str, &state, &value);
        if(state == FuriStringUTF8StateError) furi_crash(NULL);

        if(state == FuriStringUTF8StateStarting) {
            canvas_draw_glyph(canvas, x, y, value);
            x += hangman_GetGlyphWidth(&canvas->fb, value) + space;
        }
    }
}

void hangman_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

void hangman_generate_word(HangmanApp* app) {
    app->word = hangman_get_random_word();
    app->word_guessed = malloc(strlen(app->word) + 1);
    // div by 2 because of UTF-8
    size_t len = strlen(app->word) / 2;
    memset(app->word_guessed, '_', len);
    app->word_guessed[len] = 0;
}

void hangman_draw_gallows(Canvas *canvas, HangmanApp* context) {
    const Icon* gallows[HANGMAN_GALLOWS_MAX_STATE] = {&I_1, &I_2, &I_3, &I_4, &I_5, &I_6, &I_7};
    canvas_draw_icon(canvas, 0, 30, gallows[context->gallows_state]);
}

HangmanApp* hangman_app_alloc() {
    HangmanApp* app = malloc(sizeof(HangmanApp));

    furi_hal_random_init();
    hangman_generate_word(app);

    app->view_port = view_port_alloc();
    view_port_draw_callback_set(app->view_port, hangman_render_callback, app);
    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    app->event_queue = furi_message_queue_alloc(10, sizeof(InputEvent));
    view_port_input_callback_set(app->view_port, hangman_input_callback, app->event_queue);

    app->pos = 0;
    app->gallows_state = HANGMAN_GALLOWS_INIT_STATE;
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
    free((*app)->word_guessed);
    free(*app);
}

void hangman_wait_a_key(HangmanApp* app) {
    for(InputEvent event;;) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyBack:
                    return;

                case InputKeyDown:
                    if(app->pos < 33 - 8) app->pos += 8;
                    break;

                case InputKeyUp:
                    if(app->pos > 8) app->pos -= 8;
                    break;

                case InputKeyLeft:
                    if(app->pos > 0) app->pos--;
                    break;

                case InputKeyRight:
                    if(app->pos < 33) app->pos++;
                    break;

                default:
                    break;
                }

                if (++app->gallows_state >= HANGMAN_GALLOWS_MAX_STATE) {
                    app->gallows_state = HANGMAN_GALLOWS_INIT_STATE;
                }

                view_port_update(app->view_port);
            }
        }
    }
}
