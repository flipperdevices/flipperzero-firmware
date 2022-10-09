#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <stdbool.h> // Header-file for boolean data-type.

#define MAX_I2C_ADDR 0x7F

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

#define RECORD_MENU_TEXT "Record"
#define RECORD_MENU_X 70
#define RECORD_MENU_Y 48

// Sniffer Pins
#define pinSCL &gpio_ext_pc0
#define pinSDA &gpio_ext_pc1

typedef enum {
    MAIN_VIEW,
    SCAN_VIEW,
    SNIFF_VIEW,
    SEND_VIEW,
    RECORD_VIEW,

    /* Know menu Size*/
    MENU_SIZE
} i2cToolsMainMenu;

typedef enum { I2C_IDLE, I2C_START, I2C_PAUSE, I2C_STOP } i2cStates;

#define MAX_FRAMES 32

typedef struct {
    i2cStates state;
    uint8_t address;
    bool pending;
    bool rw; // true = read
    bool ack; // true = ACK, false = NACK
    uint8_t data[MAX_FRAMES];
    uint8_t bit_count;
    uint8_t frame_count;
    uint32_t lastSCLtime;
    uint32_t period;
} sniffStruct;

typedef struct {
    ViewPort* view_port;
    uint8_t available_addr[MAX_I2C_ADDR + 1];
    uint8_t nb_available;
    bool scanned;
    i2cToolsMainMenu current_menu;
    uint8_t main_menu_index;
    uint8_t scan_view_index;
    uint8_t send_view_addr_index;
    uint8_t send_view_to_send;
    bool send_view_must_send;
    uint8_t send_last_recv[2];
    bool send_get_return;
    bool send_started;
    bool sniff_started;
    sniffStruct sniff;
} i2cToolsData;

NotificationApp* notifications;

void get_available_i2c(i2cToolsData* data) {
    data->nb_available = 0;
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    for(uint8_t addr = 0x01; addr < MAX_I2C_ADDR; addr++) {
        if(addr % 2 != 0) {
            continue;
        }
        bool ret = furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, addr, 2);
        if(ret) {
            data->available_addr[data->nb_available] = (addr >> 1);
            data->nb_available++;
        }
    }
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    data->scanned = true;
}

void i2ctools_draw_main_menu(Canvas* canvas, i2cToolsData* data) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, 0, 0, 128, 64, 3);
    canvas_draw_icon(canvas, 2, 13, &I_passport_bad3_46x49);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 3, 3, AlignLeft, AlignTop, APP_NAME);

    switch(data->main_menu_index) {
    case 0:
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_str_aligned(
            canvas, SNIFF_MENU_X, SNIFF_MENU_Y, AlignLeft, AlignTop, SNIFF_MENU_TEXT);
        canvas_draw_str_aligned(
            canvas, SEND_MENU_X, SEND_MENU_Y, AlignLeft, AlignTop, SEND_MENU_TEXT);
        canvas_draw_str_aligned(
            canvas, RECORD_MENU_X, RECORD_MENU_Y, AlignLeft, AlignTop, RECORD_MENU_TEXT);

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
            canvas, RECORD_MENU_X, RECORD_MENU_Y, AlignLeft, AlignTop, RECORD_MENU_TEXT);

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
            canvas, RECORD_MENU_X, RECORD_MENU_Y, AlignLeft, AlignTop, RECORD_MENU_TEXT);

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

        canvas_draw_rbox(canvas, 60, RECORD_MENU_Y - 2, 60, 13, 3);
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_str_aligned(
            canvas, RECORD_MENU_X, RECORD_MENU_Y, AlignLeft, AlignTop, RECORD_MENU_TEXT);
        break;

    default:
        break;
    }
}

static void input_isr_SDA(void* ctx) {
    i2cToolsData* data = ctx;
    switch(data->sniff.state) {
    case I2C_STOP:
        if(!furi_hal_gpio_read(pinSDA) && furi_hal_gpio_read(pinSCL)) {
            data->sniff.state = I2C_START;
            data->sniff.address = 0;
            data->sniff.frame_count = 0;
            data->sniff.bit_count = 0;
            data->sniff.pending = false;
        }
        break;
    case I2C_START:
        if(furi_hal_gpio_read(pinSDA) && furi_hal_gpio_read(pinSCL)) {
            data->sniff.state = I2C_STOP;
        }
        break;
    default:
        break;
    }
    view_port_update(data->view_port);
}

static void input_isr_SCL(void* ctx) {
    i2cToolsData* data = ctx;
    switch(data->sniff.state) {
    case I2C_START:
        data->sniff.period = furi_get_tick() - data->sniff.lastSCLtime;
        if(!data->sniff.pending && data->sniff.address == 0 && data->sniff.bit_count <= 6) {
            data->sniff.data[0] += furi_hal_gpio_read(pinSDA) * pow(2, data->sniff.bit_count);
        } else if(!data->sniff.pending && data->sniff.address != 0 && data->sniff.bit_count <= 7) {
            data->sniff.data[data->sniff.frame_count] +=
                furi_hal_gpio_read(pinSDA) * pow(2, data->sniff.bit_count);
        }
        if(data->sniff.bit_count == 6 && data->sniff.address == 0) {
            data->sniff.address = (data->sniff.data[0] << 1);
            data->sniff.pending = true;
        } else if(data->sniff.pending && data->sniff.bit_count == 7) {
            data->sniff.rw = !furi_hal_gpio_read(pinSDA);
        } else if(data->sniff.pending && data->sniff.bit_count == 8) {
            data->sniff.ack = !furi_hal_gpio_read(pinSDA);
            data->sniff.pending = false;
            data->sniff.bit_count = 0;
        } else if(!data->sniff.pending && data->sniff.bit_count == 7) {
            data->sniff.pending = true;
            data->sniff.frame_count++;
        }
        data->sniff.bit_count++;
        break;

    default:
        break;
    }
    data->sniff.lastSCLtime = furi_get_tick();
    view_port_update(data->view_port);
}

void i2ctools_draw_sniff_view(Canvas* canvas, i2cToolsData* data) {
    if(!data->sniff_started) {
        // Add Rise Interrupt on SCL pin
        furi_hal_gpio_init(pinSCL, GpioModeInterruptRise, GpioPullNo, GpioSpeedVeryHigh);
        furi_hal_gpio_add_int_callback(pinSCL, input_isr_SCL, data);

        // Add Rise and Fall Interrupt on SDA pin
        furi_hal_gpio_init(pinSDA, GpioModeInterruptRiseFall, GpioPullNo, GpioSpeedVeryHigh);
        furi_hal_gpio_add_int_callback(pinSDA, input_isr_SDA, data);

        data->sniff_started = true;
    }

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, 0, 0, 128, 64, 3);
    canvas_draw_icon(canvas, 2, 13, &I_passport_happy2_46x49);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 3, 3, AlignLeft, AlignTop, SNIFF_MENU_TEXT);
    canvas_set_font(canvas, FontSecondary);
    char addr_text[8];
    snprintf(addr_text, sizeof(addr_text), "%#4x", (int)data->sniff.address);
    canvas_draw_str_aligned(canvas, 50, 3, AlignLeft, AlignTop, addr_text);
    if(data->sniff.rw) {
        canvas_draw_str_aligned(canvas, 75, 3, AlignLeft, AlignTop, "W");
    } else {
        canvas_draw_str_aligned(canvas, 75, 3, AlignLeft, AlignTop, "R");
    }
    if(data->sniff.ack) {
        canvas_draw_str_aligned(canvas, 83, 3, AlignLeft, AlignTop, "A");
    } else {
        canvas_draw_str_aligned(canvas, 83, 3, AlignLeft, AlignTop, "N");
    }
    for(uint8_t i = 0; i < data->sniff.frame_count; i++) {
        snprintf(addr_text, sizeof(addr_text), "%#4x", (int)data->sniff.data[i]);
        canvas_draw_str_aligned(canvas, 50 + i * 25, 43, AlignLeft, AlignTop, addr_text);
    }

    switch(data->sniff.state) {
    case I2C_IDLE:
        canvas_draw_str_aligned(canvas, 50, 33, AlignLeft, AlignTop, "IDLE");
        break;
    case I2C_START:
        canvas_draw_str_aligned(canvas, 50, 33, AlignLeft, AlignTop, "START");

        break;
    case I2C_STOP:
        canvas_draw_str_aligned(canvas, 50, 33, AlignLeft, AlignTop, "STOP");
        break;
    default:
        break;
    }
    canvas_draw_str_aligned(canvas, 50, 53, AlignLeft, AlignTop, "size: ");
    snprintf(addr_text, sizeof(addr_text), "%d", (int)data->sniff.frame_count);
    canvas_draw_str_aligned(canvas, 70, 53, AlignLeft, AlignTop, addr_text);
    canvas_draw_str_aligned(canvas, 80, 53, AlignLeft, AlignTop, "period: ");
    snprintf(addr_text, sizeof(addr_text), "%d", (int)data->sniff.lastSCLtime);
    canvas_draw_str_aligned(canvas, 110, 53, AlignLeft, AlignTop, addr_text);
}

void i2ctools_draw_record_view(Canvas* canvas, i2cToolsData* data) {
    UNUSED(data);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, 0, 0, 128, 64, 3);
    canvas_draw_icon(canvas, 2, 13, &I_passport_happy2_46x49);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 3, 3, AlignLeft, AlignTop, RECORD_MENU_TEXT);
}

void i2ctools_draw_send_view(Canvas* canvas, i2cToolsData* data) {
    if(!data->scanned) {
        get_available_i2c(data);
    }
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, 0, 0, 128, 64, 3);
    canvas_draw_icon(canvas, 2, 13, &I_passport_happy2_46x49);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 3, 3, AlignLeft, AlignTop, SEND_MENU_TEXT);
    canvas_set_font(canvas, FontSecondary);
    if(data->nb_available <= 0) {
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
        "%#04x",
        (int)data->available_addr[data->send_view_addr_index]);
    canvas_draw_str_aligned(canvas, 90, 5, AlignLeft, AlignTop, addr_text);
    canvas_draw_str_aligned(canvas, 50, 15, AlignLeft, AlignTop, "Value: ");

    canvas_draw_icon(canvas, 80, 17, &I_ButtonUp_7x4);
    canvas_draw_icon(canvas, 115, 17, &I_ButtonDown_7x4);
    snprintf(addr_text, sizeof(addr_text), "%#04x", (int)data->send_view_to_send);
    canvas_draw_str_aligned(canvas, 90, 15, AlignLeft, AlignTop, addr_text);
    if(data->send_view_must_send) {
        furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
        data->send_get_return = furi_hal_i2c_trx(
            &furi_hal_i2c_handle_external,
            data->available_addr[data->send_view_addr_index] << 1,
            &data->send_view_to_send,
            1,
            data->send_last_recv,
            sizeof(data->send_last_recv),
            3);
        furi_hal_i2c_release(&furi_hal_i2c_handle_external);
        data->send_view_must_send = false;
        data->send_started = true;
    }
    canvas_draw_str_aligned(canvas, 50, 25, AlignLeft, AlignTop, "Result: ");
    if(data->send_started) {
        if(data->send_get_return) {
            for(uint8_t i = 0; i < sizeof(data->send_last_recv); i++) {
                snprintf(addr_text, sizeof(addr_text), "%#04x", (int)data->send_last_recv[i]);
                canvas_draw_str_aligned(canvas, 90, 25 + (i * 10), AlignLeft, AlignTop, addr_text);
            }
        } else {
            canvas_draw_str_aligned(canvas, 90, 25, AlignLeft, AlignTop, "Error");
        }
    }
}

void i2ctools_draw_scan_view(Canvas* canvas, i2cToolsData* data) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, 0, 0, 128, 64, 3);
    canvas_draw_icon(canvas, 2, 13, &I_passport_happy3_46x49);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 3, 3, AlignLeft, AlignTop, SCAN_MENU_TEXT);

    char count_text[46];
    char count_text_fmt[] = "Found: %d";
    canvas_set_font(canvas, FontSecondary);
    snprintf(count_text, sizeof(count_text), count_text_fmt, (int)data->nb_available);
    canvas_draw_str_aligned(canvas, 50, 3, AlignLeft, AlignTop, count_text);
    uint8_t x_pos = 0;
    uint8_t y_pos = 0;
    uint8_t idx_to_print = 0;
    for(uint8_t i = 0; i < (int)data->nb_available; i++) {
        idx_to_print = i + data->scan_view_index * 3;
        if(idx_to_print >= MAX_I2C_ADDR) {
            break;
        }
        snprintf(
            count_text, sizeof(count_text), "%#04x ", (int)data->available_addr[idx_to_print]);
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
    y_pos = 14 + data->scan_view_index;
    canvas_draw_rbox(canvas, 125, y_pos, 3, 10, 1);
}

void i2ctools_draw_callback(Canvas* canvas, void* ctx) {
    i2cToolsData* i2c_addr = acquire_mutex((ValueMutex*)ctx, 25);

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
    case RECORD_VIEW:
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

    i2cToolsData* i2caddrs = malloc(sizeof(i2cToolsData));
    ValueMutex i2caddrs_mutex;
    if(!init_mutex(&i2caddrs_mutex, i2caddrs, sizeof(i2cToolsData))) {
        FURI_LOG_E(APP_NAME, "cannot create mutex\r\n");
        free(i2caddrs);
        return -1;
    }

    notifications = furi_record_open(RECORD_NOTIFICATION);

    i2caddrs->view_port = view_port_alloc();
    view_port_draw_callback_set(i2caddrs->view_port, i2ctools_draw_callback, &i2caddrs_mutex);
    view_port_input_callback_set(i2caddrs->view_port, i2ctools_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, i2caddrs->view_port, GuiLayerFullscreen);

    InputEvent event;
    i2caddrs->scanned = false;
    i2caddrs->send_view_addr_index = 0;
    i2caddrs->send_view_to_send = 0x01;
    i2caddrs->send_view_must_send = false;
    i2caddrs->send_started = false;
    i2caddrs->sniff_started = false;
    i2caddrs->sniff.state = I2C_STOP;
    while(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk) {
        if(event.key == InputKeyBack && event.type == InputTypeRelease) {
            if(i2caddrs->current_menu == MAIN_VIEW) {
                break;
            } else {
                if(i2caddrs->current_menu == SNIFF_VIEW) {
                    furi_hal_gpio_remove_int_callback(pinSCL);
                    furi_hal_gpio_remove_int_callback(pinSDA);
                    i2caddrs->sniff_started = false;
                    i2caddrs->sniff.state = I2C_STOP;
                }
                i2caddrs->current_menu = MAIN_VIEW;
            }
        } else if(event.key == InputKeyUp && event.type == InputTypeRelease) {
            if(i2caddrs->current_menu == MAIN_VIEW) {
                if(i2caddrs->main_menu_index > 0) {
                    i2caddrs->main_menu_index--;
                }
            } else if(i2caddrs->current_menu == SCAN_VIEW) {
                if(i2caddrs->scan_view_index > 0) {
                    i2caddrs->scan_view_index--;
                }
            } else if(i2caddrs->current_menu == SEND_VIEW) {
                if(i2caddrs->send_view_to_send < 0xFF) {
                    i2caddrs->send_view_to_send++;
                    i2caddrs->send_started = false;
                }
            }

        } else if(
            event.key == InputKeyUp &&
            (event.type == InputTypeLong || event.type == InputTypeRepeat)) {
            if(i2caddrs->current_menu == SEND_VIEW) {
                if(i2caddrs->send_view_to_send < 0xF9) {
                    i2caddrs->send_view_to_send += 5;
                    i2caddrs->send_started = false;
                }
            }

        } else if(event.key == InputKeyDown && event.type == InputTypeRelease) {
            if(i2caddrs->current_menu == MAIN_VIEW) {
                if(i2caddrs->main_menu_index < 3) {
                    i2caddrs->main_menu_index++;
                }
            } else if(i2caddrs->current_menu == SCAN_VIEW) {
                if(i2caddrs->scan_view_index < ((int)i2caddrs->nb_available / 3)) {
                    i2caddrs->scan_view_index++;
                }
            } else if(i2caddrs->current_menu == SEND_VIEW) {
                if(i2caddrs->send_view_to_send > 0x00) {
                    i2caddrs->send_view_to_send--;
                    i2caddrs->send_started = false;
                }
            }
        } else if(event.key == InputKeyDown && event.type == InputTypeLong) {
            if(i2caddrs->current_menu == SEND_VIEW) {
                if(i2caddrs->send_view_to_send > 0x05) {
                    i2caddrs->send_view_to_send -= 5;
                    i2caddrs->send_started = false;
                }
            }

        } else if(event.key == InputKeyOk && event.type == InputTypeRelease) {
            if(i2caddrs->current_menu == MAIN_VIEW) {
                if(i2caddrs->main_menu_index == 0) {
                    get_available_i2c(i2caddrs);
                    i2caddrs->current_menu = SCAN_VIEW;
                } else if(i2caddrs->main_menu_index == 1) {
                    i2caddrs->current_menu = SNIFF_VIEW;
                } else if(i2caddrs->main_menu_index == 2) {
                    i2caddrs->current_menu = SEND_VIEW;
                } else if(i2caddrs->main_menu_index == 3) {
                    i2caddrs->current_menu = RECORD_VIEW;
                }
            } else if(i2caddrs->current_menu == SCAN_VIEW) {
                get_available_i2c(i2caddrs);
            } else if(i2caddrs->current_menu == SEND_VIEW) {
                i2caddrs->send_view_must_send = true;
            }
        } else if(event.key == InputKeyRight && event.type == InputTypeRelease) {
            if(i2caddrs->current_menu == SEND_VIEW) {
                if(i2caddrs->send_view_addr_index < (i2caddrs->nb_available - 1)) {
                    i2caddrs->send_view_addr_index++;
                    i2caddrs->send_started = false;
                }
            }
        } else if(event.key == InputKeyLeft && event.type == InputTypeRelease) {
            if(i2caddrs->current_menu == SEND_VIEW) {
                if(i2caddrs->send_view_addr_index > 0) {
                    i2caddrs->send_view_addr_index--;
                    i2caddrs->send_started = false;
                }
            }
        }
        view_port_update(i2caddrs->view_port);
    }
    gui_remove_view_port(gui, i2caddrs->view_port);
    view_port_free(i2caddrs->view_port);
    furi_message_queue_free(event_queue);
    free(i2caddrs);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);

    return 0;
}