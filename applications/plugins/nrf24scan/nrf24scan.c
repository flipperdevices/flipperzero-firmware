//
// Written by vad7, 20.11.2022.
// ver. 1.0
//
#include "nrf24scan.h"

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <dialogs/dialogs.h>
#include <input/input.h>
#include <stdlib.h>
#include <dolphin/dolphin.h>
#include <nrf24.h>

#define TAG "nrf24scan"
#define MAX_CHANNEL 125
#define MAX_ADDR 6

#define SCAN_APP_PATH_FOLDER "/ext/nrf24scan"
#define ADDR_FILENAME                                            \
    "addr.txt" // File format (1 parameter per line):            \
        // 0.25/1/2 - rate in Mbps                               \
        // 0..125 - default channel                              \
        // address P0 in hex (5 byte, LSB last)                  \
        // address P1 in hex (5 byte, LSB last)                  \
        // address P2, LSB in hex (1 byte)                       \
        // address P3, LSB in hex (1 byte)                       \
        // address P4, LSB in hex (1 byte)                       \
        // address P5, LSB in hex (1 byte)                       \
        // captured data in raw format, first byte = addr # 0..5 \
        // ... up to MAX_LOG_RECORDS
#define LOG_FILENAME "log"
#define LOG_FILEEXT ".txt"
#define MAX_LOG_RECORDS 100
#define LOG_REC_SIZE 33 // max packet size
#define VIEW_LOG_MAX_X 22

Nrf24Scan* APP;
uint8_t what_doing = 0; // 0 - setup, 1 - view log
uint8_t what_to_do = 1; // 0 - view, 1 - view & scan
char screen_buf[64];
char addr_file_name[32];
uint8_t NRF_rate; // 0 - 250Kbps, 1 - 1Mbps, 2 - 2Mbps
uint8_t NRF_channel; // 0..125
struct {
    uint8_t addr_P0[5]; // MSB first
    uint8_t addr_P1[5]; // MSB first
    uint8_t addr_P2; // LSB only, MSB bytes equal addr_P1
    uint8_t addr_P3; // LSB only, MSB bytes equal addr_P1
    uint8_t addr_P4; // LSB only, MSB bytes equal addr_P1
    uint8_t addr_P5; // LSB only, MSB bytes equal addr_P1
    uint8_t addr_len; // 2..5
    uint8_t addr_count;
} addrs;
int8_t log_to_file = 0; // 0 - no, 1 - yes(new), 2 - append, -1 - only clear
uint16_t log_arr_idx;
uint16_t view_log_arr_idx = 0;
uint16_t view_log_arr_x = 0;
bool save_to_new_log = true;
uint16_t last_packet_send = -1;
uint8_t last_packet_send_st = 0;
int16_t find_channel_period = 0; // sec
uint8_t menu_selected = 0;
uint32_t start_time;

#define menu_selected_max 5
enum {
    Menu_open_file = 0,
    Menu_enter_channel,
    Menu_enter_rate,
    Menu_enter_scan_period,
    Menu_log,
    Menu_ok
};

static uint8_t GetHexVal(char hex) {
    return (uint8_t)hex - ((uint8_t)hex < 58 ? 48 : ((uint8_t)hex < 97 ? 55 : 87));
}

// Return num bytes in array
static uint8_t ConvertHexToArray(char* hex, uint8_t* array, uint8_t maxlen) {
    uint8_t len = 0;
    do {
        uint8_t ch = *hex++;
        if(ch == 0) break;
        if(ch < '0') continue;
        *array++ = (GetHexVal(ch) << 4) + GetHexVal(*hex++);
        len++;
    } while(--maxlen);
    return len;
}

static void add_to_str_hex_bytes(char* out, char* arr, int bytes) {
    if(!bytes) return;
    out += strlen(out);
    do {
        snprintf(out, 3, "%02X", *arr++);
        out += 2;
    } while(--bytes);
}

static void add_to_furi_str_hex_bytes(FuriString* str, char* arr, int bytes) {
    if(!bytes) return;
    do {
        furi_string_cat_printf(str, "%02X", *arr++);
    } while(--bytes);
}

void clear_log() {
    log_arr_idx = 0;
    view_log_arr_idx = 0;
    last_packet_send = -1;
}

void allocate_log_array() {
    APP->log_arr = malloc(LOG_REC_SIZE * MAX_LOG_RECORDS);
    if(APP->log_arr == NULL) {
        FURI_LOG_E(TAG, "Not enouch memory: %d", LOG_REC_SIZE * MAX_LOG_RECORDS);
        strcpy(addr_file_name, "MEMORY LOW!");
    }
    clear_log();
}

void write_to_log_file(Storage* storage) {
    if(log_arr_idx == 0) return;
    Stream* file_stream = file_stream_alloc(storage);
    FuriString* str = furi_string_alloc();
    furi_string_set(str, SCAN_APP_PATH_FOLDER);
    furi_string_cat(str, "/");
    furi_string_cat(str, LOG_FILENAME);
    furi_string_cat(str, LOG_FILEEXT);
    bool fl;
    if(save_to_new_log) {
        int cnt = 1;
        do {
            fl = file_stream_open(
                file_stream, furi_string_get_cstr(str), FSAM_READ_WRITE, FSOM_CREATE_NEW);
            if(fl) break;
            furi_string_set(str, SCAN_APP_PATH_FOLDER);
            furi_string_cat(str, "/");
            furi_string_cat(str, LOG_FILENAME);
            furi_string_cat_printf(str, "-%02d", cnt);
            furi_string_cat(str, LOG_FILEEXT);
        } while(++cnt < 100);
        if(!fl) {
            FURI_LOG_E(TAG, "Failed to create new log file");
            notification_message(APP->notification, &sequence_blink_red_100);
        }
    } else {
        fl = file_stream_open(
            file_stream, furi_string_get_cstr(str), FSAM_READ_WRITE, FSOM_OPEN_APPEND);
        if(stream_size(file_stream) == 0) save_to_new_log = true;
    }
    if(fl) {
        FURI_LOG_D(TAG, "Save to log %s", furi_string_get_cstr(str));
        if(save_to_new_log) {
            furi_string_printf(str, "%d\n%d\n", NRF_rate, NRF_channel);
            add_to_furi_str_hex_bytes(str, (char*)addrs.addr_P0, addrs.addr_len);
            furi_string_cat(str, "\n");
            if(addrs.addr_count > 1) {
                add_to_furi_str_hex_bytes(str, (char*)addrs.addr_P1, addrs.addr_len);
                furi_string_cat(str, "\n");
            }
            if(addrs.addr_count > 2) furi_string_cat_printf(str, "%02X\n", addrs.addr_P2);
            if(addrs.addr_count > 3) furi_string_cat_printf(str, "%02X\n", addrs.addr_P3);
            if(addrs.addr_count > 4) furi_string_cat_printf(str, "%02X\n", addrs.addr_P4);
            if(addrs.addr_count > 5) furi_string_cat_printf(str, "%02X\n", addrs.addr_P5);
            if(!(fl = stream_write_string(file_stream, str) == furi_string_size(str))) {
                FURI_LOG_E(TAG, "Failed to write header to log!");
                notification_message(APP->notification, &sequence_blink_red_100);
            }
        }
        if(fl) {
            int i = 0;
            for(; i < log_arr_idx; i++) {
                furi_string_reset(str);
                add_to_furi_str_hex_bytes(
                    str, (char*)APP->log_arr + i * LOG_REC_SIZE, LOG_REC_SIZE);
                furi_string_cat(str, "\n");
                if(stream_write_string(file_stream, str) != furi_string_size(str)) {
                    FURI_LOG_E(TAG, "Failed to write to log!");
                    break;
                }
            }
            if(i == log_arr_idx) {
                notification_message(APP->notification, &sequence_blink_yellow_100);
                FURI_LOG_D(TAG, "Log saved");
            }
        }
        save_to_new_log = false;
    } else {
        FURI_LOG_E(TAG, "Failed to open log file");
        notification_message(APP->notification, &sequence_blink_red_100);
    }
    //file_stream_close(file_stream);
    stream_free(file_stream);
    furi_string_free(str);
}

static bool select_settings_file(Stream* stream) {
    DialogsApp* dialogs = furi_record_open("dialogs");
    bool result = false;
    FuriString* path;
    path = furi_string_alloc();
    furi_string_set(path, SCAN_APP_PATH_FOLDER);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, ".txt", NULL);
    browser_options.hide_ext = false;

    bool ret = dialog_file_browser_show(dialogs, path, path, &browser_options);

    furi_record_close("dialogs");
    if(ret) {
        if(!file_stream_open(stream, furi_string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING)) {
            //file_stream_close(stream);
            FURI_LOG_D(TAG, "Cannot open file \"%s\"", furi_string_get_cstr(path));
        } else {
            FURI_LOG_D(TAG, "Open file \"%s\"", furi_string_get_cstr(path));
            strncpy(
                addr_file_name,
                furi_string_get_cstr(path) + sizeof(SCAN_APP_PATH_FOLDER),
                sizeof(addr_file_name));
            result = true;
        }
    }
    furi_string_free(path);
    return result;
}

// True - success
static bool load_settings_file(Stream* file_stream) {
    size_t file_size = 0;
    char* file_buf;
    bool loaded = false;
    file_size = stream_size(file_stream);
    if(file_size == (size_t)0) {
        FURI_LOG_D(TAG, "load failed. file_size: %d", file_size);
        return loaded;
    }
    file_buf = malloc(file_size);
    memset(file_buf, 0, file_size);
    if(stream_read(file_stream, (uint8_t*)file_buf, file_size) == file_size) {
        FURI_LOG_D(TAG, "Loading settings file");
        char* line_ptr = file_buf;
        int16_t line_num = 0;
        memset((uint8_t*)&addrs, 0, sizeof(addrs));
        bool log_loaded = false;
        while(line_ptr && line_ptr - file_buf < file_size) {
            char* end_ptr = strstr((char*)line_ptr, "\n");
            if(end_ptr == NULL)
                end_ptr = file_buf + file_size;
            else
                *end_ptr = '\0';
            FURI_LOG_D(TAG, " L#%d: [%d]%s", line_num, end_ptr - line_ptr, line_ptr);
            int line_len = end_ptr - line_ptr;
            if(*line_ptr == '\r' || line_len == 0) {
                line_ptr = end_ptr + 1;
                continue;
            }
            if(*(end_ptr - 1) < '0') {
                *(end_ptr - 1) = '\0';
                line_len--;
            }
            if(line_len >= LOG_REC_SIZE * 2) { // data
                if(!log_loaded) {
                    clear_log();
                    what_to_do = 0;
                    log_loaded = true;
                }
                if(log_arr_idx < MAX_LOG_RECORDS - 1) {
                    ConvertHexToArray(
                        line_ptr, APP->log_arr + log_arr_idx * LOG_REC_SIZE, LOG_REC_SIZE);
                    log_arr_idx++;
                }
            } else if(addrs.addr_count) {
                ConvertHexToArray(
                    line_ptr,
                    addrs.addr_count == 1 ? &addrs.addr_P1[0] :
                    addrs.addr_count == 2 ? &addrs.addr_P2 :
                    addrs.addr_count == 3 ? &addrs.addr_P3 :
                    addrs.addr_count == 4 ? &addrs.addr_P4 :
                                            &addrs.addr_P5,
                    addrs.addr_count == 1 ? 5 : 1);
                FURI_LOG_D(TAG, " +Addr_LSB: %s", line_ptr);
                if(++addrs.addr_count == MAX_ADDR) break;
            } else if(line_len < 4) { // Rate or Channel
                if(line_num == 0) { // 1st line - Rate
                    NRF_rate = atoi(line_ptr);
                    FURI_LOG_D(TAG, " Rate: %d", NRF_rate);
                } else if(line_num == 1) { // second line - Channel
                    NRF_channel = atoi(line_ptr);
                    FURI_LOG_D(TAG, " Ch: %d", NRF_channel);
                }
            } else if(line_len <= 5 * 2) { // addresses
                addrs.addr_len = ConvertHexToArray(line_ptr, addrs.addr_P0, 5);
                FURI_LOG_D(
                    TAG,
                    " +Addr(%d): %02X%02X%02X...",
                    addrs.addr_len,
                    addrs.addr_P0[0],
                    addrs.addr_P0[1],
                    addrs.addr_P0[2]);
                loaded = true;
                if(++addrs.addr_count == MAX_ADDR) break;
            }
            line_ptr = end_ptr + 1;
            line_num++;
        }
    } else {
        FURI_LOG_D(TAG, "load failed. file size: %d", file_size);
    }
    free(file_buf);
    return loaded;
}

static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);
    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void prepare_nrf24() {
    uint8_t addr[5];
    uint8_t erx_addr = (1 << 0); // Enable RX_P0
    if(addrs.addr_count == 0) return;
    nrf24_write_reg(nrf24_HANDLE, REG_CONFIG, 0x00); // Stop nRF
    nrf24_write_reg(nrf24_HANDLE, REG_STATUS, 0x70); // clear interrupts
    nrf24_write_reg(nrf24_HANDLE, REG_DYNPD, 0x0); // disable shockburst
    nrf24_write_reg(nrf24_HANDLE, REG_EN_AA, 0x00); // Disable Shockburst
    nrf24_write_reg(
        nrf24_HANDLE,
        REG_FEATURE,
        0x01); // Enables the W_TX_PAYLOAD_NOACK command, Disable Payload with ACK, Disable Dynamic Payload Length
    nrf24_set_maclen(nrf24_HANDLE, addrs.addr_len);
    for(int i = 0; i < addrs.addr_len; i++) addr[i] = addrs.addr_P0[addrs.addr_len - i - 1];
    nrf24_write_buf_reg(nrf24_HANDLE, REG_RX_ADDR_P0, &addr[0], addrs.addr_len);
    nrf24_write_reg(nrf24_HANDLE, RX_PW_P0, 32);
    if(addrs.addr_count > 1) {
        for(int i = 0; i < addrs.addr_len; i++) addr[i] = addrs.addr_P1[addrs.addr_len - i - 1];
        nrf24_write_buf_reg(nrf24_HANDLE, REG_RX_ADDR_P1, &addr[0], addrs.addr_len);
        nrf24_write_reg(nrf24_HANDLE, RX_PW_P1, 32);
        erx_addr |= (1 << 1); // Enable RX_P1
    } else
        nrf24_write_reg(nrf24_HANDLE, RX_PW_P1, 0);
    if(addrs.addr_count > 2) {
        nrf24_write_buf_reg(nrf24_HANDLE, REG_RX_ADDR_P2, &addrs.addr_P2, 1);
        nrf24_write_reg(nrf24_HANDLE, RX_PW_P2, 32);
        erx_addr |= (1 << 2); // Enable RX_P2
    } else
        nrf24_write_reg(nrf24_HANDLE, RX_PW_P2, 0);
    if(addrs.addr_count > 3) {
        nrf24_write_buf_reg(nrf24_HANDLE, REG_RX_ADDR_P3, &addrs.addr_P3, 1);
        nrf24_write_reg(nrf24_HANDLE, RX_PW_P3, 32);
        erx_addr |= (1 << 3); // Enable RX_P3
    } else
        nrf24_write_reg(nrf24_HANDLE, RX_PW_P3, 0);
    if(addrs.addr_count > 4) {
        nrf24_write_buf_reg(nrf24_HANDLE, REG_RX_ADDR_P4, &addrs.addr_P4, 1);
        nrf24_write_reg(nrf24_HANDLE, RX_PW_P4, 32);
        erx_addr |= (1 << 4); // Enable RX_P4
    } else
        nrf24_write_reg(nrf24_HANDLE, RX_PW_P4, 0);
    if(addrs.addr_count > 5) {
        nrf24_write_buf_reg(nrf24_HANDLE, REG_RX_ADDR_P5, &addrs.addr_P5, 1);
        nrf24_write_reg(nrf24_HANDLE, RX_PW_P5, 32);
        erx_addr |= (1 << 5); // Enable RX_P5
    } else
        nrf24_write_reg(nrf24_HANDLE, RX_PW_P5, 0);
    nrf24_write_reg(nrf24_HANDLE, REG_STATUS, 0x50); // clear RX_DR, MAX_RT.
    nrf24_set_idle(nrf24_HANDLE);
    nrf24_flush_rx(nrf24_HANDLE);
    nrf24_flush_tx(nrf24_HANDLE);
    nrf24_write_reg(nrf24_HANDLE, REG_EN_RXADDR, erx_addr);
    nrf24_write_reg(nrf24_HANDLE, REG_RF_CH, NRF_channel);
    nrf24_write_reg(nrf24_HANDLE, REG_RF_SETUP, NRF_rate);
    // prime for RX, no checksum
    nrf24_write_reg(nrf24_HANDLE, REG_CONFIG, 0x01); // disable AA and CRC
}

static void start_scanning() {
    prepare_nrf24();
    // prime for RX, no checksum
    nrf24_write_reg(nrf24_HANDLE, REG_CONFIG, 0x03); // PWR_UP and PRIM_RX, disable AA and CRC
    furi_hal_gpio_write(nrf24_CE_PIN, true);
    furi_delay_ms(100);

    start_time = furi_get_tick();
    FURI_LOG_D(TAG, "Start scan: Ch=%d Rate=%d", NRF_channel, NRF_rate);
}

bool nrf24_read_newpacket() {
    if(APP->log_arr == NULL) return false;
    bool found = false;
    uint8_t packetsize;
    uint8_t packet[32] = {0};
    uint8_t status = nrf24_rxpacket(nrf24_HANDLE, packet, &packetsize, true);
    if(status & 0x40) {
        uint8_t* ptr = APP->log_arr + log_arr_idx * LOG_REC_SIZE;
        *ptr++ = (status >> 1) & 7; // pipe #
        memcpy(ptr, packet, packetsize);
        if(packetsize < 32) memset(ptr + packetsize, 0, 32 - packetsize);
        if(log_arr_idx < MAX_LOG_RECORDS - 1) {
            log_arr_idx++;
        } else {
            if(log_to_file == 1 || log_to_file == 2) {
                write_to_log_file(APP->storage);
                clear_log();
            } else {
                memcpy(
                    (void*)APP->log_arr,
                    (void*)APP->log_arr + LOG_REC_SIZE,
                    log_arr_idx * LOG_REC_SIZE);
            }
        }
        FURI_LOG_D(TAG, "Found packet #%d pipe %d", log_arr_idx, (status >> 1) & 7);
        notification_message(APP->notification, &sequence_blink_white_100);
        found = true;
    }
    return found;
}

bool nrf24_send_packet() {
    if(!what_to_do) prepare_nrf24();
    if(log_arr_idx == 0) return false;
    last_packet_send_st = nrf24_txpacket(
        nrf24_HANDLE, APP->log_arr + view_log_arr_idx * LOG_REC_SIZE + 1, 32, false);
    last_packet_send = view_log_arr_idx;
    notification_message(
        APP->notification,
        last_packet_send_st ? &sequence_blink_blue_100 : &sequence_blink_red_100);
    if(what_to_do) start_scanning();
    return last_packet_send_st;
}

static void render_callback(Canvas* const canvas, void* ctx) {
    const PluginState* plugin_state = acquire_mutex((ValueMutex*)ctx, 25);
    if(plugin_state == NULL) return;
    //canvas_draw_frame(canvas, 0, 0, 128, 64); // border around the edge of the screen
    if(what_doing == 0) {
        canvas_set_font(canvas, FontSecondary); // 8x10 font
        snprintf(
            screen_buf, sizeof(screen_buf), "Settings: %s", addr_file_name); // menu_selected = 0
        canvas_draw_str(canvas, 10, 10, screen_buf);
        snprintf(screen_buf, sizeof(screen_buf), "Channel: %d", NRF_channel); // menu_selected = 1
        canvas_draw_str(canvas, 10, 20, screen_buf);
        snprintf(
            screen_buf,
            sizeof(screen_buf),
            "Rate: %sbps",
            NRF_rate == 2 ? "2M" :
            NRF_rate == 1 ? "1M" :
                            "250K"); // menu_selected = 2
        canvas_draw_str(canvas, 10, 30, screen_buf);
        strcpy(screen_buf, "Find channel period: "); // menu_selected = 3
        if(find_channel_period == 0)
            strcat(screen_buf, "off");
        else
            snprintf(
                screen_buf + strlen(screen_buf), sizeof(screen_buf), "%d s", find_channel_period);
        canvas_draw_str(canvas, 10, 40, screen_buf);
        snprintf(
            screen_buf,
            sizeof(screen_buf),
            "Log: %s",
            log_to_file == 0 ? "No" :
            log_to_file == 1 ? "Yes" :
            log_to_file == 2 ? "Append" :
                               "Clear"); // menu_selected = 4
        canvas_draw_str(canvas, 10, 50, screen_buf);
        snprintf(
            screen_buf,
            sizeof(screen_buf),
            "%s (pipes: %d)",
            what_to_do ? "Start scan" : "View log",
            addrs.addr_count); // menu_selected = 5
        canvas_draw_str(canvas, 10, 60, screen_buf);
        canvas_draw_str(canvas, 0, menu_selected * 10 + 10, ">");
    } else {
        canvas_set_font(canvas, FontBatteryPercent); // 5x7 font, 9 lines
        bool ch2 = false;
        screen_buf[0] = '\0';
        if(view_log_arr_x == 0) {
            strcat(screen_buf, "  ");
            ch2 = true;
        } else {
            snprintf(screen_buf, sizeof(screen_buf), "<%d", view_log_arr_x);
            if(view_log_arr_x < VIEW_LOG_MAX_X) ch2 = true;
        }
        snprintf(
            screen_buf + strlen(screen_buf),
            sizeof(screen_buf),
            " %s ch: %d - %d.",
            what_to_do ? "Read" : "View",
            NRF_channel,
            log_arr_idx);
        canvas_draw_str(canvas, 0, 7, screen_buf);
        if(ch2) canvas_draw_str(canvas, 121, 7, ">");
        if(log_arr_idx) {
            uint16_t page = view_log_arr_idx & ~7;
            for(uint8_t i = 0; i < 8 && page + i < log_arr_idx; i++) {
                snprintf(
                    screen_buf,
                    sizeof(screen_buf),
                    "%d:%c",
                    page + i + 1,
                    (view_log_arr_idx & 7) != i          ? ' ' :
                    last_packet_send != view_log_arr_idx ? '>' :
                    last_packet_send_st                  ? '+' :
                                                           '!');
                char* ptr = (char*)APP->log_arr + (page + i) * LOG_REC_SIZE + view_log_arr_x;
                if(view_log_arr_x == 0 && addrs.addr_count > 1) {
                    snprintf(screen_buf + strlen(screen_buf), sizeof(screen_buf), "%d-", *ptr + 1);
                    add_to_str_hex_bytes(screen_buf, ptr + 1, 9);
                } else {
                    add_to_str_hex_bytes(screen_buf, ptr + 1, 10);
                }
                canvas_draw_str(canvas, 0, 14 + i * 7, screen_buf);
            }
        }
    }
    release_mutex((ValueMutex*)ctx, plugin_state);
}

int32_t nrf24scan_app(void* p) {
    UNUSED(p);
    APP = malloc(sizeof(Nrf24Scan));
    APP->event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    APP->plugin_state = malloc(sizeof(PluginState));
    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, APP->plugin_state, sizeof(PluginState))) {
        furi_message_queue_free(APP->event_queue);
        FURI_LOG_E(TAG, "cannot create mutex\r\n");
        free(APP->plugin_state);
        return 255;
    }
    memset((uint8_t*)&addrs, 0, sizeof(addrs));
    nrf24_init();

    // Set system callbacks
    APP->view_port = view_port_alloc();
    view_port_draw_callback_set(APP->view_port, render_callback, &state_mutex);
    view_port_input_callback_set(APP->view_port, input_callback, APP->event_queue);

    // Open GUI and register view_port
    APP->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(APP->gui, APP->view_port, GuiLayerFullscreen);
    APP->notification = furi_record_open(RECORD_NOTIFICATION);
    APP->storage = furi_record_open(RECORD_STORAGE);
    storage_common_mkdir(APP->storage, SCAN_APP_PATH_FOLDER);
    Stream* file_stream = file_stream_alloc(APP->storage);
    FuriString* path = furi_string_alloc();
    furi_string_set(path, SCAN_APP_PATH_FOLDER);
    furi_string_cat(path, "/");
    furi_string_cat(path, ADDR_FILENAME);
    if(file_stream_open(file_stream, furi_string_get_cstr(path), FSAM_READ, FSOM_OPEN_EXISTING)) {
        if(load_settings_file(file_stream))
            strcpy(addr_file_name, ADDR_FILENAME);
        else
            strcpy(addr_file_name, "LOAD ERROR");
    } else {
        strcpy(addr_file_name, "NONE");
    }
    //file_stream_close(file_stream);
    stream_free(file_stream);
    furi_string_free(path);
    allocate_log_array();

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(APP->event_queue, &event, 100);
        PluginState* plugin_state = (PluginState*)acquire_mutex_block(&state_mutex);

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                switch(event.input.key) {
                case InputKeyUp:
                    if(event.input.type == InputTypePress || event.input.type == InputTypeRepeat) {
                        if(what_doing == 0) {
                            if(menu_selected > 0)
                                menu_selected--;
                            else
                                menu_selected = menu_selected_max;
                        } else {
                            view_log_arr_idx -= event.input.type == InputTypeRepeat ? 10 : 1;
                            if(view_log_arr_idx >= log_arr_idx) view_log_arr_idx = 0;
                        }
                    }
                    break;
                case InputKeyDown:
                    if(event.input.type == InputTypePress || event.input.type == InputTypeRepeat) {
                        if(what_doing == 0) {
                            if(menu_selected < menu_selected_max)
                                menu_selected++;
                            else
                                menu_selected = 0;
                        } else {
                            view_log_arr_idx += event.input.type == InputTypeRepeat ? 10 : 1;
                            if(view_log_arr_idx >= log_arr_idx) view_log_arr_idx = log_arr_idx - 1;
                        }
                    }
                    break;
                case InputKeyRight:
                    if(event.input.type == InputTypePress || event.input.type == InputTypeRepeat) {
                        if(what_doing == 0) {
                            switch(menu_selected) {
                            case Menu_enter_channel:
                                NRF_channel += event.input.type == InputTypeRepeat ? 10 : 1;
                                if(NRF_channel > MAX_CHANNEL) NRF_channel = 0;
                                break;
                            case Menu_enter_rate:
                                NRF_rate++;
                                if(NRF_rate > 2) NRF_rate = 0;
                                break;
                            case Menu_enter_scan_period:
                                find_channel_period += event.input.type == InputTypeRepeat ? 10 :
                                                                                             1;
                                break;
                            case Menu_log:
                                if(++log_to_file > 2) log_to_file = -1;
                                break;
                            case Menu_ok:
                                what_to_do = !what_to_do;
                                break;
                            }
                        } else {
                            if(view_log_arr_x < VIEW_LOG_MAX_X) view_log_arr_x++;
                        }
                    }
                    break;
                case InputKeyLeft:
                    if(event.input.type == InputTypePress || event.input.type == InputTypeRepeat) {
                        if(what_doing == 0) {
                            switch(menu_selected) {
                            case Menu_enter_channel:
                                NRF_channel -= event.input.type == InputTypeRepeat ? 10 : 1;
                                if(NRF_channel > MAX_CHANNEL) NRF_channel = MAX_CHANNEL;
                                break;
                            case Menu_enter_rate:
                                NRF_rate--;
                                if(NRF_rate > 2) NRF_rate = 2;
                                break;
                            case Menu_enter_scan_period:
                                find_channel_period -= event.input.type == InputTypeRepeat ? 10 :
                                                                                             1;
                                if(find_channel_period < 0) find_channel_period = 0;
                                break;
                            case Menu_log:
                                if(--log_to_file < -1) log_to_file = 2;
                                break;
                            case Menu_ok:
                                what_to_do = !what_to_do;
                                break;
                            }
                        } else {
                            if(view_log_arr_x > 0) view_log_arr_x--;
                        }
                    }
                    break;
                case InputKeyOk:
                    if(event.input.type == InputTypePress) {
                        if(what_doing == 0) {
                            if(menu_selected == Menu_open_file) { // File
                                file_stream = file_stream_alloc(APP->storage);
                                if(select_settings_file(file_stream)) {
                                    if(load_settings_file(file_stream))
                                        save_to_new_log = true;
                                    else
                                        strcpy(addr_file_name, "LOAD ERROR");
                                    //file_stream_close(file_stream);
                                }
                                stream_free(file_stream);
                            } else if(menu_selected == Menu_ok) { // Ok
                                what_doing = !what_doing;
                                if(what_doing) {
                                    if(what_to_do) {
                                        if(addrs.addr_count == 0)
                                            what_doing = 0;
                                        else {
                                            if(log_to_file == -1) {
                                                clear_log();
                                                save_to_new_log = true;
                                            } else if(log_to_file == 1)
                                                save_to_new_log = true;
                                            start_scanning();
                                        }
                                    }
                                } else
                                    nrf24_set_idle(nrf24_HANDLE);
                            }
                        } else { // Send
                            nrf24_send_packet();
                        }
                    } else if(event.input.type == InputTypeLong) {
                        if(!what_doing) {
                            if(menu_selected == Menu_log) { // Log
                                if(log_arr_idx && (log_to_file == 1 || log_to_file == 2)) {
                                    write_to_log_file(APP->storage);
                                    clear_log();
                                }
                            }
                            nrf24_set_idle(nrf24_HANDLE);
                        }
                    }
                    break;
                case InputKeyBack:
                    if(event.input.type == InputTypeLong)
                        processing = false;
                    else
                        what_doing = 0;
                    nrf24_set_idle(nrf24_HANDLE);
                    break;
                default:
                    break;
                }
            }
        }

        if(what_to_do) {
            nrf24_read_newpacket();
            if(find_channel_period &&
               furi_get_tick() - start_time >= (uint32_t)find_channel_period * 1000UL) {
                if(++NRF_channel > MAX_CHANNEL) NRF_channel = 0;
                start_scanning();
            }
        }

        view_port_update(APP->view_port);
        release_mutex(&state_mutex, plugin_state);
    }
    if(log_arr_idx && (log_to_file == 1 || log_to_file == 2)) {
        write_to_log_file(APP->storage);
    }
    nrf24_deinit();

    view_port_enabled_set(APP->view_port, false);
    gui_remove_view_port(APP->gui, APP->view_port);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_STORAGE);
    view_port_free(APP->view_port);
    furi_message_queue_free(APP->event_queue);
    free(APP->plugin_state);
    if(APP->log_arr) free(APP->log_arr);
    free(APP);
    return 0;
}
