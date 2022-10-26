#include "i2ctools_i.h"

#define APP_NAME "I2C Tools"

#define SCAN_MENU_TEXT "Scan"
#define SCAN_MENU_X 75
#define SCAN_MENU_Y 6

#define SNIFF_MENU_TEXT "Sniff"
#define SNIFF_MENU_X 75
#define SNIFF_MENU_Y 20

#define SEND_MENU_TEXT "Send"
#define SEND_MENU_X 75
#define SEND_MENU_Y 34

#define PLAY_MENU_TEXT "Play"
#define PLAY_MENU_X 75
#define PLAY_MENU_Y 48

void i2ctools_draw_main_menu(Canvas* canvas, i2cTools* i2ctools) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, 0, 0, 128, 64, 3);
    canvas_draw_icon(canvas, 2, 13, &I_passport_bad3_46x49);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 3, 3, AlignLeft, AlignTop, APP_NAME);

    switch(i2ctools->main_menu_index) {
    case 0:
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_str_aligned(
            canvas, SNIFF_MENU_X, SNIFF_MENU_Y, AlignLeft, AlignTop, SNIFF_MENU_TEXT);
        canvas_draw_str_aligned(
            canvas, SEND_MENU_X, SEND_MENU_Y, AlignLeft, AlignTop, SEND_MENU_TEXT);
        canvas_draw_str_aligned(
            canvas, PLAY_MENU_X, PLAY_MENU_Y, AlignLeft, AlignTop, PLAY_MENU_TEXT);

        canvas_draw_rbox(canvas, 60, SCAN_MENU_Y - 2, 60, 13, 3);
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_str_aligned(
            canvas, SCAN_MENU_X, SCAN_MENU_Y, AlignLeft, AlignTop, SCAN_MENU_TEXT);
        break;

    case 1:
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_str_aligned(
            canvas, SCAN_MENU_X, SCAN_MENU_Y, AlignLeft, AlignTop, SCAN_MENU_TEXT);
        canvas_draw_str_aligned(
            canvas, SEND_MENU_X, SEND_MENU_Y, AlignLeft, AlignTop, SEND_MENU_TEXT);
        canvas_draw_str_aligned(
            canvas, PLAY_MENU_X, PLAY_MENU_Y, AlignLeft, AlignTop, PLAY_MENU_TEXT);

        canvas_draw_rbox(canvas, 60, SNIFF_MENU_Y - 2, 60, 13, 3);
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_str_aligned(
            canvas, SNIFF_MENU_X, SNIFF_MENU_Y, AlignLeft, AlignTop, SNIFF_MENU_TEXT);
        break;

    case 2:
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_str_aligned(
            canvas, SCAN_MENU_X, SCAN_MENU_Y, AlignLeft, AlignTop, SCAN_MENU_TEXT);
        canvas_draw_str_aligned(
            canvas, SNIFF_MENU_X, SNIFF_MENU_Y, AlignLeft, AlignTop, SNIFF_MENU_TEXT);
        canvas_draw_str_aligned(
            canvas, PLAY_MENU_X, PLAY_MENU_Y, AlignLeft, AlignTop, PLAY_MENU_TEXT);

        canvas_draw_rbox(canvas, 60, SEND_MENU_Y - 2, 60, 13, 3);
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_str_aligned(
            canvas, SEND_MENU_X, SEND_MENU_Y, AlignLeft, AlignTop, SEND_MENU_TEXT);
        break;

    case 3:
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_str_aligned(
            canvas, SCAN_MENU_X, SCAN_MENU_Y, AlignLeft, AlignTop, SCAN_MENU_TEXT);
        canvas_draw_str_aligned(
            canvas, SNIFF_MENU_X, SNIFF_MENU_Y, AlignLeft, AlignTop, SNIFF_MENU_TEXT);
        canvas_draw_str_aligned(
            canvas, SEND_MENU_X, SEND_MENU_Y, AlignLeft, AlignTop, SEND_MENU_TEXT);

        canvas_draw_rbox(canvas, 60, PLAY_MENU_Y - 2, 60, 13, 3);
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_str_aligned(
            canvas, PLAY_MENU_X, PLAY_MENU_Y, AlignLeft, AlignTop, PLAY_MENU_TEXT);
        break;

    default:
        break;
    }
}

void i2ctools_draw_sniff_view(Canvas* canvas, i2cTools* i2ctools) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, 0, 0, 128, 64, 3);
    canvas_draw_icon(canvas, 2, 13, &I_passport_happy2_46x49);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 3, 3, AlignLeft, AlignTop, SNIFF_MENU_TEXT);
    canvas_set_font(canvas, FontSecondary);

    // Button
    canvas_draw_rbox(canvas, 70, 48, 45, 13, 3);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_icon(canvas, 75, 50, &I_Ok_btn_9x9);
    if(!i2ctools->sniffer->started) {
        canvas_draw_str_aligned(canvas, 85, 51, AlignLeft, AlignTop, "Start");
    } else {
        canvas_draw_str_aligned(canvas, 85, 51, AlignLeft, AlignTop, "Stop");
    }
    canvas_set_color(canvas, ColorBlack);
    // Address text
    char addr_text[8];
    snprintf(
        addr_text,
        sizeof(addr_text),
        "0x%02x",
        (int)(i2ctools->sniffer->frames[i2ctools->sniffer->menu_index].data[0] >> 1));
    canvas_draw_str_aligned(canvas, 50, 3, AlignLeft, AlignTop, "Addr: ");
    canvas_draw_str_aligned(canvas, 75, 3, AlignLeft, AlignTop, addr_text);
    // R/W
    if((int)(i2ctools->sniffer->frames[i2ctools->sniffer->menu_index].data[0]) % 2 == 0) {
        canvas_draw_str_aligned(canvas, 105, 3, AlignLeft, AlignTop, "W");
    } else {
        canvas_draw_str_aligned(canvas, 105, 3, AlignLeft, AlignTop, "R");
    }
    // nbFrame text
    canvas_draw_str_aligned(canvas, 50, 13, AlignLeft, AlignTop, "Frames: ");
    snprintf(addr_text, sizeof(addr_text), "%d", (int)i2ctools->sniffer->menu_index + 1);
    canvas_draw_str_aligned(canvas, 90, 13, AlignLeft, AlignTop, addr_text);
    canvas_draw_str_aligned(canvas, 100, 13, AlignLeft, AlignTop, "/");
    snprintf(addr_text, sizeof(addr_text), "%d", (int)i2ctools->sniffer->frame_index + 1);
    canvas_draw_str_aligned(canvas, 110, 13, AlignLeft, AlignTop, addr_text);
    // Frames content
    uint8_t x_pos = 0;
    uint8_t y_pos = 23;
    for(uint8_t i = 1; i < i2ctools->sniffer->frames[i2ctools->sniffer->menu_index].data_index;
        i++) {
        snprintf(
            addr_text,
            sizeof(addr_text),
            "0x%02x",
            (int)i2ctools->sniffer->frames[i2ctools->sniffer->menu_index].data[i]);
        x_pos = 50 + (i - 1) * 35;
        canvas_draw_str_aligned(canvas, x_pos, y_pos, AlignLeft, AlignTop, addr_text);
        if(i2ctools->sniffer->frames[i2ctools->sniffer->menu_index].ack[i]) {
            canvas_draw_str_aligned(canvas, x_pos + 24, y_pos, AlignLeft, AlignTop, "A");
        } else {
            canvas_draw_str_aligned(canvas, x_pos + 24, y_pos, AlignLeft, AlignTop, "N");
        }
    }
}

void i2ctools_draw_record_view(Canvas* canvas, i2cTools* i2ctools) {
    UNUSED(i2ctools);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, 0, 0, 128, 64, 3);
    canvas_draw_icon(canvas, 2, 13, &I_passport_happy2_46x49);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 3, 3, AlignLeft, AlignTop, PLAY_MENU_TEXT);
}

void i2ctools_draw_send_view(Canvas* canvas, i2cTools* i2ctools) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, 0, 0, 128, 64, 3);
    canvas_draw_icon(canvas, 2, 13, &I_passport_happy2_46x49);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 3, 3, AlignLeft, AlignTop, SEND_MENU_TEXT);

    if(!i2ctools->scanner->scanned) {
        scan_i2c_bus(i2ctools->scanner);
    }

    canvas_set_font(canvas, FontSecondary);
    if(i2ctools->scanner->nb_found <= 0) {
        canvas_draw_str_aligned(canvas, 60, 5, AlignLeft, AlignTop, "No peripherals");
        canvas_draw_str_aligned(canvas, 60, 15, AlignLeft, AlignTop, "Found");
        return;
    }
    canvas_draw_rbox(canvas, 70, 48, 45, 13, 3);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_icon(canvas, 75, 50, &I_Ok_btn_9x9);
    canvas_draw_str_aligned(canvas, 85, 51, AlignLeft, AlignTop, "Send");
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_str_aligned(canvas, 50, 5, AlignLeft, AlignTop, "Addr: ");
    canvas_draw_icon(canvas, 80, 5, &I_ButtonLeft_4x7);
    canvas_draw_icon(canvas, 115, 5, &I_ButtonRight_4x7);
    char addr_text[8];
    snprintf(
        addr_text,
        sizeof(addr_text),
        "0x%02x",
        (int)i2ctools->scanner->addresses[i2ctools->sender.address_idx]);
    canvas_draw_str_aligned(canvas, 90, 5, AlignLeft, AlignTop, addr_text);
    canvas_draw_str_aligned(canvas, 50, 15, AlignLeft, AlignTop, "Value: ");

    canvas_draw_icon(canvas, 80, 17, &I_ButtonUp_7x4);
    canvas_draw_icon(canvas, 115, 17, &I_ButtonDown_7x4);
    snprintf(addr_text, sizeof(addr_text), "0x%02x", (int)i2ctools->sender.value);
    canvas_draw_str_aligned(canvas, 90, 15, AlignLeft, AlignTop, addr_text);
    if(i2ctools->sender.must_send) {
        furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
        i2ctools->sender.error = furi_hal_i2c_trx(
            &furi_hal_i2c_handle_external,
            i2ctools->scanner->addresses[i2ctools->sender.address_idx] << 1,
            &i2ctools->sender.value,
            1,
            i2ctools->sender.recv,
            sizeof(i2ctools->sender.recv),
            3);
        furi_hal_i2c_release(&furi_hal_i2c_handle_external);
        i2ctools->sender.must_send = false;
        i2ctools->sender.sended = true;
    }
    canvas_draw_str_aligned(canvas, 50, 25, AlignLeft, AlignTop, "Result: ");
    if(i2ctools->sender.sended) {
        for(uint8_t i = 0; i < sizeof(i2ctools->sender.recv); i++) {
            snprintf(addr_text, sizeof(addr_text), "0x%02x", (int)i2ctools->sender.recv[i]);
            canvas_draw_str_aligned(canvas, 90, 25 + (i * 10), AlignLeft, AlignTop, addr_text);
        }
    }
}

void i2ctools_draw_scan_view(Canvas* canvas, i2cTools* i2ctools) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, 0, 0, 128, 64, 3);
    canvas_draw_icon(canvas, 2, 13, &I_passport_happy3_46x49);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 3, 3, AlignLeft, AlignTop, SCAN_MENU_TEXT);

    char count_text[46];
    char count_text_fmt[] = "Found: %d";
    canvas_set_font(canvas, FontSecondary);
    snprintf(count_text, sizeof(count_text), count_text_fmt, (int)i2ctools->scanner->nb_found);
    canvas_draw_str_aligned(canvas, 50, 3, AlignLeft, AlignTop, count_text);
    uint8_t x_pos = 0;
    uint8_t y_pos = 0;
    uint8_t idx_to_print = 0;
    for(uint8_t i = 0; i < (int)i2ctools->scanner->nb_found; i++) {
        idx_to_print = i + i2ctools->scanner->menu_index * 3;
        if(idx_to_print >= MAX_I2C_ADDR) {
            break;
        }
        snprintf(
            count_text,
            sizeof(count_text),
            "0x%02x ",
            (int)i2ctools->scanner->addresses[idx_to_print]);
        if(i < 3) {
            x_pos = 50 + (i * 26);
            y_pos = 15;
        } else if(i < 6) {
            x_pos = 50 + ((i - 3) * 26);
            y_pos = 25;
        } else if(i < 9) {
            x_pos = 50 + ((i - 6) * 26);
            y_pos = 35;
        } else if(i < 12) {
            x_pos = 50 + ((i - 9) * 26);
            y_pos = 45;
        } else if(i < 15) {
            x_pos = 50 + ((i - 12) * 26);
            y_pos = 55;
        } else {
            break;
        }
        canvas_draw_str_aligned(canvas, x_pos, y_pos, AlignLeft, AlignTop, count_text);
    }
    // Right cursor
    y_pos = 14 + i2ctools->scanner->menu_index;
    canvas_draw_rbox(canvas, 125, y_pos, 3, 10, 1);

    // Button
    canvas_draw_rbox(canvas, 70, 48, 45, 13, 3);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_icon(canvas, 75, 50, &I_Ok_btn_9x9);
    canvas_draw_str_aligned(canvas, 85, 51, AlignLeft, AlignTop, "Scan");
}

void i2ctools_draw_callback(Canvas* canvas, void* ctx) {
    i2cTools* i2c_addr = acquire_mutex((ValueMutex*)ctx, 25);

    switch(i2c_addr->current_menu) {
    case MAIN_VIEW:
        i2ctools_draw_main_menu(canvas, i2c_addr);
        break;

    case SCAN_VIEW:
        i2ctools_draw_scan_view(canvas, i2c_addr);
        break;

    case SNIFF_VIEW:
        i2ctools_draw_sniff_view(canvas, i2c_addr);
        break;
    case SEND_VIEW:
        i2ctools_draw_send_view(canvas, i2c_addr);
        break;
    case PLAY_VIEW:
        i2ctools_draw_record_view(canvas, i2c_addr);
        break;
    default:
        break;
    }
    release_mutex((ValueMutex*)ctx, i2c_addr);
}

void i2ctools_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t i2ctools_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    // Alloc i2ctools
    i2cTools* i2ctools = malloc(sizeof(i2cTools));
    ValueMutex i2ctools_mutex;
    if(!init_mutex(&i2ctools_mutex, i2ctools, sizeof(i2cTools))) {
        FURI_LOG_E(APP_NAME, "cannot create mutex\r\n");
        free(i2ctools);
        return -1;
    }

    // Alloc viewport
    i2ctools->view_port = view_port_alloc();
    view_port_draw_callback_set(i2ctools->view_port, i2ctools_draw_callback, &i2ctools_mutex);
    view_port_input_callback_set(i2ctools->view_port, i2ctools_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, i2ctools->view_port, GuiLayerFullscreen);

    InputEvent event;

    i2ctools->sniffer = i2c_sniffer_alloc();
    i2ctools->sniffer->menu_index = 0;

    i2ctools->scanner = i2c_scanner_alloc();

    i2ctools->sender.must_send = false;
    i2ctools->sender.sended = false;
    while(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk) {
        if(event.key == InputKeyBack && event.type == InputTypeRelease) {
            if(i2ctools->current_menu == MAIN_VIEW) {
                break;
            } else {
                if(i2ctools->current_menu == SNIFF_VIEW) {
                    stop_interrupts();
                    i2ctools->sniffer->started = false;
                    i2ctools->sniffer->state = I2C_BUS_FREE;
                }
                i2ctools->current_menu = MAIN_VIEW;
            }
        } else if(event.key == InputKeyUp && event.type == InputTypeRelease) {
            if(i2ctools->current_menu == MAIN_VIEW) {
                if(i2ctools->main_menu_index > 0) {
                    i2ctools->main_menu_index--;
                }
            } else if(i2ctools->current_menu == SCAN_VIEW) {
                if(i2ctools->scanner->menu_index > 0) {
                    i2ctools->scanner->menu_index--;
                }
            } else if(i2ctools->current_menu == SEND_VIEW) {
                if(i2ctools->sender.value < 0xFF) {
                    i2ctools->sender.value++;
                    i2ctools->sender.sended = false;
                }
            }
        } else if(
            event.key == InputKeyUp &&
            (event.type == InputTypeLong || event.type == InputTypeRepeat)) {
            if(i2ctools->current_menu == SEND_VIEW) {
                if(i2ctools->sender.value < 0xF9) {
                    i2ctools->sender.value += 5;
                    i2ctools->sender.sended = false;
                }
            }
        } else if(event.key == InputKeyDown && event.type == InputTypeRelease) {
            if(i2ctools->current_menu == MAIN_VIEW) {
                if(i2ctools->main_menu_index < 3) {
                    i2ctools->main_menu_index++;
                }
            } else if(i2ctools->current_menu == SCAN_VIEW) {
                if(i2ctools->scanner->menu_index < ((int)i2ctools->scanner->nb_found / 3)) {
                    i2ctools->scanner->menu_index++;
                }
            } else if(i2ctools->current_menu == SEND_VIEW) {
                if(i2ctools->sender.value > 0x00) {
                    i2ctools->sender.value--;
                    i2ctools->sender.sended = false;
                }
            }
        } else if(event.key == InputKeyDown && event.type == InputTypeLong) {
            if(i2ctools->current_menu == SEND_VIEW) {
                if(i2ctools->sender.value > 0x05) {
                    i2ctools->sender.value -= 5;
                    i2ctools->sender.sended = false;
                }
            }

        } else if(event.key == InputKeyOk && event.type == InputTypeRelease) {
            if(i2ctools->current_menu == MAIN_VIEW) {
                if(i2ctools->main_menu_index == 0) {
                    scan_i2c_bus(i2ctools->scanner);
                    i2ctools->current_menu = SCAN_VIEW;
                } else if(i2ctools->main_menu_index == 1) {
                    i2ctools->current_menu = SNIFF_VIEW;
                } else if(i2ctools->main_menu_index == 2) {
                    i2ctools->current_menu = SEND_VIEW;
                } else if(i2ctools->main_menu_index == 3) {
                    i2ctools->current_menu = PLAY_VIEW;
                }
            } else if(i2ctools->current_menu == SCAN_VIEW) {
                scan_i2c_bus(i2ctools->scanner);
            } else if(i2ctools->current_menu == SEND_VIEW) {
                i2ctools->sender.must_send = true;
            } else if(i2ctools->current_menu == SNIFF_VIEW) {
                if(i2ctools->sniffer->started) {
                    stop_interrupts();
                    i2ctools->sniffer->started = false;
                    i2ctools->sniffer->state = I2C_BUS_FREE;
                } else {
                    start_interrupts(i2ctools->sniffer);
                    i2ctools->sniffer->started = true;
                    i2ctools->sniffer->state = I2C_BUS_FREE;
                }
            }
        } else if(event.key == InputKeyRight && event.type == InputTypeRelease) {
            if(i2ctools->current_menu == SEND_VIEW) {
                if(i2ctools->sender.address_idx < (i2ctools->scanner->nb_found - 1)) {
                    i2ctools->sender.address_idx++;
                    i2ctools->sender.sended = false;
                }
            } else if(i2ctools->current_menu == SNIFF_VIEW) {
                if(i2ctools->sniffer->menu_index < i2ctools->sniffer->frame_index) {
                    i2ctools->sniffer->menu_index++;
                }
            }
        } else if(event.key == InputKeyLeft && event.type == InputTypeRelease) {
            if(i2ctools->current_menu == SEND_VIEW) {
                if(i2ctools->sender.address_idx > 0) {
                    i2ctools->sender.address_idx--;
                    i2ctools->sender.sended = false;
                }
            } else if(i2ctools->current_menu == SNIFF_VIEW) {
                if(i2ctools->sniffer->menu_index > 0) {
                    i2ctools->sniffer->menu_index--;
                }
            }
        }
        view_port_update(i2ctools->view_port);
    }
    gui_remove_view_port(gui, i2ctools->view_port);
    view_port_free(i2ctools->view_port);
    furi_message_queue_free(event_queue);
    i2c_sniffer_free(i2ctools->sniffer);
    i2c_scanner_free(i2ctools->scanner);
    free(i2ctools);
    furi_record_close(RECORD_GUI);
    return 0;
}
