#include "hangman.h"
#include "helpers/hangman_fonts.h"

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

void hangman_draw_menu(Canvas* canvas, HangmanApp* app) {
    canvas_set_custom_u8g2_font(canvas, u8g2_font_6x12_t_cyrillic);

    uint8_t max_txt_w = 0;
    for(uint8_t i = 0; i < app->menu_cnt; i += 2) {
        uint8_t txt_w = hangman_string_length(app->menu[i]);
        if(txt_w > max_txt_w) {
            max_txt_w = txt_w;
        }
    }

    max_txt_w *= canvas_glyph_width(canvas, ' ');
    uint8_t txt_h = canvas_current_font_height(canvas);

    uint8_t w = max_txt_w + 30;
    uint8_t h = txt_h * app->menu_cnt / 2 + 6;
    uint8_t x = (canvas_width(canvas) - w) / 2;
    uint8_t y = (canvas_height(canvas) - h) / 2;

    hangman_window(canvas, x, y, w, h);

    uint8_t txt_x = (canvas_width(canvas) - max_txt_w) / 2;

    for(uint8_t i = 0, menu_item = 0; i < app->menu_cnt; i += 2, menu_item++) {
        uint8_t txt_y = y + (menu_item + 1) * txt_h;

        canvas_set_color(canvas, ColorBlack);

        if(menu_item == app->menu_item) {
            canvas_draw_box(canvas, x, txt_y - txt_h + 3, w, txt_h);
            canvas_invert_color(canvas);
        }

        hangman_draw_utf8_str(canvas, txt_x, txt_y, app->menu[i]);
    }
}

void hangman_render_callback(Canvas* canvas, void* ctx) {
    HangmanApp* app = (HangmanApp*)ctx;

    canvas_clear(canvas);

    if(app->menu_show) {
        hangman_draw_menu(canvas, app);
    } else {
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
    app->need_generate = false;
    app->eog = HangmanGameOn;

    if(app->word != NULL) {
        free(app->word);
    }

    if(app->lang != NULL) {
        memset(app->opened, HangmanOpenedInit, app->lang->letters_cnt);
        app->word = hangman_get_random_word(app->lang->dict_file);
    }
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

char* hangman_add_asset_path(const char* filename) {
    FuriString* full_path = furi_string_alloc_set_str(APP_ASSETS_PATH(""));
    furi_string_cat_str(full_path, filename);

    const char* file_full_path = furi_string_get_cstr(full_path);
    char* result = strdup(file_full_path);
    furi_string_free(full_path);
    return result;
}

HangmanLangConfig* hangman_load_config(char* meta_file) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    Stream* stream = file_stream_alloc(storage);
    FuriString* line = furi_string_alloc();
    HangmanLangConfig* config = malloc(sizeof(HangmanLangConfig));

    if(!file_stream_open(stream, meta_file, FSAM_READ, FSOM_OPEN_EXISTING)) {
        furi_crash(NULL);
    }

    if(!stream_read_line(stream, line)) {
        furi_crash(NULL);
    }
    config->dict_file = hangman_add_asset_path(furi_string_get_cstr(line));
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

        if(*end == ' ') {
            token = end + 1; // +1 because of space
        } else {
            break;
        }
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

void hangman_load_lang(HangmanApp* app) {
    char* meta_file = hangman_add_asset_path(app->menu[app->menu_item * 2 + 1]);
    app->lang = hangman_load_config(meta_file);
    free(meta_file);
}

HangmanApp* hangman_app_alloc() {
    HangmanApp* app = malloc(sizeof(HangmanApp));
    furi_hal_random_init();
    app->menu_item = 0;

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

    if((*app)->word != NULL) {
        free((*app)->word);
    }
    if((*app)->lang != NULL) {
        free((*app)->lang->dict_file);
        free((*app)->lang->message_ok);
        free((*app)->lang->message_loose);
        free((*app)->lang->message_won);
        free((*app)->lang);
    }

    free(*app);
}
