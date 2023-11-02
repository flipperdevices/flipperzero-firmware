#include "hangman.h"
#include "hangman_fonts.h"

char* hangman_get_random_word(const char* dict_file) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    Stream* stream = file_stream_alloc(storage);
    FuriString* line = furi_string_alloc();

    if(file_stream_open(stream, dict_file, FSAM_READ, FSOM_OPEN_EXISTING)) {
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

    furi_string_trim(line);

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
    uint8_t glyph_w = canvas_glyph_width(canvas, ' ');
    uint8_t glyph_h = canvas_current_font_height(canvas);

    for(uint8_t j = 0; j < app->lang->keyboard_rows; j++) {
        uint8_t y = 29 + j * glyph_h * .94;

        for(uint8_t i = 0; i < app->lang->keyboard_cols; i++) {
            uint8_t x = 42 + i * glyph_w * 1.85;
            uint8_t n = j * app->lang->keyboard_cols + i;

            if(n > app->lang->letters_cnt - 1) {
                break;
            }

            uint16_t ch = app->lang->unicode_base + app->lang->letters[n];

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

    uint8_t glyph_w = canvas_glyph_width(canvas, ' ');
    uint8_t gap = app->lang->keyboard_gap;

    uint8_t center_x = (canvas_width(canvas) - (glyph_w + gap) * strlen(app->word)) / 2;

    uint8_t h = canvas_current_font_height(canvas);
    canvas_set_color(canvas, ColorBlack);

    for(uint8_t i = 0, x = center_x; i < strlen(app->word); i++) {
        if(app->opened[app->word[i] - app->lang->first_letter_offset]) {
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_glyph(
                canvas,
                x,
                h,
                app->word[i] + app->lang->unicode_base -
                    app->lang->first_letter_offset); // convert to UCS-2
        }

        canvas_set_color(canvas, ColorXOR);
        canvas_draw_glyph(canvas, x, h + 1, '_');

        x += glyph_w + app->lang->keyboard_gap;
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
            hangman_text_window(canvas, app->lang->message_ok, app->lang->message_loose);
        } else {
            hangman_text_window(canvas, app->lang->message_ok, app->lang->message_won);
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
    if(strchr(app->word, app->lang->letters[app->pos] + app->lang->first_letter_offset) == NULL) {
        app->gallows_state++;
        app->opened[app->pos] = HangmanOpenedNotFound;

        if(app->gallows_state >= HANGMAN_GALLOWS_MAX_STATE - 1) {
            app->eog = HangmanGameLoose;

            // Open the non-guessed letters
            for(uint8_t i = 0; i < strlen(app->word); i++) {
                int letter = app->word[i] - app->lang->first_letter_offset;

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
            if(app->opened[app->word[i] - app->lang->first_letter_offset] != HangmanOpenedFound) {
                app->eog = HangmanGameOn;
                break;
            }
        }
    }
}

void hangman_clear_state(HangmanApp* app) {
    app->pos = 0;
    app->gallows_state = HANGMAN_GALLOWS_INIT_STATE;
    memset(app->opened, HangmanOpenedInit, app->lang->letters_cnt);
    app->need_generate = false;
    app->eog = HangmanGameOn;

    app->word = hangman_get_random_word(app->lang->dict_file);
}

int hangman_read_int(Stream* stream) {
    FuriString* line = furi_string_alloc();

    if(!stream_read_line(stream, line)) {
        furi_crash(NULL);
    }

    int result = strtol(furi_string_get_cstr(line), NULL, 10);
    furi_string_free(line);
    return result;
}

char* hangman_read_str(Stream* stream) {
    FuriString* line = furi_string_alloc();

    if(!stream_read_line(stream, line)) {
        furi_crash(NULL);
    }

    furi_string_trim(line);
    char* result = strdup(furi_string_get_cstr(line));
    furi_string_free(line);
    return result;
}

HangmanLangConfig* hangman_load_config() {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    Stream* stream = file_stream_alloc(storage);
    FuriString* line = furi_string_alloc();
    HangmanLangConfig* config = malloc(sizeof(HangmanLangConfig));

    if(!file_stream_open(stream, HANGMAN_META_FILE, FSAM_READ, FSOM_OPEN_EXISTING)) {
        furi_crash(NULL);
    }

    FuriString* dict_path = furi_string_alloc_set_str(APP_ASSETS_PATH(""));
    if(!stream_read_line(stream, line)) {
        furi_crash(NULL);
    }
    furi_string_cat(dict_path, line);
    config->dict_file = strdup(furi_string_get_cstr(dict_path));
    furi_string_free(dict_path);

    config->keyboard_cols = hangman_read_int(stream);
    config->keyboard_gap = hangman_read_int(stream);
    config->first_letter_offset = hangman_read_int(stream);

    // letters
    config->unicode_base = 0xFFFF;
    config->letters_cnt = 0;

    const char* token = hangman_read_str(stream);
    while(*token && config->letters_cnt < HANGMAN_MAX_ALP_SIZE) {
        char* end;
        int num = strtol(token, &end, 16);
        if(num == 0) break;

        config->letters[config->letters_cnt++] = num;
        if(config->unicode_base > num) config->unicode_base = num;
        token = end + 1; // +1 because of space
    }

    config->keyboard_rows = ceil((float)config->letters_cnt / config->keyboard_cols);

    for(int i = 0; i < config->letters_cnt; i++) config->letters[i] -= config->unicode_base;

    config->message_ok = hangman_read_str(stream);
    config->message_won = hangman_read_str(stream);
    config->message_loose = hangman_read_str(stream);

    furi_string_free(line);
    file_stream_close(stream);
    stream_free(stream);
    furi_record_close(RECORD_STORAGE);

    return config;
}

HangmanApp* hangman_app_alloc() {
    HangmanApp* app = malloc(sizeof(HangmanApp));

    app->lang = hangman_load_config();

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
    free((*app)->lang->dict_file);
    free((*app)->lang->message_ok);
    free((*app)->lang->message_loose);
    free((*app)->lang->message_won);
    free((*app)->lang);
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
                    app->pos += app->lang->keyboard_cols;

                    if(app->pos >= app->lang->letters_cnt) {
                        app->pos %= app->lang->keyboard_cols;
                    }

                    break;

                case InputKeyUp:
                    if(app->pos >= app->lang->keyboard_cols) {
                        app->pos -= app->lang->keyboard_cols;
                    } else {
                        app->pos += app->lang->keyboard_cols * (app->lang->keyboard_rows - 1);
                        if(app->pos >= app->lang->letters_cnt) {
                            app->pos -= app->lang->keyboard_cols;
                        }
                    }
                    break;

                case InputKeyLeft:
                    if(app->pos > 0) {
                        app->pos--;
                    } else {
                        app->pos = app->lang->letters_cnt - 1;
                    }
                    break;

                case InputKeyRight:
                    if(app->pos < app->lang->letters_cnt - 1) {
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
