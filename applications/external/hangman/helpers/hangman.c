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

void hangman_draw_keyboard(Canvas* canvas, HangmanApp* app) {
    canvas_set_color(canvas, ColorBlack);

    canvas_set_custom_u8g2_font(canvas, u8g2_font_6x12_t_cyrillic);
    uint8_t glyph_w = hangman_GetGlyphWidth(&canvas->fb, ' ');
    uint8_t glyph_h = canvas_current_font_height(canvas);

    for(uint8_t j = 0; j < HANGMAN_LETTERS_CNT / HANGMAN_KEYBOARD_COLS; j++) {
        uint8_t y = 29 + j * glyph_h * .94;

        for(uint8_t i = 0; i < HANGMAN_KEYBOARD_COLS; i++) {
            uint8_t x = 42 + i * glyph_w * 1.85;
            uint8_t n = j * HANGMAN_KEYBOARD_COLS + i;

            uint16_t ch = 0x0410 + n;

            if(app->opened[n] != HangmanOpenedInit) {
                canvas_set_custom_u8g2_font(canvas, u8g2_font_6x12_m_symbols);
                ch = app->opened[n] == HangmanOpenedNotFound ? 0x2717 : 0x2713; // ✕ : ✓
            }

            if(n == app->pos) {
                canvas_draw_glyph(canvas, x - 1, y, ch); // Made bold
            }

            canvas_draw_glyph(canvas, x, y, ch);

            if(app->opened[n]) {
                canvas_set_custom_u8g2_font(canvas, u8g2_font_6x12_t_cyrillic);
            }
        }
    }
}

void hangman_draw_word(Canvas* canvas, HangmanApp* app) {
    canvas_set_custom_u8g2_font(canvas, u8g2_font_6x13B_t_cyrillic);

    uint8_t glyph_w = hangman_GetGlyphWidth(&canvas->fb, ' ');
    uint8_t center_x =
        (canvas_width(canvas) - (glyph_w + HANGMAN_KEYBOARD_GAP) * strlen(app->word)) / 2;

    uint8_t h = canvas_current_font_height(canvas);
    canvas_set_color(canvas, ColorBlack);

    for(uint8_t i = 0, x = center_x; i < strlen(app->word); i++) {
        if(app->opened[app->word[i] - 0x10]) {
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_glyph(canvas, x, h, app->word[i] + 0x0400); // convert to UCS-2
        }

        canvas_set_color(canvas, ColorXOR);
        canvas_draw_glyph(canvas, x, h + 1, '_');

        x += glyph_w + HANGMAN_KEYBOARD_GAP;
    }
}

void hangman_render_callback(Canvas* canvas, void* ctx) {
    HangmanApp* app = (HangmanApp*)ctx;

    canvas_clear(canvas);

    hangman_draw_word(canvas, app);
    hangman_draw_gallows(canvas, app);
    hangman_draw_keyboard(canvas, app);

    if(app->eog != HangmanGameOn) {
        if(app->eog == HangmanGameLoose) {
            hangman_text_window(canvas, "You Lose");
        } else {
            hangman_text_window(canvas, "You Win");
        }
        app->need_generate = true;
    }
}

void hangman_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

void hangman_choice_letter(HangmanApp* app) {
    if(strchr(app->word, app->pos + 0x10) == NULL) {
        app->gallows_state++;
        app->opened[app->pos] = HangmanOpenedNotFound;

        if(app->gallows_state >= HANGMAN_GALLOWS_MAX_STATE - 1) {
            app->eog = HangmanGameLoose;

            // Open the non-guessed letters
            for(uint8_t i = 0; i < strlen(app->word); i++) {
                int letter = app->word[i] - 0x10;

                if(app->opened[letter] != HangmanOpenedFound) {
                    app->opened[letter] = HangmanOpenedNotFound;
                }
            }
        }
    } else {
        app->eog = HangmanGameWin;
        app->opened[app->pos] = HangmanOpenedFound;

        // Checking if all letters were opened
        for(uint8_t i = 0; i < strlen(app->word); i++) {
            if(app->opened[app->word[i] - 0x10] != HangmanOpenedFound) {
                app->eog = HangmanGameOn;
                break;
            }
        }
    }
}

void hangman_clear_state(HangmanApp* app) {
    app->pos = 0;
    app->gallows_state = HANGMAN_GALLOWS_INIT_STATE;
    memset(app->opened, HangmanOpenedInit, HANGMAN_LETTERS_CNT);
    app->need_generate = false;
    app->eog = HangmanGameOn;

    app->word = hangman_get_random_word();
}

HangmanApp* hangman_app_alloc() {
    HangmanApp* app = malloc(sizeof(HangmanApp));

    furi_hal_random_init();
    hangman_clear_state(app);

    app->view_port = view_port_alloc();
    view_port_draw_callback_set(app->view_port, hangman_render_callback, app);
    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    app->event_queue = furi_message_queue_alloc(10, sizeof(InputEvent));
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

bool hangman_wait_close_window(HangmanApp* app) {
    InputEvent event;

    for(;;) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    hangman_clear_state(app);
                    view_port_update(app->view_port);
                    return true;

                case InputKeyBack:
                    return false;
                default:
                    break;
                }
            }
        }
    }
}

bool hangman_main_loop(HangmanApp* app) {
    InputEvent event;

    while(app->eog == HangmanGameOn && !app->need_generate) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    hangman_choice_letter(app);
                    break;

                case InputKeyBack:
                    return false;

                case InputKeyDown:
                    if(app->pos < HANGMAN_LETTERS_CNT - HANGMAN_KEYBOARD_COLS) {
                        app->pos += HANGMAN_KEYBOARD_COLS;
                    } else {
                        app->pos -= HANGMAN_KEYBOARD_COLS * (HANGMAN_KEYBOARD_ROWS - 1);
                    }
                    break;

                case InputKeyUp:
                    if(app->pos >= HANGMAN_KEYBOARD_COLS) {
                        app->pos -= HANGMAN_KEYBOARD_COLS;
                    } else {
                        app->pos += HANGMAN_KEYBOARD_COLS * (HANGMAN_KEYBOARD_ROWS - 1);
                    }
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

    return true;
}
