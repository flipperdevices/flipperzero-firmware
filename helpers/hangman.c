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

    for(uint8_t j = 0; j < HANGMAN_LETTERS_CNT / HANGMAN_KEYBOARD_ROW; j++) {
        uint8_t y = 29 + j * glyph_h * .94;

        for(uint8_t i = 0; i < HANGMAN_KEYBOARD_ROW; i++) {
            uint8_t x = 42 + i * glyph_w * 1.85;
            uint8_t n = j * HANGMAN_KEYBOARD_ROW + i;

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

void hangman_draw_gallows(Canvas* canvas, HangmanApp* app) {
    const Icon* gallows[HANGMAN_GALLOWS_MAX_STATE] = {&I_1, &I_2, &I_3, &I_4, &I_5, &I_6, &I_7};
    canvas_draw_icon(canvas, 0, 30, gallows[app->gallows_state]);
}

void hangman_choice_letter(HangmanApp* app) {
    if(strchr(app->word, app->pos + 0x10) == NULL) {
        app->opened[app->pos] = HangmanOpenedNotFound;

        if(app->gallows_state < HANGMAN_GALLOWS_MAX_STATE - 1) {
            app->gallows_state++;
        } else {
            app->need_generate = true;
        }
    } else {
        app->opened[app->pos] = HangmanOpenedFound;
    }
}

void hangman_ok_button(Canvas* canvas, uint8_t y, const char* str) {
    const Icon I_ButtonCenter_7x7 = {
        .width = 7,
        .height = 7,
        .frame_count = 1,
        .frame_rate = 0,
        .frames = (const uint8_t* const[]){
            (const uint8_t[]){0x00, 0x1c, 0x22, 0x5d, 0x5d, 0x5d, 0x22, 0x1c}}};

    const uint8_t button_height = 12;
    const uint8_t vertical_offset = 3;
    const uint8_t horizontal_offset = 1;
    const uint8_t string_width = hangman_GetGlyphWidth(&canvas->fb, ' ') * strlen(str) / 2;
    const Icon* icon = &I_ButtonCenter_7x7;
    const uint8_t icon_h_offset = 3;
    const uint8_t icon_width_with_offset = icon->width + icon_h_offset;
    const uint8_t icon_v_offset = icon->height + vertical_offset;
    const uint8_t button_width = string_width + horizontal_offset * 2 + icon_width_with_offset;

    const uint8_t x = (canvas_width(canvas) - button_width) / 2;

    canvas_draw_box(canvas, x, y - button_height, button_width, button_height);

    canvas_draw_line(canvas, x - 1, y, x - 1, y - button_height + 0);
    canvas_draw_line(canvas, x - 2, y, x - 2, y - button_height + 1);
    canvas_draw_line(canvas, x - 3, y, x - 3, y - button_height + 2);

    canvas_draw_line(canvas, x + button_width + 0, y, x + button_width + 0, y - button_height + 0);
    canvas_draw_line(canvas, x + button_width + 1, y, x + button_width + 1, y - button_height + 1);
    canvas_draw_line(canvas, x + button_width + 2, y, x + button_width + 2, y - button_height + 2);

    canvas_invert_color(canvas);
    canvas_draw_icon(canvas, x + horizontal_offset, y - icon_v_offset, &I_ButtonCenter_7x7);
    hangman_draw_utf8_str(
        canvas, x + horizontal_offset + icon_width_with_offset, y - vertical_offset, str);
    canvas_invert_color(canvas);
}

void hangman_text_window(Canvas* canvas, char* txt) {
    canvas_clear(canvas);
    elements_frame(canvas, 20, 13, 88, 37);

    hangman_ok_button(canvas, 50, "Окей");

    uint8_t txt_w = hangman_GetGlyphWidth(&canvas->fb, ' ') * strlen(txt) / 2;
    hangman_draw_utf8_str(
        canvas,
        (canvas_width(canvas) - txt_w) / 2,
        17 + canvas_current_font_height(canvas),
        txt);
}

void hangman_clear_state(HangmanApp* app) {
    app->pos = 0;
    app->gallows_state = HANGMAN_GALLOWS_INIT_STATE;
    memset(app->opened, HangmanOpenedInit, HANGMAN_LETTERS_CNT);
    app->need_generate = false;

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

bool hangman_wait_a_key(HangmanApp* app) {
    for(InputEvent event; !app->need_generate;) {
        if(furi_message_queue_get(app->event_queue, &event, 100) == FuriStatusOk) {
            if(event.type == InputTypeShort) {
                switch(event.key) {
                case InputKeyOk:
                    hangman_choice_letter(app);
                    break;

                case InputKeyBack:
                    return false;

                case InputKeyDown:
                    if(app->pos < HANGMAN_LETTERS_CNT - HANGMAN_KEYBOARD_ROW)
                        app->pos += HANGMAN_KEYBOARD_ROW;
                    break;

                case InputKeyUp:
                    if(app->pos > HANGMAN_KEYBOARD_ROW) app->pos -= HANGMAN_KEYBOARD_ROW;
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
