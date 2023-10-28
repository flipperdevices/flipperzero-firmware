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

    char* word = strdup(furi_string_get_cstr(line));

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

    for(uint8_t j = 0; j < HANGMAN_LETTERS_CNT / HANGMAN_KEYBOARD_ROW; j++) {
        uint8_t y = 29 + j * glyph_h * .94;

        for(uint8_t i = 0; i < HANGMAN_KEYBOARD_ROW; i++) {
            uint8_t x = 42 + i * glyph_w * 1.85;
            uint8_t n = j * HANGMAN_KEYBOARD_ROW + i;

            uint16_t ch = 0x0410 + n;

            if(context->opened[n]) {
                canvas_set_custom_u8g2_font(canvas, u8g2_font_6x12_m_symbols);
                ch = strchr(context->word, n + 0x10) == NULL ? 0x2717 : 0x2713; // ✕ : ✓
            }

            if(n == context->pos) {
                canvas_draw_glyph(canvas, x - 1, y, ch);
            }

            canvas_draw_glyph(canvas, x, y, ch);

            if(context->opened[n]) {
                canvas_set_custom_u8g2_font(canvas, u8g2_font_6x12_t_cyrillic);
            }
        }
    }
}

void hangman_draw_word(Canvas* canvas, HangmanApp* context) {
    canvas_set_color(canvas, ColorBlack);
    canvas_set_custom_u8g2_font(canvas, u8g2_font_6x13B_t_cyrillic);

    uint8_t glyph_w = hangman_GetGlyphWidth(&canvas->fb, 0x20);
    uint8_t center_x = (canvas_width(canvas) - (glyph_w + 3) * strlen(context->word)) / 2;

    uint8_t h = canvas_current_font_height(canvas);
    canvas_set_color(canvas, ColorBlack);

    for(uint8_t i = 0, x = center_x; i < strlen(context->word); i++) {
        if(context->opened[context->word[i] - 0x10]) {
            canvas_draw_glyph(canvas, x, h, context->word[i] + 0x0400); // convert to UCS-2
        }

        x += glyph_w + 3;
    }

    canvas_set_color(canvas, ColorXOR);
    hangman_draw_utf8_str(canvas, center_x, h + 1, 3, context->word_under);
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
    app->word_under = strdup(app->word);
    memset(app->word_under, '_', strlen(app->word));
}

void hangman_draw_gallows(Canvas* canvas, HangmanApp* context) {
    const Icon* gallows[HANGMAN_GALLOWS_MAX_STATE] = {&I_1, &I_2, &I_3, &I_4, &I_5, &I_6, &I_7};
    canvas_draw_icon(canvas, 0, 30, gallows[context->gallows_state]);
}

void hangman_choice_letter(HangmanApp* app) {
    app->opened[app->pos] = true;

    if (strchr(app->word, app->pos + 0x10) == NULL) {
        if(app->gallows_state < HANGMAN_GALLOWS_MAX_STATE - 1) {
            app->gallows_state++;
        }
    }
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
    memset(app->opened, false, HANGMAN_LETTERS_CNT);

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
    free((*app)->word_under);
    free(*app);
}

void hangman_wait_a_key(HangmanApp* app) {
    for(InputEvent event;;) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    hangman_choice_letter(app);
                    break;

                case InputKeyBack:
                    return;

                case InputKeyDown:
                    if(app->pos < HANGMAN_LETTERS_CNT - HANGMAN_KEYBOARD_ROW)
                        app->pos += HANGMAN_KEYBOARD_ROW;
                    break;

                case InputKeyUp:
                    if(app->pos > HANGMAN_KEYBOARD_ROW)
                        app->pos -= HANGMAN_KEYBOARD_ROW;
                    break;

                case InputKeyLeft:
                    if(app->pos > 0) {
                        app->pos--;
                    } else {
                        app->pos = HANGMAN_LETTERS_CNT - 1;
                    }
                    break;

                case InputKeyRight:
                    if(app->pos < HANGMAN_LETTERS_CNT - 1) {
                        app->pos++;
                    } else {
                        app->pos = 0;
                    }
                    break;

                default:
                    break;
                }

                view_port_update(app->view_port);
            }
        }
    }
}
