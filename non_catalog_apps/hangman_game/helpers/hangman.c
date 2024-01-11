#include "hangman.h"

HangmanWord hangman_get_random_word(const char* dict_file, uint16_t unicode_base) {
    CONST storage = furi_record_open(RECORD_STORAGE);

    CONST stream = file_stream_alloc(storage);
    CONST line = furi_string_alloc();

    if(file_stream_open(stream, dict_file, FSAM_READ, FSOM_OPEN_EXISTING)) {
        CONST offset = furi_hal_random_get() % stream_size(stream);

        if(offset > 0) {
            CONST seek_result = stream_seek(stream, offset, StreamOffsetFromStart) &&
                                stream_read_line(stream, line);

            if(!seek_result) {
                stream_rewind(stream);
            }
        }
        stream_read_line(stream, line);
    } else {
        furi_crash(NULL);
    }

    furi_string_trim(line, "\n");
    CONST binary_str = furi_string_get_cstr(line);
    CONST len = strlen(binary_str);

    HangmanWord word;

    word.len = len;
    word.arr = malloc(len * sizeof(uint16_t));

    for(size_t i = 0; i < len; i++) {
        word.arr[i] = binary_str[i] + unicode_base;
    }

    furi_string_free(line);
    file_stream_close(stream);
    stream_free(stream);
    furi_record_close(RECORD_STORAGE);

    return word;
}

void hangman_draw_keyboard(Canvas* canvas, HangmanApp* app) {
    canvas_set_color(canvas, ColorBlack);

    hangman_set_font(canvas, 12);
    CONST glyph_w = canvas_glyph_width(canvas, ' ');
    CONST glyph_h = canvas_current_font_height(canvas);

    CONST gap = ROUND_UP_TO(canvas_width(canvas) - 42, app->lang->keyboard_cols) - glyph_w;
    CONST y_offset = 29 + MAX(4 - app->lang->keyboard_rows, 0) * glyph_h / 2;

    for(uint8_t j = 0; j < app->lang->keyboard_rows; j++) {
        CONST y = y_offset + j * glyph_h * .94;

        for(uint8_t i = 0; i < app->lang->keyboard_cols; i++) {
            CONST x = 42 + (glyph_w + (int)gap) * i;
            CONST n = j * app->lang->keyboard_cols + i;

            if(n > app->lang->letters_cnt - 1) {
                break;
            }

            uint16_t ch = app->lang->letters[n];

            if(app->opened[n] != HangmanOpenedInit) {
                ch = app->opened[n] == HangmanOpenedNotFound ? 0x2717 : 0x2713; // ✕ : ✓
            }

            if(n == app->pos) {
                canvas_draw_box(canvas, x - 1, y - glyph_h + 2, glyph_w + 1, glyph_h);
                canvas_set_color(canvas, ColorXOR);
                canvas_draw_glyph(canvas, x, y, ch);
                canvas_set_color(canvas, ColorBlack);
            } else {
                canvas_draw_glyph(canvas, x, y, ch);
            }
        }
    }
}

uint8_t hangman_l2p(HangmanApp* app, uint16_t value) {
    for(uint8_t i = 0; i < HANGMAN_MAX_ALP_SIZE; i++) {
        if(app->lang->letters[i] == value) {
            return i;
        }
    }

    furi_crash(NULL);
}

void hangman_draw_word(Canvas* canvas, HangmanApp* app) {
    hangman_set_font(canvas, 13);

    CONST glyph_w = canvas_glyph_width(canvas, ' ');
    CONST gap = app->lang->word_letters_gap;

    CONST center_x = (canvas_width(canvas) - (glyph_w + gap) * app->word.len) / 2;

    CONST h = canvas_current_font_height(canvas);
    canvas_set_color(canvas, ColorBlack);

    for(uint8_t i = 0, x = center_x; i < app->word.len; i++) {
        canvas_draw_glyph(canvas, x, h + 1, '_');

        if(app->opened[hangman_l2p(app, app->word.arr[i])]) {
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_glyph(canvas, x, h, app->word.arr[i]);
        }

        x += glyph_w + gap;
    }
}

void hangman_draw_menu(Canvas* canvas, HangmanApp* app) {
    hangman_set_font(canvas, 12);

    uint8_t max_txt_w = 0;
    for(uint8_t i = 0; i < app->menu_cnt; i += 2) {
        max_txt_w = MAX(max_txt_w, hangman_string_length(app->menu[i]));
    }

    max_txt_w *= canvas_glyph_width(canvas, ' ');
    CONST txt_h = canvas_current_font_height(canvas);

    CONST menu_cnt = app->menu_cnt / 2;
    CONST rows = MIN(menu_cnt, HANGMAN_MAX_MENU_SIZE);

    CONST w = max_txt_w + 30;
    CONST h = txt_h * rows + 6;
    CONST x = (canvas_width(canvas) - w) / 2;
    CONST y = (canvas_height(canvas) - h) / 2;

    hangman_window(canvas, x, y, w, h);

    CONST txt_x = (canvas_width(canvas) - max_txt_w) / 2;

    if(menu_cnt > HANGMAN_MAX_MENU_SIZE) {
        elements_scrollbar(canvas, app->menu_item, menu_cnt);
    }

    for(uint8_t i = 0; i < rows; i++) {
        CONST txt_y = y + (i + 1) * txt_h;
        CONST menu_item = app->menu_frame_position + i;

        canvas_set_color(canvas, ColorBlack);

        if(menu_item == app->menu_item) {
            canvas_draw_box(canvas, x, txt_y - txt_h + 3, w, txt_h);
            canvas_invert_color(canvas);
        }

        hangman_draw_utf8_str(canvas, txt_x, txt_y, app->menu[menu_item * 2]);
    }
}

void hangman_render_callback(Canvas* canvas, void* ctx) {
    CONST app = (HangmanApp*)ctx;

    canvas_clear(canvas);

    if(app->menu_show) {
        hangman_draw_menu(canvas, app);
    } else if(app->lang != NULL) {
        hangman_draw_word(canvas, app);
        hangman_draw_gallows(canvas, app);
        hangman_draw_keyboard(canvas, app);

        if(app->eog != HangmanGameOn) {
            if(app->eog == HangmanGameLoose) {
                hangman_text_window(canvas, app->lang->message_ok, app->lang->message_lose);
            } else {
                hangman_text_window(canvas, app->lang->message_ok, app->lang->message_won);
            }
            app->need_generate = true;
        }
    }
}

void hangman_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    CONST event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

void hangman_choice_letter(HangmanApp* app) {
    bool found = false;

    for(size_t i = 0; i < app->word.len; i++) {
        if(app->word.arr[i] == app->lang->letters[app->pos]) {
            found = true;
            break;
        }
    }

    if(found) {
        app->eog = HangmanGameWin;
        app->opened[app->pos] = HangmanOpenedFound;

        // Checking if all letters were opened
        for(uint8_t i = 0; i < app->word.len; i++) {
            CONST pos = hangman_l2p(app, app->word.arr[i]);

            if(app->opened[pos] != HangmanOpenedFound) {
                app->eog = HangmanGameOn;
                break;
            }
        }
    } else {
        if(app->opened[app->pos] != HangmanOpenedNotFound) {
            app->gallows_state++;
            app->opened[app->pos] = HangmanOpenedNotFound;

            if(app->gallows_state >= HANGMAN_GALLOWS_MAX_STATE - 1) {
                app->eog = HangmanGameLoose;

                // Open the non-guessed letters
                for(size_t i = 0; i < app->word.len; i++) {
                    CONST pos = hangman_l2p(app, app->word.arr[i]);

                    if(app->opened[pos] != HangmanOpenedFound) {
                        app->opened[pos] = HangmanOpenedNotFound;
                    }
                }
            }
        }
    }
}

void hangman_clear_state(HangmanApp* app) {
    app->pos = 0;
    app->gallows_state = HANGMAN_GALLOWS_INIT_STATE;
    app->eog = HangmanGameOn;

    for(int i = 0; i < HANGMAN_MAX_ALP_SIZE; i++) {
        app->opened[i] = HangmanOpenedInit;
    }

    free(app->word.arr);
    app->word = (HangmanWord){NULL, 0};

    if(app->lang != NULL) {
        app->word = hangman_get_random_word(app->lang->dict_file, app->lang->unicode_base);
    }

    app->need_generate = false;
}

int hangman_read_int(Stream* stream) {
    CONST line = furi_string_alloc();

    if(!stream_read_line(stream, line)) {
        furi_crash(NULL);
    }

    CONST result = strtol(furi_string_get_cstr(line), NULL, 10);
    furi_string_free(line);
    return result;
}

char* hangman_read_str(Stream* stream) {
    CONST line = furi_string_alloc();

    if(!stream_read_line(stream, line)) {
        furi_crash(NULL);
    }

    furi_string_trim(line);
    CONST result = strdup(furi_string_get_cstr(line));
    furi_string_free(line);
    return result;
}

char* hangman_add_asset_path(const char* filename) {
    CONST full_path = furi_string_alloc_set_str(APP_ASSETS_PATH(""));
    furi_string_cat_str(full_path, filename);

    CONST file_full_path = furi_string_get_cstr(full_path);
    CONST result = strdup(file_full_path);
    furi_string_free(full_path);
    return result;
}

HangmanLangConfig* hangman_load_config(char* meta_file) {
    CONST storage = furi_record_open(RECORD_STORAGE);
    CONST stream = file_stream_alloc(storage);
    CONST line = furi_string_alloc();
    HangmanLangConfig* config = malloc(sizeof(HangmanLangConfig));

    if(!file_stream_open(stream, meta_file, FSAM_READ, FSOM_OPEN_EXISTING)) {
        furi_crash(NULL);
    }

    if(!stream_read_line(stream, line)) {
        furi_crash(NULL);
    }
    config->dict_file = hangman_add_asset_path(furi_string_get_cstr(line));
    config->keyboard_cols = hangman_read_int(stream);
    config->word_letters_gap = hangman_read_int(stream);

    CONST unicode_base = hangman_read_str(stream);
    config->unicode_base = unicode_base[0] == '+' ? strtol(unicode_base, NULL, 16) : 0;

    // letters
    config->letters_cnt = 0;

    const char* token = hangman_read_str(stream);
    while(*token && config->letters_cnt < HANGMAN_MAX_ALP_SIZE) {
        char* end;
        int num = strtol(token, &end, 16);
        if(num == 0) break;

        config->letters[config->letters_cnt++] = num;

        if(*end == ' ') {
            token = end + 1; // +1 because of space
        } else {
            break;
        }
    }

    config->keyboard_rows = ROUND_UP_TO(config->letters_cnt, config->keyboard_cols);

    config->message_ok = hangman_read_str(stream);
    config->message_won = hangman_read_str(stream);
    config->message_lose = hangman_read_str(stream);

    furi_string_free(line);
    file_stream_close(stream);
    stream_free(stream);
    furi_record_close(RECORD_STORAGE);

    return config;
}

void hangman_load_lang(HangmanApp* app) {
    CONST meta_file = hangman_add_asset_path(app->menu[app->menu_item * 2 + 1]);
    app->lang = hangman_load_config(meta_file);
    free(meta_file);
}

HangmanApp* hangman_app_alloc() {
    HangmanApp* app = malloc(sizeof(HangmanApp));
    furi_hal_random_init();
    app->menu_item = 0;
    app->menu_frame_position = 0;

    app->menu = hangman_menu_read(&app->menu_cnt);
    if(app->menu_cnt & 1 || app->menu_cnt < 2) {
        furi_crash(NULL);
    }

    app->menu_show = app->menu_cnt > 2;
    if(!app->menu_show) {
        hangman_load_lang(app);
    }

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

    hangman_free_menu_data((*app)->menu, (*app)->menu_cnt);

    free((*app)->word.arr);

    if((*app)->lang != NULL) {
        free((*app)->lang->dict_file);
        free((*app)->lang->message_ok);
        free((*app)->lang->message_lose);
        free((*app)->lang->message_won);
        free((*app)->lang);
    }

    free(*app);
}
